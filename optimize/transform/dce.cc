#include <dce.h>

// Reference：https://www.cnblogs.com/lixingyang/p/17728846.html

void DcePass::dce(CFG *C) {
    // 1、准备相关变量
    std::stack<int> workList;
    std::unordered_map<int, Instruction> defMap;
    std::unordered_map<int, int> useMap;
    std::unordered_set<Instruction> eraseSet;

    // 2、遍历指令，填充useMap和defMap
    for (auto [bid, block] : *C->block_map) {

        for (auto Inst : block->Instruction_list) {
            int resultRegNo = Inst->GetResultRegNo();
            std::vector<int> operandRegsNo = Inst->GetOperandRegsNo();

            // 左值放入defMap和workList
            if (resultRegNo != -1) {
                defMap[resultRegNo] = Inst;
                workList.push(resultRegNo);
            }
            // 右值放入useMap
            for (auto regNo : operandRegsNo) {
                useMap[regNo]++;
            }
        }
    }

    // 3、死代码消除：如果某寄存器的结果没有被使用，则定义该寄存器的指令可以删除。
    while (!workList.empty()) {
        int regNo = workList.top();    // 依次检查每个被def的寄存器
        workList.pop();

        // 如果该寄存器没有被使用
        if (useMap[regNo] == 0) {
            auto Inst = defMap[regNo];

            // 函数调用可能有副作用：修改全局变量、、执行 I/O 操作、改变内存状态等，不能简单删除。
            if (Inst->GetOpcode() == BasicInstruction::CALL) {
                continue;
            }
            // 已经被删除的不能再删了
            if (eraseSet.find(Inst) != eraseSet.end()) {
                continue;
            }
            eraseSet.insert(Inst);
            // Inst->PrintIR(std::cout);
            for (auto opRegNo : Inst->GetOperandRegsNo()) {
                // std::cout << opRegNo <<std::endl;
                // std::cout<<useMap[opRegNo]<<std::endl;
                useMap[opRegNo]--;
                workList.push(opRegNo);
            }
        }
    }

    // 4、执行删除
    for (auto &[bid, block] : *C->block_map) {
        std::deque<Instruction> newInstructionList;
        for (auto I : block->Instruction_list) {
            if (eraseSet.find(I) == eraseSet.end())
                newInstructionList.push_back(I);
        }
        block->Instruction_list = newInstructionList;
    }
}

void DcePass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg)
        dce(cfg);
}
