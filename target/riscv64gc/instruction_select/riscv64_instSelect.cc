#include "riscv64_instSelect.h"
#include <cassert>
#include <sstream>

// load
// Syntax: <result>=load <ty>, ptr <pointer>
template <> void RiscV64Selector::ConvertAndAppend<LoadInstruction *>(LoadInstruction *ins) {
    auto rd_op = (RegOperand *)ins->GetResult();

    // 右值为数组某元素值
    if (ins->GetPointer()->GetOperandType() == BasicOperand::REG) {
        auto ptr_op = (RegOperand *)ins->GetPointer();
        if (reg2offset.find(ptr_op->GetRegNo()) != reg2offset.end())
            ERROR("Unexpected situation!");
        if (ins->GetDataType() == BasicInstruction::I32) {
            Register rd = GetRvReg(rd_op->GetRegNo(), INT64);
            Register ptr = GetRvReg(ptr_op->GetRegNo(), INT64);
            // 类似于 lw x5, 0(x6) # x6 中的地址，加载 32 位值到 x5
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_LW, rd, ptr, 0));
        } else if (ins->GetDataType() == BasicInstruction::FLOAT32) {
            Register rd = GetRvReg(rd_op->GetRegNo(), FLOAT64);
            Register ptr = GetRvReg(ptr_op->GetRegNo(), INT64);
            cur_block->push_back(rvconstructor->ConstructIImm(RISCV_FLW, rd, ptr, 0));
        } else {
            ERROR("Unexpected data type");
        }
    }
    // 右值为全局变量/常量（非数组）
    else if (ins->GetPointer()->GetOperandType() == BasicOperand::GLOBAL) {
        auto global_op = (GlobalOperand *)ins->GetPointer();

        // lui x5, %hi(global_var)  # 加载 global_var 的高 20 位地址到 x5
        Register addr_hi = cur_func->GetNewReg(INT64);    // 获取一个新的寄存器，用于存储全局变量的高位地址部分
        auto lui_instr = rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_op->GetName(), true));
        cur_block->push_back(lui_instr);

        if (ins->GetDataType() == BasicInstruction::I32) {
            Register rd = GetRvReg(rd_op->GetRegNo(), INT64);
            // lw x6, %lo(global_var)(x5)  # 将全局变量的低 12 位地址与 addr_hi 相结合，然后加载 32 位整数到 rd
            auto lw_instr =
            rvconstructor->ConstructILabel(RISCV_LW, rd, addr_hi, RiscVLabel(global_op->GetName(), false));
            cur_block->push_back(lw_instr);

        } else if (ins->GetDataType() == BasicInstruction::FLOAT32) {
            Register rd = GetRvReg(rd_op->GetRegNo(), FLOAT64);
            auto lw_instr =
            rvconstructor->ConstructILabel(RISCV_FLW, rd, addr_hi, RiscVLabel(global_op->GetName(), false));
            cur_block->push_back(lw_instr);
        } else {
            ERROR("Unexpected data type");
        }
    }
}

// RISC-V 存储指令处理
// 语法：store <类型> <值>, ptr<pointer>
template <> void RiscV64Selector::ConvertAndAppend<StoreInstruction *>(StoreInstruction *ins) {
    // 1、提取存储值
    auto value_operand = (RegOperand *)ins->GetValue();
    Register valueReg =
    GetRvReg(value_operand->GetRegNo(), (ins->GetDataType() == BasicInstruction::I32) ? INT32 : FLOAT64);

    // 2、处理存的位置
    auto pointer_type = ins->GetPointer()->GetOperandType();
    // 2.1、存到一个寄存器指示的地址
    if (pointer_type == BasicOperand::REG) {

        auto pointer_operand = (RegOperand *)ins->GetPointer();
        Register pointer_register;
        int offset = 0;

        // 检查指针是否为栈上分配的变量
        if (reg2offset.find(pointer_operand->GetRegNo()) == reg2offset.end()) {
            // 指针未在栈上分配
            pointer_register = GetRvReg(pointer_operand->GetRegNo(), INT64);
        } else {
            pointer_register = GetPhysicalReg(RISCV_sp);
            // 指针为栈上分配
            offset = reg2offset[pointer_operand->GetRegNo()];

            // 处理偏移量超出范围 [-2048, 2047] 的情况
            if (offset > 2047 || offset < -2048) {
                // 保存偏移量
                // li offset_high_reg, <offset>
                // 例如 li x10, 4096
                Register offsetReg = cur_func->GetNewReg(INT64);
                cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, offsetReg, offset));

                // add x11, x10, sp
                // 加上sp得到最后的地址
                Register ptrReg = cur_func->GetNewReg(INT64);
                cur_block->push_back(rvconstructor->ConstructR(RISCV_ADD, ptrReg, offsetReg, GetPhysicalReg(RISCV_sp)));

                // sw 或 fsw value_register, 0(ptrReg)
                auto store_instr = rvconstructor->ConstructSImm(
                (ins->GetDataType() == BasicInstruction::I32) ? RISCV_SW : RISCV_FSW, valueReg, ptrReg, 0);

                ((RiscV64Function *)cur_func)->allocalist.push_back(store_instr);
                cur_block->push_back(store_instr);
                return;
            }
        }

        // 偏移量在范围内 [-2048, 2047]，可以直接用sw！
        // 生成存储指令：sw 或 fsw valuereg offset(pointreg)
        auto store_instr = rvconstructor->ConstructSImm(
        (ins->GetDataType() == BasicInstruction::I32) ? RISCV_SW : RISCV_FSW, valueReg, pointer_register, offset);

        if (reg2offset.find(pointer_operand->GetRegNo()) != reg2offset.end()) {
            ((RiscV64Function *)cur_func)->allocalist.push_back(store_instr);
        }
        cur_block->push_back(store_instr);
    }
    // 2.2、存到全局变量
    else {
        // 处理指针为全局变量的情况
        auto global_operand = (GlobalOperand *)ins->GetPointer();
        Register addr_hi = cur_func->GetNewReg(INT64);

        // lui addr_hi, %hi(global_var)
        // sw 或 fsw value_register, %lo(global_var)(addr_hi)
        auto lui_instr =
        rvconstructor->ConstructULabel(RISCV_LUI, addr_hi, RiscVLabel(global_operand->GetName(), true));
        cur_block->push_back(lui_instr);

        auto store_instr =
        rvconstructor->ConstructSLabel((ins->GetDataType() == BasicInstruction::I32) ? RISCV_SW : RISCV_FSW, valueReg,
                                       addr_hi, RiscVLabel(global_operand->GetName(), false));
        cur_block->push_back(store_instr);
    }
}

