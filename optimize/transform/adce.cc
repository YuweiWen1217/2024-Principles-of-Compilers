#include <adce.h>
#include <algorithm>

// Reference：https://www.cnblogs.com/lixingyang/p/17728846.html
// Reference: 虎书，P321——323

// CDG（控制依赖图）
// x -> u、v
// u -> y、exit
// v -> y -> exit
// 这个例子中，y控制依赖于x。
// 即： x有两个后继节点，一节点可以不经过y，另一个节点支配y。

// 虎书中构建CDG算法：
// 1、G中加入一个节点r，指向0号块和ret块。
// 2、逆转所有边，构建逆CFG
// 3、构建必经节点树（支配树）
// 4、根节点对应于exit节点。
// 5、计算支配边界DF
// 6、对于x的每个支配边界y1、y2···，CDG有边：y1 -> x, y2 -> x, ···，x控制依赖于y1、y2
//  即：x控制依赖于它的支配边界。
// ---------经博客作者介绍，不必在G中加入一个节点r。

// dfs
void DFS(int node, const std::vector<std::vector<LLVMBlock>> &graph, std::vector<bool> &visited,
         std::vector<int> &postOrder) {
    if (visited[node])
        return;
    visited[node] = true;
    for (auto neighborBlock : graph[node]) {
        auto blockid = ((BasicBlock *)neighborBlock)->block_id;
        if (!visited[blockid])
            DFS(blockid, graph, visited, postOrder);
    }
    postOrder.push_back(node);
}

// 构建控制依赖图（Control Dependence Graph, CDG）
DominatorTree *AdcePass::BuildCDG(CFG *C) {

    // 1、新建一个CFG对象并逆转边
    CFG *invCFG = new CFG();
    invCFG->G = C->invG;                 // 交换图：逆转后的CFG
    invCFG->invG = C->G;                 // 原CFG的正向图
    invCFG->block_map = C->block_map;    // 保持块映射一致

    invCFG->Label_num = C->Label_num;    // 保持块数量一致
    invCFG->ord.clear();                 // 清空后序遍历顺序

    // 2、后序遍历得到ord
    std::vector<bool> visited(C->Label_num, false);
    std::vector<int> postOrder;
    for (int i = 0; i < C->Label_num; ++i) {
        // 对于所有起点，进行dfs
        if (invCFG->invG[i].empty() && !invCFG->G[i].empty())
            DFS(i, invCFG->G, visited, postOrder);
    }
    std::reverse(postOrder.begin(), postOrder.end());
    invCFG->ord = postOrder;

    // 3、基于逆CFG，构建支配树
    DominatorTree *postDomTree = new DominatorTree();
    postDomTree->C = invCFG;
    postDomTree->BuildDominatorTree();
    return postDomTree;
}

