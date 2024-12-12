#include "mem2reg.h"
#include <tuple>

// 检查该条alloca指令是否可以被mem2reg
// eg. 数组不可以mem2reg
// eg. 如果该指针直接被使用不可以mem2reg(在SysY一般不可能发生,SysY不支持指针语法)
bool Mem2RegPass::IsPromotable(CFG *C, Instruction AllocaInst) { TODO("IsPromotable"); }
/*
    int a1 = 5,a2 = 3,a3 = 11,b = 4
    return b // a1,a2,a3 is useless
-----------------------------------------------
pseudo IR is:
    %r0 = alloca i32 ;a1
    %r1 = alloca i32 ;a2
    %r2 = alloca i32 ;a3
    %r3 = alloca i32 ;b
    store 5 -> %r0 ;a1 = 5
    store 3 -> %r1 ;a2 = 3
    store 11 -> %r2 ;a3 = 11
    store 4 -> %r3 ;b = 4
    %r4 = load i32 %r3
    ret i32 %r4
--------------------------------------------------
%r0,%r1,%r2只有store, 但没有load,所以可以删去
优化后的IR(pseudo)为:
    %r3 = alloca i32
    store 4 -> %r3
    %r4 - load i32 %r3
    ret i32 %r4
*/

// vset is the set of alloca regno that only store but not load
// 该函数对你的时间复杂度有一定要求, 你需要保证你的时间复杂度小于等于O(nlognlogn), n为该函数的指令数
// 提示:deque直接在中间删除是O(n)的, 可以先标记要删除的指令, 最后想一个快速的方法统一删除
// --------------------------
// llvmIR中设置了FuncRegInfo_map这个变量，用于function -> RegInfo的映射
// RegInfo里存了以下信息：
// 1、该函数里没有use的寄存器集合（alloca指令产生时放入集合，store指令）
//---------------------------
void Mem2RegPass::Mem2RegNoUseAlloca(CFG *C, std::set<int> &vset) {
    // this function is used in InsertPhi
    // std::cout << "Mem2RegNoUseAlloca" << std::endl;

    // 1. 检查 unusedRegs 是否为空，如果为空，直接返回
    if (C->regInfo.unusedRegs.empty()) {
        return;
    }

    // 2. 如果 unusedRegs 不为空，求一下涉及到的代码块
    std::unordered_set<int> blockSet;
    for (const Operand &reg : C->regInfo.unusedRegs) {
        auto regBlockIt = C->regInfo.reg2defBlocks.find(reg);
        if (regBlockIt != C->regInfo.reg2defBlocks.end()) {
            const std::unordered_set<int> &blocks = regBlockIt->second;
            blockSet.insert(blocks.begin(), blocks.end());    // 并集
        }
    }

    // 3. 删除 0 号块中的 alloca 指令
    LLVMBlock block0 = (*C->block_map)[0];
    std::deque<Instruction> &instructions = block0->Instruction_list;
    std::deque<Instruction> newDeque;    // 用于存储没被删除的指令
    // 遍历0号块的所有指令，删除不需要的 alloca 指令
    for (auto &inst : instructions) {
        if (inst->GetOpcode() == BasicInstruction::ALLOCA) {
            // 如果当前指令的操作数是 unusedRegs 中的寄存器，则跳过
            AllocaInstruction *allocaInst = dynamic_cast<AllocaInstruction *>(inst);
            if (allocaInst && C->regInfo.unusedRegs.find(allocaInst->GetResult()) != C->regInfo.unusedRegs.end()) {
                continue;
            }
        }
        // 其他指令保持不变
        newDeque.push_back(inst);
    }
    block0->Instruction_list = newDeque;

    // 4. 遍历并集中的块，删除涉及到的store指令
    for (int blockId : blockSet) {
        LLVMBlock block = (*C->block_map)[blockId];
        std::deque<Instruction> &blockInstructions = block->Instruction_list;
        std::deque<Instruction> newBlockDeque;    // 用于存储没被删除的指令
        for (auto &inst : blockInstructions) {
            auto opcode = inst->GetOpcode();
            if (opcode == BasicInstruction::STORE) {
                // 如果是 store 指令，且涉及到 unusedRegs 中的寄存器，则跳过该指令
                StoreInstruction *storeInst = dynamic_cast<StoreInstruction *>(inst);
                if (storeInst) {
                    Operand ptr = storeInst->GetPointer();
                    if (C->regInfo.unusedRegs.find(ptr) != C->regInfo.unusedRegs.end()) {
                        continue;
                    }
                }
            }
            newBlockDeque.push_back(inst);
        }
        block->Instruction_list = newBlockDeque;
    }
}

