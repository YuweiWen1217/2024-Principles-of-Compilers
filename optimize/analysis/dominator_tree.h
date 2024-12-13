#ifndef DOMINATOR_TREE_H
#define DOMINATOR_TREE_H
#include "../../include/ir.h"
#include "../pass.h"
#include <set>
#include <vector>
const int N = 7010;

class DominatorTree {
public:
    CFG *C;    // 函数、CFG、支配树一一对应

    // 支配块：v支配u，即v是u的支配块。从源点遍历到u的每条路径上，都有v。
    // dom_tree[i] 存储的是块 i 支配的块。即，从i出发，必定到达的块。
    std::vector<std::vector<LLVMBlock>> dom_tree{};
    std::vector<std::bitset<N>> dom;  // dom[i][x]，储存i的支配块，为1,则x支配i，为0，则x不支配i

    // idom[i] 存储的是块i的立即支配块（所有支配块中，距离i最近的那个）
    std::vector<LLVMBlock> idom{};

    // 如果 reverse 为 true，则建立反向支配树。
    void BuildDominatorTree(bool reverse = false);

    // DF[u] = {v}：
    // 1. u == v
    // 2. u支配v的前驱结点，但不支配v
    // 求df[u]：将u放入集合中；遍历u支配的点的后继节点，若不被u支配，则放入集合中。
    // df[u,v] = df[u] U df[v]
    std::set<int> GetDF(std::set<int> S);
    std::set<int> GetDF(int id);

    // id1是否支配id2
    bool IsDominate(int id1, int id2);

};

class DomAnalysis : public IRPass {
private:
    // CFG->支配树
    std::map<CFG *, DominatorTree> DomInfo;

public:
    DomAnalysis(LLVMIR *IR) : IRPass(IR) {}

    void Execute();

    DominatorTree *GetDomTree(CFG *C) { return &DomInfo[C]; }

};

#endif