int GetOpcodeForArithmetic(int opcode) {
    switch (opcode) {
    case BasicInstruction::FADD:
        return RISCV_FADD_S;
    case BasicInstruction::FSUB:
        return RISCV_FSUB_S;
    case BasicInstruction::FMUL:
        return RISCV_FMUL_S;
    case BasicInstruction::FDIV:
        return RISCV_FDIV_S;
    case BasicInstruction::ADD:
        return RISCV_ADD;
    case BasicInstruction::SUB:
        return RISCV_SUB;
    case BasicInstruction::MUL:
        return RISCV_MUL;
    case BasicInstruction::DIV:
        return RISCV_DIV;
    case BasicInstruction::MOD:
        return RISCV_REM;
    default:
        ERROR("Unsupported integer opcode");
    }
}

template <> void RiscV64Selector::ConvertAndAppend<ArithmeticInstruction *>(ArithmeticInstruction *ins) {
    // 判断是否为浮点运算
    int opcode = ins->GetOpcode();
    bool isfloat = (opcode == BasicInstruction::FADD || opcode == BasicInstruction::FSUB ||
                    opcode == BasicInstruction::FMUL || opcode == BasicInstruction::FDIV);
    auto imm_type = isfloat ? BasicOperand::IMMF32 : BasicOperand::IMMI32;
    auto reg_type = isfloat ? FLOAT64 : INT64;

    // 获取操作数1和操作数2
    auto op1 = ins->GetOperand1();
    auto op2 = ins->GetOperand2();

    // 获取目的寄存器（获取ir、分配rv）
    auto *result_op = (RegOperand *)ins->GetResult();
    auto rd = GetRvReg(result_op->GetRegNo(), reg_type);

    if (!isfloat) {
        // 情况1：立即数 op 立即数
        if (op1->GetOperandType() == imm_type && op2->GetOperandType() == imm_type) {
            auto *Iop1 = (ImmI32Operand *)op1;
            auto *Iop2 = (ImmI32Operand *)op2;
            int result_value;
            // 根据操作符计算立即数结果
            switch (opcode) {
            case BasicInstruction::ADD:
                result_value = Iop1->GetIntImmVal() + Iop2->GetIntImmVal();
                break;
            case BasicInstruction::SUB:
                result_value = Iop1->GetIntImmVal() - Iop2->GetIntImmVal();
                break;
            case BasicInstruction::MUL:
                result_value = Iop1->GetIntImmVal() * Iop2->GetIntImmVal();
                break;
            case BasicInstruction::DIV:
                result_value = Iop1->GetIntImmVal() / Iop2->GetIntImmVal();
                break;
            case BasicInstruction::MOD:
                result_value = Iop1->GetIntImmVal() % Iop2->GetIntImmVal();
                break;
            default:
                ERROR("Unsupported opcode for immediate operands");
            }
            // 生成立即数加载指令并追加到当前块
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, rd, result_value));
        }
        // 情况2：寄存器 op 寄存器
        else if (op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::REG) {
            auto *rs1_op = (RegOperand *)ins->GetOperand1();
            auto *rs2_op = (RegOperand *)ins->GetOperand2();
            auto rs1 = GetRvReg(rs1_op->GetRegNo(), reg_type);
            auto rs2 = GetRvReg(rs2_op->GetRegNo(), reg_type);

            int opcode_instr = GetOpcodeForArithmetic(ins->GetOpcode());
            cur_block->push_back(rvconstructor->ConstructR(opcode_instr, rd, rs1, rs2));

        }
        // 情况3：立即数 op 寄存器
        else if (op1->GetOperandType() == imm_type && op2->GetOperandType() == BasicOperand::REG) {
            auto *imm_op = (ImmI32Operand *)op1;
            auto imm = imm_op->GetIntImmVal();

            auto *rs_op = (RegOperand *)op2;
            auto rs = GetRvReg(rs_op->GetRegNo(), reg_type);    // 获取寄存器号
            // ADD 或 SUB 操作，并且立即数在范围内  -> ADDI
            if (opcode == BasicInstruction::ADD && (imm <= 2047 && imm >= -2048)) {
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, rd, rs, imm));
            } else {
                // li temp_reg, imm
                // opcode rd, rs, temp_reg
                Register temp_reg = cur_func->GetNewReg(reg_type);    // 获取新的临时寄存器
                cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_reg, imm));
                cur_block->push_back(rvconstructor->ConstructR(GetOpcodeForArithmetic(opcode), rd, temp_reg, rs));
            }
        } else {
            // std::cout << op1->GetOperandType() << " " << op2->GetOperandType() << std::endl;
            // std::cout << imm_type << " " << BasicOperand::REG << std::endl;
            ERROR("Unsupported operand type combination for ArithmeticInstruction");
        }
    }
    // 浮点数运算
    else {
        if (op1->GetOperandType() == imm_type && op2->GetOperandType() == imm_type) {

            auto *Iop1 = (ImmF32Operand *)op1;
            auto *Iop2 = (ImmF32Operand *)op2;
            double result_value;
            switch (opcode) {
            case BasicInstruction::FADD:
                result_value = Iop1->GetFloatVal() + Iop2->GetFloatVal();
                break;
            case BasicInstruction::FSUB:
                result_value = Iop1->GetFloatVal() - Iop2->GetFloatVal();
                break;
            case BasicInstruction::FMUL:
                result_value = Iop1->GetFloatVal() * Iop2->GetFloatVal();
                break;
            case BasicInstruction::FDIV:
                result_value = Iop1->GetFloatVal() / Iop2->GetFloatVal();
                break;
            default:
                ERROR("Unsupported opcode for immediate operands");
            }
            // 3.14 -> 0x4048F5C3，先当作整数放到整型寄存器，然后使用fmv.w.x将值移动到
            // 例如：
            // li t0, 0x4048F5C3    # 将立即数 0x4048F5C3 加载到整数寄存器 t0
            // fmv.w.x f0, t0       # 将整数寄存器 t0 的值移动到浮点寄存器 f0
            uint32_t result_binary;
            memcpy(&result_binary, &result_value, sizeof(float));    // 将浮点数转为二进制
            Register temp_int_reg = cur_func->GetNewReg(INT64);
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_int_reg, result_binary));
            cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, rd, temp_int_reg));

        } else if (op1->GetOperandType() == BasicOperand::REG && op2->GetOperandType() == BasicOperand::REG) {
            auto *rs1_op = (RegOperand *)op1;
            auto *rs2_op = (RegOperand *)op2;

            auto rs1 = GetRvReg(rs1_op->GetRegNo(), FLOAT64);
            auto rs2 = GetRvReg(rs2_op->GetRegNo(), FLOAT64);

            int opcode_instr = GetOpcodeForArithmetic(opcode);
            cur_block->push_back(rvconstructor->ConstructR(opcode_instr, rd, rs1, rs2));
        } else if (op1->GetOperandType() == imm_type && op2->GetOperandType() == BasicOperand::REG) {
            // 操作数1：立即数 -> 整型寄存器 -> 浮点数寄存器
            auto *imm_op = (ImmF32Operand *)op1;
            auto imm_val = imm_op->GetFloatVal();
            uint32_t imm_binary;
            memcpy(&imm_binary, &imm_val, sizeof(float));
            Register temp_int_reg = cur_func->GetNewReg(INT64);
            Register imm_reg = cur_func->GetNewReg(FLOAT64);
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_int_reg, imm_binary));
            cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, imm_reg, temp_int_reg));

            // 操作数2：寄存器
            auto *rs_op = (RegOperand *)op2;
            auto rs = GetRvReg(rs_op->GetRegNo(), FLOAT64);    // 第二个操作数寄存器

            // 添加最终指令！
            int opcode_instr = GetOpcodeForArithmetic(opcode);
            cur_block->push_back(rvconstructor->ConstructR(opcode_instr, rd, imm_reg, rs));

        } else {
            ERROR("Unsupported operand type combination for ArithmeticInstruction");
        }
    }
}

