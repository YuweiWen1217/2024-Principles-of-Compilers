#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern std::vector<std::string> error_msgs;

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
void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src, int dst) {
    error_msgs.push_back("IRgenTypeConverse. Implement it if you need it");
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
    error_msgs.push_back("Program CodeIR");
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() {
    error_msgs.push_back("Exp CodeIR");
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

void generateArithmeticInstruction(LLVMBlock B, Expression lexp, Expression rexp,
                                   BasicInstruction::LLVMIROpcode opcode) {
    bool isInt = lexp->attribute.T.type == Type::INT;
    // 左常量，右变量
    if (lexp->attribute.V.ConstTag && !rexp->attribute.V.ConstTag) {
        rexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenArithmeticI32ImmLeft(B, opcode, lexp->attribute.V.val.IntVal, reg, ++reg_now);
        else
            IRgenArithmeticF32ImmLeft(B, opint2float(opcode), lexp->attribute.V.val.FloatVal, reg, ++reg_now);
    }
    // 左变量，右常量；左右均变量
    else {
        lexp->codeIR();
        int reg1 = reg_now;
        rexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenArithmeticI32(B, opcode, reg1, reg2, ++reg_now);
        else
            IRgenArithmeticF32(B, opint2float(opcode), reg1, reg2, ++reg_now);
    }
}

void AddExp_plus::codeIR() {
    error_msgs.push_back("AddExp_plus CodeIR");
    // 类型检查已保证addexp和mulexp同时为int或同时为float，也保证了ConstTag是正确的
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && addexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, addexp->attribute.V.val.IntVal,
                                 mulexp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && addexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, addexp->attribute.V.val.FloatVal,
                                 mulexp->attribute.V.val.FloatVal, ++reg_now);
    else    // 两数中有变量的情况
        generateArithmeticInstruction(B, addexp, mulexp, BasicInstruction::ADD);
}

void AddExp_sub::codeIR() {
    error_msgs.push_back("AddExp_sub CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && addexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::SUB, addexp->attribute.V.val.IntVal,
                                 mulexp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && addexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FSUB, addexp->attribute.V.val.FloatVal,
                                 mulexp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, addexp, mulexp, BasicInstruction::SUB);
}

void MulExp_mul::codeIR() {
    error_msgs.push_back("MulExp_mul CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::MUL, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FMUL, mulexp->attribute.V.val.FloatVal,
                                 unary_exp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, mulexp, unary_exp, BasicInstruction::MUL);
}

void MulExp_div::codeIR() {
    error_msgs.push_back("MulExp_div CodeIR");
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::INT)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::DIV, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else if (attribute.V.ConstTag && mulexp->attribute.T.type == Type::FLOAT)
        IRgenArithmeticF32ImmAll(B, BasicInstruction::FDIV, mulexp->attribute.V.val.FloatVal,
                                 unary_exp->attribute.V.val.FloatVal, ++reg_now);
    else
        generateArithmeticInstruction(B, mulexp, unary_exp, BasicInstruction::DIV);
}

void MulExp_mod::codeIR() {
    error_msgs.push_back("MulExp_mod CodeIR");
    // 类型检查时已保证两数均为int
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (attribute.V.ConstTag)
        IRgenArithmeticI32ImmAll(B, BasicInstruction::MOD, mulexp->attribute.V.val.IntVal,
                                 unary_exp->attribute.V.val.IntVal, ++reg_now);
    else
        generateArithmeticInstruction(B, mulexp, unary_exp, BasicInstruction::MOD);
}

void RelExp_leq::codeIR() {
    error_msgs.push_back("RelExp_leq CodeIR");
    // 类型检查后两数依旧均为int或均为float
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sle, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OLE, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        int reg1 = reg_now;
        addexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sle, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OLE, reg1, reg2, ++reg_now);
    }
}

void RelExp_lt::codeIR() {
    error_msgs.push_back("RelExp_lt CodeIR");
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::slt, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OLT, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        int reg1 = reg_now;
        addexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::slt, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OLT, reg1, reg2, ++reg_now);
    }
}

