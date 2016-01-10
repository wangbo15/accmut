//===----------------------------------------------------------------------===//
//
// This file decribes the dynamic mutation analysis IR instrumenter pass
// 
// Add by Wang Bo. OCT 21, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/DMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutUtil.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include<fstream>
#include<sstream>


using namespace llvm;
using namespace std;

DMAInstrumenter::DMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	//getAllMutations(); 
	MutUtil::getAllMutations();
}

bool DMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		Function* finit = TheModule->getFunction("__accmut__dma_init");
		if (!finit) {
			std::vector<Type*>Fty_args;
 			FunctionType* Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()),
													Fty_args,true);
			finit = Function::Create(
			 	Fty,	//Type
				 GlobalValue::ExternalLinkage,	//Linkage
				 "__accmut__init",	//Name
				 TheModule); 
			finit->setCallingConv(CallingConv::C);
		}

		CallInst* call_init = CallInst::Create(finit, "", F.getEntryBlock().begin());
		call_init->setCallingConv(CallingConv::C);
		call_init->setTailCall(false);
		AttributeSet call_init_PAL;
		call_init->setAttributes(call_init_PAL);
		
		return true;
	}
	
	vector<Mutation*>* v= MutUtil::AllMutsMap[F.getName()];
	
	if(v != NULL && v->size() != 0)
		filtMutsByIndex(F, v);
	
	return true;
}

void DMAInstrumenter::filtMutsByIndex(Function &F, vector<Mutation*>* v){
	errs()<<"===  DMA INSTRUMENTING "<<F.getName()<<"  =====\n";
		
	std::vector<Mutation*>::iterator cur_mut = v->begin();
	std::vector<Mutation*>::iterator beg = cur_mut;

	int instrumented_insts = 0;

	while(cur_mut != v->end()){//find a mutations' interval of the vector
		if((*cur_mut)->index != (*beg)->index){
			//errs()<<"IR "<<(*beg)->index<<" : mut "<<(*beg)->id<<" ~~ mut "<<(*(cur_mut-1))->id<<"\n";
			//			errs()<<" II : "<<instrumented_insts<<"\n";

			int insts = instrument(F, (*beg)->index, (*beg)->id, (*(cur_mut-1))->id, instrumented_insts);
			instrumented_insts+=insts;

			//errs()<<" III : "<<instrumented_insts<<"\n";

			beg = cur_mut;
			continue;
		}
		if(cur_mut == (v->end() -1)){
			//errs()<<(*beg)->index<<" : "<<(*beg)->id<<"~"<<(*cur_mut)->id<<"\n";
			//			errs()<<" II : "<<instrumented_insts<<"\n";

			int insts = instrument(F, (*beg)->index, (*beg)->id, (*cur_mut)->id, instrumented_insts);
			instrumented_insts+=insts;
			
			//errs()<<" III : "<<instrumented_insts<<"\n";
		}
		cur_mut++;
	}

//	cur_it = getLocation();

}

