#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

void LLVMIR::CFGInit() {
    // function_block_map: <IR, {<0,Block>, <1,Block>, ...}>
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->G.resize(bb_map.size());
        cfg->invG.resize(bb_map.size());
        cfg->BuildCFG();
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::BuildCFG() {
    // 每个块是否被访问过
    std::vector<bool> visited(block_map->size(), false);

    // 0号块
    visited[0] = true;
    // 对每个块进行检查
    for (auto &[bbid, block] : *block_map) {
        std::deque<Instruction> &instructions = block->Instruction_list;
        auto it = instructions.begin();
        // 依次访问该块指令，到返回、跳转指令时，记录可到达块并控制流图、删除该块剩余指令
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
                visited[labelNo] = true;

                // 添加到G、invG中
                G[bbid].push_back((*block_map)[labelNo]);
                // invG[labelNo].push_back(block);
                // instructions.erase(std::next(it), instructions.end());
                break;

            } else if (opcode == BasicInstruction::BR_COND) {
                BrCondInstruction *brInst = dynamic_cast<BrCondInstruction *>(inst);

                Operand trueLabel = brInst->GetTrueLabel();
                LabelOperand *trueLabelOperand = dynamic_cast<LabelOperand *>(trueLabel);
                int trueLabelNo = trueLabelOperand->GetLabelNo();
                visited[trueLabelNo] = true;
                G[bbid].push_back((*block_map)[trueLabelNo]);
                invG[trueLabelNo].push_back(block);

                Operand falseLabel = brInst->GetFalseLabel();
                LabelOperand *labelOperand = dynamic_cast<LabelOperand *>(falseLabel);
                int falseLabelNo = labelOperand->GetLabelNo();
                visited[falseLabelNo] = true;
                G[bbid].push_back((*block_map)[falseLabelNo]);
                invG[falseLabelNo].push_back(block);

                instructions.erase(std::next(it), instructions.end());
                break;
            }
            ++it;
        }
    }
    // 删除不可到达块
    for (auto it = block_map->begin(); it != block_map->end();) {
        int bbid = it->first;
        if (!visited[bbid]) {
            it = block_map->erase(it);
        } else {
            ++it;
        }
    }
}

// 获取某个基本块节点的前驱/后继
std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }