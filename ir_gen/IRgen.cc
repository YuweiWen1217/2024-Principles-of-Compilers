#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern std::vector<std::string> debug_msgs;

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

// Type->LLVMType
// Type：VOID = 0, INT = 1, FLOAT = 2, BOOL = 3, PTR = 4, DOUBLE = 5
BasicInstruction::LLVMType Type2LLvm[6] = {BasicInstruction::LLVMType::VOID,    BasicInstruction::LLVMType::I32,
                                           BasicInstruction::LLVMType::FLOAT32, BasicInstruction::LLVMType::I1,
                                           BasicInstruction::LLVMType::PTR,     BasicInstruction::LLVMType::DOUBLE};

int reg_now = -1;
int label_now = 0;
int label_max = -1;
FuncDefInstruction func_now;

IRgenTable irgen_table;    // 中间代码生成的辅助变量
LLVMIR llvmIR;             // 我们需要在这个变量中生成中间代码

void AddLibFunctionDeclare();

// 在基本块B末尾生成一条新指令
void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg);
void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg);
void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg);
void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg);

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg);
void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg);

void IRgenFptosi(LLVMBlock B, int src, int dst);
void IRgenSitofp(LLVMBlock B, int src, int dst);
void IRgenZextI1toI32(LLVMBlock B, int src, int dst);

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                                std::vector<int> dims, std::vector<Operand> indexs);

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                                std::vector<int> dims, std::vector<Operand> indexs);

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr);

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);
void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name);
void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name);

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val);
void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val);
void IRgenRetVoid(LLVMBlock B);

void IRgenBRUnCond(LLVMBlock B, int dst_label);
void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label);

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims);

RegOperand *GetNewRegOperand(int RegNo);

// generate TypeConverse Instructions from type_src to type_dst
// eg. you can use fptosi instruction to converse float to int
// eg. you can use zext instruction to converse bool to int

void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src) {
    debug_msgs.push_back("IRgenTypeConverse: Handling type conversion");
    if (type_src ==
        type_dst)    // 由于有这种情况的存在，参数中不能直接传入dst(传入值为++reg_now)，不然这个新的寄存器是空的
        return;
    int dst = ++reg_now;
    if (type_src == Type::BOOL) {
        IRgenZextI1toI32(B, src, dst);
        if (type_dst == Type::FLOAT)
            // 调用IRgenTypeConverse时，dst为++reg_now，因此需要保证最终结果在reg_now里，而不是传入的dst里
            IRgenSitofp(B, dst, ++reg_now);
        return;
    }
    if (type_src == Type::INT && type_dst == Type::BOOL)
        IRgenIcmpImmRight(B, BasicInstruction::ne, src, 0, dst);
    else if (type_src == Type::INT && type_dst == Type::FLOAT)
        IRgenSitofp(B, src, dst);
    else if (type_src == Type::FLOAT && type_dst == Type::INT)
        IRgenFptosi(B, src, dst);
    else if (type_src == Type::FLOAT && type_dst == Type::BOOL)
        IRgenFcmpImmRight(B, BasicInstruction::ONE, src, 0, dst);
    else
        assert(false);
}

void BasicBlock::InsertInstruction(int pos, Instruction Ins) {
    assert(pos == 0 || pos == 1);
    if (pos == 0) {
        Instruction_list.push_front(Ins);
    } else if (pos == 1) {
        Instruction_list.push_back(Ins);
    }
}

/*
二元运算指令生成的伪代码：
    假设现在的语法树节点是：AddExp_plus
    该语法树表示 addexp + mulexp

    addexp->codeIR()
    mulexp->codeIR()
    假设mulexp生成完后，我们应该在基本块B0继续插入指令。
    addexp的结果存储在r0寄存器中，mulexp的结果存储在r1寄存器中
    生成一条指令r2 = r0 + r1，并将该指令插入基本块B0末尾。
    标注后续应该在基本块B0插入指令，当前节点的结果寄存器为r2。
    (如果考虑支持浮点数，需要查看语法树节点的类型来判断此时是否需要隐式类型转换)
*/

/*
while语句指令生成的伪代码：
    while的语法树节点为while(cond)stmt

    假设当前我们应该在B0基本块开始插入指令
    新建三个基本块Bcond，Bbody，Bend
    在B0基本块末尾插入一条无条件跳转指令，跳转到Bcond

    设置当前我们应该在Bcond开始插入指令
    cond->codeIR()    //在调用该函数前你可能需要设置真假值出口
    假设cond生成完后，我们应该在B1基本块继续插入指令，Bcond的结果为r0
    如果r0的类型不为bool，在B1末尾生成一条比较语句，比较r0是否为真。
    在B1末尾生成一条条件跳转语句，如果为真，跳转到Bbody，如果为假，跳转到Bend

    设置当前我们应该在Bbody开始插入指令
    stmt->codeIR()
    假设当stmt生成完后，我们应该在B2基本块继续插入指令
    在B2末尾生成一条无条件跳转语句，跳转到Bcond

    设置当前我们应该在Bend开始插入指令
*/

