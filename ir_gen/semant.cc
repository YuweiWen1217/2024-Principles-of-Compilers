#include "semant.h"
#include "../include/SysY_tree.h"
#include "../include/ir.h"
#include "../include/type.h"
/*
    语义分析阶段需要对语法树节点上的类型和常量等信息进行标注, 即NodeAttribute类
    同时还需要标注每个变量的作用域信息，即部分语法树节点中的scope变量
    你可以在utils/ast_out.cc的输出函数中找到你需要关注哪些语法树节点中的NodeAttribute类及其他变量
    以及对语义错误的代码输出报错信息
*/

/*
    错误检查的基本要求:
    • 检查 main 函数是否存在 (根据SysY定义，如果不存在main函数应当报错)；
    • 检查未声明变量，及在同一作用域下重复声明的变量；
    • 条件判断和运算表达式：int 和 bool 隐式类型转换（例如int a=5，return a+!a）；
    • 数值运算表达式：运算数类型是否正确 (如返回值为 void 的函数调用结果是否参与了其他表达式的计算)；
    • 检查未声明函数，及函数形参是否与实参类型及数目匹配；
    • 检查是否存在整型变量除以整型常量0的情况 (如对于表达式a/(5-4-1)，编译器应当给出警告或者直接报错)；

    错误检查的进阶要求:
    • 对数组维度进行相应的类型检查 (例如维度是否有浮点数，定义维度时是否为常量等)；
    • 对float进行隐式类型转换以及其他float相关的检查 (例如模运算中是否有浮点类型变量等)；
*/
extern LLVMIR llvmIR;
extern std::string type_status[5];

SemantTable semant_table;
std::vector<std::string> error_msgs{};    // 将语义错误信息保存到该变量中

// int a[5][4][3] = { { {2,3}, 6,7,5,4,3,2,11,2,4 },7,8,11 };
// 递归完成数组元素值的初始化的思想参考自SysY相关程序的66～102行。
void HandleArrayInit(InitVal init, VarAttribute &val, int count, int handled) {
    int beforeadd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
    for (InitVal iv : *(init->GetList())) {

        if (iv->IsExp()) {
            if (val.type == Type::INT) {
                if (iv->attribute.T.type == Type::INT) {
                    val.IntInitVals.push_back(iv->attribute.V.val.IntVal);
                } else if (iv->attribute.T.type == Type::FLOAT) {
                    val.IntInitVals.push_back(static_cast<int>(iv->attribute.V.val.FloatVal));
                }
            } else if (val.type == Type::FLOAT) {
                if (iv->attribute.T.type == Type::INT) {
                    val.FloatInitVals.push_back(static_cast<float>(iv->attribute.V.val.IntVal));
                } else if (iv->attribute.T.type == Type::FLOAT) {
                    val.FloatInitVals.push_back(iv->attribute.V.val.FloatVal);
                }
            }
        } else {
            int h = 1;
            for (auto d : val.dims) {
                if (h <= handled)
                    h *= d;
            }
            // 如果是多维数组，则递归处理
            HandleArrayInit(iv, val, count, h);
        }
    }
    int afteradd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
    int realcount = afteradd - beforeadd;
    if (realcount > count) {
        error_msgs.push_back("行" + std::to_string(init->GetLineNumber()) + "：数组初始值太多。");
    }
    while (realcount < count / handled) {
        if (val.type == Type::INT) {
            val.IntInitVals.push_back(0);
        } else {
            val.FloatInitVals.push_back(0);
        }
        realcount++;
    }
}

void __Program::TypeCheck() {
    error_msgs.push_back("Program Semant");
    // 进入一个新的作用域
    semant_table.symbol_table.enter_scope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
}

void Exp::TypeCheck() {
    error_msgs.push_back("Exp Semant");
    addexp->TypeCheck();

    attribute = addexp->attribute;
}

