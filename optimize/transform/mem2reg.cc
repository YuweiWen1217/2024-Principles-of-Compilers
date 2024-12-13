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

// 维护一个映射：原寄存器 ID -> 新寄存器 ID
std::map<int, int> regRenameMap;

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
    // 这些寄存器在本函数中全部处理完成，从集合中删去。
    for (const auto &reg : qualifiedRegs) {
        regInfo.usedRegs.erase(reg);
    }

    // 2、删除 0 号块中的相关 alloca 指令和 usedRegs 集合中的寄存器编号。
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

        std::deque<Instruction> newDeque;
        for (auto &inst : block->Instruction_list) {
            // 3.1 指令是目标寄存器的store指令，替换regToVal中的值为value_op(或value_op被替换的值)
            if (inst->GetOpcode() == BasicInstruction::STORE) {
                StoreInstruction *storeInst = dynamic_cast<StoreInstruction *>(inst);

                Operand storePointer = storeInst->GetPointer();
                if (regs.count(storePointer)) {
                    Operand storeValue = storeInst->GetValue();
                    RegOperand *storeRegOperand = dynamic_cast<RegOperand *>(storeValue);
                    int regNo = storeRegOperand->GetRegNo();

                    // 检查 regRenameMap 是否包含此寄存器编号，即该寄存器好是否是本身要被替换的。
                    if (regRenameMap.count(regNo)) {
                        regToVal[storePointer] = regRenameMap[regNo];
                    } else {
                        regToVal[storePointer] = regNo;
                    }
                    continue;
                }
            }
            // 3.2 指令是目标寄存器的load指令，向regRenameMap中添加映射：load指令中的result_op -> regToVal中对应的op
            else if (inst->GetOpcode() == BasicInstruction::LOAD) {
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
            // 3.3 对其他指令，更新寄存器 ID 并加入新队列
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

void Mem2RegPass::InsertPhi(CFG *C) {
    // TODO("InsertPhi");
    auto &regInfo = C->regInfo;
    auto DomTree = domtrees->GetDomTree(C);
    // 1、遍历0号块，删除usedRegs中的指令，并建立reg->指令类型的映射
    auto *entryBlock = C->block_map->at(0);
    std::deque<Instruction> newDeque;
    std::map<Operand, AllocaInstruction::LLVMType> reg2Itype;
    for (auto instr : entryBlock->Instruction_list) {
        if (instr->GetOpcode() == BasicInstruction::ALLOCA) {
            auto *allocaInst = dynamic_cast<AllocaInstruction *>(instr);
            if (allocaInst && regInfo.usedRegs.count(allocaInst->GetResult())) {
                reg2Itype[allocaInst->GetResult()] = allocaInst->GetDataType();
                continue;
            }
        }
        newDeque.push_back(instr);    // 其他指令保留
    }
    // 更新 0 号块的指令列表
    entryBlock->Instruction_list = newDeque;

    // 2、遍历 usedRegs 集合中的寄存器（特殊的已经处理好了，集合中都是剩下的）
    for (auto reg : regInfo.usedRegs) {

        // 3、计算支配边界闭包
        std::unordered_set<int> phiBlocks;
        auto &defBlocks = regInfo.reg2defBlocks[reg];
        std::unordered_set<int> worklist = defBlocks;
        while (!worklist.empty()) {
            int currentBlock = *worklist.begin();
            worklist.erase(currentBlock);

            // 遍历当前块的支配边界
            for (int df : DomTree->GetDF(currentBlock)) {
                // 插入phiBlocks中，一个新块会返回true，已经存在返回false。
                if (phiBlocks.insert(df).second)
                    worklist.insert(df);
            }
        }

        // 4、遍历支配边界闭包，将 phi 指令插入相应块
        for (int phiBlock : phiBlocks) {
            // 创建新的 phi 指令
            PhiInstruction *phi = new PhiInstruction(reg2Itype[reg], GetNewRegOperand(++C->reg_max));
            (*C->block_map)[phiBlock]->InsertInstruction(0, phi);
            // 映射 phi 指令到当前寄存器
            phi2reg[phi] = ((RegOperand *)reg)->GetRegNo();
        }
    }
}

void Mem2RegPass::VarRename(CFG *C) {
    // TODO("VarRename");

    // 1、准备一些需要用到的变量
    // visited: dfs用
    std::vector<bool> visited;
    visited.resize(C->Label_num);
    std::stack<int> tobecheck;
    std::set<Instruction> Erase_set;

    // blockid + reg -> value，功能同Mem2RegUseDefInSameBlock里的regToval
    std::unordered_map<int, std::unordered_map<Operand, int>> idreg2val;

    auto &regInfo = C->regInfo;

    // 2、DFS处理剩下的寄存器
    tobecheck.push(0);
    while (!tobecheck.empty()) {
        int block_id = tobecheck.top();
        tobecheck.pop();
        if (visited[block_id])
            continue;
        visited[block_id] = 1;

        // 3、对当前块进行变量重命名
        auto block = (C->block_map)->at(block_id);
        for (auto &Inst : block->Instruction_list) {
            auto opcode = Inst->GetOpcode();
            // 3.1、STORE指令，更新寄存器的val
            if (opcode == BasicInstruction::STORE) {
                StoreInstruction *storeInst = dynamic_cast<StoreInstruction *>(Inst);
                Operand storePointer = storeInst->GetPointer();
                if (regInfo.usedRegs.count(storePointer)) {
                    Erase_set.insert(storeInst);
                    // 逻辑同Mem2RegUseDefInSameBlock，避免regNo本身也是要被替换的。
                    auto regNo = ((RegOperand *)(storeInst->GetValue()))->GetRegNo();
                    if (regRenameMap.count(regNo))
                        idreg2val[block_id][storePointer] = regRenameMap[regNo];
                    else
                        idreg2val[block_id][storePointer] = regNo;
                }
            }
            // 3.2、PHI指令：其功能同store指令
            else if (opcode == BasicInstruction::PHI) {
                PhiInstruction *phiInst = dynamic_cast<PhiInstruction *>(Inst);
                if (Erase_set.find(phiInst) != Erase_set.end())
                    continue;
                auto regNo = ((RegOperand *)phiInst->GetResult())->GetRegNo();
                if (regRenameMap.count(regNo))
                    idreg2val[block_id][GetNewRegOperand(phi2reg[phiInst])] = regRenameMap[regNo];
                else
                    idreg2val[block_id][GetNewRegOperand(phi2reg[phiInst])] = regNo;
            }
            // 3.3、LOAD指令：向map中添加 原寄存器-> 新寄存器 的映射
            else if (opcode == BasicInstruction::LOAD) {
                LoadInstruction *loadInst = dynamic_cast<LoadInstruction *>(Inst);
                Operand loadPointer = loadInst->GetPointer();

                if (regInfo.usedRegs.count(loadPointer)) {
                    Erase_set.insert(loadInst);
                    Operand loadResult = loadInst->GetResult();
                    RegOperand *loadRegOperand = dynamic_cast<RegOperand *>(loadResult);
                    regRenameMap[loadRegOperand->GetRegNo()] = idreg2val[block_id][loadPointer];
                }
            }
        }

        // 4、处理后继块，继承当前块的寄存器映射关系
        for (auto next_block : C->G[block_id]) {
            int next_block_id = next_block->block_id;
            tobecheck.push(next_block_id);
            idreg2val[next_block_id] = idreg2val[block_id];

            // 处理后继块中的 PHI 指令
            for (auto Inst : (*C->block_map)[next_block_id]->Instruction_list) {
                if (Inst->GetOpcode() != BasicInstruction::PHI)
                    break;
                PhiInstruction *phiInst = dynamic_cast<PhiInstruction *>(Inst);

                // 查找当前 PHI 指令对应的 alloca 寄存器
                int RegNo = phi2reg[phiInst];
                auto Reg = GetNewRegOperand(RegNo);
                // 如果当前 block 没有为该 pointer_reg 赋值，则删除该 PHI 指令
                if (idreg2val[block_id].find(Reg) == idreg2val[block_id].end()) {
                    Erase_set.insert(phiInst);
                    continue;
                }
                // 为 PHI 指令添加前驱块的信息
                phiInst->InsertPhi(GetNewLabelOperand(block_id), GetNewRegOperand(idreg2val[block_id][Reg]));
            }
        }
    }

    // 5、 删除 Erase_set 中的无用指令
    for (auto &[bid, block] : *C->block_map) {
        auto tmp_Instruction_list = block->Instruction_list;
        block->Instruction_list.clear();

        // 只保留不在删除集合中的指令
        for (auto I : tmp_Instruction_list) {
            if (Erase_set.find(I) == Erase_set.end())
                block->InsertInstruction(1, I);
        }
    }

    // 6、 进行变量的重命名，替换寄存器映射
    for (auto &[bid, block] : *C->block_map) {
        for (auto I : block->Instruction_list)
            I->ReplaceRegByMap(regRenameMap);
    }
}

void Mem2RegPass::Mem2Reg(CFG *C) {
    std::set<int> s;
    Mem2RegNoUseAlloca(C, s);
    Mem2RegUseDefInSameBlock(C, s, 0);
    InsertPhi(C);
    //VarRename(C);
}

void Mem2RegPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        Mem2Reg(cfg);
    }
}