void __Program::codeIR() {
    debug_msgs.push_back("Program CodeIR");
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() {
    debug_msgs.push_back("Exp CodeIR");
    addexp->codeIR();
}

BasicInstruction::LLVMIROpcode opint2float(BasicInstruction::LLVMIROpcode opcode) {
    switch (opcode) {
    case BasicInstruction::ADD:
        return BasicInstruction::FADD;
    case BasicInstruction::SUB:
        return BasicInstruction::FSUB;
    case BasicInstruction::MUL:
        return BasicInstruction::FMUL;
    case BasicInstruction::DIV:
        return BasicInstruction::FDIV;
    default:
        assert(false);
    }
}

void generateArithmeticInstruction(LLVMBlock B, Expression fexp, Expression lexp, Expression rexp,
                                   BasicInstruction::LLVMIROpcode opcode) {
    bool isInt = fexp->attribute.T.type == Type::INT;
    // 左常量，右变量
    if (lexp->attribute.V.ConstTag && !rexp->attribute.V.ConstTag) {
        rexp->codeIR();
        IRgenTypeConverse(B, rexp->attribute.T.type, fexp->attribute.T.type, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenArithmeticI32ImmLeft(B, opcode, lexp->attribute.V.val.IntVal, reg, ++reg_now);
        else
            IRgenArithmeticF32ImmLeft(B, opint2float(opcode), lexp->attribute.V.val.FloatVal, reg, ++reg_now);
    }
    // 左变量，右常量；左右均变量
    else {
        lexp->codeIR();
        IRgenTypeConverse(B, lexp->attribute.T.type, fexp->attribute.T.type, reg_now);
        int reg1 = reg_now;
        rexp->codeIR();
        IRgenTypeConverse(B, rexp->attribute.T.type, fexp->attribute.T.type, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenArithmeticI32(B, opcode, reg1, reg2, ++reg_now);
        else
            IRgenArithmeticF32(B, opint2float(opcode), reg1, reg2, ++reg_now);
    }
}

void AddExp_plus::codeIR() {
    debug_msgs.push_back("AddExp_plus CodeIR");
    // 类型检查已保证作为常量addexp和mulexp类型已经转换，也保证了ConstTag是正确的
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, addexp->attribute.V.val.IntVal,
                                 mulexp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, addexp->attribute.V.val.FloatVal,
                                 mulexp->attribute.V.val.FloatVal, ++reg_now);
    else    // 两数中有变量的情况
        generateArithmeticInstruction(B, this, addexp, mulexp, BasicInstruction::ADD);
}

void AddExp_sub::codeIR() {
    debug_msgs.push_back("AddExp_sub CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && addexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::SUB, addexp->attribute.V.val.IntVal,
                                 mulexp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && addexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FSUB, addexp->attribute.V.val.FloatVal,
                                 mulexp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, this, addexp, mulexp, BasicInstruction::SUB);
}

void MulExp_mul::codeIR() {
    debug_msgs.push_back("MulExp_mul CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::MUL, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FMUL, mulexp->attribute.V.val.FloatVal,
                                 unary_exp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, this, mulexp, unary_exp, BasicInstruction::MUL);
}

void MulExp_div::codeIR() {
    debug_msgs.push_back("MulExp_div CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::DIV, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FDIV, mulexp->attribute.V.val.FloatVal,
                                 unary_exp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, this, mulexp, unary_exp, BasicInstruction::DIV);
}

void MulExp_mod::codeIR() {
    debug_msgs.push_back("MulExp_mod CodeIR");
    //  类型检查时已保证两数不为float
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::MOD, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else
        generateArithmeticInstruction(B, this, mulexp, unary_exp, BasicInstruction::MOD);
}

void RelExp_leq::codeIR() {
    debug_msgs.push_back("RelExp_leq CodeIR");
    //  类型检查保证作为常量右数的类型正确
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sle, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OLE, reg, addexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        addexp->codeIR();
        IRgenTypeConverse(B, addexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sle, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OLE, reg1, reg2, ++reg_now);
    }
}

void RelExp_lt::codeIR() {
    debug_msgs.push_back("RelExp_lt CodeIR");
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::slt, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OLT, reg, addexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        addexp->codeIR();
        IRgenTypeConverse(B, addexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::slt, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OLT, reg1, reg2, ++reg_now);
    }
}

void RelExp_geq::codeIR() {
    debug_msgs.push_back("RelExp_geq CodeIR");
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sge, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OGE, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        addexp->codeIR();
        IRgenTypeConverse(B, addexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sge, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OGE, reg1, reg2, ++reg_now);
    }
}

void RelExp_gt::codeIR() {
    debug_msgs.push_back("RelExp_gt CodeIR");
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sgt, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OGT, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        addexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sgt, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OGT, reg1, reg2, ++reg_now);
    }
}

void EqExp_eq::codeIR() {
    debug_msgs.push_back("EqExp_eq CodeIR");
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (relexp->attribute.V.ConstTag) {
        eqexp->codeIR();
        IRgenTypeConverse(B, eqexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::eq, reg, relexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OEQ, reg, relexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        eqexp->codeIR();
        IRgenTypeConverse(B, eqexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::eq, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OEQ, reg1, reg2, ++reg_now);
    }
}

void EqExp_neq::codeIR() {
    debug_msgs.push_back("EqExp_neq CodeIR");
    bool isInt = childType == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (relexp->attribute.V.ConstTag) {
        eqexp->codeIR();
        IRgenTypeConverse(B, eqexp->attribute.T.type, childType, reg_now);
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::ne, reg, relexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::ONE, reg, relexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        eqexp->codeIR();
        IRgenTypeConverse(B, eqexp->attribute.T.type, childType, reg_now);
        int reg1 = reg_now;
        relexp->codeIR();
        IRgenTypeConverse(B, relexp->attribute.T.type, childType, reg_now);
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::ne, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::ONE, reg1, reg2, ++reg_now);
    }
}

