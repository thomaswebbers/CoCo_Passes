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


ConstantInt* foldConstant(BinaryOperator* I){
    int64_t result;

    Type* Ty = I->getType();
    ConstantInt* lop = dyn_cast<ConstantInt>(I->getOperand(0));
    ConstantInt* rop = dyn_cast<ConstantInt>(I->getOperand(1));
    unsigned Opcode = I->getOpcode();

    int64_t lval = lop->getSExtValue();
    int64_t rval = rop->getSExtValue();

    if (Opcode == Instruction::Add){
        result = lval + rval;
    }else if (Opcode == Instruction::Sub){
        result = lval - rval;
    }else if (Opcode == Instruction::Mul){
        result = lval * rval;
    }else if(Opcode == Instruction::AShr){
        result = lval >> rval;
    }else if(Opcode == Instruction::Shl){
        result = lval << rval;
    }

    if(result){
        long print = result;
        LOG_LINE(print);
        return cast<ConstantInt>(ConstantInt::get(Ty, result));
    }
    return nullptr;
}

Value* checkEdgeCases(BinaryOperator* I){
    ConstantInt* lop = dyn_cast<ConstantInt>(I->getOperand(0));
    ConstantInt* rop = dyn_cast<ConstantInt>(I->getOperand(1));
    unsigned Opcode = I->getOpcode();

    if (Opcode == Instruction::Add){
        if(lop != nullptr && lop->isZero()){
            return I->getOperand(1);            // 0 + x = x
        }
        if(rop != nullptr && rop->isZero()){
            return I->getOperand(0);            // x + 0 = x
        }

    }else if (Opcode == Instruction::Sub){
        if(rop != nullptr && rop->isZero()){
            return I->getOperand(0);            // x - 0 = x
        }

    }else if (Opcode == Instruction::Mul){
        if(lop != nullptr){
            if(lop->isZero()){
                return I->getOperand(0);               // 0 * x = 0
            }
            if(lop->isOne()){
                return I->getOperand(1);               // 1 * x = x
            }
        }
        else if(rop != nullptr){
            if(rop->isZero()){
                return I->getOperand(1);               // x * 0 = 0
            }
            if(rop->isOne()){
                return I->getOperand(0);               // x * 1 = x
            }
        }

    }else if(Opcode == Instruction::AShr){
        if(lop != nullptr && lop->isZero()){
            return I->getOperand(0);                   // 0 >> x = 0
        }
        if (rop != nullptr && rop->isZero()){
            return I->getOperand(0);                   // x >> 0 = x
        }

    }else if(Opcode == Instruction::Shl){
        if(lop != nullptr && lop->isZero()){
            return I->getOperand(0);                   // 0 << x = 0
        }
        if (rop != nullptr && rop->isZero()){
            return I->getOperand(0);                   // x << 0 = x
        }
    }
    return nullptr;
}



bool CONSTPR::runOnFunction(Function &F) {
    // Create a worklist of instructions
    std::set<Instruction*> WorkList;
    for (Instruction &I: instructions(&F))
        WorkList.insert(&I);

    //set boolean for checking if constants were propageted if no exit
    bool changed = false;

    //work through worklist
    while(!WorkList.empty()){

    //  fetches first instruction from worklist
        Instruction *I = *WorkList.begin();
        WorkList.erase(WorkList.begin());    // Get an element from the worklist...

        //prevents using dead instructions
        if(!I->use_empty()){
            Value* resultOfComputation;
    //          check if instruction is binary operator
            if(!(isa<BinaryOperator>(I))){
                    continue;
            }
            BinaryOperator* bI = cast<BinaryOperator>(I);

    //      for(each OP in I)
            Value* op1 = I->getOperand(0);
            Value* op2 = I->getOperand(1);

            if(!isa<ConstantInt>(op1) || !isa<ConstantInt>(op2)){
                //skip this instruction and take the next since the values are not both constants
                if(!isa<ConstantInt>(op1) && !isa<ConstantInt>(op2)){
                    continue;
                }
                resultOfComputation = checkEdgeCases(bI);
                if(resultOfComputation == nullptr){
                    continue;
                }
            }else{
                resultOfComputation = foldConstant(bI);
            }

            //adds all users of the I to the workList
            for (User *U : I->users()){
               WorkList.insert(cast<Instruction>(U));
            }

    //      Replace all of the uses of a variable with uses of the constant.
            I->replaceAllUsesWith(resultOfComputation);
        }

    //  Constant was propegated
        changed = true;
    }
    return changed;
}

char CONSTPR::ID = 0;
static RegisterPass<CONSTPR> X("coco-constprop", "Propagate constants over the code");


