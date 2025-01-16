#include "physical_register.h"
bool PhysicalRegistersAllocTools::OccupyReg(int phy_id, LiveInterval interval) {
    // 你需要保证interval不与phy_id已有的冲突
    // 或者增加判断分配失败返回false的代码
    phy_occupied[phy_id].push_back(interval);
    return true;
}

bool PhysicalRegistersAllocTools::ReleaseReg(int phy_id, LiveInterval interval) {
    auto it = phy_occupied[phy_id].begin();
    while (it != phy_occupied[phy_id].end()) {
        if (*it == interval) {
            phy_occupied[phy_id].erase(it);
            return true;
        }
        it++;
    }
    return false;
}

bool PhysicalRegistersAllocTools::OccupyMem(int offset, LiveInterval interval) {
    for (int i = offset; i < offset + 2; i++) {
        while (i >= mem_occupied.size()) {
            mem_occupied.push_back({});
        }
        mem_occupied[i].push_back(interval);
    }
    return true;
}

bool PhysicalRegistersAllocTools::ReleaseMem(int offset, LiveInterval interval) {
    for (int i = offset; i < offset + 2; i++) {
        auto it = mem_occupied[i].begin();
        while (it != mem_occupied[i].end()) {
            if (*it == interval) {
                mem_occupied[i].erase(it);
                break;
            }
            it++;
        }
    }
    return true;
}

int PhysicalRegistersAllocTools::getIdleReg(LiveInterval interval) {
    std::vector<int> a = getValidRegs(interval);
    for (int i = 0; i < a.size(); i++) {
        bool ii = true;
        for (auto other_interval : phy_occupied[a[i]]) {
            if ((interval & other_interval)) {
                ii = false;
                break;
            }
        }
        if (ii)
            return a[i];
    }
    return -1;
}
int PhysicalRegistersAllocTools::getIdleMem(LiveInterval interval) {
    std::vector<bool> ok;
    ok.resize(mem_occupied.size(), true);
    for (int i = 0; i < mem_occupied.size(); i++) {
        ok[i] = true;
        for (auto other_interval : mem_occupied[i]) {
            if (interval & other_interval) {
                ok[i] = false;
                break;
            }
        }
    }
    int free_cnt = 0;
    for (int offset = 0; offset < ok.size(); offset++) {
        if (ok[offset])
            free_cnt++;
        else
            free_cnt = 0;
        if (free_cnt == interval.getReg().getDataWidth() / 4)
            return offset - free_cnt + 1;
    }
    return mem_occupied.size() - free_cnt;
}

int PhysicalRegistersAllocTools::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                              LiveInterval interval2) {
    ReleaseReg(p_reg1, interval1);
    ReleaseMem(offset_spill2, interval2);
    OccupyReg(p_reg1, interval2);
    return 0;
}

std::vector<LiveInterval> PhysicalRegistersAllocTools::getConflictIntervals(LiveInterval interval) {
    std::vector<LiveInterval> result;
    for (auto phy_intervals : phy_occupied) {
        for (auto other_interval : phy_intervals) {
            if (interval.getReg().type == other_interval.getReg().type && (interval & other_interval)) {
                result.push_back(other_interval);
            }
        }
    }
    return result;
}