// short circuit &&
void LAndExp_and::codeIR() {
    assert((true_label != -1) && (false_label != -1));

    // 新建一个块，左值的判断在当前label下进行，新建块用于左值为真情况（即对右值进行进一步判断）
    int lefttrue_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    landexp->true_label = lefttrue_label;
    landexp->false_label = false_label;
    landexp->codeIR();

    // 现在，reg_now为左边的运算结果（需要将INT/FLOAT->BOOL，类型检查中已保证只能为这两种类型）
    // false->跳过右边（短路）、true->右边
    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    IRgenTypeConverse(B1, landexp->attribute.T.type, Type::BOOL, reg_now);
    IRgenBrCond(B1, reg_now, landexp->true_label, landexp->false_label);    // 进行跳转，当前块的使命结束！

    // 切换到新创建的块，往其中添加指令
    label_now = lefttrue_label;
    eqexp->true_label = true_label;
    eqexp->false_label = false_label;
    eqexp->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    IRgenTypeConverse(B2, eqexp->attribute.T.type, Type::BOOL, reg_now);
}

// short circuit  lorexp || landexp
void LOrExp_or::codeIR() {
    debug_msgs.push_back("LOrExpOr CodeIR");
    //  新建一个块，左值的判断在当前label下进行，新建块用于左值为假情况（即对右值进行进一步判断）
    int leftfalse_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    lorexp->true_label = true_label;
    lorexp->false_label = leftfalse_label;
    lorexp->codeIR();

    // 现在，reg_now为左边的运算结果（需要将INT/FLOAT->BOOL，类型检查中已保证只能为这两种类型）
    // false->跳过右边（短路）、true->右边
    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    IRgenTypeConverse(B1, lorexp->attribute.T.type, Type::BOOL, reg_now);
    IRgenBrCond(B1, reg_now, lorexp->true_label, lorexp->false_label);    // 进行跳转，当前块的使命结束！

    // 切换到新创建的块，往其中添加指令
    label_now = leftfalse_label;
    landexp->true_label = true_label;
    landexp->false_label = false_label;
    landexp->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    IRgenTypeConverse(B2, landexp->attribute.T.type, Type::BOOL, reg_now);
}

void ConstExp::codeIR() {
    debug_msgs.push_back("ConstExp CodeIR");
    addexp->codeIR();
}

void Lval::codeIR() {
    debug_msgs.push_back("Lval CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    VarAttribute val;
    bool isarray = false;

    // 获取变量对应的寄存器编号，对于数组，ptr是首地址
    int alloca_reg = irgen_table.symbol_table.lookup(name);
    if (alloca_reg != -1) {
        ptr = GetNewRegOperand(alloca_reg);
        val = irgen_table.reg_table[alloca_reg];
    } else {
        ptr = GetNewGlobalOperand(name->get_string());
        val = semant_table.GlobalTable[name];
    }

    std::vector<Operand> arrayindexs;
    if (dims != nullptr) {
        for (int i = 0; i < dims->size(); i++) {
            auto &dim = (*dims)[i];
            dim->codeIR();
            IRgenTypeConverse(B, dim->attribute.T.type, Type::INT, reg_now);
            arrayindexs.push_back(GetNewRegOperand(reg_now));
        }
    }
    // int a[][5]   dim: {5}
    // a[2][5] -> index {2，5}
    // -----------------------
    // int a[3][5]  dim: {3, 5}
    // a[2] -> indexs {0, 2}
    // a[1][2] -> indexs {0,1,2}
    if (arrayindexs.size() != 0 || attribute.T.type == Type::PTR) {
        if (val.isFormalArray) {
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, ptr, val.dims, arrayindexs);
        } else {
            arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(0));
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, ptr, val.dims, arrayindexs);
        }
        ptr = GetNewRegOperand(reg_now);
    }

    // 左值只需要获取ptr就完成了，其他普通Lval还需要得到ptr里的值
    if (is_left == false && attribute.T.type != Type::PTR) {
        IRgenLoad(B, Type2LLvm[attribute.T.type], ++reg_now, ptr);
    }
}

void FuncRParams::codeIR() { debug_msgs.push_back("FuncRParams CodeIR"); }    // 没有用到，直接在Func_call中进行了生成

// 类型检查已保证：1、对应实参和形参类型匹配(11.19更正：没有保证了TAT)；2、数组维度匹配
void Func_call::codeIR() {
    debug_msgs.push_back("FunctionCall CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (funcr_params != nullptr) {
        // 跟typecheck里一样，不再调用funcRParams->codeIR，直接在此处转换为
        // FuncRParams后，对每个exp进行codeir，然后放到args里
        FuncDef funcAttr = semant_table.FunctionTable.find(name)->second;
        auto funcRParams = dynamic_cast<FuncRParams *>(funcr_params);
        std::vector<std::pair<BasicInstruction::LLVMType, Operand>> args;
        // 遍历每个参数
        for (int i = 0; i < funcRParams->params->size(); i++) {
            (*funcRParams->params)[i]->codeIR();
            if ((*funcRParams->params)[i]->attribute.T.type != Type::PTR) {
                IRgenTypeConverse(B, (*funcRParams->params)[i]->attribute.T.type, (*funcAttr->formals)[i]->type_decl,
                                  reg_now);
                args.push_back({Type2LLvm[(*funcAttr->formals)[i]->type_decl], GetNewRegOperand(reg_now)});
            } else
                args.push_back({Type2LLvm[Type::PTR], GetNewRegOperand(reg_now)});
        }
        if (attribute.T.type == Type::VOID)
            IRgenCallVoid(B, BasicInstruction::VOID, args, name->get_string());
        else
            IRgenCall(B, Type2LLvm[attribute.T.type], ++reg_now, args, name->get_string());
    }
    // 无参数的函数
    else {
        if (attribute.T.type == Type::VOID)
            IRgenCallVoidNoArgs(B, BasicInstruction::VOID, name->get_string());
        else
            IRgenCallNoArgs(B, Type2LLvm[attribute.T.type], ++reg_now, name->get_string());
    }
}

