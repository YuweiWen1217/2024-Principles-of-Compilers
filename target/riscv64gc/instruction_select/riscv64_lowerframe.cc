#include "riscv64_lowerframe.h"

/*
    假设IR中的函数定义为f(i32 %r0, i32 %r1)
    则parameters应该存放两个虚拟寄存器%0, %1

    在LowerFrame后应当为以下形式：
    add %0, a0, x0  // 将寄存器a0的值赋给虚拟寄存器%0
    add %1, a1, x0  // 将寄存器a1的值赋给虚拟寄存器%1

    对于浮点寄存器（如fa0, fa1），按照类似的方法处理即可。
*/

void RiscV64LowerFrame::Execute() {
    std::cout << "RiscV64LowerFrame::Execute()" << std::endl;
    // 遍历所有的函数
    for (auto func : unit->functions) {
        current_func = func;
        for (auto &b : func->blocks) {
            cur_block = b;
            // 判断是否为函数入口块（bid为0表示入口）
            if (b->getLabelId() == 0) {
                // i32_cnt、f32_cnt：用于计数当前有多少个整型/浮点型参数
                int i32_cnt = 0;
                int f32_cnt = 0;
                int para_offset = 0;
                // 遍历当前函数的所有参数
                for (auto para : func->GetParameters()) {
                    // 检查参数类型是否为64位整数类型
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {
                            // 如果参数可以使用寄存器传递（a0-a7），生成指令将物理寄存器的值赋给虚拟寄存器
                            b->push_front(rvconstructor->ConstructR(RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt),
                                                                    GetPhysicalReg(RISCV_x0)));
                        } else {
                            // 如果超过了寄存器数量限制，应该通过栈传参（尚未实现）
                            // TODO("Implement this if you need");
                            b->push_front(
                            rvconstructor->ConstructIImm(RISCV_LD, para, GetPhysicalReg(RISCV_fp), para_offset));
                            para_offset += 8;
                        }
                        i32_cnt++;    // 递增整数参数计数器
                    }    // 如果参数是 64 位浮点类型
                    else if (para.type.data_type == FLOAT64.data_type) {
                        if (f32_cnt < 8) {
                            // 如果参数可以使用浮点寄存器传递（fa0-fa7）
                            b->push_front(rvconstructor->ConstructR(
                            RISCV_FMV_S, para, GetPhysicalReg(RISCV_fa0 + f32_cnt), GetPhysicalReg(RISCV_x0)));
                        } else {
                            b->push_front(
                            rvconstructor->ConstructIImm(RISCV_FLD, para, GetPhysicalReg(RISCV_fp), para_offset));
                            para_offset += 8;
                        }
                        f32_cnt++;
                    } else {
                        ERROR("Unknown type");
                    }
                }
                if (para_offset != 0) {
                    Register para_basereg = current_func->GetNewReg(INT64);
                    current_func->hasStackParas = true;
                    b->push_front(rvconstructor->ConstructIImm(RISCV_ADDI, para_basereg, GetPhysicalReg(RISCV_fp), 0));
                }
                break;
            }
        }
    }
}

// 收集函数中寄存器的定义和读写信息
void GatherUseSregs(MachineFunction *func, std::vector<std::vector<int>> &def_ids,
                    std::vector<std::vector<int>> &rw_ids) {
    
    // 被调用者保存的寄存器编号 -> 该寄存器被使用的blockid
    // <s0, {0,1,2}>, <s1, {2,4,5}>, ...
    def_ids.resize(64);
    rw_ids.resize(64);

    // 遍历函数中的所有基本块的所有指令
    for (auto &block : func->blocks) {

        // 标记需要保存的寄存器
        int RegNeedSaved[64] = {
        [RISCV_s0] = 1,  [RISCV_s1] = 1,  [RISCV_s2] = 1,  [RISCV_s3] = 1,  [RISCV_s4] = 1,   [RISCV_s5] = 1,
        [RISCV_s6] = 1,  [RISCV_s7] = 1,  [RISCV_s8] = 1,  [RISCV_s9] = 1,  [RISCV_s10] = 1,  [RISCV_s11] = 1,

        [RISCV_fs0] = 1, [RISCV_fs1] = 1, [RISCV_fs2] = 1, [RISCV_fs3] = 1, [RISCV_fs4] = 1,  [RISCV_fs5] = 1,
        [RISCV_fs6] = 1, [RISCV_fs7] = 1, [RISCV_fs8] = 1, [RISCV_fs9] = 1, [RISCV_fs10] = 1, [RISCV_fs11] = 1,
        [RISCV_ra] = 1,
        };
        int block_id = block->getLabelId();

        // 遍历基本块中的所有指令
        for (auto instr : *block) {
            // 目的寄存器
            for (auto reg : instr->GetWriteReg()) {
                if (!reg->is_virtual && RegNeedSaved[reg->reg_no]) {
                    RegNeedSaved[reg->reg_no] = 0;
                    def_ids[reg->reg_no].push_back(block_id);
                    rw_ids[reg->reg_no].push_back(block_id);
                }
            }
            // 源寄存器
            for (auto reg : instr->GetReadReg()) {
                if (!reg->is_virtual && RegNeedSaved[reg->reg_no]) {
                    RegNeedSaved[reg->reg_no] = 0;
                    rw_ids[reg->reg_no].push_back(block_id);
                }
            }
        }
    }

    // 如果函数使用了栈中传递的参数，添加对 fp 寄存器的处理
    if (func->hasStackParas) {
        def_ids[RISCV_fp].push_back(0);    // 在入口定义
        rw_ids[RISCV_fp].push_back(0);     // 在入口读取
    }
}

