#include "semant.h"
#include "../include/SysY_tree.h"
#include "../include/ir.h"
#include "../include/type.h"
#include <unordered_set>
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
std::vector<std::string> debug_msgs{};

int loopdepth = 0;
bool has_main = false;

Type::ty now_func_return_type;

// int a[5][4][3] = { { {2,3}, 6,7,5,4,3,2,11,2,4 },7,8,11 };
std::vector<Expression> VarInits{};
void HandleArrayInit(InitVal init, VarAttribute &val, int count, int handled, bool &isAllConst) {
    debug_msgs.push_back("HandleArrayInit");
    int beforeadd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
    for (InitVal iv : *(init->GetList())) {
        if (iv->IsExp()) {
            if (!iv->attribute.V.ConstTag) {
                isAllConst = false;
                VarInits.push_back(iv->GetExp());
            }
            if (val.type == Type::INT) {
                val.IntInitValsTag.push_back(iv->attribute.V.ConstTag);
                if (iv->attribute.T.type == Type::INT) {
                    val.IntInitVals.push_back(iv->attribute.V.val.IntVal);
                } else if (iv->attribute.T.type == Type::FLOAT) {
                    val.IntInitVals.push_back(static_cast<int>(iv->attribute.V.val.FloatVal));
                }
            } else if (val.type == Type::FLOAT) {
                val.FloatInitValsTag.push_back(iv->attribute.V.ConstTag);
                if (iv->attribute.T.type == Type::INT) {
                    val.FloatInitVals.push_back(static_cast<float>(iv->attribute.V.val.IntVal));
                } else if (iv->attribute.T.type == Type::FLOAT) {
                    val.FloatInitVals.push_back(iv->attribute.V.val.FloatVal);
                }
            }
        } else {
            int h = 1;
            for (auto d : val.dims) {
                if (h < handled)    // 不能直接用h <= handled，因为某维度可能等于1
                    h *= d;
                else {
                    h *= d;
                    break;
                }
            }
            // 如果是多维数组，则递归处理
            int afteradd = (val.type == Type::INT) ? val.IntInitVals.size() : val.FloatInitVals.size();
            int realcount = afteradd - beforeadd;
            if (realcount % (count / h) != 0) {
                error_msgs.push_back("ERROR: The number of initial values in the array is too small at line " +
                                     std::to_string(init->GetLineNumber()) + ".");
                return;
            }
            HandleArrayInit(iv, val, count, h, isAllConst);
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
            val.IntInitValsTag.push_back(true);
        } else {
            val.FloatInitVals.push_back(0);
            val.FloatInitValsTag.push_back(true);
        }
        realcount++;
    }
}
struct expr_struct {
    NodeAttribute attribute;
};

// 统一为相同的类型，优先级：float > int > bool，其中（bool，bool）会转换为（int，int）
// 按值传递，而不是按引用传递：保证结点原属性不改变，在中间代码生成中，结点原属性意味着寄存器类型，现在改变会导致错误
// !!!!注意，虽然是按值传递，但传入的是指针，不能直接用，要先深拷贝
expr_struct booltoint(const Expression oriexp) {
    expr_struct exp;
    exp.attribute = oriexp->attribute;
    if (oriexp->attribute.T.type != Type::BOOL)
        return exp;
    exp.attribute.T.type = Type::INT;
    exp.attribute.V.val.IntVal = exp.attribute.V.val.BoolVal ? 1 : 0;
    return exp;
}

expr_struct inttofloat(const Expression oriexp) {
    expr_struct exp;
    exp.attribute = oriexp->attribute;
    if (oriexp->attribute.T.type != Type::INT)
        return exp;
    exp.attribute.T.type = Type::FLOAT;
    exp.attribute.V.val.FloatVal = static_cast<float>(oriexp->attribute.V.val.IntVal);
    return exp;
}

void inttofloat(expr_struct &exp) {
    if (exp.attribute.T.type != Type::INT)
        return;
    exp.attribute.T.type = Type::FLOAT;
    exp.attribute.V.val.FloatVal = static_cast<float>(exp.attribute.V.val.IntVal);
    return;
}

