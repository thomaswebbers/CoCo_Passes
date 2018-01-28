/*
 * Dummy module pass which can be used as example and starting point for
 * Assignment 3. This pass inserts calls to a helper function for every stack
 * allocation. The helper function prints the size of the stack allocation and
 * serves as an example how to create and insert helpers. The implementation of
 * the helper function can be found in runtime/dummy.c.
 */

 //!modify the above

#define DEBUG_TYPE "BoundsChecker"
#include "utils.h"

namespace {
    class BoundsChecker : public ModulePass {
    public:
        static char ID;
        BoundsChecker() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M) override;

    private:
        Function *PrintAllocFunc;

        bool instrumentAllocations(Function &F);
    };
}

/*
 * Finds all allocations in a function and inserts a call that prints its size.
 */
 //!modify the above
bool BoundsChecker::instrumentAllocations(Function &F) {
    bool Changed = false;

    return Changed;
}

bool BoundsChecker::runOnModule(Module &M) {
    bool Changed = false;

    //make map of pointer + offset
    //iterate through GEP instructions
    //check if base pointer in GEP is inside map
    //
    //  if yes: get offset & add new offset from GEP
    //  remove old pointer and add new pointer with new offset !because of SSA
    //
    //  if no: add new pointer to map with offset of GEP
    //! this all needs to be added into a function boundschecker that returns a Value*?
    //! no...maybe the entire module BoundsChecker (this file) needs to return a Value*

    return Changed;
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-dummymodulepass, the second argument is a description shown in the help
// text about this pass.
//!modify the above
char BoundsChecker::ID = 0;
static RegisterPass<BoundsChecker> X("coco-boundscheck", "");
