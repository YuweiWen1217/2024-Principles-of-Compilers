#ifndef MACHINE_SELECTOR_H
#define MACHINE_SELECTOR_H
#include "../machine_instruction_structures/machine.h"
class MachineSelector {
    // 指令选择基类
protected:
    MachineUnit *dest;            // 指向目标机器单元的指针，表示整个目标代码程序
    MachineFunction *cur_func;    // 当前正在处理的目标机器函数
    MachineBlock *cur_block;      // 当前正在处理的目标机器基本块
    LLVMIR *IR;                   // 输入的 LLVM IR 表示

public:
    MachineSelector(MachineUnit *dest, LLVMIR *IR) : dest(dest), IR(IR) {}
    virtual void SelectInstructionAndBuildCFG() = 0;
    MachineUnit *GetMachineUnit() { return dest; }
};
#endif