/*
    先看整型：
        对于跳转指令
        例如，有如下llvm ir
            %r2 = icmp sge i32 %r0,%r1
            br i1 %r2, label %L2, label %L3
        如果r0 >= r1，则跳转到L2，否则跳转到L3。
        一种方法是：
            sge x3, %1, %2
            bne %3, x0, L2
            j .L3
        实验指导书上的方法是：
            bge	%1, %2, .L2
            j .L3
        如果采用实验指导书上的方法，那么需要先储存第一个指令（icmp那个），然后在br时再统一处理。
        另：框架中没有j，用jal x0 .L3 代替
    对于浮点数：
        bge、beq这种只能针对整数，因此浮点数采用第一个方法。
*/
struct CmpStruct {
    Register op1;    // 第一个操作数（寄存器）
    Register op2;    // 第二个操作数（寄存器）
    int cond;        // 比较条件（例如：等于、不等于、大于等）
    bool isfloat;    // 主要给zext那个函数进行指示
};
std::unordered_map<int, CmpStruct> reg2cmpInfo;

template <> void RiscV64Selector::ConvertAndAppend<IcmpInstruction *>(IcmpInstruction *ins) {
    // 获取操作数
    auto op1 = ins->GetOp1();
    auto op2 = ins->GetOp2();
    Register op1_reg, op2_reg;

    // 处理第一个操作数，只可能是reg（详见ir部分）
    if (op1->GetOperandType() == BasicOperand::REG) {
        op1_reg = GetRvReg(((RegOperand *)op1)->GetRegNo(), INT64);
    } else {
        ERROR("Unexpected ICMP op1 type");
    }

    // 处理第二个操作数
    if (op2->GetOperandType() == BasicOperand::IMMI32) {
        op2_reg = cur_func->GetNewReg(INT64);
        int imm_val = ((ImmI32Operand *)op2)->GetIntImmVal();
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, op2_reg, imm_val));
    } else if (op2->GetOperandType() == BasicOperand::REG) {
        op2_reg = GetRvReg(((RegOperand *)op2)->GetRegNo(), INT64);
    } else {
        ERROR("Unexpected ICMP op2 type");
    }

    // 将比较指令的信息保存下来，等待 brcond 处理
    auto result = ins->GetResult();
    CmpStruct cmp_info = {op1_reg, op2_reg, ins->GetCond(), false};
    reg2cmpInfo[((RegOperand *)result)->GetRegNo()] = cmp_info;
}

template <> void RiscV64Selector::ConvertAndAppend<FcmpInstruction *>(FcmpInstruction *ins) {
    auto op1 = ins->GetOp1();
    auto op2 = ins->GetOp2();
    Register op1_reg, op2_reg;

    // 处理第一个操作数，只可能是reg（详见ir部分）
    if (op1->GetOperandType() == BasicOperand::REG) {
        op1_reg = GetRvReg(((RegOperand *)op1)->GetRegNo(), FLOAT64);
    } else {
        ERROR("Unexpected FCMP op1 type");
    }

    // 处理第二个操作数
    if (op2->GetOperandType() == BasicOperand::IMMF32) {
        op2_reg = cur_func->GetNewReg(FLOAT64);
        float imm_val = ((ImmF32Operand *)op2)->GetFloatVal();
        uint32_t immr;
        memcpy(&immr, &imm_val, sizeof(float));
        Register temp_reg = cur_func->GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_reg, immr));
        cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, op2_reg, temp_reg));
    } else if (op2->GetOperandType() == BasicOperand::REG) {
        op2_reg = GetRvReg(((RegOperand *)op2)->GetRegNo(), FLOAT64);
    } else {
        ERROR("Unexpected FCMP op2 type");
    }

    // 用于保存结果的寄存器
    Register rd = cur_func->GetNewReg(INT64);

    // 根据浮点条件生成指令
    int cond = ins->GetCond();
    switch (cond) {
    case BasicInstruction::OEQ:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, rd, op1_reg, op2_reg));
        break;
    case BasicInstruction::OGT:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, rd, op1_reg, op2_reg));
        break;
    case BasicInstruction::OGE:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, rd, op1_reg, op2_reg));
        break;
    case BasicInstruction::OLT:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FLT_S, rd, op1_reg, op2_reg));
        break;
    case BasicInstruction::OLE:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FLE_S, rd, op1_reg, op2_reg));
        break;
    case BasicInstruction::ONE:
        cur_block->push_back(rvconstructor->ConstructR(RISCV_FEQ_S, rd, op1_reg, op2_reg));
        break;
    default:
        ERROR("Unexpected condition in FcmpInstruction");
    }
    // OEQ: 如果为真，rd是1，因此和0做ne比较；
    // 其余：如果为真，那么此条指令后，rd为0，因此和0做eq比较。
    auto brcond = (cond == BasicInstruction::OEQ) ? BasicInstruction::ne : BasicInstruction::eq;

    // 保存比较信息到 reg2cmpInfo
    auto result = ins->GetResult();
    CmpStruct cmp_info = {rd, GetPhysicalReg(RISCV_x0), brcond, true};
    reg2cmpInfo[((RegOperand *)result)->GetRegNo()] = cmp_info;
}

