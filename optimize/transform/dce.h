#ifndef DCE_H
#define DCE_H
#include "../../include/ir.h"
#include "../pass.h"

class DcePass: public IRPass {
private:

    void dce(CFG *C);

public:
    DcePass(LLVMIR *IR) : IRPass(IR) {}
    void Execute();
};

#endif