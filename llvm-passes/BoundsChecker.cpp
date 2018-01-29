/*
 * Bounds checker which adds checks to prevent out of bounds read/writes on arrays
 */

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


bool BoundsChecker::instrumentAllocations(Function &F) {
    bool Changed = false;

    return Changed;
}

///Gets offset compared to the original base pointer
Value* getOffset(GetElementPtrInst *GEP){
    //get input GEP offset and put into Value*
    Type* offsetType = Type::getInt32Ty(M.getContext()); //! this does not feel right
    Value* offset = ConstantInt::get(offsetType, 0);

    //if GEP has an index, set offset to that index. otherwise remain 0
    if(GEP->hasIndices()){
        Value* index = GEP->getOperand(1);
        Type* indexType = index->getType();
        if(indexType->isIntegerTy(32)){
            offset = index;
        }
    }

    //Start recursive backtracking to get full offset from base pointer
    Value* source = GEP->getOperand(0);

    if(GetElementPtrInst *sGEP = dyn_cast<GetElementPtrInst>(source)){
        Value* sourceOffset = getOffset(sGEP);
        offset = BinaryOperator::Create(Instruction::Add, sourceOffset, offset); //! double check this
    }

    return offset;
}

///Return the allocated size of the original base pointer
Value* getMaxSize(GetElementPtrInst *GEP){
    Value* target = GEP;
    Value* source = target->getOperand(0);

    //1: trace back until you find a non-GEP Value*
    while(isa<GetElementPtrInst>(source)){
        target = source;
        source = source->getOperand(0);
    }

    //2: check if it is a alloca, load, funcArg or constant...+
    //3: determine max array size
    Value* maxSize;

    if(isa<Constant>(source)){
        // return size of constant
        return source; //! correct?
    }
    if(isa<LoadInst>(source)){
        // return size of loadInst
        return nullptr; //! implement later
    }
    if(isa<Argument>(source)){
        // return size of Function Argument
        char* name = source.getName();
        if(strncmp(name, "argv", 5)){
            return argc; //! figure this out. How to get argc with only name
        }
        return nullptr; //! implement later
    }
    if(isa<AllocaInst>(source)){
        // return size of alloca instruction
        maxSize = source.getOperand(1); //! correct operand?, need to check type?
        return maxSize;
    }
    //! RAISE ERROR
    return nullptr; //! will we ever reach this?
}

///Main run on module function
bool BoundsChecker::runOnModule(Module &M) {
    bool Changed = false;

    for (Instruction &II : instructions(F)) {
        Instruction *I = &II;

        if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(I)) {
            Value* offset = getOffset(GEP);
            Value* maxSize = getMaxSize(GEP);

        }
    }

    return Changed;
}

// Register the pass with LLVM so we can invoke it with opt. The first argument
// to RegisterPass is the commandline switch to run this pass (e.g., opt
// -coco-dummymodulepass, the second argument is a description shown in the help
// text about this pass.
char BoundsChecker::ID = 0;
static RegisterPass<BoundsChecker> X("coco-boundscheck", "add checks before GEP instructions to prevent out of bound reads/writes on arrays");