void AddExp_plus::TypeCheck() {
    error_msgs.push_back("AddExp_plus Semant");
    addexp->TypeCheck();
    mulexp->TypeCheck();
    std::cout << addexp->attribute.V.val.IntVal << " " << mulexp->attribute.V.val.IntVal << std::endl;
    if (addexp->attribute.T.type == Type::INT && mulexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;    // 加法结果为整型
        attribute.V.val.IntVal = addexp->attribute.V.val.IntVal + mulexp->attribute.V.val.IntVal;
    } else if (addexp->attribute.T.type == Type::FLOAT || mulexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;    // 如果有浮点数则结果为浮点型
        float addexp_val = (addexp->attribute.T.type == Type::INT) ? static_cast<float>(addexp->attribute.V.val.IntVal)
                                                                   : addexp->attribute.V.val.FloatVal;
        float mulexp_val = (mulexp->attribute.T.type == Type::INT) ? static_cast<float>(mulexp->attribute.V.val.IntVal)
                                                                   : mulexp->attribute.V.val.FloatVal;
        attribute.V.val.FloatVal = addexp_val + mulexp_val;    // 计算浮点数相加
    } else {
        error_msgs.push_back("Type " + type_status[addexp->attribute.T.type] + " and " +
                             type_status[mulexp->attribute.T.type] + " mismatch in AddExp_plus at line " +
                             std::to_string(line_number) + ".");
    }
}

void AddExp_sub::TypeCheck() {
    error_msgs.push_back("AddExp_sub Semant");
    addexp->TypeCheck();
    mulexp->TypeCheck();
    if (addexp->attribute.T.type == Type::INT && mulexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        attribute.V.val.IntVal = addexp->attribute.V.val.IntVal - mulexp->attribute.V.val.IntVal;
    } else if (addexp->attribute.T.type == Type::FLOAT || mulexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        float addexp_val = (addexp->attribute.T.type == Type::INT) ? static_cast<float>(addexp->attribute.V.val.IntVal)
                                                                   : addexp->attribute.V.val.FloatVal;
        float mulexp_val = (mulexp->attribute.T.type == Type::INT) ? static_cast<float>(mulexp->attribute.V.val.IntVal)
                                                                   : mulexp->attribute.V.val.FloatVal;
        attribute.V.val.FloatVal = addexp_val - mulexp_val;
    } else {
        error_msgs.push_back("Type " + type_status[addexp->attribute.T.type] + " and " +
                             type_status[mulexp->attribute.T.type] + " mismatch in AddExp_sub at line " +
                             std::to_string(line_number) + ".");
    }
}

void MulExp_mul::TypeCheck() {
    error_msgs.push_back("MulExp_mul Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    if (mulexp->attribute.T.type == Type::INT && unary_exp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        attribute.V.val.IntVal = mulexp->attribute.V.val.IntVal * unary_exp->attribute.V.val.IntVal;
    } else if (mulexp->attribute.T.type == Type::FLOAT || unary_exp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        float mulexp_val = (mulexp->attribute.T.type == Type::INT) ? static_cast<float>(mulexp->attribute.V.val.IntVal)
                                                                   : mulexp->attribute.V.val.FloatVal;
        float unary_val = (unary_exp->attribute.T.type == Type::INT)
                          ? static_cast<float>(unary_exp->attribute.V.val.IntVal)
                          : unary_exp->attribute.V.val.FloatVal;
        attribute.V.val.FloatVal = mulexp_val * unary_val;
    } else {
        error_msgs.push_back("Type " + type_status[mulexp->attribute.T.type] + " and " +
                             type_status[unary_exp->attribute.T.type] + " mismatch in MulExp_mul at line " +
                             std::to_string(line_number) + ".");
    }
}

