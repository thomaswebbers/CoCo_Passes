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

bool isTriviallyLive (Instruction &I){
    if(I.mayHaveSideEffects()){ //may have side effects
        return true;
    }else if(I.isTerminator()){ //is terminator
        return true;
    }else if(dyn_cast<LoadInst>(I).isVolatile()) //is load AND !?volatile?
        return true;
    }else if (isa<StoreInst>(I)) //is write
        return true;
    }else if (isa<CallInst>(I)){ // Is call
        return true;
    }
    return false;
}

bool ADCE::runOnFunction(Function &F) {

    df_iterator_default_set<BasicBlock*> Reachable;

    //!Initial pass to mark trivially live and dead instructions
    //LiveSet = emptySet;
    SmallSet<Instruction, 16> liveSet;
    //for (each BB in F in depth-first order)
    for (BasicBlock &BB : depth_first_ext(&F, Reachable)){
    //  for (each instruction in BB)
        for (Instruction &II : BB) {
    //      if (isTriviallyLive(I))
            if (isTriviallyLive(I)){
    //          markLive(I)
                liveSet.insert(I);
    //      else if (I.use_empty())
            }else if(I.use_empty()){
    //          remove I from BB
                I.eraseFromParent();
            }
    //
        }
    }
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
