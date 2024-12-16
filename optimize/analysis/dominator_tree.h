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
    // dom_tree[i] 存储的是支配块 i 的块。即，从这些块出发，一定能到达i。
    std::vector<std::vector<LLVMBlock>> dom_tree{};
    std::vector<std::bitset<N>> dom;    // dom[i][x]，储存i的支配块，为1,则x支配i，为0，则x不支配i

    std::unordered_map<int, std::set<int>> df;    // 支配边界

    // idom[i] 存储的是块i的立即支配块（所有支配块中，距离i最近的那个）
    std::vector<LLVMBlock> idom{};

    // 如果 reverse 为 true，则建立反向支配树。
    void BuildDominatorTree(bool reverse = false);

    // DF[u] = {v}：
    // u支配v的前驱结点，但不支配v，或者v就是u。
    // v = u <=> u支配其自己的前驱节点。
    // 求df[u]：遍历u支配的点的后继节点，若不被u支配或就是u本身，则放入集合中。
    // df[u,v] = df[u] U df[v]
    std::set<int> GetDF(std::set<int> S);
    std::set<int> GetDF(int id);

    // id1是否支配id2
    bool IsDominate(int id1, int id2);
    void printRows(size_t n) {
        if (dom.empty() || n == 0) {
            std::cout << "No rows to print." << std::endl;
            return;
        }

        for (size_t row = 0; row < n; ++row) {
            std::vector<size_t> positions;

            for (size_t col = 0; col < n; ++col) {
                if (dom[row][col]) {
                    positions.push_back(col);
                }
            }

            if (!positions.empty()) {
                std::cout << row << " <-";
                for (size_t pos : positions) {
                    std::cout << " " << pos;
                }
                std::cout << std::endl;
            }
        }
    }
    void PrintDF(size_t n) {
        for (int i = 0; i < n; i++) {
            int id = i;
            const std::set<int> &dfSet = df[id];
            //  打印块号
            std::cout << id << ": ";
            // 打印支配边界集合
            for (const int block : dfSet) {
                std::cout << " " << block;
            }
            std::cout << std::endl;
        }
    }
    void Printidom(size_t n) {
        for (int i = 0; i < n; i++) {
            if (idom[i])
                std::cout << "idom[" << i << "]: " << idom[i];
            else
                std::cout << "idom[" << i << "]: nullptr";
            std::cout << std::endl;
        }
    }
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