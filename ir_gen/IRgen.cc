#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

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
      if (type_src == Type::FLOAT&& type_dst == Type::FLOAT) {
        IRgenSitofp(B, src, dst);  // Conversely, we use sitofp for float to int
    } else if (type_src == Type::INT && type_dst == Type::FLOAT) {
        IRgenFptosi(B, src, dst);  // Use fptosi for int to float
    } else if (type_src == Type::BOOL && type_dst == Type::INT) {
        IRgenZextI1toI32(B, src, dst);  // Zero extend bool to int
    } else {
        // Handle other type conversions as needed
        assert(false && "Unsupported type conversion");
    }
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
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() { addexp->codeIR(); }

void AddExp_plus::codeIR() {  // 生成左操作数和右操作数的代码
    addexp->codeIR();
    mulexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果


    // 获取新的寄存器编号，用于存储加法结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数加法还是整数加法
    if (type == Type::FLOAT) {
        IRgenArithmeticF32(currentBlock, BasicInstruction::FADD, r0, r1, r2->GetRegNo());
    } else {
        IRgenArithmeticI32(currentBlock, BasicInstruction::ADD, r0, r1, r2->GetRegNo());
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void AddExp_sub::codeIR() { 
    // 生成左操作数和右操作数的代码
    addexp->codeIR();
    mulexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储减法结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数减法还是整数减法
    if (type == Type::FLOAT) {
        IRgenArithmeticF32(currentBlock, BasicInstruction::FSUB, r0, r1, r2->GetRegNo());
    } else {
        IRgenArithmeticI32(currentBlock, BasicInstruction::SUB, r0, r1, r2->GetRegNo());
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void MulExp_mul::codeIR() { 
     // 生成左操作数和右操作数的代码
    mulexp->codeIR();
    unary_exp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储乘法结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数乘法还是整数乘法
    if (type == Type::FLOAT) {
        IRgenArithmeticF32(currentBlock, BasicInstruction::FMUL, r0, r1, r2->GetRegNo());
    } else {
        IRgenArithmeticI32(currentBlock, BasicInstruction::MUL, r0, r1, r2->GetRegNo());
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void MulExp_div::codeIR() { 
    // 生成左操作数和右操作数的代码
    mulexp->codeIR();
    unary_exp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储除法结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数除法还是整数除法
    if (type == Type::FLOAT) {
        IRgenArithmeticF32(currentBlock, BasicInstruction::FDIV, r0, r1, r2->GetRegNo());
    } else {
        IRgenArithmeticI32(currentBlock, BasicInstruction::DIV, r0, r1, r2->GetRegNo());
         }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}


void MulExp_mod::codeIR() { 
     // 生成左操作数和右操作数的代码
    mulexp->codeIR();
    unary_exp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储取模结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 取模操作仅适用于整数类型
    if (type == Type::FLOAT) {
        // 假设不允许浮点数取模，这里可以根据需求处理错误
        throw std::runtime_error("Modulo operation not allowed on floating point numbers");
    } else {
        IRgenArithmeticI32(currentBlock, BasicInstruction::MOD, r0, r1, r2->GetRegNo());
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}




void RelExp_leq::codeIR() {
    // 生成左操作数和右操作数的代码
    relexp->codeIR();
    addexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::OLE, r0, r1, r2->GetRegNo()); // 浮点数 <= (Ordered Less or Equal)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::sle, r0, r1, r2->GetRegNo()); // 整数 <= (signed less or equal)
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void RelExp_lt::codeIR() {
    // 生成左操作数和右操作数的代码
    relexp->codeIR();
    addexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::OLT, r0, r1, r2->GetRegNo()); // 浮点数 < (Ordered Less Than)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::slt, r0, r1, r2->GetRegNo()); // 整数 < (signed less than)
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void RelExp_geq::codeIR() {
    // 生成左操作数和右操作数的代码
    relexp->codeIR();
    addexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::OGE, r0, r1, r2->GetRegNo()); // 浮点数 >= (Ordered Greater or Equal)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::sge, r0, r1, r2->GetRegNo()); // 整数 >= (signed greater or equal)
    }

        // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void RelExp_gt::codeIR() {
    // 生成左操作数和右操作数的代码
    relexp->codeIR();
    addexp->codeIR();

    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::OGT, r0, r1, r2->GetRegNo()); // 浮点数 > (Ordered Greater Than)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::sgt, r0, r1, r2->GetRegNo()); // 整数 > (signed greater than)
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void EqExp_eq::codeIR() {
    // 生成左操作数和右操作数的代码
    eqexp->codeIR();
    relexp->codeIR();


    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::OEQ, r0, r1, r2->GetRegNo()); // 浮点数 == (Ordered and Equal)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::eq, r0, r1, r2->GetRegNo()); // 整数 == (equal)
    }

    // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}

void EqExp_neq::codeIR() {
    // 生成左操作数和右操作数的代码
    eqexp->codeIR();
    relexp->codeIR();


    // 获取最近使用的两个寄存器编号
    int r0 = irgen_table.getLastReg();   // 左操作数结果
    int r1 = irgen_table.getLastReg() - 1; // 右操作数结果

    // 获取新的寄存器编号，用于存储比较结果
    RegOperand* r2 = GetNewRegOperand(irgen_table.getNewRegNo());

    // 获取当前基本块
    LLVMBlock currentBlock = irgen_table.getCurrentBlock();

    // 根据操作数的类型，选择生成浮点数比较还是整数比较
    if (type == Type::FLOAT) {
        IRgenFcmp(currentBlock, BasicInstruction::FcmpCond::ONE, r0, r1, r2->GetRegNo()); // 浮点数 != (Ordered and Not Equal)
    } else {
        IRgenIcmp(currentBlock, BasicInstruction::IcmpCond::ne, r0, r1, r2->GetRegNo()); // 整数 != (not equal)
    }

        // 更新最后一个寄存器编号
    irgen_table.setLastReg(r2->GetRegNo());
}


    

// short circuit &&
void LAndExp_and::codeIR() {

    landexp->codeIR();                   
    eqexp->codeIR();                     
    int leftResultReg =irgen_table.getLastReg(); 
    int rightResultReg = irgen_table.getLastReg() - 1; 

    // 3. 执行逻辑与操作，结果存入 leftResultReg
    IRgenAnd(getCurrentBlock(), leftResultReg, leftResultReg, rightResultReg);

    
}

// short circuit ||
void LOrExp_or::codeIR() { TODO("LOrExpOr CodeIR"); }

void ConstExp::codeIR() { addexp->codeIR(); }

void Lval::codeIR() { TODO("Lval CodeIR"); }

void FuncRParams::codeIR() { TODO("FuncRParams CodeIR"); }

void Func_call::codeIR() { TODO("FunctionCall CodeIR"); }

void UnaryExp_plus::codeIR() { TODO("UnaryExpPlus CodeIR"); }

void UnaryExp_neg::codeIR() { TODO("UnaryExpNeg CodeIR"); }

void UnaryExp_not::codeIR() { TODO("UnaryExpNot CodeIR"); }

void IntConst::codeIR() { TODO("IntConst CodeIR"); }

void FloatConst::codeIR() { TODO("FloatConst CodeIR"); }

void StringConst::codeIR() { TODO("StringConst CodeIR"); }

void PrimaryExp_branch::codeIR() { exp->codeIR(); }

void assign_stmt::codeIR() { TODO("AssignStmt CodeIR"); }

void expr_stmt::codeIR() { exp->codeIR(); }

void block_stmt::codeIR() { TODO("BlockStmt CodeIR"); }

void ifelse_stmt::codeIR() { TODO("IfElseStmt CodeIR"); }

void if_stmt::codeIR() { TODO("IfStmt CodeIR"); }

void while_stmt::codeIR() { TODO("WhileStmt CodeIR"); }

void continue_stmt::codeIR() { TODO("ContinueStmt CodeIR"); }

void break_stmt::codeIR() { TODO("BreakStmt CodeIR"); }

void return_stmt::codeIR() { TODO("ReturnStmt CodeIR"); }

void return_stmt_void::codeIR() { TODO("ReturnStmtVoid CodeIR"); }

void ConstInitVal::codeIR() { TODO("ConstInitVal CodeIR"); }

void ConstInitVal_exp::codeIR() { TODO("ConstInitValWithExp CodeIR"); }

void VarInitVal::codeIR() { TODO("VarInitVal CodeIR"); }

void VarInitVal_exp::codeIR() { TODO("VarInitValWithExp CodeIR"); }

void VarDef_no_init::codeIR() { TODO("VarDefNoInit CodeIR"); }

void VarDef::codeIR() { TODO("VarDef CodeIR"); }

void ConstDef::codeIR() { TODO("ConstDef CodeIR"); }

void VarDecl::codeIR() { TODO("VarDecl CodeIR"); }

void ConstDecl::codeIR() { TODO("ConstDecl CodeIR"); }

void BlockItem_Decl::codeIR() { TODO("BlockItemDecl CodeIR"); }

void BlockItem_Stmt::codeIR() { TODO("BlockItemStmt CodeIR"); }

void __Block::codeIR() { TODO("Block CodeIR"); }

void __FuncFParam::codeIR() { TODO("FunctionFParam CodeIR"); }

void __FuncDef::codeIR() { TODO("FunctionDef CodeIR"); }

void CompUnit_Decl::codeIR() { TODO("CompUnitDecl CodeIR"); }

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