void RelExp_geq::codeIR() {
    error_msgs.push_back("RelExp_geq CodeIR");
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sge, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OGE, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        int reg1 = reg_now;
        addexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sge, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OGE, reg1, reg2, ++reg_now);
    }
}

void RelExp_gt::codeIR() {
    error_msgs.push_back("RelExp_gt CodeIR");
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (addexp->attribute.V.ConstTag) {
        relexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::sgt, reg, addexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OGT, reg, addexp->attribute.V.val.FloatVal, ++reg_now);

    } else {
        relexp->codeIR();
        int reg1 = reg_now;
        addexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::sgt, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OGT, reg1, reg2, ++reg_now);
    }
}

void EqExp_eq::codeIR() {
    error_msgs.push_back("EqExp_eq CodeIR");
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (relexp->attribute.V.ConstTag) {
        eqexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::eq, reg, relexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::OEQ, reg, relexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        eqexp->codeIR();
        int reg1 = reg_now;
        relexp->codeIR();
        int reg2 = reg_now;
        if (isInt)
            IRgenIcmp(B, BasicInstruction::IcmpCond::eq, reg1, reg2, ++reg_now);
        else
            IRgenFcmp(B, BasicInstruction::FcmpCond::OEQ, reg1, reg2, ++reg_now);
    }
}

