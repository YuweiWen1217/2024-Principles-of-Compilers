#ifndef SEMANT_H
#define SEMANT_H

#include "../include/SysY_tree.h"
#include "../include/symtab.h"
#include "../include/type.h"
#include <map>
#include <set>
#include <vector>
extern IdTable id_table;  // 符号表
/*
semant
*/
class SemantTable {
public:
    // 如果你对已有的成员变量不满意，可以删除它们并添加自己想要的

    // （函数名->函数名对应的语法树、函数定义）
    // 可以用于函数的错误检查(例如函数调用实参与形参是否匹配)
    std::map<Symbol, FuncDef> FunctionTable;

    // 存储局部变量名与该局部变量的信息(初始值，类型等)
    SymbolTable symbol_table;

    // 全局变量名->其信息（初始值、类型等）
    // 我们可以在semant阶段就在llvmIR中生成全局变量定义指令
    std::map<Symbol, VarAttribute> GlobalTable;

    // 构造函数，用于初始化符号表
    SemantTable() {
        // 添加库函数到函数表中, 我们不要求实现putf这个库函数
        // 可以非常方便地检查对库函数的调用是否符合定义
        Symbol getint = id_table.add_id("getint"); // 添加函数名到符号表
        FunctionTable[getint] = new __FuncDef(Type::INT, getint, new std::vector<FuncFParam>{}, nullptr);
        // getint函数返回整型，无参数

        Symbol getch = id_table.add_id("getch");
        FunctionTable[getch] = new __FuncDef(Type::INT, getch, new std::vector<FuncFParam>{}, nullptr);
        // getch函数返回整型，无参数

        Symbol getfloat = id_table.add_id("getfloat");
        FunctionTable[getfloat] = new __FuncDef(Type::FLOAT, getfloat, new std::vector<FuncFParam>{}, nullptr);
        // getfloat函数返回浮点型，无参数

        Symbol getarray = id_table.add_id("getarray");
        FunctionTable[getarray] = new __FuncDef(
            Type::INT, getarray,
            new std::vector<FuncFParam>{new __FuncFParam(Type::INT, new std::vector<Expression>(1, nullptr))}, nullptr);
        // getarray函数返回整型，接受一个整型参数（数组索引）

        Symbol getfarray = id_table.add_id("getfarray");
        FunctionTable[getfarray] = new __FuncDef(
            Type::INT, getfarray,
            new std::vector<FuncFParam>{new __FuncFParam(Type::FLOAT, new std::vector<Expression>(1, nullptr))}, nullptr);
        // getfarray函数返回整型，接受一个浮点型参数（数组索引）

        Symbol putint = id_table.add_id("putint");
        FunctionTable[putint] =
            new __FuncDef(Type::VOID, putint, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);
        // putint函数返回空，接受一个整型参数

        Symbol putch = id_table.add_id("putch");
        FunctionTable[putch] =
            new __FuncDef(Type::VOID, putch, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);
        // putch函数返回空，接受一个整型参数

        Symbol putfloat = id_table.add_id("putfloat");
        FunctionTable[putfloat] =
            new __FuncDef(Type::VOID, putfloat, new std::vector<FuncFParam>{new __FuncFParam(Type::FLOAT)}, nullptr);
        // putfloat函数返回空，接受一个浮点型参数

        Symbol putarray = id_table.add_id("putarray");
        FunctionTable[putarray] =
            new __FuncDef(Type::VOID, putarray,
                          new std::vector<FuncFParam>{new __FuncFParam(Type::INT),
                                                      new __FuncFParam(Type::INT, new std::vector<Expression>(1, nullptr))},
                          nullptr);
        // putarray函数返回空，接受两个参数：整型和整型（数组索引）

        Symbol putfarray = id_table.add_id("putfarray");
        FunctionTable[putfarray] = new __FuncDef(
            Type::VOID, putfarray,
            new std::vector<FuncFParam>{new __FuncFParam(Type::INT),
                                        new __FuncFParam(Type::FLOAT, new std::vector<Expression>(1, nullptr))},
            nullptr);
        // putfarray函数返回空，接受两个参数：整型和浮点型（数组索引）

        Symbol starttime = id_table.add_id("_sysy_starttime");
        FunctionTable[starttime] =
            new __FuncDef(Type::VOID, starttime, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);
        // _sysy_starttime函数返回空，接受一个整型参数

        Symbol stoptime = id_table.add_id("_sysy_stoptime");
        FunctionTable[stoptime] =
            new __FuncDef(Type::VOID, stoptime, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);
        // _sysy_stoptime函数返回空，接受一个整型参数

        // 这里你可能还需要对这些语法树上的节点进行类型的标注, 进而检查对库函数的调用是否符合形参
        // 即正确填写NodeAttribute或者使用其他方法来完成检查
    }
};

#endif