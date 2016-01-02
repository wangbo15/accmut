//===----------------------------------------------------------------------===//
//
// This file decribes the sattic mutation analysis IR instrumenter pass
// 
// Add by Wang Bo. DEC 23, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/SMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutUtil.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include<fstream>
#include<sstream>

#include "clang/AST/Stmt.h"

using namespace llvm;
using namespace std;

using clang::Stmt;

extern std::map<Value*, Stmt*> toStmtMap;

void dumpToStmtMap(Function &F);

SMAInstrumenter::SMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	MutUtil::getAllMutations();
}

bool SMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){/*
		Function* finit = TheModule->getFunction("__accmut__sma_init");
		if (!finit) {
			std::vector<Type*>Fty_args;
				FunctionType* Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()),
													Fty_args,true);
			finit = Function::Create(
			 	Fty,	//Type
				 GlobalValue::ExternalLinkage,	//Linkage
				 "__accmut__sma_init",	//Name
				 TheModule); 
			finit->setCallingConv(CallingConv::C);
		}

		CallInst* call_init = CallInst::Create(finit, "", F.getEntryBlock().begin());
		call_init->setCallingConv(CallingConv::C);
		call_init->setTailCall(false);
		AttributeSet call_init_PAL;
		call_init->setAttributes(call_init_PAL);*/
		return true;
	}
	// TODO:

	vector<Mutation*>* v= MutUtil::AllMutsMap[F.getName()];
	
	if(v != NULL && v->size() != 0)
		filtMutsByIndex(F, v);
	

	dumpToStmtMap(F);

	return true;
}

void SMAInstrumenter::filtMutsByIndex(Function &F, vector<Mutation*>* v){
	errs()<<"===  SMA INSTRUMENTING "<<F.getName()<<"  =====\n";
	std::vector<Mutation*>::iterator cur_mut = v->begin();
	std::vector<Mutation*>::iterator beg = cur_mut;

	int instrumented_insts = 0;
	while(cur_mut != v->end()){//find a mutations' interval of the vector
		if((*cur_mut)->index != (*beg)->index){
			int insts = instrument(F, (*beg)->index, (*beg)->id, (*(cur_mut-1))->id, instrumented_insts);
			instrumented_insts+=insts;
			beg = cur_mut;
			continue;
		}
		if(cur_mut == (v->end() -1)){
			int insts = instrument(F, (*beg)->index, (*beg)->id, (*cur_mut)->id, instrumented_insts);
			instrumented_insts+=insts;			
		}
		cur_mut++;
	}	
}

#if ACCMUT_STATIC_ANALYSIS_INSTRUMENT_EVAL
int SMAInstrumenter::instrument(Function &F, int index, int mut_from, int mut_to, int instrumented_insts){
	BasicBlock::iterator cur_it = MutUtil::getLocation(F, instrumented_insts, index);
	errs()<<" 	instrumenting this IR >>>> ";
	cur_it->dump();
	
	
}

#elif ACCMUT_STATIC_ANALYSIS_INSTRUEMENT_FORK

#endif

void dumpToStmtMap(Function &F){

	errs()<<"\n\n  ############  FUNCTION : "<<F.getName()<<"  ########\n\n\n";
	
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI){
			errs()<<'\n';
			errs()<<*(BI)<<"  ----> \n\n";
			toStmtMap[&(*BI)]->dump();
			errs()<<"\n\n";
		}
	}
}


/*------------------reserved begin-------------------*/
void SMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char SMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/