void MulExp_div::TypeCheck() {
    error_msgs.push_back("MulExp_div Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();
    // 如果两个int相除，其结果也有可能为浮点数，因此，此处均用浮点数储存，在上一级进行类型检查。如果相除确为整数（小数部分为0），再转换为整数。
    if ((mulexp->attribute.T.type == Type::INT || mulexp->attribute.T.type == Type::FLOAT) &&
        (unary_exp->attribute.T.type == Type::INT || unary_exp->attribute.T.type == Type::FLOAT)) {
        float mulexp_val = (mulexp->attribute.T.type == Type::INT) ? static_cast<float>(mulexp->attribute.V.val.IntVal)
                                                                   : mulexp->attribute.V.val.FloatVal;
        float unary_val = (unary_exp->attribute.T.type == Type::INT)
                          ? static_cast<float>(unary_exp->attribute.V.val.IntVal)
                          : unary_exp->attribute.V.val.FloatVal;

        if (unary_val == 0) {
            error_msgs.push_back("Division by zero error in MulExp_div at line " + std::to_string(line_number) + ".");
            return;
        }
        float floatresult = mulexp_val / unary_val;
        int intresult = mulexp_val / unary_val;
        if (std::abs(floatresult - intresult) < 1e5) {
            attribute.V.val.IntVal = intresult;
            attribute.T.type = Type::INT;
        } else {
            attribute.V.val.FloatVal = floatresult;
            attribute.T.type = Type::FLOAT;
        }
    } else {
        error_msgs.push_back("Type " + type_status[mulexp->attribute.T.type] + " and " +
                             type_status[unary_exp->attribute.T.type] + " mismatch in MulExp_div at line " +
                             std::to_string(line_number) + ".");
    }
}

void MulExp_mod::TypeCheck() {
    error_msgs.push_back("MulExp_mod Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    // 浮点数没有模运算
    if (mulexp->attribute.T.type == Type::INT && unary_exp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
    } else {
        error_msgs.push_back("Type " + type_status[mulexp->attribute.T.type] + " and " +
                             type_status[unary_exp->attribute.T.type] + " mismatch in MulExp_mod at line " +
                             std::to_string(line_number) + ".");
    }
}

void RelExp_leq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void RelExp_lt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void RelExp_geq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void RelExp_gt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void EqExp_eq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void EqExp_neq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void LAndExp_and::TypeCheck() {
    landexp->TypeCheck();
    eqexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void LOrExp_or::TypeCheck() {
    lorexp->TypeCheck();
    landexp->TypeCheck();

    error_msgs.push_back("BinaryExp Semant");
}

void ConstExp::TypeCheck() {
    addexp->TypeCheck();
    attribute = addexp->attribute;
    if (!attribute.V.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(line_number) + "\n");
    }
}

void Lval::TypeCheck() {
    error_msgs.push_back("Lval Semant");
    // 检查符号是否在符号表中定义
    scope = semant_table.symbol_table.lookup_scope(name);
    VarAttribute varAttr;
    if (scope == -1) {
        if (semant_table.GlobalTable.find(name) != semant_table.GlobalTable.end()) {
            varAttr = semant_table.GlobalTable[name];
            scope = 0;
        } else
            error_msgs.push_back("Error: Undefined variable at line " + std::to_string(line_number) + ".");
    } else {
        varAttr = semant_table.symbol_table.lookup_val(name);
    }

    // 如果dims不为空，进行数组下标的检查(维度数量，以及每个维度的类型、大小)
    if (dims != nullptr) {
        // 检查维度数量
        if (dims->size() > varAttr.dims.size()) {
            error_msgs.push_back("Error: Array dimension mismatch for variable. Expected " +
                                 std::to_string(varAttr.dims.size()) + ", but got " + std::to_string(dims->size()) +
                                 " at line " + std::to_string(line_number) + ".");
        } else if (dims->size() < varAttr.dims.size()) {
            attribute.V.ConstTag = false;
            attribute.T.type = Type::PTR;
            return;
        } else {
            // 检查每个维度的类型
            for (size_t i = 0; i < dims->size(); ++i) {
                auto &dim = (*dims)[i];
                dim->TypeCheck();
                if (dim->attribute.T.type != Type::INT) {
                    error_msgs.push_back("Error: Array index must be of type int at line " +
                                         std::to_string(line_number) + ".");
                }
                // 左值表达式中，数组下标不要求为const，因此不做常值检查
                // 检查下标是否在有效范围内
                int indexValue = dim->attribute.V.val.IntVal;
                int arraySize = varAttr.dims[i];
                if (indexValue >= arraySize) {
                    error_msgs.push_back("Error: Array index " + std::to_string(indexValue) +
                                         " out of bounds for variable  with size " + std::to_string(arraySize) +
                                         " at line " + std::to_string(line_number) + ".");
                }
            }
        }
    }
    // 类型赋值
    attribute.T.type = varAttr.type;
    // 数值赋值
    if (dims == nullptr) {
        // 非数组变量，直接赋值
        if (varAttr.type == Type::INT) {
            attribute.V.val.IntVal = varAttr.IntInitVals[0];
        } else if (varAttr.type == Type::FLOAT) {
            attribute.V.val.FloatVal = varAttr.FloatInitVals[0];
        }
    } else {
        // 数组变量，计算下标进行赋值
        int total = 1;
        for (size_t i = 0; i < varAttr.dims.size(); i++) {
            total *= varAttr.dims[i];
        }
        int offset = total;
        int index = 1;
        for (size_t i = 0; i < dims->size(); ++i) {
            total /= varAttr.dims[i];
            index += (*dims)[i]->attribute.V.val.IntVal * total;
        }
        if (index >= total) {
            error_msgs.push_back("The array index is out of range at line " + std::to_string(line_number) + ".");
            return;
        }
        if (varAttr.type == Type::INT) {
            attribute.V.val.IntVal = varAttr.IntInitVals[index];
        } else if (varAttr.type == Type::FLOAT) {
            attribute.V.val.FloatVal = varAttr.FloatInitVals[index];
        }
    }
}

void FuncRParams::TypeCheck() { error_msgs.push_back("FuncRParams Semant"); }

void Func_call::TypeCheck() { error_msgs.push_back("FunctionCall Semant"); }

void UnaryExp_plus::TypeCheck() { error_msgs.push_back("UnaryExp Semant"); }

void UnaryExp_neg::TypeCheck() { error_msgs.push_back("UnaryExp Semant"); }

void UnaryExp_not::TypeCheck() { error_msgs.push_back("UnaryExp Semant"); }

void IntConst::TypeCheck() {
    attribute.T.type = Type::INT;
    attribute.V.ConstTag = true;
    attribute.V.val.IntVal = val;
}

void FloatConst::TypeCheck() {
    attribute.T.type = Type::FLOAT;
    attribute.V.ConstTag = true;
    attribute.V.val.FloatVal = val;
}

void StringConst::TypeCheck() { error_msgs.push_back("StringConst Semant"); }

void PrimaryExp_branch::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void assign_stmt::TypeCheck() { error_msgs.push_back("AssignStmt Semant"); }

void expr_stmt::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void block_stmt::TypeCheck() { b->TypeCheck(); }

void ifelse_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
    elsestmt->TypeCheck();
}

void if_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
}

