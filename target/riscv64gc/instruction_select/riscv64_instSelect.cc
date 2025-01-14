#include "riscv64_instSelect.h"
#include <sstream>

template <> void RiscV64Selector::ConvertAndAppend<LoadInstruction *>(LoadInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<StoreInstruction *>(StoreInstruction *ins) {
    TODO("Implement this if you need");
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
        else if (op1->GetOperandType() == BasicOperand::REG && op1->GetOperandType() == BasicOperand::REG) {
            auto *rs1_op = (RegOperand *)ins->GetOperand1();
            auto *rs2_op = (RegOperand *)ins->GetOperand2();
            auto rs1 = GetRvReg(rs1_op->GetRegNo(), reg_type);
            auto rs2 = GetRvReg(rs2_op->GetRegNo(), reg_type);

            int opcode_instr = GetOpcodeForArithmetic(ins->GetOpcode());
            cur_block->push_back(rvconstructor->ConstructR(opcode_instr, rd, rs1, rs2));

        }
        // 情况3：立即数 op 寄存器
        else if (op1->GetOperandType() == imm_type && op1->GetOperandType() == BasicOperand::REG) {
            auto *imm_op = (ImmI32Operand *)op1;
            auto imm = imm_op->GetIntImmVal();

            auto *rs_op = (RegOperand *)op2;
            auto rs = GetRvReg(rs_op->GetRegNo(), reg_type);    // 获取寄存器号
            // ADD 或 SUB 操作，并且立即数在范围内  -> ADDI
            if ((opcode == BasicInstruction::ADD || opcode == BasicInstruction::SUB) && (imm <= 2047 && imm >= -2048)) {
                imm = opcode == BasicInstruction::ADD ? imm : -imm;
                cur_block->push_back(rvconstructor->ConstructIImm(RISCV_ADDI, rd, rs, imm));
            } else {
                // li temp_reg, imm
                // opcode rd, rs, temp_reg
                Register temp_reg = cur_func->GetNewReg(reg_type);    // 获取新的临时寄存器
                cur_block->push_back(rvconstructor->ConstructUImm(RISCV_LI, temp_reg, imm));
                cur_block->push_back(rvconstructor->ConstructR(GetOpcodeForArithmetic(opcode), rd, rs, temp_reg));
            }
        } else {
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

        } else if (op1->GetOperandType() == BasicOperand::REG && op1->GetOperandType() == BasicOperand::REG) {
            auto *rs1_op = (RegOperand *)op1;
            auto *rs2_op = (RegOperand *)op2;

            auto rs1 = GetRvReg(rs1_op->GetRegNo(), FLOAT64);
            auto rs2 = GetRvReg(rs2_op->GetRegNo(), FLOAT64);

            int opcode_instr = GetOpcodeForArithmetic(opcode);
            cur_block->push_back(rvconstructor->ConstructR(opcode_instr, rd, rs1, rs2));
        } else if (op1->GetOperandType() == imm_type && op1->GetOperandType() == BasicOperand::REG) {
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
    CmpStruct cmp_info = {op1_reg, op2_reg, ins->GetCond()};
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
    CmpStruct cmp_info = {rd, GetPhysicalReg(RISCV_x0), brcond};
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

template <> void RiscV64Selector::ConvertAndAppend<AllocaInstruction *>(AllocaInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<BrUncondInstruction *>(BrUncondInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<CallInstruction *>(CallInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<RetInstruction *>(RetInstruction *ins) {
    if (ins->GetRetVal() != NULL) {
        if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMI32) {
            auto retimm_op = (ImmI32Operand *)ins->GetRetVal();
            auto imm = retimm_op->GetIntImmVal();

            auto retcopy_instr = rvconstructor->ConstructUImm(RISCV_LI, GetPhysicalReg(RISCV_a0), imm);
            cur_block->push_back(retcopy_instr);
        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::IMMF32) {
            TODO("Implement this if you need");
        } else if (ins->GetRetVal()->GetOperandType() == BasicOperand::REG) {
            TODO("Implement this if you need");
        }
    }

    auto ret_instr = rvconstructor->ConstructIImm(RISCV_JALR, GetPhysicalReg(RISCV_x0), GetPhysicalReg(RISCV_ra), 0);
    if (ins->GetType() == BasicInstruction::I32) {
        ret_instr->setRetType(1);
    } else if (ins->GetType() == BasicInstruction::FLOAT32) {
        ret_instr->setRetType(2);
    } else {
        ret_instr->setRetType(0);
    }
    cur_block->push_back(ret_instr);
}

template <> void RiscV64Selector::ConvertAndAppend<FptosiInstruction *>(FptosiInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<SitofpInstruction *>(SitofpInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<ZextInstruction *>(ZextInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<GetElementptrInstruction *>(GetElementptrInstruction *ins) {
    TODO("Implement this if you need");
}

template <> void RiscV64Selector::ConvertAndAppend<PhiInstruction *>(PhiInstruction *ins) {
    TODO("Implement this if you need");
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

        // 清空指令选择状态(可能需要自行添加初始化操作)
        ClearFunctionSelectState();

        // 2、添加函数参数(推荐先阅读一下riscv64_lowerframe.cc中的代码和注释)
        // See MachineFunction::AddParameter()

        // 遍历当前函数的每个block
        for (auto [id, block] : *(cfg->block_map)) {
            cur_block = new RiscV64Block(id);
            // 将新块添加到Machine CFG中
            cur_mcfg->AssignEmptyNode(id, cur_block);
            cur_func->UpdateMaxLabel(id);

            cur_block->setParent(cur_func);
            cur_func->blocks.push_back(cur_block);

            // 指令选择主要函数, 请注意指令选择时需要维护变量cur_offset
            for (auto instruction : block->Instruction_list) {
                // Log("Selecting Instruction");
                ConvertAndAppend<Instruction>(instruction);
            }
        }

        // RISCV 8字节对齐（）
        if (cur_offset % 8 != 0) {
            cur_offset = ((cur_offset + 7) / 8) * 8;
        }
        cur_func->SetStackSize(cur_offset + cur_func->GetParaSize());

        // 控制流图连边
        for (int i = 0; i < cfg->G.size(); i++) {
            const auto &arcs = cfg->G[i];
            for (auto arc : arcs) {
                cur_mcfg->MakeEdge(i, arc->block_id);
            }
        }
    }
}

void RiscV64Selector::ClearFunctionSelectState() { cur_offset = 0; }

Register RiscV64Selector::GetRvReg(int irRegNo, MachineDataType type) {
    // std::cout << "GetRvReg" << std::endl;
    if (irReg2rvReg.find(irRegNo) == irReg2rvReg.end())
        irReg2rvReg[irRegNo] = cur_func->GetNewRegister(type.data_type, type.data_length);
    return irReg2rvReg[irRegNo];
}
