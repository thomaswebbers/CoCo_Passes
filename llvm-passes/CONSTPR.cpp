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
    }else if(isShift(Opcode)->isArithmeticShift()){
        result = lval << rval;
    }

    /*else if (Opcode == Instruction::Ashr){
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
    }else if(Opcode == Instruction::AShr){
        result = lval << rval;
    }


    /*else if (Opcode == Instruction::Ashr){
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
            ConstantInt *resultOfComputation;
            for (Instruction &II : *BB) {
    //          check if instruction is binary operator
                if(!(isa<binaryOperator>(I))){
                    continue;
                }
    //         for(each OP in I)
               Value* op1 = I->getOperand(0);
               Value* op2 = I->getOperand(1);

                //TODO add possible non double constant edge cases
               if(!(ConstantInt* constantInteger = dyn_cast<ConstantInt>(op1)) || !(ConstantInt* constantInteger = dyn_cast<ConstantInt>(op2))){
                    //skip this instruction and take the next since the values are not both constants
                    continue;
                }

                resultOfComputation = foldConstant(cInt);
            }

            //adds all users of the I to the workList
            for (User *U : I->users()){
               WorkList.insert(cast<Instruction>(U));
            } 

    //      Replace all of the uses of a variable with uses of the constant.
            I->replaceAllUsesWith(resultOfComputation);
        }

    //  Constant was propegated
        Changed = true;
    }
    return changed;
}
/*



     std::set<Constant> constantSet; //! not using SmallSet, because not certain if can do .begin
    //TODO Map of values(computated numbers) and keys(variable names)

     /*
    for (BasicBlock &BB : F) {
        for (Instruction &II : *BB) {
            Instruction *I = &II;
            if (CallInst *CI = dyn_cast<CallInst>(I)) {
                LOG_LINE(" Found call: " << *CI);
    


    //bool changed

    //while(changed)                          // has users (keep looping till nothing is changed)
    while(changed){
    //  changed == false;
    //  for (each BB in F)
        for (BasicBlock &BB : F) {
    //      for(each I in BB)
            for (Instruction &II : *BB) {
    //          for(each OP in I)
                for(Use &U : &II->operands()){
                    Value *v = U.get();
    //              for(each CONST in OP)  //checks if OP is CONST by casting to ConstantInt
                    if(!(ConstantInt* constantInteger = dyn_cast<ConstantInt>(v)){
                        break;
                    }
                    ConstantInt cInt = dyn_cast<ConstantInt>(v);

                }
    //          for(each OP in I)
                I.getOperandList()           //Inherited user so should have this call

    //              for(each CONST in OP) // TODO: Check whether an operand is a constant by casting it to a ConstantInt.
    //                  add CONST to constantSet
    //  end loops
            }
        }

    //  for (each CONST in ConstantSet)
    //      calculate value of constant in order it was added
    //      For every type of computation: lhs.getZExtValue() op rhs.getZEXxtValue()
    //      addition,
    //      subtraction,
    //      multiplica- tion,
    //      arithmetic shift
    //      changed == true
    //      Insert in map
    //
    //  go through code and replace if find variable as key in map replace with value
    //
    }



    //bool changed
    //while(changed)                          // has users (keep looping till nothing is changed)
    //  changed == false;
    //  for (each BB in F)
    //      for(each I in BB)
    //          for(each OP in I)
    //              for(each CONST in OP) // TODO: Check whether an operand is a constant by casting it to a ConstantInt.
    //                  add CONST to constantSet
    //  end loops


    //  for (each CONST in ConstantSet)
    //      calculate value of constant in order it was added
    //      For every type of computation: lhs.getZExtValue() op rhs.getZEXxtValue()
    //      addition,
    //      subtraction,
    //      multiplica- tion,
    //      arithmetic shift
    //      changed == true
    //      Insert in map
    //
    //  go through code and replace if find variable as key in map replace with value
    //

/*
        //for (each BB in F)
        for (BasicBlock &BB : F) {
        //  for(each I in BB)
            for (Instruction &II : BB) {
                Instruction *I = &II;
        //      for(each OP in I)
                if (CallInst *CI = dyn_cast<ConstantInt>(I)) {
        //TODO add to set

                }
            }
        }

    //for (each CONST in ConstantSet)
    //  calculate value of constant in order it was added
    //  insert in map
    //

    //go through code and replace if find variable as key in map replace with value

    return true;  // We did alter the IR
}
*/

char CONSTPR::ID = 0;
static RegisterPass<CONSTPR> X("coco-constprop", "Propagate constants over the code");


