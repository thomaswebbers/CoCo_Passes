/*
 * Dummy (and minimal) function pass to serve as a starting point for your
 * Assignment 2 passes. It simply visits every function and prints every call
 * instruction it finds.
 */

#define DEBUG_TYPE "DummyPass"
#include "utils.h"

namespace {
    class DummyPass : public FunctionPass {
    public:
        static char ID;
        DummyPass() : FunctionPass(ID) {}
        virtual bool runOnFunction(Function &F) override;
    };
}

ConstantInt* foldConstant(BinaryOperator* I){
    uint64_t result;

    Type* Ty = I->getType();
    ConstantInt* lop = dyn_cast<ConstantInt>(I->getOperand(0));
    ConstantInt* rop = dyn_cast<ConstantInt>(I->getOperand(1));
    unsigned Opcode = I->getOpcode();

    uint64_t lval = lop->getZExtValue();
    uint64_t rval = rop->getZExtValue();

    if (Opcode == Instruction::Add){
        result = lval + rval;
    }else if (Opcode == Instruction::Sub){
        result = lval - rval;
    }else if (Opcode == Instruction::Mul){
        result = lval * rval;
    }/*else if (Opcode == Instruction::Ashr){
        result = lval << rval; //! not certain about this
    }else if (Opcode == Instruction::Shl){
        result = lval >> rval; //! >> ? <<
    }*/
    if(result){
        LOG_LINE(result);
        return cast<ConstantInt>(ConstantInt::get(Ty, result));
    }
    return nullptr;
}

bool DummyPass::runOnFunction(Function &F) {
    LOG_LINE("Visiting function " << F.getName());

    for (BasicBlock &BB : F) {
        for (Instruction &II : BB) {
            Instruction *I = &II;
            if (BinaryOperator *CI = dyn_cast<BinaryOperator>(I)) {
                LOG_LINE(" Found op: " << *CI << " " << foldConstant(CI));
            }
        }
    }

    return false;  // We did not alter the IR
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-dummypass, the second argument is a description shown in the help text
// about this pass.
char DummyPass::ID = 0;
static RegisterPass<DummyPass> X("coco-dummypass", "Example LLVM pass printing each function it visits, and every call instruction it finds");