int GetRiscVCondOpcode(int cond) {
    switch (cond) {
    case BasicInstruction::eq:
        return RISCV_BEQ;
    case BasicInstruction::ne:
        return RISCV_BNE;
    case BasicInstruction::sle:
        return RISCV_BLE;
    case BasicInstruction::slt:
        return RISCV_BLT;
    case BasicInstruction::sge:
        return RISCV_BGE;
    case BasicInstruction::sgt:
        return RISCV_BGT;
    default:
        ERROR("Unexpected condition for BrCondInstruction");
        return -1;
    }
}

template <> void RiscV64Selector::ConvertAndAppend<BrCondInstruction *>(BrCondInstruction *ins) {
    // 获取已保存的比较信息
    auto cond_reg = (RegOperand *)ins->GetCond();
    CmpStruct cmp_info = reg2cmpInfo[cond_reg->GetRegNo()];

    // 获取 RISC-V 条件跳转指令的 opcode
    int opcode = GetRiscVCondOpcode(cmp_info.cond);

    // 构造两条指令
    cur_block->push_back(rvconstructor->ConstructBLabel(
    opcode, cmp_info.op1, cmp_info.op2, RiscVLabel(((LabelOperand *)(ins->GetTrueLabel()))->GetLabelNo())));

    cur_block->push_back(rvconstructor->ConstructJLabel(
    RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(((LabelOperand *)(ins->GetFalseLabel()))->GetLabelNo())));
}

template <> void RiscV64Selector::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *ins) {
    auto target_label = ((LabelOperand *)(ins->GetDestLabel()))->GetLabelNo();
    auto br_ins = rvconstructor->ConstructJLabel(RISCV_JAL, GetPhysicalReg(RISCV_x0), RiscVLabel(target_label));
    cur_block->push_back(br_ins);
}

template <> void RiscV64Selector::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *ins) {
    // 提取 Alloca 指令的结果操作数（即分配的指针寄存器）
    auto result_operand = (RegOperand *)ins->GetResult();
    int size = 1;
    for (auto d : ins->GetDims()) {
        size *= d;
    }
    // 一个元素占4字节
    int allocation_size = size << 2;
    reg2offset[result_operand->GetRegNo()] = cur_offset;
    // 更新当前栈的偏移量
    cur_offset += allocation_size;
}

