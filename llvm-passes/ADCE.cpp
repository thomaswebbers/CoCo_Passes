/*
 * ADCE to eliminate dead instructions
 */

#define DEBUG_TYPE "ADCE"
#include "utils.h"

namespace {
    class ADCE : public FunctionPass {
    public:
        static char ID;
        ADCE() : FunctionPass(ID) {}
        virtual bool runOnFunction(Function &F) override;
    };
}

bool ADCE::runOnFunction(Function &F) {
    LOG_LINE("Visiting function " << F.getName());

    for (BasicBlock &BB : F) {
        for (Instruction &II : BB) {
            Instruction *I = &II;
            if (CallInst *CI = dyn_cast<CallInst>(I)) {
                LOG_LINE(" Found call: " << *CI);
            }
        }
    }

    //!Initial pass to mark trivially live and dead instructions
    //LiveSet = emptySet;
    //for (each BB in F in depth-first order)
    //  for (each instruction in BB)
    //      if (isTriviallyLive(I))
    //          markLive(I)
    //      else if (I.use_empty())
    //          remove I from BB
    //
    //!Worklist to find new live instructions
    //while (WorkList is not empty)
    //  I = get instruction at head of working list;
    //  if (basic block containing I is reachable)
    //      for (all operands op of I)
    //          if (operand op is an instruction)
    //              markLive(op)
    //
    //!Delete instructions not in LiveSet
    //for (each BB in F in any order)
    //  if (BB is reachable)
    //      for (each non-live instruction I in BB)
    //          I.dropAllReferences();
    //for (each BB in F in any order)
    //  if (BB is reachable)
    //      for (each non-live instruction I in BB)
    //          erase I from BB;


    return true;  // We did alter the IR
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-ADCE, the second argument is a description shown in the help text
// about this pass.
char ADCE::ID = 0;
static RegisterPass<ADCE> X("coco-ADCE", "Eliminate dead instructions from the IR");