expr_struct floattoint(const Expression oriexp) {
    expr_struct exp;
    exp.attribute = oriexp->attribute;
    if (oriexp->attribute.T.type != Type::FLOAT)
        return exp;
    exp.attribute.T.type = Type::INT;
    exp.attribute.V.val.IntVal = static_cast<int>(oriexp->attribute.V.val.FloatVal);
    return exp;
}

void floattoint(expr_struct &exp) {
    if (exp.attribute.T.type != Type::FLOAT)
        return;
    exp.attribute.T.type = Type::INT;
    exp.attribute.V.val.IntVal = static_cast<int>(exp.attribute.V.val.FloatVal);
    return;
}

std::pair<expr_struct, expr_struct> unifyTypes(const Expression oriexp1, const Expression oriexp2) {
    auto exp1 = booltoint(oriexp1);
    auto exp2 = booltoint(oriexp2);
    if (exp1.attribute.T.type == exp2.attribute.T.type)
        return {exp1, exp2};
    if (exp1.attribute.T.type == Type::FLOAT || exp2.attribute.T.type == Type::FLOAT) {
        inttofloat(exp1);
        inttofloat(exp2);
    }
    return {exp1, exp2};
}

void __Program::TypeCheck() {
    debug_msgs.push_back("Program Semant");
    // 进入一个新的作用域
    semant_table.symbol_table.enter_scope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
    // 检查是否存在main函数
    if (!has_main) {
        error_msgs.push_back("ERROR: The 'main' function is not defined.");
    }
}

void Exp::TypeCheck() {
    debug_msgs.push_back("Exp Semant");
    addexp->TypeCheck();
    attribute = addexp->attribute;
}

void AddExp_plus::TypeCheck() {
    debug_msgs.push_back("AddExp_plus Semant");
    addexp->TypeCheck();
    mulexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(addexp, mulexp);
    if (addexp->attribute.V.ConstTag) {
        addexp->attribute = exp1.attribute;
        if (mulexp->attribute.V.ConstTag)
            mulexp->attribute = exp2.attribute;
    }
    attribute.V.ConstTag = addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = exp1.attribute.V.val.IntVal + exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = exp1.attribute.V.val.FloatVal + exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '+' at line " + std::to_string(line_number) + ".");
    }
}

void AddExp_sub::TypeCheck() {
    debug_msgs.push_back("AddExp_sub Semant");
    addexp->TypeCheck();
    mulexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(addexp, mulexp);
    if (addexp->attribute.V.ConstTag) {
        addexp->attribute = exp1.attribute;
        if (mulexp->attribute.V.ConstTag)
            mulexp->attribute = exp2.attribute;
    }
    attribute.V.ConstTag = addexp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = exp1.attribute.V.val.IntVal - exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = exp1.attribute.V.val.FloatVal - exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '-' at line " + std::to_string(line_number) + ".");
    }
}

void MulExp_mul::TypeCheck() {
    debug_msgs.push_back("MulExp_mul Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(mulexp, unary_exp);
    if (mulexp->attribute.V.ConstTag) {
        mulexp->attribute = exp1.attribute;
        if (unary_exp->attribute.V.ConstTag)
            unary_exp->attribute = exp2.attribute;
    }
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = exp1.attribute.V.val.IntVal * exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = exp1.attribute.V.val.FloatVal * exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '*' at line " + std::to_string(line_number) + ".");
    }
}