int DMAInstrumenter::instrument(Function &F, int index, int mut_from, int mut_to, int instrumented_insts){
	int insts = 0;
	
	BasicBlock::iterator cur_it = MutUtil::getLocation(F, instrumented_insts, index);
	//Function::iterator cur_bb = cur_it->getParent();

	errs()<<" 	instrumenting this IR >>>> ";
	cur_it->dump();

	if(cur_it->getOpcode() >= 14 && cur_it->getOpcode() <= 31){ // FOR ARITH INST
		Type* ori_ty = cur_it->getType();
		Function* f_process;
		if(ori_ty->isIntegerTy(32)){
			f_process = TheModule->getFunction("__accmut__process_i32_arith");// TODO:: int or unsigned ??!!
		}else if(ori_ty->isIntegerTy(64)){
			f_process = TheModule->getFunction("__accmut__process_i64_arith");
		}else{
			llvm::errs()<<"TYPE ERROR @ instrument() ArithInst\n";
			return insts;
		}

		std::vector<Value*> int_call_params;
		std::stringstream ss;
		ss<<mut_from;
		ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10)); 
		int_call_params.push_back(from_i32);
		ss.str("");
		ss<<mut_to;
		ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10));
		int_call_params.push_back(to_i32);
		int_call_params.push_back(cur_it->getOperand(0));
		int_call_params.push_back(cur_it->getOperand(1));
		CallInst *call = CallInst::Create(f_process, int_call_params);
		ReplaceInstWithInst(cur_it, call);
			
	}
	else if(cur_it->getOpcode() == 52){// FOR ICMP INST
		Function* f_process;

		if(cur_it->getOperand(0)->getType()->isIntegerTy(32)){
			f_process = TheModule->getFunction("__accmut__process_i32_cmp");
		}else if(cur_it->getOperand(0)->getType()->isIntegerTy(64)){
			f_process = TheModule->getFunction("__accmut__process_i64_cmp");
		}else{
			llvm::errs()<<"TYPE ERROR @ instrument() ICmpInst\n";			
			return insts;
		}
		
		std::vector<Value*> int_call_params;
		std::stringstream ss;
		ss<<mut_from;
		ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
				APInt(32, StringRef(ss.str()), 10)); 
		int_call_params.push_back(from_i32);
		ss.str("");
		ss<<mut_to;
		ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
			APInt(32, StringRef(ss.str()), 10));
		int_call_params.push_back(to_i32);
		int_call_params.push_back(cur_it->getOperand(0));
		int_call_params.push_back(cur_it->getOperand(1));
		CallInst *call = CallInst::Create(f_process, int_call_params, "", cur_it);
		CastInst* i32_conv = new TruncInst(call, IntegerType::get(TheModule->getContext(), 1), "");
		insts++;
		
		ReplaceInstWithInst(cur_it, i32_conv);
	}
	else if(cur_it->getOpcode() == 34){// FOR STORE INST
		Function *f_process_st;
		if(cur_it->getOperand(0)->getType()->isIntegerTy(32)){
			f_process_st = TheModule->getFunction("__accmut__process_st_i32");
		}else if(cur_it->getOperand(0)->getType()->isIntegerTy(64)){
			f_process_st = TheModule->getFunction("__accmut__process_st_i64");			
		}else{
			llvm::errs()<<"TYPE ERROR @ instrument() StoreInst\n";
			return insts;
		}		
		std::vector<Value*> params;
		std::stringstream ss;
		ss<<mut_from;
		ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10)); 
		params.push_back(from_i32);
		ss.str("");
		ss<<mut_to;
		ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10));
		params.push_back(to_i32);	
		params.push_back(cur_it->getOperand(1));	
		CallInst *call = CallInst::Create(f_process_st, params);		
		ReplaceInstWithInst(cur_it, call);
	}
/*	else if(cur_it->getOpcode() == 55){// FOR CALL INST
		Function *f;
		Type* ori_ty = cur_it->getType();
		if(ori_ty->isIntegerTy(32)){
			f = TheModule->getFunction("__accmut__process_call_i32");	
		}else if(ori_ty->isVoidTy()){
			f = TheModule->getFunction("__accmut__process_call_void");	
		}else if(ori_ty->isIntegerTy(64)){
			f = TheModule->getFunction("__accmut__process_call_i64");
		}else{
			llvm::errs()<<"TYPE ERROR @ instrument() CallInst\n";			
			return insts;
		}	
		CallInst* call = CallInst::Create(f, "");
		call->setCallingConv(CallingConv::C);
		call->setTailCall(false);
		AttributeSet attr;
		call->setAttributes(attr);		
		ReplaceInstWithInst(cur_it, call);
		
	}*/
	
	return insts;
}

/*
BasicBlock::iterator DMAInstrumenter::getLocation(Function &F, int instrumented_insts, int index){
	int cur = 0;
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI, cur++){
			if(index + instrumented_insts == cur ){
				return BI;
			}
		}
	}
	return F.back().end();	
}
*/

/*------------------reserved begin-------------------*/
void DMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char DMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/
