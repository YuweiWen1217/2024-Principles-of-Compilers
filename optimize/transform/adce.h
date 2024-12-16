#ifndef ADCE_H
#define ADCE_H
#include "../../include/ir.h"
#include "../pass.h"
#include "../analysis/dominator_tree.h"

class AdcePass: public IRPass {
private:

    void adce(CFG *C);
     DominatorTree* BuildCDG(CFG *C);

public:
    AdcePass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif