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

<<<<<<< HEAD
    for (BasicBlock &BB : F) {
        for (Instruction &II : BB) {
            Instruction *I = &II;
            if (CallInst *CI = dyn_cast<CallInst>(I)) {
                LOG_LINE(" Found call: " << *CI);
=======
    std::set<Constant> constantSet; //! not using SmallSet, because not certain if can do .begin
    //TODO Map of values(computated numbers) and keys(variable names)

    //while used                          // has users (keep looping till nothing is changed)
        //for (each BB in F)
        //  for(each I in BB)
        //       for(each OP in I)
        //          for(each CONST in OP) // TODO: Check whether an operand is a constant by casting it to a ConstantInt.
        //              add CONST to constantSet
        //end loops

        //for (each CONST in ConstantSet)
        //  calculate value of constant in order it was added
        //  For every type of computation: lhs.getZExtValue() op rhs.getZEXxtValue()
        //  addition, 
        //  subtraction, 
        //  multiplica- tion, 
        //  arithmetic shift
        //  Insert in map 
        //

        //go through code and replace if find variable as key in map replace with value
    //






    //for (each BB in F)
    for (BasicBlock &BB : F) {
    //  for(each I in BB)  
        for (Instruction &II : BB) {
            Instruction *I = &II;
    //      for(each OP in I)        
            if (CallInst *CI = dyn_cast<ConstantInt>(I)) {
    //TODO add to set          
>>>>>>> 549dbadcee7b4051e60cd18fe15114380d758148
            }
        }
    }

<<<<<<< HEAD
=======
    //for (each CONST in ConstantSet)
    //  calculate value of constant in order it was added
    //  insert in map 
    //

    //go through code and replace if find variable as key in map replace with value

>>>>>>> 549dbadcee7b4051e60cd18fe15114380d758148
    return false;  // We did not alter the IR
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-dummypass, the second argument is a description shown in the help text
// about this pass.
char CONSTPR::ID = 0;
static RegisterPass<CONSTPR> X("coco-dummypass", "Example LLVM pass printing each function it visits, and every call instruction it finds");
