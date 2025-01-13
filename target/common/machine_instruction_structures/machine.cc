#include "machine.h"
#include "assert.h"

MachineDataType INT32(MachineDataType::INT, MachineDataType::B32);
MachineDataType INT64(MachineDataType::INT, MachineDataType::B64);
MachineDataType INT128(MachineDataType::INT, MachineDataType::B128);

MachineDataType FLOAT_32(MachineDataType::FLOAT, MachineDataType::B32);
MachineDataType FLOAT64(MachineDataType::FLOAT, MachineDataType::B64);
MachineDataType FLOAT128(MachineDataType::FLOAT, MachineDataType::B128);

// 将一个空的基本块节点分配到控制流图（CFG）中
void MachineCFG::AssignEmptyNode(int id, MachineBlock *Mblk) {
    // 如果给定的基本块编号 id 大于当前的最大标签值，则更新最大标签值
    if (id > this->max_label) {
        this->max_label = id;    // 更新控制流图的最大基本块标签值
    }

    // 创建一个新的 MachineCFGNode 并与提供的 MachineBlock 关联
    MachineCFGNode *node = new MachineCFGNode;
    node->Mblock = Mblk;     // 将当前的 MachineBlock 关联到新的 CFG 节点
    block_map[id] = node;    // 将该节点存储到 block_map 中，索引为基本块的 id

    // 确保控制流图（G）的大小足够容纳 id + 1 个基本块
    while (G.size() < id + 1) {
        G.push_back({});    // 如果当前图的大小不足，则添加一个空的基本块列表
        // G.resize(id + 1); // 也可以使用 resize 方法扩展图的大小
    }

    // 确保反向控制流图（invG）的大小足够容纳 id + 1 个基本块
    while (invG.size() < id + 1) {
        invG.push_back({});    // 如果当前反向图的大小不足，则添加一个空的基本块列表
        // invG.resize(id + 1); // 也可以使用 resize 方法扩展反向图的大小
    }
}

// Just modify CFG edge, no change on branch instructions
void MachineCFG::MakeEdge(int edg_begin, int edg_end) {
    Assert(block_map.find(edg_begin) != block_map.end());
    Assert(block_map.find(edg_end) != block_map.end());
    Assert(block_map[edg_begin] != nullptr);
    Assert(block_map[edg_end] != nullptr);
    G[edg_begin].push_back(block_map[edg_end]);
    invG[edg_end].push_back(block_map[edg_begin]);
}

// Just modify CFG edge, no change on branch instructions
void MachineCFG::RemoveEdge(int edg_begin, int edg_end) {
    assert(block_map.find(edg_begin) != block_map.end());
    assert(block_map.find(edg_end) != block_map.end());
    auto it = G[edg_begin].begin();
    for (; it != G[edg_begin].end(); ++it) {
        if ((*it)->Mblock->getLabelId() == edg_end) {
            break;
        }
    }
    G[edg_begin].erase(it);

    auto jt = invG[edg_end].begin();
    for (; jt != invG[edg_end].end(); ++jt) {
        if ((*jt)->Mblock->getLabelId() == edg_begin) {
            break;
        }
    }
    invG[edg_end].erase(jt);
}

Register MachineFunction::GetNewRegister(int regtype, int reglength) {
    static int new_regno = 0;
    Register new_reg;
    new_reg.is_virtual = true;
    new_reg.reg_no = new_regno++;
    new_reg.type.data_type = regtype;
    new_reg.type.data_length = reglength;
    // InitializeNewVirtualRegister(new_regno);
    return new_reg;
}

Register MachineFunction::GetNewReg(MachineDataType type) { return GetNewRegister(type.data_type, type.data_length); }

MachineBlock *MachineFunction::InitNewBlock() {
    int new_id = ++max_exist_label;
    MachineBlock *new_block = block_factory->CreateBlock(new_id);
    new_block->setParent(this);
    blocks.push_back(new_block);
    mcfg->AssignEmptyNode(new_id, new_block);
    return new_block;
}
