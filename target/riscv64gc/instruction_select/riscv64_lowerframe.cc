#include "riscv64_lowerframe.h"

void RiscV64LowerFrame::Execute() {
    // 在每个函数的开头处插入获取参数的指令
    for (auto func : unit->functions) {
        current_func = func;
        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) { // 函数入口，需要插入获取参数的指令
                Register para_basereg = current_func->GetNewReg(INT64);
                int i32_cnt = 0;
                int para_offset = 0;
                for (auto para : func->GetParameters()) {
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) { // 插入使用寄存器传参的指令
                            b->push_front(
                            rvconstructor->ConstructR(RISCV_ADD,para,GetPhysicalReg(RISCV_a0 + i32_cnt),GetPhysicalReg(RISCV_x0)));
                        }
                        if (i32_cnt >= 8) { // 插入使用内存传参的指令
                            TODO("Implement this if necessary");
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) { // 处理浮点数
                        TODO("Implement this if necessary");
                    } else {
                        ERROR("Unknown type");
                    }
                }
            }
        }
    }
}

void RiscV64LowerStack::Execute() {
    // TODO: 在函数开头保存s寄存器，并开辟栈空间
    // TODO: 在函数结尾恢复s寄存器，并收回栈空间
    Log("Save s reg and alloc stack space if necessary");
}