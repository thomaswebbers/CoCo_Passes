/*
 * Dummy (and minimal) function pass to serve as a starting point for your
 * Assignment 2 passes. It simply visits every function and prints every call
 * instruction it finds.
 */

#define DEBUG_TYPE "CONSTPR"
#include "utils.h"

namespace {
    class CONSTPR : public FunctionPass {
    public:
        static char ID;
        CONSTPR() : FunctionPass(ID) {}
        virtual bool runOnFunction(Function &F) override;
    };
}

bool CONSTPR::runOnFunction(Function &F) {
    LOG_LINE("Visiting function " << F.getName());

    for (BasicBlock &BB : F) {
        for (Instruction &II : BB) {
            Instruction *I = &II;
            if (CallInst *CI = dyn_cast<CallInst>(I)) {
                LOG_LINE(" Found call: " << *CI);
            }
        }
    }

    return false;  // We did not alter the IR
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-dummypass, the second argument is a description shown in the help text
// about this pass.
char CONSTPR::ID = 0;
static RegisterPass<CONSTPR> X("coco-dummypass", "Example LLVM pass printing each function it visits, and every call instruction it finds");