template <> void RiscV64Selector::ConvertAndAppend<CallInstruction *>(CallInstruction *ins) {

    // void *memset(起始地址, 每字节初始化的值, 字节数);
    // memsetCall->push_back_Parameter(BasicInstruction::PTR, GetNewRegOperand(reg_now));
    // memsetCall->push_back_Parameter(BasicInstruction::I8, new ImmI32Operand(0x3f));
    // memsetCall->push_back_Parameter(BasicInstruction::I32, new ImmI32Operand(size * sizeof(int)));
    // memsetCall->push_back_Parameter(BasicInstruction::I1, new ImmI32Operand(0));
    if (ins->GetFunctionName() == "llvm.memset.p0.i32") {
        auto paras = ins->GetParameterList();
        // 参数1：起始地址
        int ptrRegNo = ((RegOperand *)paras[0].second)->GetRegNo();
        auto offset_on_stack = reg2offset[ptrRegNo];

        if (offset_on_stack > 2047 || offset_on_stack < -2048) {
            Register temp = cur_func->GetNewReg(INT64);
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp, offset_on_stack));
            auto ld_alloca =
            rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0), GetPhysicalReg(RISCV_sp), temp);
            cur_block->push_back(ld_alloca);
            ((RiscV64Function *)cur_func)->allocalist.push_back(ld_alloca);

        } else {
            auto ld_alloca = rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a0),
                                                          GetPhysicalReg(RISCV_sp), offset_on_stack);
            cur_block->push_back(ld_alloca);
            ((RiscV64Function *)cur_func)->allocalist.push_back(ld_alloca);
        }

        // 参数2：填充值
        auto imm_op = (ImmI32Operand *)(paras[1].second);
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a1), imm_op->GetIntImmVal()));

        // 参数3：size
        assert(paras[2].second->GetOperandType() == BasicOperand::IMMI32);
        int size = ((ImmI32Operand *)paras[2].second)->GetIntImmVal();
        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a2), size));

        // 生成call指令
        cur_block->push_back(rvconstructor->ConstructCall(RISCV_CALL, "memset", 3, 0));
        return;
    }

    int int_reg_used = 0;         // 已使用的整数参数寄存器数量
    int float_reg_used = 0;       // 已使用的浮点参数寄存器数量
    int stack_param_count = 0;    // 使用栈传递的参数数量
    int stack_offset = 0;         // 栈偏移量
    // 遍历指令的参数列表
    for (auto &param : ins->GetParameterList()) {
        auto param_type = param.first;        // 参数类型
        auto param_operand = param.second;    // 参数操作数
        assert(param_operand->GetOperandType() == BasicOperand::REG);
        auto regOperandNo = ((RegOperand *)param_operand)->GetRegNo();    // llvmir寄存器号

        if (param_type == BasicInstruction::I32 || param_type == BasicInstruction::PTR) {
            // 如果整数寄存器未超出限制，将参数分配到 a0~a7
            if (int_reg_used < 8) {
                // 参数在寄存器中，直接将其拷贝到目标寄存器
                if (reg2offset.find(regOperandNo) == reg2offset.end()) {
                    auto copy_instr = rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a0 + int_reg_used),
                                                                   GetRvReg(regOperandNo, INT64), 0);
                    cur_block->push_back(copy_instr);
                }
                // 参数在栈中，根据偏移量加载到目标寄存器
                else {
                    auto stack_offset = reg2offset[regOperandNo];
                    if (stack_offset > 2047 || stack_offset < -2048) {
                        Register temp_reg = cur_func->GetNewReg(INT64);
                        cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_reg, stack_offset));
                        auto load_instr = rvconstructor->ConstructR(RISCV_ADD, GetPhysicalReg(RISCV_a0 + int_reg_used),
                                                                    GetPhysicalReg(RISCV_sp), temp_reg);
                        cur_block->push_back(load_instr);
                    } else {
                        auto load_instr = rvconstructor->ConstructIImm(
                        RISCV_ADDI, GetPhysicalReg(RISCV_a0 + int_reg_used), GetPhysicalReg(RISCV_sp), stack_offset);
                        cur_block->push_back(load_instr);
                    }
                }
            }
            // 如果寄存器已用完，将参数存放到栈中
            else {
                // SD reg, stack_offset(sp)
                // 把 reg 寄存器的值存储到当前栈指针（sp）加上 stack_offset 的地址处。
                auto reg = GetRvReg(regOperandNo, INT64);
                cur_block->push_back(
                rvconstructor->ConstructSImm(RISCV_SD, reg, GetPhysicalReg(RISCV_sp), stack_offset));
                stack_offset += 8;
                stack_param_count++;
            }
            int_reg_used++;
        }
        // 浮点参数
        else {
            if (float_reg_used < 8) {
                cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_S, GetPhysicalReg(RISCV_fa0 + float_reg_used),
                                                                GetRvReg(regOperandNo, FLOAT64)));
            } else {
                cur_block->push_back(rvconstructor->ConstructSImm(RISCV_FSD, GetRvReg(regOperandNo, FLOAT64),
                                                                  GetPhysicalReg(RISCV_sp), stack_offset));
                stack_offset += 8;
                stack_param_count++;
            }
            float_reg_used++;
        }
    }

    // 生成函数调用指令
    auto function_name = ins->GetFunctionName();
    cur_block->push_back(
    rvconstructor->ConstructCall(RISCV_CALL, function_name, std::min(int_reg_used, 8), std::min(float_reg_used, 8)));

    // 更新栈的大小，每个参数在栈上的存储位置需要占用 8字节 (64位) 的对齐单位
    cur_func->UpdateParaSize(stack_param_count * 8);

    // 处理函数返回值
    auto return_type = ins->GetReturnType();
    auto resultOperandNo = ins->GetResultRegNo();
    if (return_type == BasicInstruction::I32) {
        cur_block->push_back(
        rvconstructor->ConstructIImm(RISCV_ADDI, GetRvReg(resultOperandNo, INT64), GetPhysicalReg(RISCV_a0), 0));
    } else if (return_type == BasicInstruction::FLOAT32) {
        cur_block->push_back(
        rvconstructor->ConstructR2(RISCV_FMV_S, GetRvReg(resultOperandNo, FLOAT64), GetPhysicalReg(RISCV_fa0)));
    } else if (return_type == BasicInstruction::VOID) {
        // Do nothing for void return type
    }
}

template <> void RiscV64Selector::ConvertAndAppend<RetInstruction *>(RetInstruction *ins) {
    if (ins->GetRetVal() != NULL) {
        // 返回值是整数
        if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMI32) {
            auto retimm_op = (ImmI32Operand *)ins->GetRetVal();
            auto imm = retimm_op->GetIntImmVal();

            // 使用 LI 指令将立即数加载到 a0 寄存器作为返回值
            auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a0), imm);
            cur_block->push_back(retcopy_instr);
        }
        // 返回值是浮点数
        else if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMF32) {
            auto retimm_op = (ImmF32Operand *)ins->GetRetVal();
            float imm_val = retimm_op->GetFloatVal();
            uint32_t immr;
            memcpy(&immr, &imm_val, sizeof(float));
            Register temp_reg = cur_func->GetNewReg(INT64);
            // LI + FMV.W.X
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_reg, immr));
            cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_W_X, GetPhysicalReg(RISCV_fa0), temp_reg));
        }
        // 返回值是寄存器
        else if (ins->GetRetVal()->GetOperandType() == BasicOperand::REG) {
            auto retRegNo = ((RegOperand *)ins->GetRetVal())->GetRegNo();
            if (ins->GetType() == BasicInstruction::FLOAT32) {
                auto retreg = GetRvReg(retRegNo, FLOAT64);
                cur_block->push_back(rvconstructor->ConstructR2(RISCV_FMV_S, GetPhysicalReg(RISCV_fa0), retreg));
            } else {
                auto retreg = GetRvReg(retRegNo, FLOAT64);
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, GetPhysicalReg(RISCV_a0), retreg, 0));
            }
        }
    }

    // 构造返回指令 JALR x0, ra, 0
    auto ret_instr = rvconstructor->ConstructIImm(RISCV_JALR, GetPhysicalReg(RISCV_x0), GetPhysicalReg(RISCV_ra), 0);

    // 根据函数返回类型设置标志（1: int, 2: float, 0: void）
    if (ins->GetType() == BasicInstruction::I32) {
        ret_instr->setRetType(1);
    } else if (ins->GetType() == BasicInstruction::FLOAT32) {
        ret_instr->setRetType(2);
    } else {
        ret_instr->setRetType(0);
    }

    // 添加返回指令到当前基本块
    cur_block->push_back(ret_instr);
}

