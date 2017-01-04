
#include "llvm/Transforms/AccMut/StatisticsUtils.h"

#include "llvm/Pass.h"
#include "llvm/ADT/SmallVector.h"
//#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/MathExtras.h"
#include <algorithm>

map<StringRef, int> ExecInstNums::funcNameID;
int ExecInstNums::curID = 0;


ExecInstNums::ExecInstNums(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
}

#if 0
bool ExecInstNums::runOnFunction(Function & F){
	if( F.getName().equals("main")){	//F.getName().startswith("__accmut__") ||
		return false;
	}

	if( F.getName().startswith("__accmut__exec_inst") ){
		return false;
	}

	Function *f = TheModule->getFunction("__accmut__exec_inst_nums_fname");
	
	if(F.getName().equals("__accmut__filter__variant") || 
	   F.getName().equals("__accmut__divide__eqclass") ||
	   F.getName().equals("__accmut__filter__mutants") ||
	   F.getName().equals("__accmut__fork__eqclass")){

	if(	/*F.getName().startswith("__accmut__process") || 
	   F.getName().startswith("__accmut__apply") ||
	   F.getName().startswith("__accmut__prepare") ||*/
	   F.getName().startswith("__accmut__filter__mutants")){

		f = TheModule->getFunction("__accmut__exec_inst_nums_process");// 
		
	}
	

	if(F.getName().startswith("__accmut__")){
		f = TheModule->getFunction("__accmut__exec_inst_nums_acc");// __accmut__divide__eqclass
	}

	else{
		f = TheModule->getFunction("__accmut__exec_inst_nums");// 
	}
	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		Instruction* inst = &(*I);
		if(inst->getOpcode() == Instruction::PHI){
			continue;
		}
		CallInst* call = CallInst::Create(f, "", inst);
		call->setCallingConv(CallingConv::C);
		call->setTailCall(false);
		AttributeSet call_PAL;
		call->setAttributes(call_PAL);
	}
	return true;
}

#endif

#define OMIT_MAIN 0

bool ExecInstNums::runOnFunction(Function & F){

#if OMIT_MAIN
	if( F.getName().equals("main")){	//F.getName().startswith("__accmut__") ||
		return false;
	}
#endif

	if( F.getName().startswith("__accmut__exec_inst") ){
		return false;
	}

	if( F.getName().equals("__accmut__strlen") || F.getName().equals("__accmut__strcat")
		|| F.getName().equals("__accmut__itoa")){	//F.getName().startswith("__accmut__") ||
			return false;
	}

	StringRef name = F.getName();

	if(funcNameID.find(name) == funcNameID.end()){
		funcNameID[name] = curID++;
		llvm::errs()<<name<<":"<<funcNameID[name]<<"\n";
	}

	ConstantInt* const_key = ConstantInt::get(TheModule->getContext(), 
			APInt(32, funcNameID[name], true));

	
	int name_len = name.size();

	ArrayType* ArrayTy_0 = ArrayType::get(IntegerType::get(TheModule->getContext(), 8), name_len + 1);


	GlobalVariable* gvar_array_str = new GlobalVariable(/*Module=*/*TheModule, 
	/*Type=*/ArrayTy_0,
	/*isConstant=*/false,
	/*Linkage=*/GlobalValue::ExternalLinkage,
	/*Initializer=*/0, // has initializer, specified below
	/*Name=*/"str");
	gvar_array_str->setAlignment(1);

	Constant *const_array_8 = ConstantDataArray::getString(TheModule->getContext(), name, true);

	ConstantInt* const_int32_10 = ConstantInt::get(TheModule->getContext(), APInt(32, StringRef("0"), 10));

	std::vector<Constant*> const_ptr_11_indices;
	const_ptr_11_indices.push_back(const_int32_10);
	const_ptr_11_indices.push_back(const_int32_10);
	Constant* const_ptr_11 = ConstantExpr::getGetElementPtr(nullptr, gvar_array_str, const_ptr_11_indices);
	gvar_array_str->setInitializer(const_array_8);


	Function *f = TheModule->getFunction("__accmut__exec_inst_nums_fname");

	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		Instruction* inst = &(*I);
		if(inst->getOpcode() == Instruction::PHI){
			continue;
		}

		std::vector<Value*> params;
		params.push_back(const_ptr_11);
		params.push_back(const_key);
		
		CallInst* call = CallInst::Create(f, params, "", inst);
//		CallInst* call = CallInst::Create(f, "", inst);
		call->setCallingConv(CallingConv::C);
		call->setTailCall(false);
		AttributeSet call_PAL;
		call->setAttributes(call_PAL);
	}
	return true;
}





/*------------------reserved begin-------------------*/
void ExecInstNums::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char ExecInstNums::ID = 0;
/*-----------------reserved end --------------------*/

