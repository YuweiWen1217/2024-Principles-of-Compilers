#ifndef CFG_H
#define CFG_H

#include "SysY_tree.h"
#include "basic_block.h"
#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

class CFG {
public:
    FuncDefInstruction function_def;    // 存储函数定义的指令信息

    /*
    这是指向LLVMIR中 `function_block_map` 的指针，
    在 `LLVMIR::CFGInit()` 中可以看到这个指针的使用，
    它存储了函数的基本块信息。
    */
    /*this is the pointer to the value of LLVMIR.function_block_map
       you can see it in the LLVMIR::CFGInit()*/
    std::map<int, LLVMBlock> *block_map;

    // 使用邻接表表示图结构
    std::vector<std::vector<LLVMBlock>> G{};       // 控制流图（CFG），存储每个基本块的后继基本块
    std::vector<std::vector<LLVMBlock>> invG{};    // 逆控制流图，存储每个基本块的前驱基本块

    /**
     * 构建控制流图（CFG）和逆控制流图（invG）。
     * CFG 描述了程序中基本块之间的控制流方向，invG 描述了基本块的前驱关系。
     */
    void BuildCFG();

    // 获取某个基本块节点的前驱/后继
    std::vector<LLVMBlock> GetPredecessor(LLVMBlock B);
    std::vector<LLVMBlock> GetPredecessor(int bbid);
    std::vector<LLVMBlock> GetSuccessor(LLVMBlock B);
    std::vector<LLVMBlock> GetSuccessor(int bbid);
};

#endif