template <> void RiscV64Selector::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *ins) {
    auto src_op = (RegOperand *)ins->GetSrc();
    auto dest_op = (RegOperand *)ins->GetResult();
    auto src_reg = GetRvReg(src_op->GetRegNo(), FLOAT64);
    auto dest_reg = GetRvReg(dest_op->GetRegNo(), INT64);

    // 使用 RISC-V 的浮点到整数转换指令
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FCVT_W_S, dest_reg, src_reg));
}

template <> void RiscV64Selector::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *ins) {
    auto src_op = (RegOperand *)ins->GetSrc();
    auto dest_op = (RegOperand *)ins->GetResult();
    auto src_reg = GetRvReg(src_op->GetRegNo(), INT64);
    auto dest_reg = GetRvReg(dest_op->GetRegNo(), FLOAT64);

    // 使用 RISC-V 的整数到浮点转换指令
    cur_block->push_back(rvconstructor->ConstructR2(RISCV_FCVT_S_W, dest_reg, src_reg));
}

/*
    什么时候会出现zext？
    先以整型为例：
        %r4 = icmp eq i32 %r3,0
        %r5 = zext i1 %r4 to i32
    注意到：zext的%r4一定来源于一个eq比较语句，且第二个op一定是0。
    因此只需要让%r3和0作比较，直接给%r5赋值。

    对于浮点数：
        %r4 = fcmp oeq float %r3,0x0
        %r5 = zext i1 %r4 to i32
    在处理fcmp时，会生成如下指令：
        LI x5, 0x00000000          # 将 0.0 的 IEEE-754 表示加载到 x5
        FMV.W.X f2, x5             # 将 x5 的值移动到浮点寄存器 f2
        FEQ.S x6, f1, f2           # 如果 f1 == f2，则 x6 = 1，否则 x6 = 0
    因此，我们只需要将 %r5 与 rd建立联系即可
*/
template <> void RiscV64Selector::ConvertAndAppend<ZextInstruction *>(ZextInstruction *ins) {
    // 获取指令的目的寄存器和源寄存器
    auto result = ins->GetResult();
    auto src = ins->GetSrc();
    auto result_op = (RegOperand *)result;
    auto src_op = (RegOperand *)src;
    auto result_reg = GetRvReg(result_op->GetRegNo(), INT32);
    auto src_reg = GetRvReg(src_op->GetRegNo(), INT32);

    // 从reg2cmpInfo中获取源寄存器的比较信息（op1）
    CmpStruct cmp_info = reg2cmpInfo[src_op->GetRegNo()];

    if (cmp_info.isfloat) {
        // 如果是浮点型，将结果寄存器直接映射到 cmp_info 的 rd
        irReg2rvReg[result_op->GetRegNo()] = cmp_info.op1;
    } else {
        // 如果是整型比较，生成 SLTIU 指令，将 op1_reg 与 0 进行比较（无符号小于1）
        auto op1_reg = cmp_info.op1;
        auto sltiu_instr = rvconstructor->ConstructIImm(RISCV_SLTIU, result_reg, op1_reg, 1);    // op1_reg == 0
        cur_block->pop_back();    // 把li op2reg  0给删了
        cur_block->push_back(sltiu_instr);
    }
}

/*
    int a[][5]   dim: {5}
    a[2][5] -> index {2，5}
    -----------------------
    int a[3][5]  dim: {3, 5}
    a[2] -> indexs {0, 2}
    a[1][2] -> indexs {0,1,2}

    访问从ptr开始的一个数组，数组的维度为dim，索引的维度储存在indexs（其中第一个索引为跳过整个数组的值）中。
*/
template <> void RiscV64Selector::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *ins) {
    // ins->PrintIR(std::cout);
    size_t index = 0;
    // 1、数组维度计算
    auto dims = ins->GetDims();
    int arraysize = 1;
    for (int i = 0; i < dims.size(); i++)
        arraysize *= dims[i];

    int assigned = 0;
    int i = 0;

    // 2、索引计算，最终偏移量存在offsetReg中（先以元素为单位，后以字节为单位）
    auto indexs = ins->GetIndexes();
    int offset = 0;    // offset初始化为0,最后储存的是imm型index的总offset
    auto offsetReg =
    cur_func->GetNewReg(INT64);    // offsetReg初始化为0, 先储存的是reg型index的总offset，最后是全部总的offset
    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, offsetReg, GetPhysicalReg(RISCV_x0), 0));
    for (int i = 0; i < indexs.size(); i++) {
        // imm
        if (indexs[i]->GetOperandType() == BasicOperand::IMMI32) {
            int imm_value = ((ImmI32Operand *)ins->GetIndexes()[i])->GetIntImmVal();
            offset += imm_value * arraysize;
        }
        // reg
        else {
            auto index_op = (RegOperand *)indexs[i];
            auto indexReg = GetRvReg(index_op->GetRegNo(), INT64);
            auto tempOffsetReg = cur_func->GetNewReg(INT64);
            auto arraysizeReg = cur_func->GetNewReg(INT64);

            // offset += index * arraysize
            cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, arraysizeReg, arraysize));
            cur_block->push_back(rvconstructor->ConstructR(RISCV_MUL, tempOffsetReg, indexReg, arraysizeReg));
            cur_block->push_back(rvconstructor->ConstructR(RISCV_ADD, offsetReg, offsetReg, tempOffsetReg));
        }
        // 更新arraysize
        if (i < ins->GetDims().size())
            arraysize /= ins->GetDims()[i];
    }

    // offsetReg = offset + offsetReg ->  *4 变为字节
    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, offsetReg, offsetReg, offset));
    cur_block->push_back(rvconstructor->ConstructIImm(RISCV_SLLI, offsetReg, offsetReg, 2));

    // 3、获取基地址
    auto baseReg = cur_func->GetNewReg(INT64);    // 存储基地址
    if (ins->GetPtrVal()->GetOperandType() == BasicOperand::REG) {
        auto ptrOp = (RegOperand *)ins->GetPtrVal();

        if (reg2offset.find(ptrOp->GetRegNo()) != reg2offset.end()) {
            auto offset_on_stack = reg2offset[ptrOp->GetRegNo()];
            if (offset_on_stack > 2047 || offset_on_stack < -2048) {
                Register temp = cur_func->GetNewReg(INT64);
                cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp, offset_on_stack));
                auto ld_alloca = rvconstructor->ConstructR(RISCV_ADD, baseReg, GetPhysicalReg(RISCV_sp), temp);
                cur_block->push_back(ld_alloca);
                ((RiscV64Function *)cur_func)->allocalist.push_back(ld_alloca);
            } else {
                auto ld_alloca =
                rvconstructor->ConstructIImm(RISCV_ADDI, baseReg, GetPhysicalReg(RISCV_sp), offset_on_stack);
                cur_block->push_back(ld_alloca);
                ((RiscV64Function *)cur_func)->allocalist.push_back(ld_alloca);
            }
        }
        // 数组是参数
        else
            baseReg = GetRvReg(ptrOp->GetRegNo(), INT64);
    } else if (ins->GetPtrVal()->GetOperandType() == BasicOperand::GLOBAL) {
        auto ptrOp = (GlobalOperand *)ins->GetPtrVal();
        auto basehiReg = cur_func->GetNewReg(INT64);
        cur_block->push_back(rvconstructor->ConstructULabel(RISCV_LUI, basehiReg, RiscVLabel(ptrOp->GetName(), true)));
        cur_block->push_back(
        rvconstructor->ConstructILabel(RISCV_ADDI, baseReg, basehiReg, RiscVLabel(ptrOp->GetName(), false)));
    }

    // 4、最终地址 = 基地址 + 字节偏移
    auto resultOp = (RegOperand *)ins->GetResult();
    auto resultReg = GetRvReg(resultOp->GetRegNo(), INT64);
    cur_block->push_back(rvconstructor->ConstructR(RISCV_ADD, resultReg, baseReg, offsetReg));
}

