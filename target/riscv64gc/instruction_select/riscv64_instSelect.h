#ifndef RISCV64_INSTSELECT_H
#define RISCV64_INSTSELECT_H
#include "../../common/machine_passes/machine_selector.h"
#include "../riscv64.h"
#include <unordered_map>
class RiscV64Selector : public MachineSelector {
private:
    int cur_offset;    // 局部变量在栈中的偏移
    // 你需要保证在每个函数的指令选择结束后, cur_offset的值为局部变量所占栈空间的大小


    
    // TODO(): 添加更多你需要的成员变量和函数
public:
    RiscV64Selector(MachineUnit *dest, LLVMIR *IR) : MachineSelector(dest, IR) {}
    void SelectInstructionAndBuildCFG();
    void ClearFunctionSelectState();
    template <class INSPTR> void ConvertAndAppend(INSPTR);


    // llvm ir的寄存器编号 -> Risc-V虚拟寄存器
    std::unordered_map<int, Register> irReg2rvReg;
    Register GetRvReg(int irRegNo, MachineDataType type);

    // llvm ir的寄存器编号 -> 该寄存器在栈上的偏移量
    std::unordered_map<int, int> reg2offset;
};
#endif