//===----------------------------------------------------------------------===//
//
// This file decribes the static mutation schemata IR instrumenter pass
// 
// Add by Wang Bo. DEC 27, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/MSInstrumenter.h"
#include "llvm/Transforms/AccMut/MutUtil.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include<fstream>
#include<sstream>


using namespace llvm;
using namespace std;

MSInstrumenter::MSInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	MutUtil::getAllMutations();
}

bool MSInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		Function* finit = TheModule->getFunction("__accmut__ms_init");
		if (!finit) {
			std::vector<Type*>Fty_args;
				FunctionType* Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()),
													Fty_args,true);
			finit = Function::Create(
			 	Fty,	//Type
				 GlobalValue::ExternalLinkage,	//Linkage
				 "__accmut__ms_init",	//Name
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
	// TODO:
	vector<Mutation*>* v= MutUtil::AllMutsMap[F.getName()];
	if(v != NULL && v->size() != 0)
		filtMutsByIndex(F, v);
	
	return true;
}


void MSInstrumenter::filtMutsByIndex(Function &F, vector<Mutation*>* v){
	errs()<<"===  MS INSTRUMENTING "<<F.getName()<<"  =====\n";
	std::vector<Mutation*>::iterator cur_mut = v->begin();
	std::vector<Mutation*>::iterator beg = cur_mut;
	int instrumented_insts = 0;
	while(cur_mut != v->end()){
		if((*cur_mut)->index != (*beg)->index){
//			errs()<<"IR "<<(*beg)->index<<" : mut "<<(*beg)->id<<" ~~ mut "<<(*(cur_mut-1))->id<<"\n";
//						errs()<<" II : "<<instrumented_insts<<"\n";

			int insts = instrument(F, v, (*beg)->index, (*beg)->id, (*(cur_mut-1))->id, instrumented_insts);
			instrumented_insts+=insts;

//			errs()<<" III : "<<instrumented_insts<<"\n";

			beg = cur_mut;
			continue;
		}
		if(cur_mut == (v->end() -1)){
//			errs()<<(*beg)->index<<" : "<<(*beg)->id<<"~"<<(*cur_mut)->id<<"\n";
//						errs()<<" II : "<<instrumented_insts<<"\n";

			int insts = instrument(F, v, (*beg)->index, (*beg)->id, (*cur_mut)->id, instrumented_insts);
			instrumented_insts+=insts;
//			errs()<<" III : "<<instrumented_insts<<"\n";
		}
		cur_mut++;
	}

//	cur_it = getLocation();

}


int MSInstrumenter::instrument(Function &F, vector<Mutation *>* v, int index, int mut_from, int mut_to, int instrumented_insts){
	int insts = 0;
	
	BasicBlock::iterator cur_it = MutUtil::getLocation(F, instrumented_insts, index);

	errs()<<">>INSTRUMENTING MUTS FROM "<<mut_from<<" TO "<<mut_to<<" IN IR "<<index<<" >> ";
	cur_it->dump();

	Function::iterator cur_bb = cur_it->getParent();

	stringstream name;
	name<<"FINAL_BB_OF_INST_"<<index;

	BasicBlock *FINAL_END = cur_bb->splitBasicBlock(cur_it, name.str());

	for(int i = mut_from; i<=mut_to; i++){
		Mutation *cur_m = (*v)[i];
	}
	
	
}


/*------------------reserved begin-------------------*/
void MSInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char MSInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/


