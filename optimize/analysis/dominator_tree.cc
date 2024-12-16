#include "dominator_tree.h"
#include "../../include/ir.h"

void DomAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        DomInfo[cfg].C = cfg;
        DomInfo[cfg].BuildDominatorTree();
    }
}

void DominatorTree::BuildDominatorTree(bool reverse) {
    const int n = C->Label_num;
    dom_tree.clear();
    dom_tree.resize(n);
    idom.clear();
    idom.resize(n);
    dom.clear();
    dom.resize(n);
    df.clear();

    // dom的初始化：起点设置为0、其余点设置为1。（原因：环）
    for (int index : C->ord) {
        if (C->invG[index].size() != 0)
            dom[index].set();
    }

    bool flag = true;
    while (flag) {
        flag = false;
        for (int u : C->ord) {
            std::bitset<N> tmp;
            if (C->invG[u].size() != 0)
                tmp.set();
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

    // 赋值 dom_tree && df
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dom[i][j]) {
                // 构建 dom_tree[j]
                dom_tree[i].push_back((*C->block_map)[j]);
                // 同时计算 df[j]（遍历u支配的点的后继节点，若不被u支配，或其本身就是u，则放入集合中）
                for (auto successor : C->G[i]) {
                    int succ_id = successor->block_id;
                    if (!dom[succ_id][j] || succ_id == j) {
                        df[j].insert(succ_id);
                    }
                }
            }
        }
    }

    // 下面开始计算idom
    for (int u = 0; u < n; ++u) {
        if (dom_tree[u].size() == 1){
            idom[u] = nullptr;    // 起点块没有立即支配块
            continue;
        }
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
    return df[id];
    // std::set<int> result;
    // // 遍历id支配的点的后继节点（该方法按列访问，空间局部性较差）
    // for (int u = 0; u < dom.size(); u++) {
    //     if (dom[u][id]) {
    //         for (auto successor : C->G[u]) {
    //             int succ_id = successor->block_id;
    //             // 如果后继节点不被 id 支配，则加入结果集
    //             if (!dom[succ_id][id] || succ_id == j) {
    //                 result.insert(succ_id);
    //             }
    //         }
    //     }
    // }
    // return result;
}

// id1是否支配id2
bool DominatorTree::IsDominate(int id1, int id2) { return dom[id2][id1]; }
