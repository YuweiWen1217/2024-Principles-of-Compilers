#include "basic_register_allocation.h"

void VirtualRegisterRewrite::Execute() {
    for (auto func : unit->functions) {
        current_func = func;
        ExecuteInFunc();
    }
}

void VirtualRegisterRewrite::ExecuteInFunc() {
    auto func = current_func;
    auto block_it = func->getMachineCFG()->getSeqScanIterator();
    block_it->open();
    while (block_it->hasNext()) {
        auto block = block_it->next()->Mblock;
        for (auto it = block->begin(); it != block->end(); ++it) {
            auto ins = *it;
            // 根据alloc_result将ins的虚拟寄存器重写为物理寄存器
            for (auto reg : ins->GetReadReg()) {
                if (reg->is_virtual == false) {
                    Assert(alloc_result.find(func) == alloc_result.end() ||
                           alloc_result.find(func)->second.find(*reg) == alloc_result.find(func)->second.end());
                    continue;
                }
                auto result = alloc_result.find(func)->second.find(*reg)->second;
                if (result.in_mem == true) {
                    ERROR("Shouldn't reach here");
                } else {
                    reg->is_virtual = false;
                    reg->reg_no = result.phy_reg_no;
                }
            }
            for (auto reg : ins->GetWriteReg()) {
                if (reg->is_virtual == false) {
                    Assert(alloc_result.find(func) == alloc_result.end() ||
                           alloc_result.find(func)->second.find(*reg) == alloc_result.find(func)->second.end());
                    continue;
                }
                auto result = alloc_result.find(func)->second.find(*reg)->second;
                if (result.in_mem == true) {
                    ERROR("Shouldn't reach here");
                } else {
                    reg->is_virtual = false;
                    reg->reg_no = result.phy_reg_no;
                }
            }
        }
    }
}

void SpillCodeGen::ExecuteInFunc(MachineFunction *function, std::map<Register, AllocResult> *alloc_result) {
    this->function = function;
    this->alloc_result = alloc_result;
    auto block_it = function->getMachineCFG()->getSeqScanIterator();
    block_it->open();
    while (block_it->hasNext()) {
        cur_block = block_it->next()->Mblock;
        for (auto it = cur_block->begin(); it != cur_block->end(); ++it) {
            auto ins = *it;
            // 根据alloc_result对ins溢出的寄存器生成溢出代码
            // 在溢出虚拟寄存器的read前插入load，在write后插入store
            // 注意load的结果寄存器是虚拟寄存器, 因为我们接下来要重新分配直到不再溢出
            for (auto reg : ins->GetReadReg()) {
                if (reg->is_virtual == false)
                    continue;
                auto result = alloc_result->find(*reg)->second;
                if (result.in_mem == true) {
                    // Spill Code Gen
                    *reg = GenerateReadCode(it, result.stack_offset * 4, reg->type);
                }
            }
            for (auto reg : ins->GetWriteReg()) {
                if (reg->is_virtual == false)
                    continue;
                auto result = alloc_result->find(*reg)->second;
                if (result.in_mem == true) {
                    // Spill Code Gen
                    *reg = GenerateWriteCode(it, result.stack_offset * 4, reg->type);
                }
            }
        }
    }
}