// +/- 已保证unary_exp结点本身为int或float
void UnaryExp_plus::codeIR() {
    debug_msgs.push_back("UnaryExpPlus CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    unary_exp->codeIR();
    IRgenTypeConverse(B, unary_exp->attribute.T.type, attribute.T.type, reg_now);
}

void UnaryExp_neg::codeIR() {
    debug_msgs.push_back("UnaryExpNeg CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    unary_exp->codeIR();
    IRgenTypeConverse(B, unary_exp->attribute.T.type, attribute.T.type, reg_now);
    int ori_reg = reg_now;
    if (attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmLeft(B, BasicInstruction::SUB, 0, ori_reg, ++reg_now);
    else
        IRgenArithmeticF32ImmLeft(B, BasicInstruction::FSUB, 0, ori_reg, ++reg_now);
}

// ! 已保证unary_exp为int或float、结点本身为bool
void UnaryExp_not::codeIR() {
    debug_msgs.push_back("UnaryExpNot CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    unary_exp->codeIR();
    if (unary_exp->attribute.T.type == Type::BOOL) {
        IRgenTypeConverse(B, unary_exp->attribute.T.type, Type::INT, reg_now);
        unary_exp->attribute.T.type = Type::INT;
    }
    int ori_reg = reg_now;
    if (unary_exp->attribute.T.type == Type::INT)
        IRgenIcmpImmRight(B, BasicInstruction::eq, ori_reg, 0, ++reg_now);
    else
        IRgenFcmpImmRight(B, BasicInstruction::OEQ, ori_reg, 0, ++reg_now);
}

void IntConst::codeIR() {
    debug_msgs.push_back("IntConst CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, val, 0, ++reg_now);
}

void FloatConst::codeIR() {
    debug_msgs.push_back("FloatConst CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, val, 0, ++reg_now);
}

void StringConst::codeIR() { debug_msgs.push_back("StringConst CodeIR"); }    // 未实现string。

void PrimaryExp_branch::codeIR() {
    debug_msgs.push_back("PrimaryExp_branch CodeIR");
    exp->codeIR();
}

// lval = exp
void assign_stmt::codeIR() {
    debug_msgs.push_back("AssignStmt CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    lval->codeIR();
    exp->codeIR();
    IRgenTypeConverse(B, exp->attribute.T.type, lval->attribute.T.type, reg_now);
    IRgenStore(B, Type2LLvm[lval->attribute.T.type], reg_now, ((Lval *)lval)->ptr);
}

void expr_stmt::codeIR() {
    debug_msgs.push_back("expr_stmt CodeIR");
    exp->codeIR();
}

void block_stmt::codeIR() {
    debug_msgs.push_back("BlockStmt CodeIR");
    irgen_table.symbol_table.enter_scope();
    b->codeIR();
    irgen_table.symbol_table.exit_scope();
}
// if{
//     if_label
// }
// else{
//     else_label
// }
// end_label
void ifelse_stmt::codeIR() {
    debug_msgs.push_back("IfElseStmt CodeIR");
    int if_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    int else_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    int end_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;

    Cond->true_label = if_label;
    Cond->false_label = else_label;
    Cond->codeIR();

    // 此时，reg_now已经存了cond结果
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    int ori_reg = reg_now;
    if (Cond->attribute.T.type == Type::INT)
        IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::ne, ori_reg, 0, ++reg_now);
    else if (Cond->attribute.T.type == Type::FLOAT)
        IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::ONE, ori_reg, 0, ++reg_now);
    IRgenBrCond(B, reg_now, Cond->true_label, Cond->false_label);    // 该块使命结束！

    // 切换到if内部语句块
    label_now = if_label;
    ifstmt->codeIR();
    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B1, end_label);

    // 切换至else块
    label_now = else_label;
    elsestmt->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B2, end_label);

    label_now = end_label;
}

// if{
//     if_label
// }
// end_label
void if_stmt::codeIR() {
    debug_msgs.push_back("IfStmt CodeIR");
    int if_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    int end_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;

    Cond->true_label = if_label;
    Cond->false_label = end_label;
    Cond->codeIR();

    // 此时，reg_now已经存了cond结果
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    int ori_reg = reg_now;
    if (Cond->attribute.T.type == Type::INT)
        IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::ne, ori_reg, 0, ++reg_now);
    else if (Cond->attribute.T.type == Type::FLOAT)
        IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::ONE, ori_reg, 0, ++reg_now);
    IRgenBrCond(B, reg_now, Cond->true_label, Cond->false_label);    // 该块使命结束！

    // 切换到if内部语句块
    label_now = if_label;
    ifstmt->codeIR();

    // 结束，切换至end_label块.
    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B1, end_label);
    label_now = end_label;
}

int now_cond_label = -1;
int now_end_label = -1;

