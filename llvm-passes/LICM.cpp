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

bool isLoopInvariant(Instruction &I, Loop *L){
    //is binary operator, shift, select, cast or getelementptr...
    if(isa<BinaryOperator>(I) ||I.isShift() || isa<SelectInst>(I) || isa<CastInst>(I) || isa<GetElementPtrInst>(I)){
        //...and all operand of the instruction are either a constant or computed outside the loop
        unsigned opCount = I.getNumOperands();
        for (unsigned i = 0; i < opCount; ++i){
            Value* op = I.getOperand(i);
            if(isa<Constant>(op)){
                continue;
            }
            Instruction* opIn = dyn_cast<Instruction>(op);
            if(opIn && !L->contains(opIn)){
                continue;
            }
            return false;
        }
        return true;
    }
    return false;
}

bool isSafeToHoist(Instruction &I, Loop *L, DominatorTree* DT){
    //safe to hoist if it has no side effects...
    if (!I.mayHaveSideEffects()){
        return true;
    }
    //...or the basic block containing the instruction dominates all exit blocks for the loop
    SmallVector<BasicBlock* , 4> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    bool result = true;
    BasicBlock* Parent = I.getParent();
    for(unsigned int i = 0; i < ExitBlocks.size(); i++){
        if(!DT->dominates(Parent, ExitBlocks[i])){
            result = false;
        }
    }
    return result;
}

bool LICM::runOnLoop(Loop *L, LPPassManager &LPM) {
    std::set<Instruction*> moveSet;
    //get current loop header
    BasicBlock *Header = L->getHeader();
    //get the current loop preheader terminator (for later)
    BasicBlock *PreHeader = L->getLoopPreheader();
    Instruction *preTerm = PreHeader->getTerminator();

    //get every basic block dominated by the current loop header
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfoWrapperPass *loopInfo = &getAnalysis<LoopInfoWrapperPass>();
    for (BasicBlock *BB : L->blocks()) {
        if (DT->dominates(Header, BB)) {
            //Check if a basic block is not in a sub-loop
            Loop* innerLoop = loopInfo->getLoopInfo().getLoopFor(BB);
            if (innerLoop == L){
                //move every instruction in BB...
                for (Instruction &I : *BB){
                    //...that is a "loop invariant", and is "safe to hoist"
                    if (isLoopInvariant(I, L) && isSafeToHoist(I, L, DT)){
                        //...to the preheader of the loop.
                        moveSet.insert(&I);
                    }
                }
            }
        }
    }
    std::set<Instruction*>::iterator it;
    for (it = moveSet.begin(); it != moveSet.end(); ++it){
        Instruction* pI = *it;
        pI->moveBefore(preTerm);
    }

    return true; //we did modify the IR
}

char LICM::ID = 0;
RegisterPass<LICM> X("coco-licm", "CoCo Loop Invariant Code Motion");
