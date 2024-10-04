#include "../../machine_instruction_structures/machine.h"
#include "liveinterval.h"

// 为了实现方便，这里直接使用set进行活跃变量分析，如果你不满意，可以自行更换更高效的数据结构(例如bitset)
template <class T> std::set<T> SetIntersect(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret;
    for (auto x : b) {
        if (a.count(x) != 0) {
            ret.insert(x);
        }
    }
    return ret;
}

template <class T> std::set<T> SetUnion(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.insert(x);
    }
    return ret;
}

// a-b
template <class T> std::set<T> SetDiff(const std::set<T> &a, const std::set<T> &b) {
    std::set<T> ret(a);
    for (auto x : b) {
        ret.erase(x);
    }
    return ret;
}

std::vector<Register *> MachinePhiInstruction::GetReadReg() {
    std::vector<Register *> ret;
    for (auto [label, op] : phi_list) {
        if (op->op_type == MachineBaseOperand::REG) {
            ret.push_back(&(((MachineRegister *)op)->reg));
        }
    }
    return ret;
}
std::vector<Register *> MachinePhiInstruction::GetWriteReg() { return std::vector<Register *>({&result}); }

void Liveness::UpdateDefUse() {

    DEF.clear();
    USE.clear();

    auto mcfg = current_func->getMachineCFG();
    // 顺序遍历每个基本块
    auto seq_it = mcfg->getSeqScanIterator();
    seq_it->open();
    while (seq_it->hasNext()) {
        auto node = seq_it->next();

        // DEF[B]: 在基本块B中定义，并且定义前在B中没有被使用的变量集合
        // USE[B]: 在基本块B中使用，并且使用前在B中没有被定义的变量集合
        DEF[node->Mblock->getLabelId()].clear();
        USE[node->Mblock->getLabelId()].clear();

        auto &cur_def = DEF[node->Mblock->getLabelId()];
        auto &cur_use = USE[node->Mblock->getLabelId()];

        // 根据DEF和USE的定义编写代码
        TODO("Calculate DEF and USE in this block, and store them in cur_def and cur_use");
    }
}

void Liveness::Execute() {
    UpdateDefUse();

    OUT.clear();
    IN.clear();

    auto mcfg = current_func->getMachineCFG();
    bool changed = 1;
    // 基于数据流分析的活跃变量分析
    while (changed) {
        changed = 0;
        // 顺序遍历每个基本块
        auto seq_it = mcfg->getSeqScanIterator();
        seq_it->open();
        while (seq_it->hasNext()) {
            auto node = seq_it->next();
            std::set<Register> out;
            int cur_id = node->Mblock->getLabelId();
            for (auto succ : mcfg->GetSuccessorsByBlockId(cur_id)) {
                out = SetUnion<Register>(out, IN[succ->Mblock->getLabelId()]);
            }
            if (out != OUT[cur_id]) {
                OUT[cur_id] = out;
            }
            std::set<Register> in = SetUnion<Register>(USE[cur_id], SetDiff<Register>(OUT[cur_id], DEF[cur_id]));
            if (in != IN[cur_id]) {
                changed = 1;
                IN[cur_id] = in;
            }
        }
    }
}