// while(cond_label)
//     while_label
// }
// end_label
void while_stmt::codeIR() {
    debug_msgs.push_back("WhileStmt CodeIR");
    int cond_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    int while_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    int end_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;

    // 状态保存
    int pre_cond_label = now_cond_label;
    int pre_end_label = now_end_label;
    now_cond_label = cond_label;
    now_end_label = end_label;

    Cond->true_label = while_label;
    Cond->false_label = end_label;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B, cond_label);
    label_now = cond_label;
    Cond->codeIR();

    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    int ori_reg = reg_now;
    if (Cond->attribute.T.type == Type::INT)
        IRgenIcmpImmRight(B1, BasicInstruction::IcmpCond::ne, ori_reg, 0, ++reg_now);
    else if (Cond->attribute.T.type == Type::FLOAT)
        IRgenFcmpImmRight(B1, BasicInstruction::FcmpCond::ONE, ori_reg, 0, ++reg_now);
    IRgenBrCond(B1, reg_now, Cond->true_label, Cond->false_label);
    // 以上均为cond块的内容

    label_now = while_label;
    body->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B2, cond_label);

    label_now = end_label;
    now_cond_label = pre_cond_label;
    now_end_label = pre_end_label;
}

void continue_stmt::codeIR() {
    debug_msgs.push_back("ContinueStmt CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B, now_cond_label);
    // continue应当处于一个块的最后，接下来的指令应当在一个新的块中添加
    label_now = llvmIR.NewBlock(func_now, ++label_max)->block_id;
}

void break_stmt::codeIR() {
    debug_msgs.push_back("BreakStmt CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenBRUnCond(B, now_end_label);
    label_now = llvmIR.NewBlock(func_now, ++label_max)->block_id;
}

void return_stmt::codeIR() {
    debug_msgs.push_back("ReturnStmt CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    return_exp->codeIR();
    IRgenTypeConverse(B, return_exp->attribute.T.type, expected_type, reg_now);
    IRgenRetReg(B, Type2LLvm[expected_type], reg_now);
}

void return_stmt_void::codeIR() {
    debug_msgs.push_back("ReturnStmtVoid CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    IRgenRetVoid(B);
}

void ConstInitVal::codeIR() { debug_msgs.push_back("ConstInitVal CodeIR"); }    // 不会出现这种情况了

void ConstInitVal_exp::codeIR() {
    debug_msgs.push_back("ConstInitValWithExp CodeIR");
    exp->codeIR();
}

void VarInitVal::codeIR() { debug_msgs.push_back("VarInitVal CodeIR"); }    // 不会出现这种情况了

void VarInitVal_exp::codeIR() {
    debug_msgs.push_back("VarInitValWithExp CodeIR");
    exp->codeIR();
}

void VarDef_no_init::codeIR() {
    debug_msgs.push_back("VarDefNoInit CodeIR");
    if (!isglobal) {
        // 声明语句在0号块（分配大小）、初始值语句在当前块
        LLVMBlock B0 = llvmIR.GetBlock(func_now, 0);
        LLVMBlock B = llvmIR.GetBlock(func_now, label_now);

        // 1、符号表：name->reg
        irgen_table.symbol_table.add_Symbol(name, ++reg_now);

        // 2、寄存器表：reg->VarAttribute
        VarAttribute val;
        val.ConstTag = false;
        val.type = type_decl;
        if (dims == nullptr) {
            irgen_table.reg_table[reg_now];
            //  3、非数组：分配->默认值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            if (type_decl == Type::INT)
                IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, 0, 0, ++reg_now);
            else if (type_decl == Type::FLOAT)
                IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, 0, 0, ++reg_now);
            IRgenStore(B, Type2LLvm[type_decl], GetNewRegOperand(reg_now), GetNewRegOperand(reg_alloc));
            return;
        }
        // 剩下的就是数组了，维度由于是const，因此在类型检查时，已经转换好了。
        int size = 1;
        for (auto dim : *dims) {
            val.dims.push_back(dim->attribute.V.val.IntVal);
            size *= dim->attribute.V.val.IntVal;
        }
        irgen_table.reg_table[reg_now] = val;
        //  3、分配->memset函数初始化值
        // void *memset(起始地址, 每字节初始化的值, 字节数, 对齐方式);
        IRgenAllocaArray(B0, Type2LLvm[type_decl], reg_now, val.dims);
        // Reference: https://github.com/yuhuifishash/SysY/ir_gen/IRgen.cc line618-line623
        CallInstruction *memsetCall =
        new CallInstruction(BasicInstruction::VOID, nullptr, std::string("llvm.memset.p0.i32"));
        memsetCall->push_back_Parameter(BasicInstruction::PTR, GetNewRegOperand(reg_now));
        memsetCall->push_back_Parameter(BasicInstruction::I8, new ImmI32Operand(0x3f));
        memsetCall->push_back_Parameter(BasicInstruction::I32, new ImmI32Operand(size * sizeof(int)));
        memsetCall->push_back_Parameter(BasicInstruction::I1, new ImmI32Operand(0));
        B->InsertInstruction(1, memsetCall);
        return;
    }
    // isglobal
    VarAttribute val = semant_table.GlobalTable[name];
    BasicInstruction::LLVMType lltype = Type2LLvm[type_decl];
    Instruction globalDecl;
    if (dims != nullptr)
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, val);
    else
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, nullptr);
    llvmIR.global_def.push_back(globalDecl);
}

