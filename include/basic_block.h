#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "Instruction.h"
#include <deque>
#include <iostream>
#include <set>
#include <vector>

// 请注意代码中的typedef，为了方便书写，将一些类的指针进行了重命名, 如果不习惯该种风格，可以自行修改
class BasicBlock {
public:
    std::string comment;    // used for debug 存储该基本块的注释或调试信息

    int block_id = 0;    // 基本块的唯一标识符

    // 基本块中 IR 指令的集合；双端队列允许我们在队列的头部和尾部快速插入或删除指令。
    std::deque<Instruction> Instruction_list{};

    FuncDefInstruction Function;    // 该块所属函数。

    /*
        pos = 1 -> end   pos = 0 -> begin
        inst1  <- front
        ...
        inst5  <- back
    */

    // 插入指令，pos = 1在末尾插入、pos = 0`在开头插入
    void InsertInstruction(int pos, Instruction Ins);

    void printIR(std::ostream &s);

    BasicBlock(int id, FuncDefInstruction func) : block_id(id), Function(func) {}
};
typedef BasicBlock *LLVMBlock;

#endif