void while_stmt::TypeCheck() { error_msgs.push_back("WhileStmt Semant"); }

void continue_stmt::TypeCheck() { error_msgs.push_back("ContinueStmt Semant"); }

void break_stmt::TypeCheck() { error_msgs.push_back("BreakStmt Semant"); }

void return_stmt::TypeCheck() { return_exp->TypeCheck(); }

void return_stmt_void::TypeCheck() {}

void ConstInitVal::TypeCheck() { error_msgs.push_back("ConstInitVal Semant"); }

void ConstInitVal_exp::TypeCheck() { error_msgs.push_back("ConstInitValExp Semant"); }

void VarInitVal::TypeCheck() {
    error_msgs.push_back("VarInitVal Semant");
    for (auto init : *initval) {
        init->TypeCheck();
    }
}

void VarInitVal_exp::TypeCheck() {
    error_msgs.push_back("VarInitValExp Semant");
    exp->TypeCheck();
    attribute.T.type = exp->attribute.T.type;
    attribute.V = exp->attribute.V;
}

void VarDef_no_init::TypeCheck() { error_msgs.push_back("VarDefNoInit Semant"); }

void VarDef::TypeCheck() {
    error_msgs.push_back("VarDef Semant");
    // 检查同名
    if (semant_table.symbol_table.lookup_scope(name) != -1) {
        error_msgs.push_back("Variable " + name->get_string() + " is redefined at line " + std::to_string(line_number) +
                             "\n");
    }

    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    init->TypeCheck();

    // 数组检查
    if (dims != nullptr) {
        for (auto dim : *dims) {
            dim->TypeCheck();
            if (!dim->attribute.V.ConstTag) {
                error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                     "\n");
            }
            if (dim->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            }
            val.dims.push_back(dim->attribute.V.val.IntVal);
        }
        // 处理数组初始化
        int totalElements = 1;
        for (int dim : val.dims) {
            totalElements *= dim;    // 计算总元素数
        }
        HandleArrayInit(init, val, totalElements, 1);
        // 确保初始化值的数量匹配
        if (val.IntInitVals.size() != totalElements && val.FloatInitVals.size() != totalElements) {
            error_msgs.push_back("Error: Initialization values count does not match array dimensions at line " +
                                 std::to_string(line_number) + ".");
        }
        // std::cout << val.IntInitVals.size() << "1111111111111" << std::endl;
    }
    // 非数组检查
    else {
        if (init->attribute.T.type != val.type) {
            if (val.type == Type::INT && init->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("Warning: Implicit conversion from float to int at line " +
                                     std::to_string(line_number) + ". Fractional part will be discarded.");
                attribute.V.val.IntVal = static_cast<int>(init->attribute.V.val.FloatVal);
            } else if (val.type == Type::FLOAT && init->attribute.T.type == Type::INT) {
                error_msgs.push_back("Warning: Implicit conversion from int to float at line " +
                                     std::to_string(line_number) + ". Fractional part will be discarded.");
                attribute.V.val.FloatVal = static_cast<float>(init->attribute.V.val.IntVal);
            } else {
                error_msgs.push_back("Type mismatch in initialization at line " + std::to_string(line_number) +
                                     ". Cannot convert from " + type_status[init->attribute.T.type] + " to " +
                                     type_status[val.type] + ".");
            }
        }
        if (val.type == Type::INT) {
            val.IntInitVals.push_back(init->attribute.V.val.IntVal);
        } else if (val.type == Type::FLOAT) {
            val.FloatInitVals.push_back(init->attribute.V.val.FloatVal);
        }
    }
    attribute.V.val = init->attribute.V.val;
    semant_table.symbol_table.add_Symbol(name, val);
}

