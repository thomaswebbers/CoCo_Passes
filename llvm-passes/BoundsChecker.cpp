/*
 * Bounds checker which adds checks to prevent out of bounds read/writes on arrays
 */

#define DEBUG_TYPE "BoundsChecker"
#include "utils.h"

Function *BoundsCheckFunc; //! watch out, global variable :(, ask why wont work otherwise

namespace {
    class BoundsChecker : public ModulePass {
    public:
        static char ID;
        BoundsChecker() : ModulePass(ID) {}
        virtual bool runOnModule(Module &M) override;

    private:
        bool InsertBoundsCheck(GetElementPtrInst* GEP);
    };
}

///Gets offset compared to the original base pointer
Value* getOffset(GetElementPtrInst *GEP){
    BasicBlock* BB = GEP->getParent();
    Function* parent = BB->getParent();
    Module* M = parent->getParent();

    //get input GEP offset and put into Value*
    Type* offsetType;
    Value* offset;

    //if GEP has an index, set offset to that index. otherwise remain 0
    if(GEP->hasIndices()){
        Value* index = GEP->getOperand(1);
        Type* indexType = index->getType();
        if(indexType->isIntegerTy(32)){
            offset = index;
        }else{
        LOG_LINE("Wrong index Type in getOffset");
        }
    }else{
        offsetType = Type::getInt32Ty(M->getContext());
        offset = ConstantInt::get(offsetType, 0);
    }

    //Start recursive backtracking to get full offset from base pointer
    Value* source = GEP->getOperand(0);

    if(GetElementPtrInst *sGEP = dyn_cast<GetElementPtrInst>(source)){
        Value* sourceOffset = getOffset(sGEP);
        const Twine &Name=Twine();
        offset = BinaryOperator::Create(Instruction::Add, sourceOffset, offset, Name ,  GEP);//!
        LOG_LINE("newOffset: " << *offset);
    }

    return offset;
}

///Return the allocated size of the original base pointer
Value* getMaxSize(GetElementPtrInst *GEP){
    BasicBlock* BB = GEP->getParent();
    Function* parent = BB->getParent();
    Module* M = parent->getParent();


    GetElementPtrInst* target = GEP;

    Value* source = target->getOperand(0);

    //1: trace back until you find a non-GEP Value*
    while(isa<GetElementPtrInst>(source)){
        target = cast<GetElementPtrInst>(source);
        source = target->getOperand(0);
    }

    //2: check if it is a alloca, load, funcArg or constant...+
    //3: determine max array size
    Value* maxSize;

    if(isa<Constant>(source)){
        // return size of constant
        //cast constant to array. Read about arraytype
        //get size of array
        if(dyn_cast<ConstantDataArray>(source)){
            ConstantDataArray* sCDA = cast<ConstantDataArray>(source);
            int constantSize = sCDA->getNumElements();
            Type* sizeType = Type::getInt32Ty(M->getContext());
            maxSize = ConstantInt::get(sizeType, constantSize);
            return maxSize;
        }
        return nullptr;
    }
    if(isa<LoadInst>(source)){
        // return size of loadInst
        return nullptr; //! implement later
    }
    if(isa<Argument>(source)){
        // return size of Function Argument

        // argv exception
        StringRef name = source->getName();
        const char* namePointer = name.data();
        if(strncmp(namePointer, "argv", 5)){
            StringRef parentName = parent->getName();
            const char* parentNamePointer = parentName.data();
            if(strncmp(parentNamePointer, "main", 5)){
                return &cast<Value>(*parent->arg_begin());
            }
        }
        return nullptr; //! implement later
    }
    if(isa<AllocaInst>(source)){
        // return size of alloca instruction
        AllocaInst* sAlloca = dyn_cast<AllocaInst>(source);
        maxSize = sAlloca->getArraySize();
        return maxSize;
    }
    return nullptr;
}

bool InsertBoundsCheck(GetElementPtrInst* GEP){
    bool Changed = false;

    BasicBlock* BB = GEP->getParent();

    IRBuilder<> IRbuild(BB);

    LOG_LINE("GEP: " << *GEP);

    Value* offset = getOffset(GEP);
    Value* maxSize = getMaxSize(GEP);

    if (maxSize == nullptr){
        LOG_LINE("pointer found with unknown origin. Bad things have happened \n");
        return Changed;
    }
    if (offset == nullptr){
        LOG_LINE("offset is nullptr. Something even worse has happened \n");
        return Changed;
    }

    LOG_LINE("offsetFinal: " << *offset);
    LOG_LINE("maxSizeFound: " << *maxSize << "\n");

    IRbuild.SetInsertPoint(GEP);
    IRbuild.CreateCall(BoundsCheckFunc, {offset, maxSize});
    Changed = true;

    return Changed;
}

///Main run on module function
bool BoundsChecker::runOnModule(Module &M) {
    bool Changed = false;

    LLVMContext &C = M.getContext();
    Type *VoidTy = Type::getVoidTy(C);
    Type *Int32Ty = Type::getInt32Ty(C);
    BoundsCheckFunc = cast<Function>(M.getOrInsertFunction("__coco_check_bounds", VoidTy, Int32Ty, Int32Ty));

    for (Function &F : M) {
        for (Instruction &II : instructions(F)) {
            Instruction *I = &II;

            if (GetElementPtrInst *GEP = dyn_cast<GetElementPtrInst>(I)) {
                Changed |= InsertBoundsCheck(GEP);
            }
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