// Reference: https://github.com/yuhuifishash/SysY
// phi syntax:
// <result>=phi <ty> [val1,label1],[val2,label2],……
// 每一个pair里，第一个是label，第二个是val

// template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
//     // 确保Phi指令的结果操作数是一个寄存器操作数
//     assert(ins->GetResult()->GetOperandType() == BasicOperand::REG);

//     // 分配目的寄存器
//     auto resulstOp = (RegOperand *)ins->GetResult();
//     Register resultReg;
//     if (ins->GetDataType() == BasicInstruction::I32) {
//         resultReg = GetRvReg(resulstOp->GetRegNo(), INT64);
//     } else if (ins->GetDataType() == BasicInstruction::FLOAT32) {
//         resultReg = GetRvReg(resulstOp->GetRegNo(), FLOAT64);
//     }

//     auto phimInst = new MachinePhiInstruction(resultReg);

//     // 遍历Phi指令的phi列表（label和对应值的映射）
//     for (auto [label, val] : ins->GetPhiList()) {
//         auto labelOp = (LabelOperand *)label;
//         auto regOp = (RegOperand *)val;
//         if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::I32) {
//             auto valReg = GetRvReg(regOp->GetRegNo(), INT64);
//             phimInst->pushPhiList(labelOp->GetLabelNo(), valReg);
//         } else if (val->GetOperandType() == BasicOperand::REG && ins->GetDataType() == BasicInstruction::FLOAT32) {
//             auto valReg = GetRvReg(regOp->GetRegNo(), FLOAT64);
//             phimInst->pushPhiList(labelOp->GetLabelNo(), valReg);
//         } else {
//             ERROR("Unexpected OperandType");
//         }
//     }
//     cur_block->push_back(phimInst);
// }

std::unordered_map<int, std::vector<RiscV64Instruction *>> block_insert_map;

template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
    // 分配目的寄存器
    auto resulstOp = (RegOperand *)ins->GetResult();
    Register resultReg;
    if (ins->GetDataType() == BasicInstruction::I32) {
        resultReg = GetRvReg(resulstOp->GetRegNo(), INT64);
    } else if (ins->GetDataType() == BasicInstruction::FLOAT32) {
        resultReg = GetRvReg(resulstOp->GetRegNo(), FLOAT64);
    }

    // 遍历 Phi 指令的 [label, val] 对
    for (auto [label, val] : ins->GetPhiList()) {
        auto labelOp = (LabelOperand *)label;
        int label_id = labelOp->GetLabelNo();
        auto regOp = (RegOperand *)val;
        Register valReg;
        if (ins->GetDataType() == BasicInstruction::I32) {
            valReg = GetRvReg(regOp->GetRegNo(), INT64);
            RiscV64Instruction *inst =
            rvconstructor->ConstructR(RISCV_ADD, resultReg, valReg, GetPhysicalReg(RISCV_x0));
            block_insert_map[label_id].push_back(inst);
        } else if (ins->GetDataType() == BasicInstruction::FLOAT32) {
            valReg = GetRvReg(regOp->GetRegNo(), FLOAT64);
            RiscV64Instruction *inst = rvconstructor->ConstructR2(RISCV_FMV_S, resultReg, valReg);
            block_insert_map[label_id].push_back(inst);
        } else {
            ERROR("Unexpected OperandType");
        }
    }
}