// 将函数被调用者需要保存的寄存器保存到栈中，并处理栈帧的开辟与释放。
void RiscV64LowerStack::Execute() {
    // 1、遍历目标代码中所有的函数
    for (auto func : unit->functions) {
        current_func = func;

        // 2、初始化保存寄存器的元信息
        std::vector<std::vector<int>> saveregs_occurblockids, saveregs_rwblockids;

        // 3、收集函数中被调用者需要保存的寄存器使用信息
        GatherUseSregs(func, saveregs_occurblockids, saveregs_rwblockids);

        // 4、初始化保存寄存器的基本块信息和偏移量
        std::vector<int> sd_blocks(64, 0);        // 保存指令的块
        std::vector<int> ld_blocks(64, 0);        // 恢复指令的块
        std::vector<int> restore_offset(64, 0);   // 保存到栈上的偏移量

        int saveregnum = 0;  // 需要保存的寄存器数
        int cur_restore_offset = 0;  // 当前栈帧的偏移量

        // 5、统计需要保存的寄存器数量
        for (int i = 0; i < saveregs_occurblockids.size(); i++) {
            if (!saveregs_rwblockids[i].empty()) {
                saveregnum++;
            }
        }

        // 6、根据需要保存的寄存器数扩展栈帧大小（每个寄存器占8字节）
        func->AddStackSize(saveregnum * 8);

        // 7、获取函数的控制流图
        auto mcfg = func->getMachineCFG();

        // 8、遍历函数中的每个基本块，处理保存和恢复寄存器的逻辑
        for (auto &b : func->blocks) {
            cur_block = b;

            // 8.1、在函数入口（基本块ID为0）保存寄存器并开辟栈空间
            if (b->getLabelId() == 0) {
                // 8.1.1、调整栈指针（sp），分配栈帧
                if (func->GetStackSize() <= 2032) {
                    // 小于2032字节时，直接调整sp
                    b->push_front(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                               GetPhysicalReg(RISCV_sp), -func->GetStackSize()));
                } else {
                    // 大于2032字节时，使用中间寄存器t0存储大小并调整sp
                    auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                    b->push_front(rvconstructor->ConstructR(RISCV_SUB, GetPhysicalReg(RISCV_sp),
                                                            GetPhysicalReg(RISCV_sp), stacksz_reg));
                    b->push_front(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                }

                // 8.1.2、若需要，设置帧指针（fp）
                if (func->hasStackParas) {
                    b->push_front(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_fp),
                                                            GetPhysicalReg(RISCV_sp), GetPhysicalReg(RISCV_x0)));
                }

                // 8.1.3、保存所有需要的寄存器到栈中
                int offset = 0;
                for (int i = 0; i < 64; i++) {
                    if (!saveregs_occurblockids[i].empty()) {
                        int regno = i;
                        offset -= 8;  // 每个寄存器占8字节
                        if (regno >= RISCV_x0 && regno <= RISCV_x31) {
                            // 保存整数寄存器
                            b->push_front(rvconstructor->ConstructSImm(RISCV_SD, GetPhysicalReg(regno),
                                                                       GetPhysicalReg(RISCV_sp), offset));
                        } else {
                            // 保存浮点寄存器
                            b->push_front(rvconstructor->ConstructSImm(RISCV_FSD, GetPhysicalReg(regno),
                                                                       GetPhysicalReg(RISCV_sp), offset));
                        }
                    }
                }
            }

            // 8.2、在函数尾部恢复寄存器并释放栈空间
            auto last_ins = *(b->ReverseBegin());
            auto riscv_last_ins = (RiscV64Instruction *)last_ins;

            // 检查最后一条指令是否为 `ret`，即 JALR x0, x1, 0
            if (riscv_last_ins->getOpcode() == RISCV_JALR && 
                riscv_last_ins->getRd() == GetPhysicalReg(RISCV_x0) && 
                riscv_last_ins->getRs1() == GetPhysicalReg(RISCV_ra) && 
                riscv_last_ins->getImm() == 0) {

                // 删除原来的 `ret` 指令
                b->pop_back();

                // 8.2.1、恢复寄存器
                int offset = 0;
                for (int i = 0; i < 64; i++) {
                    if (!saveregs_occurblockids[i].empty()) {
                        int regno = i;
                        offset -= 8;  // 每个寄存器占8字节
                        if (regno >= RISCV_x0 && regno <= RISCV_x31) {
                            // 恢复整数寄存器
                            b->push_back(rvconstructor->ConstructIImm(RISCV_LD, GetPhysicalReg(regno),
                                                                      GetPhysicalReg(RISCV_sp), offset));
                        } else {
                            // 恢复浮点寄存器
                            b->push_back(rvconstructor->ConstructIImm(RISCV_FLD, GetPhysicalReg(regno),
                                                                      GetPhysicalReg(RISCV_sp), offset));
                        }
                    }
                }

                // 8.2.2、释放栈空间
                if (func->GetStackSize() <= 2032) {
                    // 小于2032字节时，直接调整sp
                    b->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_sp),
                                                              GetPhysicalReg(RISCV_sp), func->GetStackSize()));
                } else {
                    // 大于2032字节时，使用中间寄存器调整sp
                    auto stacksz_reg = GetPhysicalReg(RISCV_t0);
                    b->push_back(rvconstructor->ConstructUImm(RISCV_LI, stacksz_reg, func->GetStackSize()));
                    b->push_back(rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_sp),
                                                           GetPhysicalReg(RISCV_sp), stacksz_reg));
                }

                // 8.2.3、重新插入 `ret` 指令
                b->push_back(riscv_last_ins);
            }
        }
    }

    // 9、到此，栈帧的保存与恢复逻辑已完成，目标代码生成工作结束
}
