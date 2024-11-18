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

int loopdepth = 0;
bool has_main = false;

Type::ty now_func_return_type;

// int a[5][4][3] = { { {2,3}, 6,7,5,4,3,2,11,2,4 },7,8,11 };
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
            int afteradd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
            int realcount = afteradd - beforeadd;
            if (realcount % (count / h) != 0) {
                error_msgs.push_back("ERROR: The number of initial values in the array is too small at line " +
                                     std::to_string(init->GetLineNumber()) + ".");
                return;
            }
            HandleArrayInit(iv, val, count, h);
        }
    }
    int afteradd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
    int realcount = afteradd - beforeadd;
    if (realcount > count / handled) {
        error_msgs.push_back("ERROR: The number of initial values in the array is excessive at line " +
                             std::to_string(init->GetLineNumber()) + ".");
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

void booltoint(Expression &exp) {
    if (exp->attribute.T.type != Type::BOOL)
        return;
    exp->attribute.T.type = Type::INT;
    exp->attribute.V.val.IntVal = exp->attribute.V.val.BoolVal ? 1 : 0;
}
void inttofloat(Expression &exp) {
    if (exp->attribute.T.type != Type::INT)
        return;
    exp->attribute.T.type = Type::FLOAT;
    exp->attribute.V.val.FloatVal = static_cast<float>(exp->attribute.V.val.IntVal);
}
void floattoint(Expression &exp) {
    if (exp->attribute.T.type != Type::FLOAT)
        return;
    exp->attribute.T.type = Type::INT;
    exp->attribute.V.val.IntVal = static_cast<int>(exp->attribute.V.val.FloatVal);
}

// 统一为相同的类型，优先级：float > int > bool，其中（bool，bool）会转换为（int，int）
void unifyTypes(Expression &exp1, Expression &exp2) {
    booltoint(exp1);
    booltoint(exp2);
    if (exp1->attribute.T.type == exp2->attribute.T.type) {
        return;
    } else if (exp1->attribute.T.type == Type::FLOAT || exp2->attribute.T.type == Type::FLOAT) {
        inttofloat(exp1);
        inttofloat(exp2);
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
    // 检查是否存在main函数
    if (!has_main) {
        error_msgs.push_back("ERROR: 'main' function is not defined.");
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

    unifyTypes(addexp, mulexp);
    attribute.V.ConstTag = addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (addexp->attribute.T.type == Type::INT && mulexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = addexp->attribute.V.val.IntVal + mulexp->attribute.V.val.IntVal;
    } else if (addexp->attribute.T.type == Type::FLOAT && mulexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = addexp->attribute.V.val.FloatVal + mulexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type " + type_status[addexp->attribute.T.type] + " and " +
                             type_status[mulexp->attribute.T.type] + " mismatch in AddExp_plus at line " +
                             std::to_string(line_number) + ".");
    }
}

void AddExp_sub::TypeCheck() {
    error_msgs.push_back("AddExp_sub Semant");
    addexp->TypeCheck();
    mulexp->TypeCheck();

    unifyTypes(addexp, mulexp);
    attribute.V.ConstTag = addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (addexp->attribute.T.type == Type::INT && mulexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = addexp->attribute.V.val.IntVal + mulexp->attribute.V.val.IntVal;
    } else if (addexp->attribute.T.type == Type::FLOAT && mulexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = addexp->attribute.V.val.FloatVal - mulexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type " + type_status[addexp->attribute.T.type] + " and " +
                             type_status[mulexp->attribute.T.type] + " mismatch in AddExp_sub at line " +
                             std::to_string(line_number) + ".");
    }
}

void MulExp_mul::TypeCheck() {
    error_msgs.push_back("MulExp_mul Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    unifyTypes(unary_exp, mulexp);
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (mulexp->attribute.T.type == Type::INT && unary_exp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = mulexp->attribute.V.val.IntVal * unary_exp->attribute.V.val.IntVal;
    } else if (mulexp->attribute.T.type == Type::FLOAT && unary_exp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = mulexp->attribute.V.val.FloatVal * unary_exp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type " + type_status[mulexp->attribute.T.type] + " and " +
                             type_status[unary_exp->attribute.T.type] + " mismatch in MulExp_mul at line " +
                             std::to_string(line_number) + ".");
    }
}

void MulExp_div::TypeCheck() {
    error_msgs.push_back("MulExp_div Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    unifyTypes(unary_exp, mulexp);
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (mulexp->attribute.T.type == Type::INT && unary_exp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (unary_exp->attribute.V.ConstTag && unary_exp->attribute.V.val.IntVal == 0) {
            error_msgs.push_back("ERROR: Division by zero error in MulExp_div at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = mulexp->attribute.V.val.IntVal / unary_exp->attribute.V.val.IntVal;
    } else if (mulexp->attribute.T.type == Type::FLOAT && unary_exp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (unary_exp->attribute.V.ConstTag && unary_exp->attribute.V.val.FloatVal == 0) {
            error_msgs.push_back("ERROR: Division by zero error in MulExp_div at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = mulexp->attribute.V.val.FloatVal / unary_exp->attribute.V.val.FloatVal;
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

    unifyTypes(unary_exp, mulexp);
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (mulexp->attribute.T.type == Type::INT && unary_exp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (unary_exp->attribute.V.val.IntVal == 0) {
            error_msgs.push_back("ERROR: Division by zero in '%' operation at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = mulexp->attribute.V.val.IntVal % unary_exp->attribute.V.val.IntVal;
    } else {
        error_msgs.push_back("ERROR: Type " + type_status[mulexp->attribute.T.type] + " and " +
                             type_status[unary_exp->attribute.T.type] + " mismatch in MulExp_mod at line " +
                             std::to_string(line_number) + ".");
    }
}

void RelExp_leq::TypeCheck() {
    error_msgs.push_back("<= Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    unifyTypes(relexp, addexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && addexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal <= addexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && addexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal <= addexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '<=' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_lt::TypeCheck() {
    error_msgs.push_back("< Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    unifyTypes(relexp, addexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && addexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal < addexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && addexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal < addexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '<' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_geq::TypeCheck() {
    error_msgs.push_back(">= Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    unifyTypes(relexp, addexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && addexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal >= addexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && addexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal >= addexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '>=' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_gt::TypeCheck() {
    error_msgs.push_back("> Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    unifyTypes(relexp, addexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && addexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal > addexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && addexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal > addexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '>' expression at line " + std::to_string(line_number) + ".");
    }
}

void EqExp_eq::TypeCheck() {
    error_msgs.push_back("== Semant");
    eqexp->TypeCheck();
    relexp->TypeCheck();

    unifyTypes(relexp, eqexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && eqexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal == eqexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && eqexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal == eqexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '==' expression at line " + std::to_string(line_number) + ".");
    }
}

void EqExp_neq::TypeCheck() {
    error_msgs.push_back("!= Semant");
    eqexp->TypeCheck();
    relexp->TypeCheck();

    unifyTypes(relexp, eqexp);
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (relexp->attribute.T.type == Type::INT && eqexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.IntVal != eqexp->attribute.V.val.IntVal;
    } else if (relexp->attribute.T.type == Type::FLOAT && eqexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = relexp->attribute.V.val.FloatVal != eqexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '!=' expression at line " + std::to_string(line_number) + ".");
    }
}

void LAndExp_and::TypeCheck() {
    error_msgs.push_back("&& Semant");
    landexp->TypeCheck();
    eqexp->TypeCheck();

    unifyTypes(landexp, eqexp);
    attribute.V.ConstTag = landexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (landexp->attribute.T.type == Type::INT && eqexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = landexp->attribute.V.val.IntVal && eqexp->attribute.V.val.IntVal;
    } else if (landexp->attribute.T.type == Type::FLOAT && eqexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = landexp->attribute.V.val.FloatVal && eqexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '&&' expression at line " + std::to_string(line_number) + ".");
    }
}

void LOrExp_or::TypeCheck() {
    error_msgs.push_back("|| Semant");
    lorexp->TypeCheck();
    landexp->TypeCheck();

    unifyTypes(landexp, lorexp);
    attribute.V.ConstTag = landexp->attribute.V.ConstTag & lorexp->attribute.V.ConstTag;

    if (landexp->attribute.T.type == Type::INT && lorexp->attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = landexp->attribute.V.val.IntVal || lorexp->attribute.V.val.IntVal;
    } else if (landexp->attribute.T.type == Type::FLOAT && lorexp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = landexp->attribute.V.val.FloatVal || lorexp->attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '||' expression at line " + std::to_string(line_number) + ".");
    }
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
            error_msgs.push_back("ERROR: Undefined variable at line " + std::to_string(line_number) + ".");
    } else {
        varAttr = semant_table.symbol_table.lookup_val(name);
    }

    bool constindex = true;
    // 如果符号表中查找到的变量的dims不为空，进行数组下标的检查(维度数量，以及每个维度的类型、大小)
    if (varAttr.dims.size() != 0) {
        // 检查维度数量
        if (dims == nullptr || dims->size() < varAttr.dims.size()) {
            if (dims != nullptr) {
                for (size_t i = 0; i < dims->size(); ++i) {
                    auto &dim = (*dims)[i];
                    dim->TypeCheck();
                    booltoint(dim);
                    if (dim->attribute.T.type != Type::INT) {
                        error_msgs.push_back("ERROR: Array index must be of type int at line " +
                                             std::to_string(line_number) + ".");
                    }
                    int indexValue = dim->attribute.V.val.IntVal;
                    int arraySize = varAttr.dims[i];
                    if (indexValue >= arraySize) {
                        error_msgs.push_back("ERROR: Array index " + std::to_string(indexValue) +
                                             " out of bounds for variable  with size " + std::to_string(arraySize) +
                                             " at line " + std::to_string(line_number) + ".");
                    }
                }
            }
            attribute.V.ConstTag = false;
            attribute.T.type = Type::PTR;
            ptrtype = varAttr.type == Type::INT ? Type::INT : Type::FLOAT;
            return;
        } else if (dims->size() > varAttr.dims.size()) {
            error_msgs.push_back("ERROR: Array dimension mismatch for variable. Expected " +
                                 std::to_string(varAttr.dims.size()) + ", but got " + std::to_string(dims->size()) +
                                 " at line " + std::to_string(line_number) + ".");
        } else {
            // 检查每个维度的类型
            for (size_t i = 0; i < dims->size(); ++i) {
                auto &dim = (*dims)[i];
                booltoint(dim);
                dim->TypeCheck();
                if (dim->attribute.T.type != Type::INT) {
                    error_msgs.push_back("ERROR: Array index must be of type int at line " +
                                         std::to_string(line_number) + ".");
                }

                // 左值表达式中，数组下标不要求为const，因此不做常值检查，但需要确认全部是常量还是有变量，以便后面赋值
                constindex &= dim->attribute.V.ConstTag;

                // 检查下标是否在有效范围内
                int indexValue = dim->attribute.V.val.IntVal;
                int arraySize = varAttr.dims[i];
                if (indexValue >= arraySize) {
                    error_msgs.push_back("ERROR: Array index " + std::to_string(indexValue) +
                                         " out of bounds for variable  with size " + std::to_string(arraySize) +
                                         " at line " + std::to_string(line_number) + ".");
                }
            }
        }
    }

    // 类型赋值
    attribute.T.type = varAttr.type;
    // 数值赋值
    attribute.V.ConstTag = constindex & varAttr.ConstTag;
    if (attribute.V.ConstTag) {
        if (dims == nullptr) {
            // 左值非数组，直接赋值
            if (varAttr.type == Type::INT) {
                attribute.V.val.IntVal = varAttr.IntInitVals[0];
            } else if (varAttr.type == Type::FLOAT) {
                attribute.V.val.FloatVal = varAttr.FloatInitVals[0];
            }
        } else {
            // 左值为数组，计算下标进行赋值
            int total = 1;
            for (size_t i = 0; i < varAttr.dims.size(); i++) {
                total *= varAttr.dims[i];
            }
            int offset = total;
            int index = 1;
            for (size_t i = 0; i < dims->size(); ++i) {
                offset /= varAttr.dims[i];
                index += (*dims)[i]->attribute.V.val.IntVal * offset;
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
}

void FuncRParams::TypeCheck() {
    // 由于需要用到父节点的相关成员变量，因此该函数内容直接在Func_call中实现。
    error_msgs.push_back("FuncRParams Semant");
}

void Func_call::TypeCheck() {
    error_msgs.push_back("FunctionCall Semant");

    // 检查函数是否存在
    auto funcIt = semant_table.FunctionTable.find(name);
    if (funcIt == semant_table.FunctionTable.end()) {
        error_msgs.push_back("ERROR: Function '" + name->get_string() + "' is not defined at line " +
                             std::to_string(line_number) + ".");
        return;
    }

    // 获取函数属性(形参)
    FuncDef funcAttr = funcIt->second;

    // 转换 funcr_params 为 FuncRParams 指针（实参）
    auto funcRParams = dynamic_cast<FuncRParams *>(funcr_params);

    // 检查参数数量
    if ((funcRParams == nullptr && funcAttr->formals->size() > 0) ||
        (funcRParams != nullptr && funcRParams->params->size() != funcAttr->formals->size())) {
        error_msgs.push_back("ERROR: Function parameter count mismatch for function '" + name->get_string() +
                             "' at line " + std::to_string(line_number) + ".");
        return;
    } else if (funcRParams != nullptr) {
        funcr_params->TypeCheck();    // 此行仅作标记，没有实际内容，可略
        for (int i = 0; i < funcRParams->params->size(); ++i) {
            (*funcRParams->params)[i]->TypeCheck();
            error_msgs.push_back(type_status[(*funcRParams->params)[i]->attribute.T.type]);
            error_msgs.push_back(type_status[(*funcAttr->formals)[i]->type_decl]);

            // 检查是否为指针类型ptr，意味着传入数组
            if ((*funcRParams->params)[i]->attribute.T.type == Type::PTR) {
                auto lvalParam = dynamic_cast<Lval *>((*funcRParams->params)[i]);
                if (lvalParam) {
                    // 1. 检查维度是否匹配
                    if (lvalParam->dims->size() != (*funcAttr->formals)[i]->dims->size()) {
                        error_msgs.push_back("ERROR: Array dimension mismatch for parameter " + std::to_string(i + 1) +
                                             " in function '" + name->get_string() + "' at line " +
                                             std::to_string(line_number) + ".");
                    }
                    // 2. 检查数组的基本类型是否匹配
                    if (lvalParam->ptrtype != (*funcAttr->formals)[i]->type_decl) {
                        error_msgs.push_back("ERROR: Array base type mismatch for parameter " + std::to_string(i + 1) +
                                             " in function '" + name->get_string() + "' at line " +
                                             std::to_string(line_number) + ".");
                    }
                } else {
                    error_msgs.push_back("ERROR: Expected array argument for parameter " + std::to_string(i + 1) +
                                         " in function '" + name->get_string() + "' at line " +
                                         std::to_string(line_number) + ".");
                }
            }
            // 非数组
            if ((*funcAttr->formals)[i]->type_decl == Type::INT) {
                floattoint((*funcRParams->params)[i]);
                booltoint((*funcRParams->params)[i]);
            } else if ((*funcAttr->formals)[i]->type_decl == Type::FLOAT) {
                booltoint((*funcRParams->params)[i]);
                inttofloat((*funcRParams->params)[i]);
            }
            if ((*funcRParams->params)[i]->attribute.T.type != (*funcAttr->formals)[i]->type_decl) {
                error_msgs.push_back("ERROR: Parameter type mismatch for parameter " + std::to_string(i + 1) +
                                     " in function '" + name->get_string() + "' at line " +
                                     std::to_string(line_number) + ".");
            }
        }
    }
    // 设置返回类型和常量标志
    attribute.T.type = funcAttr->return_type;
    attribute.V.ConstTag = false;
}

void UnaryExp_plus::TypeCheck() {
    error_msgs.push_back("UnaryExp_plus");
    unary_exp->TypeCheck();
    booltoint(unary_exp);
    if (unary_exp->attribute.T.type != Type::INT && unary_exp->attribute.T.type != Type::FLOAT)
        error_msgs.push_back("ERROR: Unary '+' operator requires an integer or float.");
    attribute.T.type = unary_exp->attribute.T.type;
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    if (attribute.V.ConstTag) {
        attribute.V.val = unary_exp->attribute.V.val;
    }
}

void UnaryExp_neg::TypeCheck() {
    error_msgs.push_back("UnaryExp_neg");
    unary_exp->TypeCheck();
    booltoint(unary_exp);
    attribute.T.type = unary_exp->attribute.T.type;
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    if (attribute.V.ConstTag) {
        if (unary_exp->attribute.T.type == Type::INT) {
            if (unary_exp->attribute.V.val.IntVal == -2147483648)
                attribute.V.val.IntVal = unary_exp->attribute.V.val.IntVal;
            else
                attribute.V.val.IntVal = -unary_exp->attribute.V.val.IntVal;
        } else if (unary_exp->attribute.T.type == Type::FLOAT) {
            attribute.V.val.FloatVal = -unary_exp->attribute.V.val.FloatVal;
        } else {
            error_msgs.push_back("ERROR: Unary '-' operator requires an integer or float.");
        }
    }
}

// 为了便于中间代码生成部分，即使unary_exp已经是bool，也先转换为int
void UnaryExp_not::TypeCheck() {
    error_msgs.push_back("UnaryExp_not");
    unary_exp->TypeCheck();
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    booltoint(unary_exp);
    if (unary_exp->attribute.T.type == Type::INT || unary_exp->attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal =
            (unary_exp->attribute.V.val.IntVal != 0 || unary_exp->attribute.V.val.FloatVal != 0);
    } else {
        error_msgs.push_back("ERROR: Unary '!' operator requires a bool, int, or float.");
    }
}

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

void assign_stmt::TypeCheck() {
    error_msgs.push_back("AssignStmt Semant");
    lval->TypeCheck();
    lval->is_left = true;
    exp->TypeCheck();
    if (lval->attribute.T.type == Type::INT) {
        floattoint(exp);
        booltoint(exp);
    } else if (lval->attribute.T.type == Type::FLOAT) {
        booltoint(exp);
        inttofloat(exp);
    }
    if (lval->attribute.T.type != exp->attribute.T.type) {
        error_msgs.push_back("ERROR: Type mismatch in assignment at line " + std::to_string(line_number) + ".");
    }
    if (lval->attribute.V.ConstTag) {
        error_msgs.push_back("ERROR: The lvalue is not a variable at line " + std::to_string(line_number) + ".");
    }
}

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

void while_stmt::TypeCheck() {
    error_msgs.push_back("WhileStmt Semant");
    loopdepth++;    // 进入while循环时，增加嵌套深度
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("ERROR: While condition is invalid at line " + std::to_string(line_number) + ".");
    }
    body->TypeCheck();
    loopdepth--;    // 离开while循环时，减少嵌套深度
}

void continue_stmt::TypeCheck() {
    error_msgs.push_back("ContinueStmt Semant");
    if (loopdepth == 0) {
        error_msgs.push_back("ERROR: Break can only be used inside a loop.");
    }
}

void break_stmt::TypeCheck() {
    error_msgs.push_back("BreakStmt Semant");
    if (loopdepth == 0) {
        error_msgs.push_back("ERROR: Break can only be used inside a loop.");
    }
}

void return_stmt::TypeCheck() {
    error_msgs.push_back("return_stmt Semant");
    return_exp->TypeCheck();
    booltoint(return_exp);
    if (return_exp->attribute.T.type == now_func_return_type)
        return;
    if (now_func_return_type == Type::VOID)
        error_msgs.push_back("ERROR: Function declared with return type 'void' cannot return a value at line " +
                             std::to_string(line_number) + ".");
    else if (now_func_return_type == Type::INT)
        floattoint(return_exp);
    else if (now_func_return_type == Type::FLOAT)
        inttofloat(return_exp);
    if (return_exp->attribute.T.type != now_func_return_type)
        error_msgs.push_back("ERROR: Return type mismatchat at line " + std::to_string(line_number) + ".");
}

void return_stmt_void::TypeCheck() {
    if (now_func_return_type != Type::VOID)
        error_msgs.push_back("ERROR: Return type mismatch at line " + std::to_string(line_number) + ".");
}

void ConstInitVal::TypeCheck() {
    error_msgs.push_back("ConstInitVal Semant");
    for (auto init : *initval) {
        init->TypeCheck();
    }
}

void ConstInitVal_exp::TypeCheck() {
    error_msgs.push_back("ConstInitValExp Semant");
    exp->TypeCheck();
    attribute.T.type = exp->attribute.T.type;
    attribute.V = exp->attribute.V;
    // 如果不是const
}

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

void VarDef_no_init::TypeCheck() {
    error_msgs.push_back("VarDefNoInit Semant");
    if (isglobal) {
        if (semant_table.GlobalTable.find(name) != semant_table.GlobalTable.end()) {
            error_msgs.push_back("ERROR: multilpe definitions of vars at line " + std::to_string(line_number) + ".");
        }
        scope = 0;
    } else {
        if (semant_table.symbol_table.lookup_scope(name) == semant_table.symbol_table.get_current_scope()) {
            error_msgs.push_back("ERROR: Variable " + name->get_string() + " is redefined at line " +
                                 std::to_string(line_number) + ".");
        }
        scope = semant_table.symbol_table.get_current_scope();
    }

    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    if (isglobal)
        scope = 0;
    else
        scope = semant_table.symbol_table.get_current_scope();
    // 数组检查
    if (dims != nullptr) {
        for (auto dim : *dims) {
            dim->TypeCheck();
            if (!dim->attribute.V.ConstTag) {
                error_msgs.push_back("ERROR: Array Dim must be const expression in line " +
                                     std::to_string(line_number) + "\n");
            }
            booltoint(dim);
            if (dim->attribute.T.type != Type::INT) {
                error_msgs.push_back("ERROR: Array Dim must be int at line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(dim->attribute.V.val.IntVal);
        }
    }
    if (isglobal)
        semant_table.GlobalTable[name] = val;
    else
        semant_table.symbol_table.add_Symbol(name, val);
}

void VarDef::TypeCheck() {
    error_msgs.push_back("VarDef Semant");
    // 检查同名
    if (isglobal) {
        if (semant_table.GlobalTable.find(name) != semant_table.GlobalTable.end()) {
            error_msgs.push_back("ERROR: multilpe definitions of vars at line " + std::to_string(line_number) + ".");
        }
        scope = 0;
    } else {
        if (semant_table.symbol_table.lookup_scope(name) == semant_table.symbol_table.get_current_scope()) {
            error_msgs.push_back("ERROR: Variable " + name->get_string() + " is redefined at line " +
                                 std::to_string(line_number) + ".");
        }
        scope = semant_table.symbol_table.get_current_scope();
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
                error_msgs.push_back("ERROR: Array Dim must be const expression in line " +
                                     std::to_string(line_number) + ".");
            }
            booltoint(dim);
            if (dim->attribute.T.type != Type::INT) {
                error_msgs.push_back("ERROR: Array Dim must be int at line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(dim->attribute.V.val.IntVal);
        }
        // 处理数组初始化
        int totalElements = 1;
        for (int dim : val.dims) {
            totalElements *= dim;    // 计算总元素数
        }
        HandleArrayInit(init, val, totalElements, 1);
        // 中间代码生成时不用再次初始化数组值了
        IntInitVals = val.IntInitVals;
        FloatInitVals = val.FloatInitVals;
        // 确保初始化值的数量匹配
        if (val.IntInitVals.size() != totalElements && val.FloatInitVals.size() != totalElements) {
            error_msgs.push_back("ERROR: Initialization values count does not match array dimensions at line " +
                                 std::to_string(line_number) + ".");
        }
        // std::cout << val.IntInitVals.size() << "1111111111111" << std::endl;
    }
    // 非数组检查
    else {
        if (init->attribute.T.type != val.type) {
            if (init->attribute.T.type == Type::BOOL) {
                init->attribute.V.val.IntVal = static_cast<int>(init->attribute.V.val.BoolVal);
                init->attribute.T.type = Type::INT;
            }
            if (val.type == Type::INT && init->attribute.T.type == Type::FLOAT) {
                init->attribute.V.val.IntVal = static_cast<int>(init->attribute.V.val.FloatVal);
                init->attribute.T.type = Type::INT;
                attribute.V.val.IntVal = init->attribute.V.val.IntVal;
            } else if (val.type == Type::FLOAT && init->attribute.T.type == Type::INT) {
                init->attribute.V.val.FloatVal = static_cast<float>(init->attribute.V.val.IntVal);
                init->attribute.T.type = Type::FLOAT;
                attribute.V.val.FloatVal = init->attribute.V.val.FloatVal;
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
    if (isglobal)
        semant_table.GlobalTable[name] = val;
    else
        semant_table.symbol_table.add_Symbol(name, val);
}

void ConstDef::TypeCheck() {
    error_msgs.push_back("ConstDef Semant");
    // 检查同名
    if (isglobal) {
        if (semant_table.GlobalTable.find(name) != semant_table.GlobalTable.end()) {
            error_msgs.push_back("ERROR: multilpe definitions of vars in line " + std::to_string(line_number) + ".");
        }
        scope = 0;
    } else {
        if (semant_table.symbol_table.lookup_scope(name) == semant_table.symbol_table.get_current_scope()) {
            error_msgs.push_back("ERROR: Variable " + name->get_string() + " is redefined at line " +
                                 std::to_string(line_number) + ".");
        }
        scope = semant_table.symbol_table.get_current_scope();
    }
    VarAttribute val;
    val.ConstTag = true;
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
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(dim->attribute.V.val.IntVal);
        }
        // 处理数组初始化
        int totalElements = 1;
        for (int dim : val.dims) {
            totalElements *= dim;    // 计算总元素数
        }
        HandleArrayInit(init, val, totalElements, 1);
        // 中间代码生成时不用再次初始化数组值了
        IntInitVals = val.IntInitVals;
        FloatInitVals = val.FloatInitVals;

        // for (int i = 0; i < val.IntInitVals.size(); i++) {
        //     std::cout << val.IntInitVals[i] << " ";
        // }
        std::cout << std::endl;
        // 确保初始化值的数量匹配
        if (val.IntInitVals.size() != totalElements && val.FloatInitVals.size() != totalElements) {
            error_msgs.push_back("ERROR: Initialization values count does not match array dimensions at line " +
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
    if (isglobal)
        semant_table.GlobalTable[name] = val;
    else
        semant_table.symbol_table.add_Symbol(name, val);
}

void VarDecl::TypeCheck() {
    error_msgs.push_back("VarDecl Semant");
    // 检查变量定义列表是否为空
    if (var_def_list == nullptr) {
        error_msgs.push_back("Variable declaration list is null.");
        return;
    }
    // 遍历变量定义列表，进行类型检查
    for (auto &var_def : *var_def_list) {
        var_def->isglobal = isglobal;
        var_def->type_decl = type_decl;
        var_def->TypeCheck();
    }
}

void ConstDecl::TypeCheck() {
    error_msgs.push_back("ConstDecl Semant");
    if (var_def_list == nullptr) {
        error_msgs.push_back("Variable declaration list is null.");
        return;
    }
    for (auto &var_def : *var_def_list) {
        var_def->isglobal = isglobal;
        var_def->type_decl = type_decl;
        var_def->TypeCheck();
    }
}

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
    now_func_return_type = return_type;
    // 函数表是直接通过函数名进行储存的，不支持函数重载，需要检查函数是否重复声明
    if (semant_table.FunctionTable.find(name) != semant_table.FunctionTable.end()) {
        error_msgs.push_back("Function " + name->get_string() + " is redefined at line " + std::to_string(line_number) +
                             "\n");
    }
    if (name->get_string() == "main") {
        has_main = true;
        if (return_type != Type::INT) {
            error_msgs.push_back("ERROR: 'main' function must return an integer.");
        }
        // 检查main函数的参数是否符合规范
        if (formals->size() != 0 && formals->size() != 2) {
            error_msgs.push_back("ERROR: 'main' function must have 0 or 2 parameters.");
        }
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

void CompUnit_Decl::TypeCheck() {
    error_msgs.push_back("CompUnitDecl Semant");
    decl->isglobal = true;
    decl->TypeCheck();
}

void CompUnit_FuncDef::TypeCheck() {
    error_msgs.push_back("CompUnit_FuncDef Semant");
    func_def->TypeCheck();
}