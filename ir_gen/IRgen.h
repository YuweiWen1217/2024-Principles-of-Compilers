#ifndef IRGEN_H
#define IRGEN_H

#include "../include/SysY_tree.h"
#include "../include/cfg.h"
#include "../include/symtab.h"
#include "../include/type.h"
#include <assert.h>
#include <map>
#include <vector>

class IRgenTable {
public:
    // 如果你无从下手,推荐先阅读LLVMIR类的printIR函数,了解我们是如何输出中间代码的
    // 然后你可以尝试往和输出相关联的变量中随便添加一些函数定义指令, 新建一些基本块或添加几条指令看看输出是怎么变化的
    // 弄懂LLVMIR类是如何存储中间代码后，剩余的就是理解中间代码生成算法了

    // TODO():添加更多你需要的成员变量和成员函数
    SymbolRegTable symbol_table;


    IRgenTable() {}

     // 当前基本块
    LLVMBlock currentBlock;

    // 寄存器栈，用于记录最近使用的寄存器编号
    std::vector<int> regStack;

    // 构造函数
    IRgenTable() {}

    // 获取新的寄存器编号
    int getNewRegNo() {
        // 假设寄存器编号从0开始，并依次递增
        return regStack.size();
    }

    // 获取最近使用的寄存器编号
    int getLastReg() const {
        assert(!regStack.empty());
        return regStack.back();
    }

    // 更新最近使用的寄存器编号
    void setLastReg(int regNo) {
        if (!regStack.empty()) {
            regStack.back() = regNo;
        } else {
            regStack.push_back(regNo);
        }
    }

    // 添加新的寄存器编号到寄存器栈
    void pushReg(int regNo) {
        regStack.push_back(regNo);
    }

    // 从寄存器栈中弹出最近使用的寄存器编号
    void popReg() {
        assert(!regStack.empty());
        regStack.pop_back();
    }

    // 获取当前基本块
    LLVMBlock getCurrentBlock() const {
        return currentBlock;
    }

    // 设置当前基本块
    void setCurrentBlock(LLVMBlock block) {
        currentBlock = block;
    }

    // 使用现有的GetNewRegOperand函数获取新的寄存器操作数
    RegOperand* getNewRegOperand() {
        int newRegNo = getNewRegNo();
        return GetNewRegOperand(newRegNo);
    }

};

#endif