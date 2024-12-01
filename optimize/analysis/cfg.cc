#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <algorithm>
#include <assert.h>

void LLVMIR::CFGInit() {
    // function_block_map: <IR, {<0,Block>, <1,Block>, ...}>
    // 一个函数一个cfg，传入function_block_map里的所有内容
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->regInfo = FuncRegInfo_map[defI];
        cfg->Label_num = bb_map.size();
        cfg->G.resize(cfg->Label_num);
        cfg->invG.resize(cfg->Label_num);

        cfg->BuildCFG();

        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->G.clear();
        cfg->invG.clear();
        cfg->G.resize(cfg->Label_num);
        cfg->invG.resize(cfg->Label_num);
        cfg->ord.clear();

        cfg->BuildCFG();
        cfg->PrintCFG();
    }
}

void CFG::Check_one_block(int bbid) {
    auto block = (*block_map)[bbid];
    std::deque<Instruction> &instructions = block->Instruction_list;
    // 直接获取最后一条指令
    auto it = instructions.end() - 1;

    Instruction &inst = *it;
    auto opcode = inst->GetOpcode();
    if (opcode == BasicInstruction::BR_UNCOND) {
        BrUncondInstruction *brInst = dynamic_cast<BrUncondInstruction *>(inst);
        Operand destLabel = brInst->GetDestLabel();
        LabelOperand *labelOperand = dynamic_cast<LabelOperand *>(destLabel);
        int labelNo = labelOperand->GetLabelNo();
        // 添加到G、invG中
        G[bbid].push_back((*block_map)[labelNo]);
        invG[labelNo].push_back(block);
        // 之前没有遍历过才遍历
        if (invG[labelNo].size() == 1)
            Check_one_block(labelNo);

    } else if (opcode == BasicInstruction::BR_COND) {
        BrCondInstruction *brInst = dynamic_cast<BrCondInstruction *>(inst);

        Operand trueLabel = brInst->GetTrueLabel();
        LabelOperand *trueLabelOperand = dynamic_cast<LabelOperand *>(trueLabel);
        int trueLabelNo = trueLabelOperand->GetLabelNo();
        G[bbid].push_back((*block_map)[trueLabelNo]);
        invG[trueLabelNo].push_back(block);
        if (invG[trueLabelNo].size() == 1)
            Check_one_block(trueLabelNo);

        Operand falseLabel = brInst->GetFalseLabel();
        LabelOperand *labelOperand = dynamic_cast<LabelOperand *>(falseLabel);
        int falseLabelNo = labelOperand->GetLabelNo();
        G[bbid].push_back((*block_map)[falseLabelNo]);
        invG[falseLabelNo].push_back(block);
        if (invG[falseLabelNo].size() == 1)
            Check_one_block(falseLabelNo);
    }
    ord.push_back(bbid);
}

// DFS
void CFG::BuildCFG() {
    Check_one_block(0);
    // 结束后，ord储存的是后序遍历的顺序，要将其逆转
    std::reverse(ord.begin(), ord.end());
}

// 获取某个基本块节点的前驱/后继
std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }