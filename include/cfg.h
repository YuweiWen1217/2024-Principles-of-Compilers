#ifndef CFG_H
#define CFG_H

#include "SysY_tree.h"
#include "basic_block.h"
#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct FuncRegInfo {
    std::unordered_set<Operand> unusedRegs;
    std::unordered_map<Operand, std::unordered_set<int>> regToBlocks;
};

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

    //----新增变量&函数-----
    FuncRegInfo regInfo;     // CFG对应函数的寄存器信息
    int Label_num;           // 用于重新构建G、invG时，resize大小
    std::vector<int> ord;    // 逆后序
    void Check_one_block(int bbid); // 用于dfs构建cfg时检查

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

    // PrintCFG仅作调试用
    void PrintCFG() {
        std::cout << "Graph G (Control Flow Graph):" << std::endl;
        for (size_t i = 0; i < G.size(); ++i) {
            std::cout << "Block " << i << " -> ";
            for (auto &block : G[i]) {
                std::cout << block->block_id << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "Graph invG (Inverse Control Flow Graph):" << std::endl;
        for (size_t i = 0; i < invG.size(); ++i) {
            std::cout << "Block " << i << " <- ";
            for (auto &block : invG[i]) {
                std::cout << block->block_id << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "ord: ";
        for (auto &i : ord) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
};

#endif