/*
    int b = getint();
    b = b + 10
    return b // def and use of b are in same block
-----------------------------------------------
pseudo IR is:
    %r0 = alloca i32 ;b
    %r1 = call getint()
    store %r1 -> %r0
    %r2 = load i32 %r0
    %r3 = %r2 + 10
    store %r3 -> %r0
    %r4 = load i32 %r0
    ret i32 %r4
--------------------------------------------------
%r0的所有load和store都在同一个基本块内
优化后的IR(pseudo)为:
    %r1 = call getint()
    %r3 = %r1 + 10
    ret %r3

对于每一个load，我们只需要找到最近的store,然后用store的值替换之后load的结果即可
*/

// vset is the set of alloca regno that load and store are all in the BB block_id
// 该函数对你的时间复杂度有一定要求，你需要保证你的时间复杂度小于等于O(nlognlogn), n为该函数的指令数
void Mem2RegPass::Mem2RegUseDefInSameBlock(CFG *C, std::set<int> &vset, int block_id) {
    // this function is used in InsertPhi
    // TODO("Mem2RegUseDefInSameBlock");

    auto &regInfo = C->regInfo;
    // 1、 构建 block_id -> 满足条件的寄存器集合
    std::unordered_map<int, std::unordered_set<Operand>> blockToRegs;
    std::unordered_set<Operand> qualifiedRegs;
    for (const auto &reg : regInfo.usedRegs) {
        const auto &useBlocks = regInfo.reg2useBlocks[reg];
        const auto &defBlocks = regInfo.reg2defBlocks[reg];
        if (useBlocks.size() == 1 && defBlocks.size() == 1 && *useBlocks.begin() == *defBlocks.begin()) {
            blockToRegs[*useBlocks.begin()].insert(reg);
            qualifiedRegs.insert(reg);
        }
    }

    // 2、删除 0 号块中的相关alloca 指令
    auto *entryBlock = C->block_map->at(0);
    std::deque<Instruction> newDeque;
    for (const auto &instr : entryBlock->Instruction_list) {
        if (instr->GetOpcode() == BasicInstruction::ALLOCA) {
            AllocaInstruction *allocaInst = dynamic_cast<AllocaInstruction *>(instr);
            if (allocaInst && qualifiedRegs.count(allocaInst->GetResult()) > 0)
                continue;
        }
        newDeque.push_back(instr);
    }
    entryBlock->Instruction_list = newDeque;

    // 3、遍历映射表，处理每个块中满足条件的寄存器
    for (const auto &[blockId, regs] : blockToRegs) {
        auto *block = C->block_map->at(blockId);    // 获取目标块

        // 为每个寄存器维护一个 int 型变量 val，初始化为 -1
        std::unordered_map<Operand, int> regToVal;
        for (const auto &reg : regs) {
            regToVal[reg] = -1;    // 初始化
        }
        // 维护一个映射：原寄存器 ID -> 新寄存器 ID
        std::map<int, int> regRenameMap;

        std::deque<Instruction> newDeque;
        for (auto &inst : block->Instruction_list) {
            if (inst->GetOpcode() == BasicInstruction::STORE) {
                StoreInstruction *storeInst = dynamic_cast<StoreInstruction *>(inst);

                Operand storePointer = storeInst->GetPointer();
                if (regs.count(storePointer)) {
                    Operand storeValue = storeInst->GetValue();
                    RegOperand *storeRegOperand = dynamic_cast<RegOperand *>(storeValue);
                    regToVal[storePointer] = storeRegOperand->GetRegNo();
                    continue;
                }
            } else if (inst->GetOpcode() == BasicInstruction::LOAD) {
                LoadInstruction *loadInst = dynamic_cast<LoadInstruction *>(inst);
                if (loadInst) {
                    Operand loadTarget = loadInst->GetPointer();
                    if (regs.count(loadTarget)) {
                        Operand loadResult = loadInst->GetResult();
                        RegOperand *loadRegOperand = dynamic_cast<RegOperand *>(loadResult);
                        regRenameMap[loadRegOperand->GetRegNo()] = regToVal[loadTarget];
                        continue;
                    }
                }
            }
            // 对普通指令，更新寄存器 ID 并加入新队列
            inst->ReplaceRegByMap(regRenameMap);
            newDeque.push_back(inst);
        }
        block->Instruction_list = newDeque;
    }
}

// vset is the set of alloca regno that one store dominators all load instructions
// 该函数对你的时间复杂度有一定要求，你需要保证你的时间复杂度小于等于O(nlognlogn)
void Mem2RegPass::Mem2RegOneDefDomAllUses(CFG *C, std::set<int> &vset) {
    // this function is used in InsertPhi
    TODO("Mem2RegOneDefDomAllUses");
}

void Mem2RegPass::InsertPhi(CFG *C) { TODO("InsertPhi"); }

void Mem2RegPass::VarRename(CFG *C) { TODO("VarRename"); }

void Mem2RegPass::Mem2Reg(CFG *C) {
    std::set<int> s;
    Mem2RegNoUseAlloca(C, s);
    Mem2RegUseDefInSameBlock(C, s, 0);
    // InsertPhi(C);
    // VarRename(C);
}

void Mem2RegPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        Mem2Reg(cfg);
    }
}