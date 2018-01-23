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
    }

    if(I.isTerminator()){ //is terminator
        return true;
    }

    if(LoadInst *AI = dyn_cast<LoadInst>(&I)){ //is load AND !?volatile?
        return AI->isVolatile();
    }

    if (isa<StoreInst>(I)){ //is write
        return true;
    }

    if (isa<CallInst>(I)){ // Is call
        return true;
    }

    return false;
}

bool ADCE::runOnFunction(Function &F) {

    df_iterator_default_set<BasicBlock*> Reachable;
    //!Initial pass to mark trivially live and dead instructions
    //LiveSet = emptySet;
    std::set<Instruction*> liveSet; //! not using SmallSet, because not certain if can do .begin
    std::set<Instruction*> deadSet;
    //for (each BB in F in depth-first order)
    for (BasicBlock *BB : depth_first_ext(&F, Reachable)){
    //  for (each instruction in BB)
        for (Instruction &I : *BB) {
    //      if (isTriviallyLive(I))
            if (isTriviallyLive(I)){
    //          markLive(I)
                liveSet.insert(&I);
    //      else if (I.use_empty())
            }else if(I.use_empty()){
    //          remove I from BB
                deadSet.insert(&I);
            }
        }
    }
    //
    //!Worklist to find new live instructions
    std::set<Instruction*> checkedSet; //! using set of instructions, but giving reference instead of objects
    //while (WorkList is not empty)
    while(!liveSet.empty()){
    //  I = get instruction at head of working list;
        std::set<Instruction*>::iterator instructionIt = liveSet.begin();
        const Instruction* I = *instructionIt;
        checkedSet.insert(*instructionIt);
        liveSet.erase(instructionIt);
    //  if (basic block containing I is reachable)
        const BasicBlock* instructionBlock = I->getParent();
        if (Reachable.count(instructionBlock)){
    //      for (all operands op of I)
            unsigned opCount = I->getNumOperands();
            for (unsigned i = 0; i < opCount; ++i){
                Value* op = I->getOperand(i);
    //          if (operand op is an instruction)
                if (dyn_cast<Instruction>(op)){
    //              markLive(op)
                    Instruction* opIn = cast<Instruction>(op);
                    if(checkedSet.count(opIn) == 0){
                        liveSet.insert(opIn);
                    }
                }
            }
        }
    }
    //
    //!Delete instructions not in LiveSet
    //for (each BB in F in any order)
    for (BasicBlock &BB : F) {
    //  if (BB is reachable)
        if (Reachable.count(&BB)){ //!
    //      for (each non-live instruction I in BB)
            for (Instruction &I : BB) {
                if(checkedSet.count(&I) == 0){
    //              I.dropAllReferences();
                    I.dropAllReferences();
                }
            }
        }
    }
    //for (each BB in F in any order)
    for (BasicBlock &BB : F) {
    //  if (BB is reachable)
        if (Reachable.count(&BB)){ //!
    //      for (each non-live instruction I in BB)
            for (Instruction &I : BB) {
                if(checkedSet.count(&I) == 0){
    //              erase I from BB;
                    deadSet.insert(&I);
                }
            }
        }
    }
    std::set<Instruction*>::iterator it;
    for (it = deadSet.begin(); it != deadSet.end(); ++it){
        Instruction* pI = *it;
        pI->eraseFromParent();
    }
    return true;  // We did alter the IR
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-ADCE, the second argument is a description shown in the help text
// about this pass.
char ADCE::ID = 0;
static RegisterPass<ADCE> X("coco-adce", "Eliminate dead instructions from the IR");