void MulExp_div::TypeCheck() {
    debug_msgs.push_back("MulExp_div Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(mulexp, unary_exp);
    if (mulexp->attribute.V.ConstTag) {
        mulexp->attribute = exp1.attribute;
        if (unary_exp->attribute.V.ConstTag)
            unary_exp->attribute = exp2.attribute;
    }
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (exp2.attribute.V.ConstTag && exp2.attribute.V.val.IntVal == 0) {
            error_msgs.push_back("ERROR: Division by zero error in MulExp_div at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = exp1.attribute.V.val.IntVal / exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::FLOAT;
        if (exp2.attribute.V.ConstTag && exp2.attribute.V.val.FloatVal == 0) {
            error_msgs.push_back("ERROR: Division by zero error in MulExp_div at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.FloatVal = exp1.attribute.V.val.FloatVal / exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '/' at line " + std::to_string(line_number) + ".");
    }
}

void MulExp_mod::TypeCheck() {
    debug_msgs.push_back("MulExp_mod Semant");
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(mulexp, unary_exp);
    if (mulexp->attribute.V.ConstTag && unary_exp->attribute.V.ConstTag) {
        mulexp->attribute = exp1.attribute;
        unary_exp->attribute = exp2.attribute;
    }
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag & mulexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::INT;
        if (unary_exp->attribute.V.ConstTag && exp2.attribute.V.val.IntVal == 0) {
            error_msgs.push_back("ERROR: Division by zero in '%' operation at line " + std::to_string(line_number) +
                                 ".");
            return;
        }
        if (attribute.V.ConstTag)
            attribute.V.val.IntVal = exp1.attribute.V.val.IntVal % exp2.attribute.V.val.IntVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '%' at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_leq::TypeCheck() {
    debug_msgs.push_back("<= Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(relexp, addexp);
    if (addexp->attribute.V.ConstTag)
        addexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal <= exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal <= exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '<=' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_lt::TypeCheck() {
    debug_msgs.push_back("< Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(relexp, addexp);
    if (addexp->attribute.V.ConstTag)
        addexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal < exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal < exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '<' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_geq::TypeCheck() {
    debug_msgs.push_back(">= Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(relexp, addexp);
    if (addexp->attribute.V.ConstTag)
        addexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal >= exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal >= exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '>=' expression at line " + std::to_string(line_number) + ".");
    }
}

void RelExp_gt::TypeCheck() {
    debug_msgs.push_back("> Semant");
    relexp->TypeCheck();
    addexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(relexp, addexp);
    if (addexp->attribute.V.ConstTag)
        addexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & addexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal > exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal > exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '>' expression at line " + std::to_string(line_number) + ".");
    }
}

void EqExp_eq::TypeCheck() {
    debug_msgs.push_back("== Semant");
    eqexp->TypeCheck();
    relexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(eqexp, relexp);
    if (relexp->attribute.V.ConstTag)
        relexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal == exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal == exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '==' expression at line " + std::to_string(line_number) + ".");
    }
}

void EqExp_neq::TypeCheck() {
    debug_msgs.push_back("!= Semant");
    eqexp->TypeCheck();
    relexp->TypeCheck();
    auto [exp1, exp2] = unifyTypes(eqexp, relexp);
    if (relexp->attribute.V.ConstTag)
        relexp->attribute = exp2.attribute;
    attribute.V.ConstTag = relexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        childType = Type::INT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal != exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        childType = Type::FLOAT;
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal != exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '!=' expression at line " + std::to_string(line_number) + ".");
    }
}

void LAndExp_and::TypeCheck() {
    debug_msgs.push_back("&& Semant");
    landexp->TypeCheck();
    eqexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(landexp, eqexp);
    attribute.V.ConstTag = landexp->attribute.V.ConstTag & eqexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal && exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal && exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '&&' expression at line " + std::to_string(line_number) + ".");
    }
}

void LOrExp_or::TypeCheck() {
    debug_msgs.push_back("|| Semant");
    lorexp->TypeCheck();
    landexp->TypeCheck();

    auto [exp1, exp2] = unifyTypes(landexp, lorexp);
    attribute.V.ConstTag = landexp->attribute.V.ConstTag & lorexp->attribute.V.ConstTag;

    if (exp1.attribute.T.type == Type::INT && exp2.attribute.T.type == Type::INT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.IntVal || exp2.attribute.V.val.IntVal;
    } else if (exp1.attribute.T.type == Type::FLOAT && exp2.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = exp1.attribute.V.val.FloatVal || exp2.attribute.V.val.FloatVal;
    } else {
        error_msgs.push_back("ERROR: Type mismatch in '||' expression at line " + std::to_string(line_number) + ".");
    }
}

void ConstExp::TypeCheck() {
    addexp->TypeCheck();
    attribute = addexp->attribute;
    if (!attribute.V.ConstTag) {    // addexp is not const
        error_msgs.push_back("ERROR: Expression is not const " + std::to_string(line_number) + "\n");
    }
}

void Lval::TypeCheck() {
    debug_msgs.push_back("Lval Semant");

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
        // a[2][2]
        // 1、a / a[1]
        if (dims == nullptr || dims->size() < varAttr.dims.size()) {
            if (dims != nullptr) {
                for (size_t i = 0; i < dims->size(); ++i) {
                    auto &dim = (*dims)[i];
                    dim->TypeCheck();
                    auto temp_dim = booltoint(dim);
                    if (temp_dim.attribute.T.type != Type::INT) {
                        error_msgs.push_back("ERROR: Array index must be of type int at line " +
                                             std::to_string(line_number) + ".");
                    }
                    int indexValue = temp_dim.attribute.V.val.IntVal;
                    int arraySize = varAttr.dims[i];
                    if (indexValue >= arraySize && arraySize != -1 && dim->attribute.V.ConstTag) {
                        error_msgs.push_back("ERROR1: Array index " + std::to_string(indexValue) +
                                             " out of bounds for variable  with size " + std::to_string(arraySize) +
                                             " at line " + std::to_string(line_number) + ".");
                    }
                }
            }
            attribute.V.ConstTag = false;
            attribute.T.type = Type::PTR;
            ptrtype = varAttr.type == Type::INT ? Type::INT : Type::FLOAT;
            return;
        }
        // 2、a[1][0][2]
        else if (dims->size() > varAttr.dims.size()) {
            error_msgs.push_back("ERROR: Array dimension mismatch for variable. Expected " +
                                 std::to_string(varAttr.dims.size()) + ", but got " + std::to_string(dims->size()) +
                                 " at line " + std::to_string(line_number) + ".");
        }
        // 3、a[1][0]
        else {
            // 检查每个维度的类型
            for (size_t i = 0; i < dims->size(); ++i) {
                auto &dim = (*dims)[i];
                dim->TypeCheck();
                auto temp_dim = booltoint(dim);
                if (temp_dim.attribute.T.type != Type::INT) {
                    error_msgs.push_back("ERROR: Array index must be of type int at line " +
                                         std::to_string(line_number) + ".");
                }
                // 左值表达式中，数组下标不要求为const，因此不做常值检查，但需要确认全部是常量还是有变量，以便后面赋值
                constindex &= dim->attribute.V.ConstTag;
                // 检查下标是否在有效范围内
                int indexValue = temp_dim.attribute.V.val.IntVal;
                int arraySize = varAttr.dims[i];
                if (indexValue >= arraySize && arraySize != -1 && dim->attribute.V.ConstTag) {
                    error_msgs.push_back("ERROR2: Array index " + std::to_string(indexValue) +
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
            int index = 0;
            for (size_t i = 0; i < dims->size(); ++i) {
                offset /= varAttr.dims[i];
                index += (*dims)[i]->attribute.V.val.IntVal * offset;
            }
            if (index >= total) {
                error_msgs.push_back("ERROR: The array index is out of range at line " + std::to_string(line_number) +
                                     ".");
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
    debug_msgs.push_back("FuncRParams Semant");
}

static const std::unordered_set<std::string> specialFuncs = {"getfarray", "getarray", "putfarray", "putarray"};
bool isSpecialFunc(Symbol name) {
    std::string func = name->get_string();
    return specialFuncs.find(func) != specialFuncs.end();
}

// 对于一个数组参数，如果从函数表中先获取参数、再获取类型，则为ptr、dims类型为expression；如果从符号表中查找变量名获取类型，则int/float、dims类型为int
void Func_call::TypeCheck() {
    debug_msgs.push_back("FunctionCall Semant");

    bool isSpecial = isSpecialFunc(name);

    // 检查函数是否存在
    auto funcIt = semant_table.FunctionTable.find(name);
    if (funcIt == semant_table.FunctionTable.end()) {
        error_msgs.push_back("ERROR: Function '" + name->get_string() + "' is not defined at line " +
                             std::to_string(line_number) + ".");
        return;
    }

    // 获取函数属性(形参)
    FuncDef funcAttr = funcIt->second;
    auto FPs = funcAttr->formals;

    // 转换 funcr_params 为 FuncRParams 指针（实参）
    auto funcRParams = dynamic_cast<FuncRParams *>(funcr_params);

    // 1、检查参数数量(没有实参是funcRParams == nullptr，没有形参是FPs->size()==0)
    if ((funcRParams == nullptr && FPs->size() > 0) ||
        (funcRParams != nullptr && funcRParams->params->size() != FPs->size())) {
        error_msgs.push_back("ERROR: Function parameter count mismatch for function '" + name->get_string() +
                             "' at line " + std::to_string(line_number) + ".");
        return;
    }
    // 2、有参数，则检查每个参数的类型
    else if (funcRParams != nullptr) {
        auto RPs = funcRParams->params;
        for (int i = 0; i < RPs->size(); ++i) {
            (*RPs)[i]->TypeCheck();
            if (isSpecial)
                continue;
            // 检查是否为指针类型ptr，意味着传入数组
            bool isArray = (*RPs)[i]->attribute.T.type == Type::PTR || (*FPs)[i]->dims != nullptr;
            if (isArray) {
                if ((*RPs)[i]->attribute.T.type != Type::PTR || (*FPs)[i]->dims == nullptr) {
                    error_msgs.push_back("ERROR: The parameter type is incorrect at line " +
                                         std::to_string(line_number) + ".");
                    return;
                }
                auto RP = dynamic_cast<Lval *>((*RPs)[i]);
                if (RP) {
                    // 1. 检查维度是否匹配(实参定义维度 - 实参引用维度 = 形参的维度)
                    int scope = semant_table.symbol_table.lookup_scope(RP->name);
                    VarAttribute varAttr;
                    if (scope == -1) {
                        if (semant_table.GlobalTable.find(RP->name) != semant_table.GlobalTable.end()) {
                            varAttr = semant_table.GlobalTable[RP->name];
                        } else
                            error_msgs.push_back("ERROR: Undefined variable at line " + std::to_string(line_number) +
                                                 ".");
                    } else {
                        varAttr = semant_table.symbol_table.lookup_val(RP->name);
                    }
                    auto Varoridims = varAttr.dims.size();
                    auto Varnowdims = RP->dims == nullptr ? 0 : RP->dims->size();
                    auto Arraydims = (*FPs)[i]->dims->size();
                    if (Varoridims - Varnowdims != Arraydims)
                        error_msgs.push_back("ERROR: Array dimension mismatch in function '" + name->get_string() +
                                             "' at line " + std::to_string(line_number) + ".");

                    // 2. 维度值检查
                    for (int j = 1; j < Arraydims; j++) {
                        if ((*(*FPs)[i]->dims)[j]->attribute.V.ConstTag)
                            continue;
                        auto Arraydim = (*(*FPs)[i]->dims)[j]->attribute.V.val.IntVal;
                        if (Arraydim != varAttr.dims[Varnowdims + j])
                            error_msgs.push_back("ERROR: Array dimension mismatch in function '" + name->get_string() +
                                                 "' at line " + std::to_string(line_number) + ".");
                    }
                    // 3. 检查数组的基本类型是否匹配
                    if (RP->ptrtype != (*FPs)[i]->type_decl) {
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
            else if (((*FPs)[i]->type_decl != Type::INT && (*FPs)[i]->type_decl != Type::FLOAT) ||
                     ((*RPs)[i]->attribute.T.type != Type::INT && (*RPs)[i]->attribute.T.type != Type::FLOAT &&
                      (*RPs)[i]->attribute.T.type != Type::BOOL)) {
                error_msgs.push_back("ERROR: The parameter type is incorrect at line " + std::to_string(line_number) +
                                     ".");
            }
        }
    }
    // 设置返回类型和常量标志
    attribute.T.type = funcAttr->return_type;
    attribute.V.ConstTag = false;
}

void UnaryExp_plus::TypeCheck() {
    debug_msgs.push_back("UnaryExp_plus");
    unary_exp->TypeCheck();
    auto exp = booltoint(unary_exp);
    if (exp.attribute.T.type != Type::INT && exp.attribute.T.type != Type::FLOAT)
        error_msgs.push_back("ERROR: Unary '+' operator requires an integer or float.");
    attribute.T.type = exp.attribute.T.type;
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    if (attribute.V.ConstTag) {
        attribute.V.val = exp.attribute.V.val;
    }
}

void UnaryExp_neg::TypeCheck() {
    debug_msgs.push_back("UnaryExp_neg");
    unary_exp->TypeCheck();
    auto exp = booltoint(unary_exp);
    attribute.T.type = exp.attribute.T.type;
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    if (attribute.V.ConstTag) {
        if (exp.attribute.T.type == Type::INT) {
            if (exp.attribute.V.val.IntVal == -2147483648)
                attribute.V.val.IntVal = exp.attribute.V.val.IntVal;
            else
                attribute.V.val.IntVal = -exp.attribute.V.val.IntVal;
        } else if (exp.attribute.T.type == Type::FLOAT) {
            attribute.V.val.FloatVal = -exp.attribute.V.val.FloatVal;
        } else {
            error_msgs.push_back("ERROR: Unary '-' operator requires an integer or float.");
        }
    }
}

void UnaryExp_not::TypeCheck() {
    debug_msgs.push_back("UnaryExp_not");
    unary_exp->TypeCheck();
    attribute.V.ConstTag = unary_exp->attribute.V.ConstTag;
    auto exp = booltoint(unary_exp);
    if (exp.attribute.T.type == Type::INT || exp.attribute.T.type == Type::FLOAT) {
        attribute.T.type = Type::BOOL;
        if (attribute.V.ConstTag)
            attribute.V.val.BoolVal = (exp.attribute.V.val.IntVal == 0 && exp.attribute.T.type == Type::INT) ||
                                      (exp.attribute.V.val.FloatVal == 0 && exp.attribute.T.type == Type::FLOAT);
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

void StringConst::TypeCheck() { debug_msgs.push_back("StringConst Semant"); }

void PrimaryExp_branch::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void assign_stmt::TypeCheck() {
    debug_msgs.push_back("AssignStmt Semant");
    lval->TypeCheck();
    lval->is_left = true;
    exp->TypeCheck();
    Expression temp_exp = exp;
    if ((lval->attribute.T.type != Type::INT && lval->attribute.T.type != Type::FLOAT) ||
        (exp->attribute.T.type != Type::INT && exp->attribute.T.type != Type::FLOAT &&
         exp->attribute.T.type != Type::BOOL)) {
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
    debug_msgs.push_back("WhileStmt Semant");
    loopdepth++;    // 进入while循环时，增加嵌套深度
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("ERROR: While condition is invalid at line " + std::to_string(line_number) + ".");
    }
    body->TypeCheck();
    loopdepth--;    // 离开while循环时，减少嵌套深度
}

void continue_stmt::TypeCheck() {
    debug_msgs.push_back("ContinueStmt Semant");
    if (loopdepth == 0) {
        error_msgs.push_back("ERROR: Break can only be used inside a loop.");
    }
}

void break_stmt::TypeCheck() {
    debug_msgs.push_back("BreakStmt Semant");
    if (loopdepth == 0) {
        error_msgs.push_back("ERROR: Break can only be used inside a loop.");
    }
}

void return_stmt::TypeCheck() {
    debug_msgs.push_back("return_stmt Semant");
    return_exp->TypeCheck();
    auto exp1 = booltoint(return_exp);
    expected_type = now_func_return_type;
    if (exp1.attribute.T.type == expected_type)
        return;
    if (expected_type == Type::VOID) {
        error_msgs.push_back("ERROR: Function declared with return type 'void' cannot return a value at line " +
                             std::to_string(line_number) + ".");
        return;
    } else if (exp1.attribute.T.type != Type::INT && exp1.attribute.T.type != Type::FLOAT)
        error_msgs.push_back("ERROR: Return type mismatch at line " + std::to_string(line_number) + ".");
}

void return_stmt_void::TypeCheck() {
    if (now_func_return_type != Type::VOID)
        error_msgs.push_back("ERROR: Return type mismatch at line " + std::to_string(line_number) + ".");
}

void ConstInitVal::TypeCheck() {
    debug_msgs.push_back("ConstInitVal Semant");
    for (auto init : *initval) {
        init->TypeCheck();
    }
}

void ConstInitVal_exp::TypeCheck() {
    debug_msgs.push_back("ConstInitValExp Semant");
    exp->TypeCheck();
    attribute.T.type = exp->attribute.T.type;
    attribute.V = exp->attribute.V;
    // 如果不是const
}

void VarInitVal::TypeCheck() {
    debug_msgs.push_back("VarInitVal Semant");
    for (auto init : *initval) {
        init->TypeCheck();
    }
}

void VarInitVal_exp::TypeCheck() {
    debug_msgs.push_back("VarInitValExp Semant");
    exp->TypeCheck();
    attribute.T.type = exp->attribute.T.type;
    attribute.V = exp->attribute.V;
}

void VarDef_no_init::TypeCheck() {
    debug_msgs.push_back("VarDefNoInit Semant");
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
            auto temp_dim = booltoint(dim);
            dim->attribute = temp_dim.attribute;
            if (dim->attribute.T.type != Type::INT) {
                error_msgs.push_back("ERROR: Array Dim must be int at line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(temp_dim.attribute.V.val.IntVal);
        }
    }
    if (isglobal)
        semant_table.GlobalTable[name] = val;
    else
        semant_table.symbol_table.add_Symbol(name, val);
}

void VarDef::TypeCheck() {
    debug_msgs.push_back("VarDef Semant");
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
            auto temp_dim = booltoint(dim);
            dim->attribute = temp_dim.attribute;
            if (dim->attribute.T.type != Type::INT) {
                error_msgs.push_back("ERROR: Array Dim must be int at line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(temp_dim.attribute.V.val.IntVal);
        }
        // 处理数组初始化
        int totalElements = 1;
        for (int dim : val.dims) {
            totalElements *= dim;    // 计算总元素数
        }
        bool isAllconst = true;
        VarInits.clear();
        HandleArrayInit(init, val, totalElements, 1, isAllconst);
        // 中间代码生成时不用再次初始化数组值了
        IntInitVals = val.IntInitVals;
        FloatInitVals = val.FloatInitVals;
        IntInitValsTag = val.IntInitValsTag;
        FloatInitValsTag = val.FloatInitValsTag;
        varinits = VarInits;
        // 确保初始化值的数量匹配
        if (val.IntInitVals.size() != totalElements && val.FloatInitVals.size() != totalElements) {
            error_msgs.push_back("ERROR: Initialization values count does not match array dimensions at line " +
                                 std::to_string(line_number) + ".");
        }
        // std::cout << val.IntInitVals.size() << "1111111111111" << std::endl;
    }
    // 非数组检查
    else {
        // 初始值是常数，进行类型转换
        if (init->attribute.V.ConstTag) {
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
                    error_msgs.push_back("ERROR: Type mismatch in initialization at line " +
                                         std::to_string(line_number) + ". Cannot convert from " +
                                         type_status[init->attribute.T.type] + " to " + type_status[val.type] + ".");
                }
            }
            if (val.type == Type::INT) {
                val.IntInitVals.push_back(init->attribute.V.val.IntVal);
            } else if (val.type == Type::FLOAT) {
                val.FloatInitVals.push_back(init->attribute.V.val.FloatVal);
            }
        }
        // 初始值是变量，仅检查类型
        else if (init->attribute.T.type != Type::BOOL && init->attribute.T.type != Type::FLOAT &&
                 init->attribute.T.type != Type::INT) {
            error_msgs.push_back("ERROR: Type mismatch in initialization at line " + std::to_string(line_number) +
                                 ". Cannot convert from " + type_status[init->attribute.T.type] + " to " +
                                 type_status[val.type] + ".");
        }
    }
    attribute.V.val = init->attribute.V.val;
    if (isglobal)
        semant_table.GlobalTable[name] = val;
    else
        semant_table.symbol_table.add_Symbol(name, val);
}

void ConstDef::TypeCheck() {
    debug_msgs.push_back("ConstDef Semant");
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
                error_msgs.push_back("ERROR: Array Dim must be const expression in line " +
                                     std::to_string(line_number) + "\n");
            }
            if (dim->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("ERROR: Array Dim can not be float in line " + std::to_string(line_number) + ".");
            }
            val.dims.push_back(dim->attribute.V.val.IntVal);
        }
        // 处理数组初始化
        int totalElements = 1;
        for (int dim : val.dims) {
            totalElements *= dim;    // 计算总元素数
        }
        bool isAllconst = true;
        HandleArrayInit(init, val, totalElements, 1, isAllconst);
        VarInits.clear();
        if (!isAllconst)
            error_msgs.push_back("ERROR: Constarray initialization must be constant values at line " +
                                 std::to_string(line_number) + ".");
        // 中间代码生成时不用再次初始化数组值了
        IntInitVals = val.IntInitVals;
        FloatInitVals = val.FloatInitVals;

        // for (int i = 0; i < val.IntInitVals.size(); i++) {
        //     std::cout << val.IntInitVals[i] << " ";
        // }
        // std::cout << std::endl;
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
                init->attribute.V.val.IntVal = static_cast<int>(init->attribute.V.val.FloatVal);
            } else if (val.type == Type::FLOAT && init->attribute.T.type == Type::INT) {
                init->attribute.V.val.FloatVal = static_cast<float>(init->attribute.V.val.IntVal);
            } else {
                error_msgs.push_back("ERROR: Type mismatch in initialization at line " + std::to_string(line_number) +
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
    debug_msgs.push_back("VarDecl Semant");
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
    debug_msgs.push_back("ConstDecl Semant");
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
    debug_msgs.push_back("BlockItem_Decl Semant");
    decl->TypeCheck();
}

void BlockItem_Stmt::TypeCheck() {
    debug_msgs.push_back("BlockItem_Decl Semant");
    stmt->TypeCheck();
}

void __Block::TypeCheck() {
    debug_msgs.push_back("Block Semant");

    semant_table.symbol_table.enter_scope();
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.exit_scope();
}

// 对于一个数组参数，如果从函数表中先获取参数、再获取类型，则为ptr、dims类型为expression；如果从符号表中查找变量名获取类型，则int/float、dims类型为int
void __FuncFParam::TypeCheck() {
    debug_msgs.push_back("FuncFParam Semant");
    // 创建一个变量属性对象 val，用于存储当前函数形参的属性
    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    // 将作用域设置为 1，表示当前参数在一个特定的作用域内
    scope = 1;
    // 检查参数是否为数组类型
    if (dims != nullptr) {
        auto dim_vector = *dims;    // 取出数组维度列表

        // 对于数组，第一维为空，例如 int f(int A[][30][40])
        // 参数    dim    实参
        // a[][2]  {2}    A[1]
        // a[]    null    A
        val.dims.push_back(-1);
        // 从第二维开始检查每个维度
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();    // 对当前维度进行类型检查
            // 检查维度是否为常量表达式
            if (d->attribute.V.ConstTag == false) {
                error_msgs.push_back("ERROR: Array Dim must be const expression in line " +
                                     std::to_string(line_number) + "\n");
            }
            // 检查维度类型是否为浮点型
            if (d->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("ERROR: Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            }
            // 形参也是const
            if (d->attribute.T.type == Type::BOOL) {
                d->attribute.T.type = Type::INT;
                d->attribute.V.val.IntVal = d->attribute.V.val.BoolVal ? 1 : 0;
            }
            val.dims.push_back(d->attribute.V.val.IntVal);
        }
        // 如果参数是数组，则将类型设置为指针
        attribute.T.type = Type::PTR;
    } else {
        // 如果不是数组，则直接设置为声明的类型
        attribute.T.type = type_decl;
    }

    // 检查当前作用域中是否已经存在同名的参数
    if (semant_table.symbol_table.lookup_scope(name) != -1) {
        error_msgs.push_back("multiple definitions of formals in function " + name->get_string() + " in line " +
                             std::to_string(line_number) + "\n");
    }

    // 将参数的名称和属性添加到符号表中
    semant_table.symbol_table.add_Symbol(name, val);
}

void __FuncDef::TypeCheck() {
    debug_msgs.push_back("FuncDef Semant");

    semant_table.symbol_table.enter_scope();
    if (return_type != Type::INT && return_type != Type::FLOAT && return_type != Type::VOID)
        error_msgs.push_back("ERROR: Function " + name->get_string() + " has a wrong return type at line " +
                             std::to_string(line_number) + "\n");
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

    // 获取函数的参数列表，并对每个参数进行类型检查
    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();
    }
    semant_table.FunctionTable[name] = this;

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
    debug_msgs.push_back("CompUnitDecl Semant");
    decl->isglobal = true;
    decl->TypeCheck();
}

void CompUnit_FuncDef::TypeCheck() {
    debug_msgs.push_back("CompUnit_FuncDef Semant");
    func_def->TypeCheck();
}