void VarDef::codeIR() {
    debug_msgs.push_back("VarDef CodeIR");
    if (!isglobal) {
        // 声明语句在0号块（分配大小）、初始值语句在当前块
        LLVMBlock B0 = llvmIR.GetBlock(func_now, 0);
        LLVMBlock B = llvmIR.GetBlock(func_now, label_now);

        // 1、符号表：name->reg
        irgen_table.symbol_table.add_Symbol(name, ++reg_now);

        // 2、寄存器表：reg->VarAttribute
        VarAttribute val;
        val.ConstTag = false;
        val.type = type_decl;
        if (dims == nullptr) {
            irgen_table.reg_table[reg_now] = val;
            //  3、分配->声明值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            init->codeIR();    // 执行后，regnow储存初始化值
            if (!init->attribute.V.ConstTag)
                IRgenTypeConverse(B, init->attribute.T.type, type_decl, reg_now);
            IRgenStore(B, Type2LLvm[type_decl], GetNewRegOperand(reg_now), GetNewRegOperand(reg_alloc));
            return;
        }
        // 剩下的就是数组了
        int size = 1;
        for (auto dim : *dims) {
            val.dims.push_back(dim->attribute.V.val.IntVal);
            size *= dim->attribute.V.val.IntVal;
        }
        irgen_table.reg_table[reg_now] = val;
        int reg_array = reg_now;    // 存放数组首地址
        //  3、分配->声明值->储存
        // 3.1 在函数0号块分配空间
        IRgenAllocaArray(B0, Type2LLvm[type_decl], reg_now, val.dims);
        // 3.2 这个空间先清0
        CallInstruction *memsetCall =
        new CallInstruction(BasicInstruction::VOID, nullptr, std::string("llvm.memset.p0.i32"));
        memsetCall->push_back_Parameter(BasicInstruction::PTR, GetNewRegOperand(reg_now));
        memsetCall->push_back_Parameter(BasicInstruction::I8, new ImmI32Operand(0));
        memsetCall->push_back_Parameter(BasicInstruction::I32, new ImmI32Operand(size * sizeof(int)));
        memsetCall->push_back_Parameter(BasicInstruction::I1, new ImmI32Operand(0));
        B->InsertInstruction(1, memsetCall);

        // 3.3 按照类型检查时，初始化好的varinits，进行初始值赋值
        for (int i = 0; i < varinits.size(); ++i) {
            // 回忆一下varinits：<exp, 线性索引>
            int remainder = varinits[i].second;
            // remainder -> 索引
            // arrayindexs储存地址
            // int a[3][2][2]
            // remainder = 10 -> a[2][1][0] -> arrayindexs:{0,2,1,0}
            std::vector<Operand> arrayindexs;
            for (int d = val.dims.size() - 1; d >= 0; --d) {
                int idx = remainder % val.dims[d];
                remainder /= val.dims[d];
                arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(idx));
            }
            arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(0));

            // 计算应当赋值的地址
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, GetNewRegOperand(reg_array), val.dims,
                                       arrayindexs);
            int reg_adr = reg_now;    // 存放应当赋值的地址
            // 获取初始化值
            if (val.type == Type::INT) {
                varinits[i].first->codeIR();
                IRgenTypeConverse(B, varinits[i].first->attribute.T.type, Type::INT, reg_now);
            } else if (val.type == Type::FLOAT) {
                varinits[i].first->codeIR();
                IRgenTypeConverse(B, varinits[i].first->attribute.T.type, Type::FLOAT, reg_now);
            }
            // 此时reg_now存放的是获取的数值
            IRgenStore(B, Type2LLvm[val.type], GetNewRegOperand(reg_now), GetNewRegOperand(reg_adr));
        }
        return;
    }
    // isglobal
    VarAttribute val = semant_table.GlobalTable[name];
    BasicInstruction::LLVMType lltype = Type2LLvm[type_decl];
    Instruction globalDecl;
    if (dims != nullptr)
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, val);
    else if (type_decl == Type::INT)
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, new ImmI32Operand(val.IntInitVals[0]));
    else if (type_decl == Type::FLOAT)
        globalDecl =
        new GlobalVarDefineInstruction(name->get_string(), lltype, new ImmF32Operand(val.FloatInitVals[0]));
    llvmIR.global_def.push_back(globalDecl);
}