void AdcePass::adce(CFG *C) {

    // 1. 定义用于追踪活跃指令和寄存器的工作空间
    std::stack<Instruction> workList;               // 工作列表，用于记录待处理的活跃指令
    std::unordered_map<int, Instruction> defmap;    // 映射寄存器号到其定义的指令

    std::unordered_map<Instruction, int> Inst2bid;

    // 活跃指令 + 所有有活跃指令的块
    std::unordered_set<Instruction> liveInsts;
    std::set<int> liveBlocks;

    // 构建控制依赖图（CDG）的前驱关系
    auto CDG = BuildCDG(C);    // 控制依赖前驱
    // CDG->PrintDF(C->Label_num);
    // CDG->printRows(C->Label_num);
    // CDG->Printidom(C->Label_num);


    // 2. 初始化 defmap 和初始的活跃指令工作列表
    for (auto [bid, block] : *C->block_map) {
        for (auto Inst : block->Instruction_list) {
            Inst2bid[Inst] = bid;
            // 2.1 所有调用函数，函数返回，对存储器的操作(产生副作用)为有效代码
            auto Opcode = Inst->GetOpcode();
            if (Opcode == BasicInstruction::CALL || Opcode == BasicInstruction::RET ||
                Opcode == BasicInstruction::STORE)
                workList.push(Inst);

            // 2.2 填充defmap，同dce
            auto resultRegNo = Inst->GetResultRegNo();
            if (resultRegNo != -1) {
                defmap[resultRegNo] = Inst;
            }
        }
    }

    // 3. 遍历工作列表，标记活跃指令及其相关依赖
    // 哪些需要被标记？
    // 1) 对于一条phi指令，它的每一个前驱都应当被标注为活跃的(块加入liveBlocks、最后一条指令加入workList)
    // 2) 加入该块的所有控制依赖前驱
    // 3) 对于这个指令的操作数Reg，将其def加入workList
    while (!workList.empty()) {

        auto Inst = workList.top();
        workList.pop();

        // 如果指令已被标记为有效代码，则跳过
        if (liveInsts.find(Inst) != liveInsts.end())
            continue;
        // 将指令标记为有效代码
        liveInsts.insert(Inst);
        // 获取指令所属的基本块
        auto block_id = Inst2bid[Inst];    // 当前指令的基本块 ID

        auto block = (*C->block_map)[block_id];    // 当前基本块

        liveBlocks.insert(block_id);    // 标记基本块为活跃

        // 3.1 PHI 指令，每一个前驱标注为活跃的
        auto Opcode = Inst->GetOpcode();
        if (Opcode == BasicInstruction::PHI) {
            auto PhiI = (PhiInstruction *)Inst;
            for (auto [Labelop, Regop] : PhiI->GetPhiList()) {
                auto Label = (LabelOperand *)Labelop;
                auto Labelno = Label->GetLabelNo();    // 来源基本块 ID
                auto LastInst = (*C->block_map)[Labelno]->Instruction_list.back();

                // 如果来源基本块的最后一条指令未标记为活跃，则标记为活跃
                if (liveInsts.find(LastInst) == liveInsts.end()) {
                    workList.push(LastInst);
                    liveBlocks.insert(Labelno);
                }
            }
        }
        // 3.2 处理控制依赖前驱(控制依赖于的点，即支配边界)
        for (auto DFBlockid : CDG->GetDF(block_id)) {
            auto LastInst = (*C->block_map)[DFBlockid]->Instruction_list.back();
            if (liveInsts.find(LastInst) == liveInsts.end()) {
                workList.push(LastInst);
            }
        }
        // 3.3 处理其每个use的变量
        for (auto Regopno : Inst->GetOperandRegsNo()) {
            if (defmap.find(Regopno) != defmap.end()) {
                auto DefI = defmap[Regopno];
                if (liveInsts.find(DefI) == liveInsts.end()) {
                    workList.push(DefI);
                }
            }
        }
    }

    // 4. 删除非活跃指令
    for (auto [bid, block] : *C->block_map) {
        auto LastInst = (*C->block_map)[bid]->Instruction_list.back();
        auto tmp_Instruction_list = block->Instruction_list;    // 暂存指令列表

        block->Instruction_list.clear();    // 清空基本块的指令列表

        for (auto I : tmp_Instruction_list) {
            if (liveInsts.find(I) == liveInsts.end()) {
                // 如果是非活跃的终止指令，替换为跳转到活跃基本块的指令
                if (LastInst == I) {
                    auto liveblockid = CDG->idom[bid]->block_id;    // 后支配的直接前驱块 ID
                    while (liveBlocks.find(liveblockid) == liveBlocks.end()) {
                        liveblockid = CDG->idom[bid]->block_id;    // 找到最近的活跃基本块
                    }
                    I = new BrUncondInstruction(GetNewLabelOperand(liveblockid));    // 创建无条件跳转指令
                } else {
                    continue;    // 跳过其他非活跃指令
                }
            }
            block->InsertInstruction(1, I);    // 将指令插入到新的列表
        }
    }
}

void AdcePass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        // std::cout << "111" << std::endl;
        adce(cfg);
    }
}

// 激进的死代码删除算法会删除没有输出的无限循环，从而会改变程序的含义。
// 因为在原来的程序不产生任何输出的情况下，删除这种无限循环后，程序会执行该循环之后的语句，而这些语句有可能会产生输出。
// 在许多环境下，这被认为是不可接受的。