void ConstDef::TypeCheck() { error_msgs.push_back("ConstDef Semant"); }

void VarDecl::TypeCheck() {
    error_msgs.push_back("VarDecl Semant");
    // 检查变量定义列表是否为空
    if (var_def_list == nullptr) {
        error_msgs.push_back("Variable declaration list is null.");
        return;
    }
    // 遍历变量定义列表，进行类型检查
    for (auto &var_def : *var_def_list) {
        var_def->type_decl = type_decl;
        var_def->TypeCheck();
    }
}

void ConstDecl::TypeCheck() { error_msgs.push_back("ConstDecl Semant"); }

void BlockItem_Decl::TypeCheck() {
    error_msgs.push_back("BlockItem_Decl Semant");

    decl->TypeCheck();
}

void BlockItem_Stmt::TypeCheck() {

    error_msgs.push_back("BlockItem_Decl Semant");

    stmt->TypeCheck();
}

void __Block::TypeCheck() {
    error_msgs.push_back("Block Semant");

    semant_table.symbol_table.enter_scope();
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.exit_scope();
}

void __FuncFParam::TypeCheck() {
    error_msgs.push_back("FuncFParam Semant");
    // 创建一个变量属性对象 val，用于存储当前函数形参的属性
    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    // 将作用域设置为 1，表示当前参数在一个特定的作用域内
    scope = 1;
    // 检查参数是否为数组类型
    if (dims != nullptr) {
        auto dim_vector = *dims;    // 取出数组维度列表

        // 对于函数参数，第一维通常为空，例如 int f(int A[][30][40])
        val.dims.push_back(-1);    // 使用 -1 表示空维度
        // 从第二维开始检查每个维度
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();    // 对当前维度进行类型检查
            // 检查维度是否为常量表达式
            if (d->attribute.V.ConstTag == false) {
                error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                     "\n");
            }
            // 检查维度类型是否为浮点型
            if (d->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            }
            val.dims.push_back(d->attribute.V.val.IntVal);
        }
        // 如果参数是数组，则将类型设置为指针
        attribute.T.type = Type::PTR;
    } else {
        // 如果不是数组，则直接设置为声明的类型
        attribute.T.type = type_decl;
    }
    // 检查参数名称是否为 nullptr
    if (name != nullptr) {
        // 检查当前作用域中是否已经存在同名的参数
        if (semant_table.symbol_table.lookup_scope(name) != -1) {

            error_msgs.push_back("multiple definitions of formals in function " + name->get_string() + " in line " +
                                 std::to_string(line_number) + "\n");
        }
        // 将参数的名称和属性添加到符号表中
        semant_table.symbol_table.add_Symbol(name, val);
    }
}

void __FuncDef::TypeCheck() {

    error_msgs.push_back("FuncDef Semant");

    semant_table.symbol_table.enter_scope();

    // 函数表是直接通过函数名进行储存的，不支持函数重载，需要检查函数是否重复声明
    if (semant_table.FunctionTable.find(name) != semant_table.FunctionTable.end()) {
        error_msgs.push_back("Function " + name->get_string() + " is redefined at line " + std::to_string(line_number) +
                             "\n");
    }
    semant_table.FunctionTable[name] = this;

    // 获取函数的参数列表，并对每个参数进行类型检查
    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();
    }
    // 如果函数体存在，则进行块内的类型检查
    if (block != nullptr) {
        auto item_vector = *(block->item_list);
        for (auto item : item_vector) {
            item->TypeCheck();
        }
    }
    // 退出当前作用域，恢复到上一个作用域
    semant_table.symbol_table.exit_scope();
}

void CompUnit_Decl::TypeCheck() { error_msgs.push_back("CompUnitDecl Semant"); }

void CompUnit_FuncDef::TypeCheck() {

    error_msgs.push_back("CompUnit_FuncDef Semant");

    func_def->TypeCheck();
}