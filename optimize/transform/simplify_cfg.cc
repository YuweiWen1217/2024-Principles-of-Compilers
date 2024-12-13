#include "simplify_cfg.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}

// 删除从函数入口开始到达不了的基本块和指令
// 不需要考虑控制流为常量的情况，你只需要从函数入口基本块(0号基本块)开始dfs，将没有被访问到的基本块和指令删去即可
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) {

    std::vector<bool> visited(C->block_map->size(), false);
    std::stack<int> tobecheck;

    // 0号块
    visited[0] = true;
    tobecheck.push(0);

    // 对栈中的块进行检查
    while (!tobecheck.empty()) {
        int block_id = tobecheck.top();
        tobecheck.pop();

        auto block = (C->block_map)->at(block_id);
        std::deque<Instruction> &instructions = block->Instruction_list;
        auto it = instructions.begin();
        while (it != instructions.end()) {
            Instruction &inst = *it;
            auto opcode = inst->GetOpcode();

            // 如果是返回指令或跳转指令（无条件或条件跳转）
            if (opcode == BasicInstruction::RET) {
                instructions.erase(std::next(it), instructions.end());
                break;
            } else if (opcode == BasicInstruction::BR_UNCOND) {
                BrUncondInstruction *brInst = dynamic_cast<BrUncondInstruction *>(inst);
                Operand destLabel = brInst->GetDestLabel();
                LabelOperand *labelOperand = dynamic_cast<LabelOperand *>(destLabel);
                int labelNo = labelOperand->GetLabelNo();
                if (!visited[labelNo]) {
                    visited[labelNo] = true;
                    tobecheck.push(labelNo);
                }
                break;
            } else if (opcode == BasicInstruction::BR_COND) {
                BrCondInstruction *brInst = dynamic_cast<BrCondInstruction *>(inst);

                Operand trueLabel = brInst->GetTrueLabel();
                LabelOperand *trueLabelOperand = dynamic_cast<LabelOperand *>(trueLabel);
                int trueLabelNo = trueLabelOperand->GetLabelNo();
                if (!visited[trueLabelNo]) {
                    visited[trueLabelNo] = true;
                    tobecheck.push(trueLabelNo);
                }

                Operand falseLabel = brInst->GetFalseLabel();
                LabelOperand *labelOperand = dynamic_cast<LabelOperand *>(falseLabel);
                int falseLabelNo = labelOperand->GetLabelNo();
                if (!visited[falseLabelNo]) {
                    visited[falseLabelNo] = true;
                    tobecheck.push(falseLabelNo);
                }

                instructions.erase(std::next(it), instructions.end());
                break;
            }
            // 以下是mem2reg的准备工作
            else if (opcode == BasicInstruction::LOAD) {
                LoadInstruction *loadInst = dynamic_cast<LoadInstruction *>(inst);
                Operand ptr = loadInst->GetPointer();
                if (ptr->GetOperandType() == BasicOperand::REG) {
                    C->regInfo.unusedRegs.erase(ptr);
                    C->regInfo.usedRegs.insert(ptr);
                    C->regInfo.reg2useBlocks[ptr].insert(block_id);
                }
            } else if (opcode == BasicInstruction::STORE) {
                StoreInstruction *storeInst = dynamic_cast<StoreInstruction *>(inst);
                Operand ptr = storeInst->GetPointer();
                if (ptr->GetOperandType() == BasicOperand::REG) {
                    C->regInfo.reg2defBlocks[ptr].insert(block_id);
                }
            }
            ++it;
        }
    }
    // 删除不可到达块
    for (auto it = C->block_map->begin(); it != C->block_map->end();) {
        int bbid = it->first;
        if (!visited[bbid]) {
            it = C->block_map->erase(it);
        } else {
            ++it;
        }
    }
    //C->printFuncRegInfo();
}