void ConstDef::codeIR() {
    debug_msgs.push_back("ConstDef CodeIR");
    if (!isglobal) {
        // 声明语句在0号块（分配大小）、初始值语句在当前块
        LLVMBlock B0 = llvmIR.GetBlock(func_now, 0);
        LLVMBlock B = llvmIR.GetBlock(func_now, label_now);

        // 1、符号表：name->reg
        irgen_table.symbol_table.add_Symbol(name, ++reg_now);

        // 2、寄存器表：reg->VarAttribute
        VarAttribute val;
        val.ConstTag = true;
        val.type = type_decl;
        if (dims == nullptr) {
            irgen_table.reg_table[reg_now] = val;
            //  3、分配->声明值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            init->codeIR();    // 执行后，regnow储存初始化值
            if (!init->attribute.V.ConstTag)
                IRgenTypeConverse(B, init->attribute.T.type, type_decl, reg_now);
            IRgenStore(B, Type2LLvm[type_decl], GetNewRegOperand(reg_now), GetNewRegOperand(reg_alloc));
            return;
        }
        // 剩下的就是数组了
        int size = 1;
        for (auto dim : *dims) {
            val.dims.push_back(dim->attribute.V.val.IntVal);
            size *= dim->attribute.V.val.IntVal;
        }
        irgen_table.reg_table[reg_now] = val;
        int reg_array = reg_now;    // 存放数组首地址
        //  3、分配->声明值->储存
        IRgenAllocaArray(B0, Type2LLvm[type_decl], reg_now, val.dims);
        CallInstruction *memsetCall =
        new CallInstruction(BasicInstruction::VOID, nullptr, std::string("llvm.memset.p0.i32"));
        memsetCall->push_back_Parameter(BasicInstruction::PTR, GetNewRegOperand(reg_now));
        memsetCall->push_back_Parameter(BasicInstruction::I8, new ImmI32Operand(0));
        memsetCall->push_back_Parameter(BasicInstruction::I32, new ImmI32Operand(size * sizeof(int)));
        memsetCall->push_back_Parameter(BasicInstruction::I1, new ImmI32Operand(0));
        B->InsertInstruction(1, memsetCall);
        // 3.3 按照类型检查时，初始化好的varinits，进行初始值赋值
        for (int i = 0; i < varinits.size(); ++i) {
            // 回忆一下varinits：<exp, 线性索引>
            int remainder = varinits[i].second;
            // remainder -> 索引
            // arrayindexs储存地址
            // int a[3][2][2]
            // remainder = 10 -> a[2][1][0] -> arrayindexs:{0,2,1,0}
            std::vector<Operand> arrayindexs;
            for (int d = val.dims.size() - 1; d >= 0; --d) {
                int idx = remainder % val.dims[d];
                remainder /= val.dims[d];
                arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(idx));
            }
            arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(0));

            // 计算应当赋值的地址
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, GetNewRegOperand(reg_array), val.dims,
                                       arrayindexs);
            int reg_adr = reg_now;    // 存放应当赋值的地址
            // 获取初始化值
            if (val.type == Type::INT) {
                varinits[i].first->codeIR();
                IRgenTypeConverse(B, varinits[i].first->attribute.T.type, Type::INT, reg_now);
            } else if (val.type == Type::FLOAT) {
                varinits[i].first->codeIR();
                IRgenTypeConverse(B, varinits[i].first->attribute.T.type, Type::FLOAT, reg_now);
            }
            // 此时reg_now存放的是获取的数值
            IRgenStore(B, Type2LLvm[val.type], GetNewRegOperand(reg_now), GetNewRegOperand(reg_adr));
        }
        return;
    }
    // isglobal
    VarAttribute val = semant_table.GlobalTable[name];
    BasicInstruction::LLVMType lltype = Type2LLvm[type_decl];
    Instruction globalDecl;
    if (dims != nullptr)
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, val);
    else if (type_decl == Type::INT)
        globalDecl = new GlobalVarDefineInstruction(name->get_string(), lltype, new ImmI32Operand(val.IntInitVals[0]));
    else if (type_decl == Type::FLOAT)
        globalDecl =
        new GlobalVarDefineInstruction(name->get_string(), lltype, new ImmF32Operand(val.FloatInitVals[0]));
    llvmIR.global_def.push_back(globalDecl);
}

void VarDecl::codeIR() {
    debug_msgs.push_back("VarDecl CodeIR");
    for (auto &var_def : *var_def_list) {
        var_def->codeIR();
    }
}

void ConstDecl::codeIR() {
    debug_msgs.push_back("ConstDecl CodeIR");
    for (auto &var_def : *var_def_list) {
        var_def->codeIR();
    }
}

void BlockItem_Decl::codeIR() {
    debug_msgs.push_back("BlockItemDecl CodeIR");
    decl->codeIR();
}

void BlockItem_Stmt::codeIR() {
    debug_msgs.push_back("BlockItemStmt CodeIR");
    stmt->codeIR();
}

void __Block::codeIR() {
    debug_msgs.push_back("Block CodeIR");

    irgen_table.symbol_table.enter_scope();
    auto item_vector = *item_list;
    for (auto item : item_vector)
        item->codeIR();
    irgen_table.symbol_table.exit_scope();
}

void __FuncFParam::codeIR() { debug_msgs.push_back("FunctionFParam CodeIR"); }

