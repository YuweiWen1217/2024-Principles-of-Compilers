#include "dominator_tree.h"
#include "../../include/ir.h"

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg].C = cfg;
        DomInfo[cfg].BuildDominatorTree();
    }
}

void DominatorTree::BuildDominatorTree(bool reverse) {
    const int n = C->Label_num;    // 这里用的是G的size，不是C的Label_num，写后面的内容时注意两者的改变。
    dom_tree.clear();
    dom_tree.resize(n);
    idom.clear();
    idom.resize(n);
    dom.clear();
    dom.resize(n);

    dom[0][0] = true;
    bool flag = true;
    while (flag) {
        flag = false;
        for (int u : C->ord) {
            std::bitset<N> tmp = std::bitset<N>(~0);    // 全true
            // 前驱节点的交集
            for (auto v : C->invG[u]) {
                int id = v->block_id;
                tmp &= dom[id];
            }
            // 并上自己
            tmp[u] = true;
            // 本次迭代只要有一个发生变化，那么就进入下次循环。
            if (tmp != dom[u]) {
                dom[u] = tmp;
                flag = true;
            }
        }
    }

    dom_tree[0].push_back((*C->block_map)[0]);
    for (int i = 1; i < n; ++i) {
         for (int j = 0; j < n; ++j) {
             if (dom[i][j]) {
                dom_tree[j].push_back((*C->block_map)[i]);
             }
        }
    }

    // 下面开始计算idom
    idom[0] = nullptr;    // 0号块没有立即支配块
    for (int u = 2; u < n; ++u) {
        for (auto v : dom_tree[u]) {
            // s -> v1 -> ... -> v -> ...->vm ->...-> vn ->...-> u
            // dom[v->block_id] & dom[u]:  s ～ v
            // s ～ v ^ dom[u] :  vm ～ u
            // 如果v是u的立即支配块，那么vm ～ u实际上只有u一个元素
            std::bitset<N> tmp = (dom[v->block_id] & dom[u]) ^ dom[u];
            if (tmp.count() == 1 and tmp[u]) {
                idom[u] = v;
                break;
            }
        }
    }
}

std::set<int> DominatorTree::GetDF(std::set<int> S) {
    std::set<int> result;
    for (int id : S) {
        std::set<int> df = GetDF(id);
        result.insert(df.begin(), df.end());
    }
    return result;
}

// u的支配边界：DF[u]，意为恰好支配不到的块
// DF[u] = {v}：
// 1. u == v
// 2. u支配v的前驱结点，但不支配v

// 求df[u]：将u放入集合中；遍历u支配的点的后继节点，若不被u支配，则放入集合中。
// df[u,v] = df[u] U df[v]
std::set<int> DominatorTree::GetDF(int id) {
    std::set<int> result;
    result.insert(id);
    // 遍历id支配的点的后继节点（该方法按列访问，空间局部性较差）
    for (int u = 0; u < dom.size(); u++) {
        if (u != id && dom[u][id]) {
            for (auto successor : C->G[u]) {
                int succ_id = successor->block_id;
                // 如果后继节点不被 id 支配，则加入结果集
                if (!dom[succ_id][id]) {
                    result.insert(succ_id);
                }
            }
        }
    }
    return result;
}

// id1是否支配id2
bool DominatorTree::IsDominate(int id1, int id2) { return dom[id2][id1]; }