void EqExp_neq::codeIR() {
    error_msgs.push_back("EqExp_neq CodeIR");
    bool isInt = relexp->attribute.T.type == Type::INT;
    LLVMBlock B = llvmIR.GetBlock(func_now, label_now);
    if (relexp->attribute.V.ConstTag) {
        eqexp->codeIR();
        int reg = reg_now;
        if (isInt)
            IRgenIcmpImmRight(B, BasicInstruction::IcmpCond::ne, reg, relexp->attribute.V.val.IntVal, ++reg_now);
        else
            IRgenFcmpImmRight(B, BasicInstruction::FcmpCond::ONE, reg, relexp->attribute.V.val.FloatVal, ++reg_now);
    } else {
        eqexp->codeIR();
        int reg1 = reg_now;
        relexp->codeIR();
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
    if (landexp->attribute.T.type == Type::INT) {
        IRgenIcmpImmRight(B1, BasicInstruction::IcmpCond::ne, reg_now, 0, reg_now);
    } else if (landexp->attribute.T.type == Type::FLOAT) {
        IRgenFcmpImmRight(B1, BasicInstruction::FcmpCond::ONE, reg_now, 0, reg_now);
    }
    IRgenBrCond(B1, reg_now, landexp->true_label, landexp->false_label);    // 进行跳转，当前块的使命结束！

    // 切换到新创建的块，往其中添加指令
    label_now = lefttrue_label;
    eqexp->true_label = true_label;
    eqexp->false_label = false_label;
    eqexp->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    if (eqexp->attribute.T.type == Type::INT) {
        IRgenIcmpImmRight(B2, BasicInstruction::IcmpCond::ne, reg_now, 0, reg_now);
    } else if (eqexp->attribute.T.type == Type::FLOAT) {
        IRgenFcmpImmRight(B2, BasicInstruction::FcmpCond::ONE, reg_now, 0, reg_now);
    }
    // TODO：：：这里需不需要跳转？
}

// short circuit  lorexp || landexp
void LOrExp_or::codeIR() {
    error_msgs.push_back("LOrExpOr CodeIR");
    // 新建一个块，左值的判断在当前label下进行，新建块用于左值为假情况（即对右值进行进一步判断）
    int leftfalse_label = llvmIR.NewBlock(func_now, ++label_max)->block_id;
    lorexp->true_label = true_label;
    lorexp->false_label = leftfalse_label;
    lorexp->codeIR();

    // 现在，reg_now为左边的运算结果（需要将INT/FLOAT->BOOL，类型检查中已保证只能为这两种类型）
    // false->跳过右边（短路）、true->右边
    LLVMBlock B1 = llvmIR.GetBlock(func_now, label_now);
    if (landexp->attribute.T.type == Type::INT) {
        IRgenIcmpImmRight(B1, BasicInstruction::IcmpCond::ne, reg_now, 0, reg_now);
    } else if (landexp->attribute.T.type == Type::FLOAT) {
        IRgenFcmpImmRight(B1, BasicInstruction::FcmpCond::ONE, reg_now, 0, reg_now);
    }
    IRgenBrCond(B1, reg_now, landexp->true_label, landexp->false_label);    // 进行跳转，当前块的使命结束！

    // 切换到新创建的块，往其中添加指令
    label_now = leftfalse_label;
    landexp->true_label = true_label;
    landexp->false_label = false_label;
    landexp->codeIR();
    LLVMBlock B2 = llvmIR.GetBlock(func_now, label_now);
    if (landexp->attribute.T.type == Type::INT) {
        IRgenIcmpImmRight(B2, BasicInstruction::IcmpCond::ne, reg_now, 0, reg_now);
    } else if (landexp->attribute.T.type == Type::FLOAT) {
        IRgenFcmpImmRight(B2, BasicInstruction::FcmpCond::ONE, reg_now, 0, reg_now);
    }
    // TODO：：：这里需不需要跳转？
}

void ConstExp::codeIR() { addexp->codeIR(); }

void Lval::codeIR() { error_msgs.push_back("Lval CodeIR"); }

void FuncRParams::codeIR() { error_msgs.push_back("FuncRParams CodeIR"); }

void Func_call::codeIR() { error_msgs.push_back("FunctionCall CodeIR"); }

void UnaryExp_plus::codeIR() { error_msgs.push_back("UnaryExpPlus CodeIR"); }

void UnaryExp_neg::codeIR() { error_msgs.push_back("UnaryExpNeg CodeIR"); }

void UnaryExp_not::codeIR() { error_msgs.push_back("UnaryExpNot CodeIR"); }

void IntConst::codeIR() { error_msgs.push_back("IntConst CodeIR"); }

void FloatConst::codeIR() { error_msgs.push_back("FloatConst CodeIR"); }

void StringConst::codeIR() { error_msgs.push_back("StringConst CodeIR"); }

void PrimaryExp_branch::codeIR() { exp->codeIR(); }

void assign_stmt::codeIR() { error_msgs.push_back("AssignStmt CodeIR"); }

void expr_stmt::codeIR() { exp->codeIR(); }

void block_stmt::codeIR() { error_msgs.push_back("BlockStmt CodeIR"); }

void ifelse_stmt::codeIR() { error_msgs.push_back("IfElseStmt CodeIR"); }

void if_stmt::codeIR() { error_msgs.push_back("IfStmt CodeIR"); }

void while_stmt::codeIR() { error_msgs.push_back("WhileStmt CodeIR"); }

void continue_stmt::codeIR() { error_msgs.push_back("ContinueStmt CodeIR"); }

void break_stmt::codeIR() { error_msgs.push_back("BreakStmt CodeIR"); }

void return_stmt::codeIR() { error_msgs.push_back("ReturnStmt CodeIR"); }

void return_stmt_void::codeIR() { error_msgs.push_back("ReturnStmtVoid CodeIR"); }

void ConstInitVal::codeIR() { error_msgs.push_back("ConstInitVal CodeIR"); }

void ConstInitVal_exp::codeIR() { error_msgs.push_back("ConstInitValWithExp CodeIR"); }

void VarInitVal::codeIR() { error_msgs.push_back("VarInitVal CodeIR"); }

void VarInitVal_exp::codeIR() { error_msgs.push_back("VarInitValWithExp CodeIR"); }

void VarDef_no_init::codeIR() {
    error_msgs.push_back("VarDefNoInit CodeIR");
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
            //  3、分配->默认值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            if (type_decl == Type::INT)
                IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, 0, 0, ++reg_now);
            else if (type_decl == Type::FLOAT)
                IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, 0, 0, ++reg_now);
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
        //  3、分配->memset函数初始化值
        // void *memset(起始地址, 每字节初始化的值, 字节数, 对齐方式);
        IRgenAllocaArray(B0, Type2LLvm[type_decl], reg_now, val.dims);
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
    error_msgs.push_back("VarDef CodeIR");
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
            //  3、分配->声明值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            init->codeIR();    // 执行后，regnow储存初始化值
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
        // 元素已经储存在了std::vector<int> IntInitVals或 std::vector<float> FloatInitVals中，大小已经确保和size匹配
        for (int i = 0; i < size; ++i) {
            // i -> 索引
            // arrayindexs储存地址
            // int a[3][2][2]
            // i = 10 -> arrayindexs:{2,1,0}
            std::vector<Operand> arrayindexs;
            int remainder = i;
            for (int d = val.dims.size() - 1; d >= 0; --d) {
                int idx = remainder % val.dims[d];
                remainder /= val.dims[d];
                arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(idx));
            }
            // 计算应当赋值的地址
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, GetNewRegOperand(reg_array), val.dims,
                                       arrayindexs);
            int reg_adr = reg_now;    // 存放应当赋值的地址
            // 获取初始化值
            if (val.type == Type::INT) {
                IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, IntInitVals[i], 0, ++reg_now);
            } else if (val.type == Type::FLOAT) {
                IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, FloatInitVals[i], 0, ++reg_now);
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
    error_msgs.push_back("ConstDef CodeIR");
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
            irgen_table.reg_table[reg_now];
            //  3、分配->声明值->储存
            IRgenAlloca(B0, Type2LLvm[type_decl], reg_now);
            int reg_alloc = reg_now;
            init->codeIR();    // 执行后，regnow储存初始化值
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
        // 元素已经储存在了std::vector<int> IntInitVals或 std::vector<float> FloatInitVals中，大小已经确保和size匹配
        for (int i = 0; i < size; ++i) {
            // i -> 索引
            // arrayindexs储存地址
            // int a[3][2][2]
            // i = 10 -> arrayindexs:{2,1,0}
            std::vector<Operand> arrayindexs;
            int remainder = i;
            for (int d = val.dims.size() - 1; d >= 0; --d) {
                int idx = remainder % val.dims[d];
                remainder /= val.dims[d];
                arrayindexs.insert(arrayindexs.begin(), new ImmI32Operand(idx));
            }
            // 计算应当赋值的地址
            IRgenGetElementptrIndexI32(B, Type2LLvm[val.type], ++reg_now, GetNewRegOperand(reg_array), val.dims,
                                       arrayindexs);
            int reg_adr = reg_now;    // 存放应当赋值的地址
            // 获取初始化值
            if (val.type == Type::INT) {
                IRgenArithmeticI32ImmAll(B, BasicInstruction::ADD, IntInitVals[i], 0, ++reg_now);
            } else if (val.type == Type::FLOAT) {
                IRgenArithmeticF32ImmAll(B, BasicInstruction::FADD, FloatInitVals[i], 0, ++reg_now);
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
    error_msgs.push_back("VarDecl CodeIR");
    for (auto &var_def : *var_def_list) {
        var_def->codeIR();
    }
}

void ConstDecl::codeIR() {
    error_msgs.push_back("ConstDecl CodeIR");
    for (auto &var_def : *var_def_list) {
        var_def->TypeCheck();
    }
}

void BlockItem_Decl::codeIR() { error_msgs.push_back("BlockItemDecl CodeIR"); }

void BlockItem_Stmt::codeIR() { error_msgs.push_back("BlockItemStmt CodeIR"); }

void __Block::codeIR() { error_msgs.push_back("Block CodeIR"); }

void __FuncFParam::codeIR() { error_msgs.push_back("FunctionFParam CodeIR"); }

void __FuncDef::codeIR() {
    error_msgs.push_back("FunctionDef CodeIR");
    /*
        // 符号表进入新的作用域
        irgen_table.symbol_table.enter_scope();

        // 函数定义的IR指令
        FuncDefInstruction FuncDefIns = new FunctionDefineInstruction(Type2LLvm[return_type], name->get_string());

        // Step 3: Initialize function-level metadata
        // 初始化寄存器和符号表的相关数据结构，用于管理函数内的变量和临时值。
        max_reg = -1;                            // 用于跟踪当前函数中分配的最大寄存器编号。
        irgen_table.RegTable.clear();            // 清空寄存器表以准备存储新的变量信息。
        irgen_table.FormalArrayTable.clear();    // 用于存储数组参数的信息。

        now_label = 0;                        // 当前基本块的标签。
        max_label = -1;                       // 当前函数分配的最大标签编号。
        function_now = FuncDefIns;            // 当前正在生成代码的函数。
        function_returntype = return_type;    // 函数返回类型。

        // Step 4: Register the function in the IR generator
        // 创建一个新的函数并将其注册到 IR 生成器中。
        llvmIR.NewFunction(function_now);
        LLVMBlock B = llvmIR.NewBlock(function_now, max_label);    // 为函数创建入口基本块。

        // Step 5: Process the function's formal parameters
        // 遍历函数参数列表，生成相应的 IR 指令并记录参数信息。
        auto formal_vector = *formals;         // 获取函数参数的列表。
        max_reg = formal_vector.size() - 1;    // 初始化寄存器计数器。

        for (int i = 0; i < formal_vector.size(); ++i) {
            auto formal = formal_vector[i];
            VarAttribute val;    // 用于存储变量的属性（类型、数组维度等）。
            val.type = formal->type_decl;
            LLVMType lltype = Type2LLvm[formal->type_decl];    // 获取 LLVM 对应的类型。

            if (formal->dims != nullptr) {    // Step 5.1: If the parameter is an array
                // 对于数组参数，不需要为地址分配内存，只需记录其地址类型即可。
                FuncDefIns->InsertFormal(LLVMType::PTR);    // 插入数组指针作为参数类型。

                for (int i = 1; i < formal->dims->size(); ++i) {
                    // 忽略数组的第一个维度，因为它在 SysY 中是隐式传递的。
                    auto d = formal->dims->at(i);
                    val.dims.push_back(d->attribute.V.val.IntVal);    // 存储数组的后续维度信息。
                }

                // 更新符号表和寄存器表，以记录数组的相关信息。
                irgen_table.FormalArrayTable[i] = 1;    // 标记当前参数为数组。
                irgen_table.symbol_table.add_Symbol(formal->name, i);
                irgen_table.RegTable[i] = val;
            } else {    // Step 5.2: If the parameter is not an array
                // 普通参数需要分配空间，并生成 Alloca 和 Store 指令。
                FuncDefIns->InsertFormal(lltype);                                         // 插入普通类型的参数。
                IRgenAlloca(B, lltype, ++max_reg);                                        // 为参数分配内存。
                IRgenStore(B, lltype, GetNewRegOperand(i), GetNewRegOperand(max_reg));    // 将参数存入分配的寄存器。

                // 更新符号表和寄存器表。
                irgen_table.symbol_table.add_Symbol(formal->name, max_reg);
                irgen_table.RegTable[max_reg] = val;
            }
        }

        // Step 6: Add an unconditional branch to the next block
        // 生成一个无条件跳转到函数体的基本块。
        IRgenBRUnCond(B, 1);

        // Step 7: Generate IR for the function body
        // 为函数创建一个新的基本块，并生成函数体的 IR 代码。
        B = llvmIR.NewBlock(function_now, max_label);
        now_label = max_label;    // 更新当前基本块标签。
        block->codeIR();          // 调用函数体的 `codeIR` 方法，生成其 IR 指令。

        // Step 8: Handle no-return blocks
        // 如果函数未明确返回值，需要补充一个默认的返回基本块。
        AddNoReturnBlock();

        // Step 9: Record the maximum register and label for this function
        // 记录当前函数的最大寄存器编号和标签编号，用于后续分析和优化。
        max_reg_map[FuncDefIns] = max_reg;
        max_label_map[FuncDefIns] = max_label;

        // Step 10: Exit the function's scope
        // 离开当前函数的作用域，清除局部变量信息。
        irgen_table.symbol_table.exit_scope();
        */
}

void CompUnit_Decl::codeIR() {
    error_msgs.push_back("CompUnitDecl CodeIR");
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