void __FuncDef::codeIR() {
    debug_msgs.push_back("FunctionDef CodeIR");

    irgen_table.symbol_table.enter_scope();
    // 从头开始
    reg_now = -1;
    irgen_table.reg_table.clear();
    label_now = 0;
    label_max = -1;

    // 创建函数定义指令，作为一个函数的标识符
    // <FuncDefIns, { <id, block>, <id, block>, ···} >
    FuncDefInstruction FuncDefIns = new FunctionDefineInstruction(Type2LLvm[return_type], name->get_string());
    llvmIR.NewFunction(FuncDefIns);

    func_now = FuncDefIns;
    LLVMBlock B0 = llvmIR.NewBlock(func_now, ++label_max);    // 创建0号块

    // 本身label_now已经是0号块，不用再label_now = label_max赋值了。

    auto formal_vector = *formals;
    reg_now = formal_vector.size() - 1;

    // 遍历形参
    for (int i = 0; i < formal_vector.size(); i++) {
        auto formal = formal_vector[i];
        VarAttribute val;
        val.ConstTag = false;
        val.type = formal->type_decl;

        if (formal->dims == nullptr) {
            // 保存传入值
            FuncDefIns->InsertFormal(Type2LLvm[formal->type_decl]);
            // 分配寄存器作为函数内的执行者，并初始化其值为传入值
            IRgenAlloca(B0, Type2LLvm[formal->type_decl], ++reg_now);
            IRgenStore(B0, Type2LLvm[formal->type_decl], GetNewRegOperand(i), GetNewRegOperand(reg_now));
            // 保存到符号表
            irgen_table.symbol_table.add_Symbol(formal->name, reg_now);
            irgen_table.reg_table[reg_now] = val;
        }
        // 数组
        else if (formal->dims != nullptr) {
            // 保存传入值
            FuncDefIns->InsertFormal(BasicInstruction::LLVMType::PTR);
            // 按照IRgenGetElementptrIndexI32的用法，作为参数的数组，第一维度为空不用放入dims中，传入indexs时，第一维的含义是跳过整个数组的大小
            // a[]    dim:{}
            // a[][3] dim:{3}
            for (int j = 1; j < formal->dims->size(); j++) {
                auto d = (*formal->dims)[j];
                val.dims.push_back(d->attribute.V.val.IntVal);
            }
            val.isFormalArray = true;    // 这是一个作为参数的数组
            irgen_table.symbol_table.add_Symbol(formal->name, i);
            irgen_table.reg_table[i] = val;
        }
    }

    // 0号块使命结束，进入1号块
    LLVMBlock B1 = llvmIR.NewBlock(func_now, ++label_max);
    label_now = label_max;    // =1
    IRgenBRUnCond(B0, label_now);

    // 让下面的慢慢生成吧
    block->codeIR();

    // <FuncDefIns, { <0, block>, <1, block>, ···} >
    // 未定义行为处理：代码执行结束后没有return语句，返回随机值即可（为了不再将main函数与其他int类型函数作区分，我们返回0）
    // Reference: https://github.com/yuhuifishash/SysY/ir_gen/IRgen.cc line91-line104
    for (auto block : llvmIR.function_block_map[func_now]) {
        LLVMBlock B = block.second;
        int opcode = -1;
        if (!B->Instruction_list.empty())
            opcode = B->Instruction_list.back()->GetOpcode();
        if (B->Instruction_list.empty() || (opcode != BasicInstruction::BR_COND &&
                                            opcode != BasicInstruction::BR_UNCOND && opcode != BasicInstruction::RET)) {
            if (return_type == Type::VOID)
                IRgenRetVoid(B);
            else if (return_type == Type::INT)
                IRgenRetImmInt(B, BasicInstruction::LLVMType::I32, 0);
            else if (return_type == Type::FLOAT)
                IRgenRetImmFloat(B, BasicInstruction::LLVMType::FLOAT32, 0);
        }
    }
    irgen_table.symbol_table.exit_scope();
    FuncDefIns->reg_max = reg_now;
}

void CompUnit_Decl::codeIR() {
    debug_msgs.push_back("CompUnitDecl CodeIR");
    // 通过decl、def的isglobal变量区分全局和局部，已在类型检查部分正确设置该变量
    decl->codeIR();
}

void CompUnit_FuncDef::codeIR() { func_def->codeIR(); }

void AddLibFunctionDeclare() {
    FunctionDeclareInstruction *getint = new FunctionDeclareInstruction(BasicInstruction::I32, "getint");
    llvmIR.function_declare.push_back(getint);

    FunctionDeclareInstruction *getchar = new FunctionDeclareInstruction(BasicInstruction::I32, "getch");
    llvmIR.function_declare.push_back(getchar);

    FunctionDeclareInstruction *getfloat = new FunctionDeclareInstruction(BasicInstruction::FLOAT32, "getfloat");
    llvmIR.function_declare.push_back(getfloat);

    FunctionDeclareInstruction *getarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getarray");
    getarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getarray);

    FunctionDeclareInstruction *getfloatarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getfarray");
    getfloatarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getfloatarray);

    FunctionDeclareInstruction *putint = new FunctionDeclareInstruction(BasicInstruction::VOID, "putint");
    putint->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putint);

    FunctionDeclareInstruction *putch = new FunctionDeclareInstruction(BasicInstruction::VOID, "putch");
    putch->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putch);

    FunctionDeclareInstruction *putfloat = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfloat");
    putfloat->InsertFormal(BasicInstruction::FLOAT32);
    llvmIR.function_declare.push_back(putfloat);

    FunctionDeclareInstruction *putarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putarray");
    putarray->InsertFormal(BasicInstruction::I32);
    putarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putarray);

    FunctionDeclareInstruction *putfarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfarray");
    putfarray->InsertFormal(BasicInstruction::I32);
    putfarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putfarray);

    FunctionDeclareInstruction *starttime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_starttime");
    starttime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(starttime);

    FunctionDeclareInstruction *stoptime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_stoptime");
    stoptime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(stoptime);

    // 一些llvm自带的函数，也许会为你的优化提供帮助
    FunctionDeclareInstruction *llvm_memset =
    new FunctionDeclareInstruction(BasicInstruction::VOID, "llvm.memset.p0.i32");
    llvm_memset->InsertFormal(BasicInstruction::PTR);
    llvm_memset->InsertFormal(BasicInstruction::I8);
    llvm_memset->InsertFormal(BasicInstruction::I32);
    llvm_memset->InsertFormal(BasicInstruction::I1);
    llvmIR.function_declare.push_back(llvm_memset);

    FunctionDeclareInstruction *llvm_umax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umax.i32");
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umax);

    FunctionDeclareInstruction *llvm_umin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umin.i32");
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umin);

    FunctionDeclareInstruction *llvm_smax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smax.i32");
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smax);

    FunctionDeclareInstruction *llvm_smin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smin.i32");
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smin);
}
