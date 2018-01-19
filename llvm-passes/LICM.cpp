/*
 * LICM (Loop Invariant Code Motion): move computations out of a loop
 *      without changing the semantics of the program
 */

#define DEBUG_TYPE "LICM"
#include "utils.h"

namespace {
    class LICM : public LoopPass {
    public:
        static char ID;
        LICM() : LoopPass(ID) {}
        virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override;
        void getAnalysisUsage(AnalysisUsage &AU) const override;
    };
}

void LICM::getAnalysisUsage(AnalysisUsage &AU) const {
    // Tell LLVM we need some analysis info which we use for analyzing the
    // DominatorTree.
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
    getLoopAnalysisUsage(AU);
}

bool LICM::isLoopInvariant(Instruction &I){ //!
    //is binary operator, shift, select, cast or getelementptr...
    if(isa<BinaryOperator>(I) ||I.isShift() || isa<SelectInst>(I) || isa<CastInst>(I) || isa<GetElementPtrInst>(I)){
        //...and all operand of the instruction are either a constant or computed outside the loop
        //! same issue as ADCE, first part easy with isa<Constant>(op)
    }
    return false;
}

bool LICM::isSafeToHoist(Instruction &I){//!
    //safe to hoist if it has no side effects...
    if (!I.mayHaveSideEffects()){
        return true;
    }
    //...or the basic block containing the instruction dominates all exit blocks for the loop
    //! use Loop::getExitBlocks()
    for ()
    return false;
}

bool LICM::runOnLoop(Loop *L, LPPassManager &LPM) {
    //get current loop header
    BasicBlock *Header = L->getHeader();
    //!get the current loop preheader (for later)

    //get every basic block dominated by the current loop header
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    for (BasicBlock *BB : L->blocks()) {
        if (DT->dominates(Header, BB)) {
            //Check if a basic block is not in a sub-loop
            //!use LoopInfoWrapperPass
            if (false){ //!
                //move every instruction in BB...
                for (Instruction &I : *BB){
                    //...that is a "loop invariant", and is "safe to hoist"
                    if (isLoopInvariant(I) && isSafeToHoist(I)){
                        //...to the preheader of the loop.
                        //!move to preheader, cannot create new Instruction?
                        //remove from original location
                    }
                }
            }
        }
    }

    return true; //we did modify the IR
}

char LICM::ID = 0;
RegisterPass<LICM> X("coco-licm", "CoCo Loop Invariant Code Motion");