template <> void RiscV64Selector::ConvertAndAppend<Instruction>(Instruction inst) {
    switch (inst->GetOpcode()) {
    case BasicInstruction::LOAD:
        ConvertAndAppend<LoadInstruction *>((LoadInstruction *)inst);
        break;
    case BasicInstruction::STORE:
        ConvertAndAppend<StoreInstruction *>((StoreInstruction *)inst);
        break;
    case BasicInstruction::ADD:
    case BasicInstruction::SUB:
    case BasicInstruction::MUL:
    case BasicInstruction::DIV:
    case BasicInstruction::FADD:
    case BasicInstruction::FSUB:
    case BasicInstruction::FMUL:
    case BasicInstruction::FDIV:
    case BasicInstruction::MOD:
    case BasicInstruction::SHL:
    case BasicInstruction::BITXOR:
        // shl、bitxor前面根本没实现，可以不管
        ConvertAndAppend<ArithmeticInstruction *>((ArithmeticInstruction *)inst);
        break;
    case BasicInstruction::ICMP:
        ConvertAndAppend<IcmpInstruction *>((IcmpInstruction *)inst);
        break;
    case BasicInstruction::FCMP:
        ConvertAndAppend<FcmpInstruction *>((FcmpInstruction *)inst);
        break;
    case BasicInstruction::ALLOCA:
        ConvertAndAppend<AllocaInstruction *>((AllocaInstruction *)inst);
        break;
    case BasicInstruction::BR_COND:
        ConvertAndAppend<BrCondInstruction *>((BrCondInstruction *)inst);
        break;
    case BasicInstruction::BR_UNCOND:
        ConvertAndAppend<BrUncondInstruction *>((BrUncondInstruction *)inst);
        break;
    case BasicInstruction::RET:
        ConvertAndAppend<RetInstruction *>((RetInstruction *)inst);
        break;
    case BasicInstruction::ZEXT:
        ConvertAndAppend<ZextInstruction *>((ZextInstruction *)inst);
        break;
    case BasicInstruction::FPTOSI:
        ConvertAndAppend<FptosiInstruction *>((FptosiInstruction *)inst);
        break;
    case BasicInstruction::SITOFP:
        ConvertAndAppend<SitofpInstruction *>((SitofpInstruction *)inst);
        break;
    case BasicInstruction::GETELEMENTPTR:
        ConvertAndAppend<GetElementptrInstruction *>((GetElementptrInstruction *)inst);
        break;
    case BasicInstruction::CALL:
        ConvertAndAppend<CallInstruction *>((CallInstruction *)inst);
        break;
    case BasicInstruction::PHI:
        ConvertAndAppend<PhiInstruction *>((PhiInstruction *)inst);
        break;
    default:
        ERROR("Unknown LLVM IR instruction");
    }
}

void RiscV64Selector::SelectInstructionAndBuildCFG() {
    // 与中间代码生成一样, 如果你完全无从下手, 可以先看看输出是怎么写的
    // 即riscv64gc/instruction_print/*  common/machine_passes/machine_printer.h
    // 指令选择除了一些函数调用约定必须遵守的情况需要物理寄存器，其余情况必须均为虚拟寄存器

    dest->global_def = IR->global_def;

    // 遍历每个LLVM IR函数，每个有指令列表和cfg两个属性
    for (auto [defI, cfg] : IR->llvm_cfg) {
        if (cfg == nullptr) {
            ERROR("LLVMIR CFG is Empty, you should implement BuildCFG in MidEnd first");
        }

        // 1、设置cur_func的相关信息
        // 新建：RiscV64Function、MachineCFG
        // 可以使用cur_func->GetNewRegister来获取新的虚拟寄存器
        std::string name = cfg->function_def->GetFunctionName();
        cur_func = new RiscV64Function(name);
        cur_func->SetParent(dest);
        dest->functions.push_back(cur_func);    // 将新建的函数对象加入目标函数列表。

        // 创建并关联 MachineCFG（用于记录低级指令控制流图）
        auto cur_mcfg = new MachineCFG;
        cur_func->SetMachineCFG(cur_mcfg);

        // 清空指令选择状态
        ClearFunctionSelectState();

        // 2、添加函数参数(推荐先阅读一下riscv64_lowerframe.cc中的代码和注释)
        // See MachineFunction::AddParameter()
        for (int i = 0; i < defI->formals.size(); i++) {
            MachineDataType type;
            // 判断参数的LLVM类型并映射到对应的RISC-V数据类型
            if (defI->formals[i] == BasicInstruction::I32 || defI->formals[i] == BasicInstruction::PTR)
                type = INT64;
            else if (defI->formals[i] == BasicInstruction::FLOAT32)
                type = FLOAT64;
            else
                ERROR("Unknown llvm type");
            cur_func->AddParameter(GetRvReg(((RegOperand *)defI->formals_reg[i])->GetRegNo(), type));
        }

        // 3、遍历当前函数的每个block
        for (auto [id, block] : *(cfg->block_map)) {
            cur_block = new RiscV64Block(id);
            // 将新块添加到Machine CFG中
            cur_mcfg->AssignEmptyNode(id, cur_block);
            cur_func->UpdateMaxLabel(id);

            cur_block->setParent(cur_func);
            cur_func->blocks.push_back(cur_block);

            // 4、遍历每条指令，进行指令选择, 请注意指令选择时需要维护变量cur_offset
            for (auto instruction : block->Instruction_list) {
                // instruction->PrintIR(std::cout);
                ConvertAndAppend<Instruction>(instruction);
            }
        }

        // 处理phi指令
        for (auto &block : cur_func->blocks) {
            int block_id = block->getLabelId();
            if (block_insert_map.find(block_id) != block_insert_map.end()) {
                auto &instr_list = block_insert_map[block_id];
                auto last_instr = block->back();
                block->pop_back();
                for (auto &instr : instr_list) {
                    block->push_back(instr);
                }
                if (last_instr) {
                    block->push_back(last_instr);
                }
            }
        }

        // RISCV 8字节对齐（）
        if (cur_offset % 8 != 0) {
            cur_offset = ((cur_offset + 7) / 8) * 8;
        }
        cur_func->SetStackSize(cur_offset + cur_func->GetParaSize());
        // Reference: https://github.com/yuhuifishash/SysY
        ((RiscV64Function *)cur_func)->AddParameterSize(cur_func->GetParaSize());

        // 控制流图连边
        for (int i = 0; i < cfg->G.size(); i++) {
            const auto &arcs = cfg->G[i];
            for (auto arc : arcs) {
                cur_mcfg->MakeEdge(i, arc->block_id);
            }
        }
    }
}

void RiscV64Selector::ClearFunctionSelectState() {
    cur_offset = 0;
    irReg2rvReg.clear();
    reg2offset.clear();
    reg2cmpInfo.clear();
    block_insert_map.clear();
}

Register RiscV64Selector::GetRvReg(int irRegNo, MachineDataType type) {
    // std::cout << "GetRvReg" << std::endl;
    if (irReg2rvReg.find(irRegNo) == irReg2rvReg.end())
        irReg2rvReg[irRegNo] = cur_func->GetNewRegister(type.data_type, type.data_length);
    return irReg2rvReg[irRegNo];
}