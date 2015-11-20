//===----------------------------------------------------------------------===//
//
 // This file decribes the dynamic mutation analysis IR instrumenter pass
// 
 // Add by Wang Bo. OCT 21, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/DMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutationGen.h"

//#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"


#include<fstream>
#include<sstream>


using namespace llvm;
using namespace std;

//vector<Mutation*> DMAInstrumenter::AllMutsVector;

map<string, vector<Mutation*>*> DMAInstrumenter::AllMutsMap;

DMAInstrumenter::DMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	getAllMutations(); 
}

bool DMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		// TODO: intrument main

		Function* finit = TheModule->getFunction("__accmut__init");
		if (!finit) {

			std::vector<Type*>Fty_args;
 			FunctionType* Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()),
													Fty_args,true);
			finit = Function::Create(
			 /*Type=*/Fty,
			 /*Linkage=*/GlobalValue::ExternalLinkage,
			 /*Name=*/"__accmut__init", TheModule); 
			finit->setCallingConv(CallingConv::C);
		}

		//FunctionType *Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()), NULL, false);
                                          
		//Constant* c = TheModule->getOrInsertFunction("__accmut__init",Fty);
		//Function* finit =  cast<Function>(c);
		
		CallInst* call_init = CallInst::Create(finit, "", F.getEntryBlock().begin());
		call_init->setCallingConv(CallingConv::C);
		call_init->setTailCall(false);
		AttributeSet call_init_PAL;
		call_init->setAttributes(call_init_PAL);


		
		return false;
	}
	
	vector<Mutation*>* v= AllMutsMap[F.getName()];

	instrument(F, v);
	
	return true;
}

 void DMAInstrumenter::instrument(Function &F, vector<Mutation*>* v){
	
 }

void DMAInstrumenter::getAllMutations(){
	string buf;
	string path = getenv("HOME");
	path += "/tmp/accmut/mutations.txt";
	
	std::ifstream  fin(path, ios::in); 
	
	if(!fin.is_open()){
		errs()<<"FILE ERROR : mutations.txt @ "<<path<<"\n";
		exit(-1);
	}
	
	while( fin>>buf) { 
		Mutation *m = getMutation(buf);
		//AllMutsVector.push_back(m);
		if(AllMutsMap.count(m->func) == 0){
			AllMutsMap[m->func] = new vector<Mutation*>();
		}
		AllMutsMap[m->func]->push_back(m);
	}
	fin.close();
}

Mutation *DMAInstrumenter::getMutation(string line){
	stringstream ss;
	ss<<line;
	int id;
	string mtype, func;
	int index;
	ss>>id;
	char colon;
	ss>>colon;
	getline(ss, mtype, ':');
	getline(ss, func, ':');
	ss>>index;
	ss>>colon;
	
	Mutation *m;
	if(mtype == "AOR"){
		AORMut *aor = new AORMut();
		int sop, top;
		ss>>sop;
		ss>>colon;
		ss>>top;
		aor->src_op = sop;
		aor->tar_op = top;
		m = dyn_cast<Mutation>(aor);
	}else if(mtype == "LOR"){
		LORMut *lor = new LORMut();
		int sop, top;
		ss>>sop;
		ss>>colon;
		ss>>top;
		lor->src_op = sop;
		lor->tar_op = top;
		m = dyn_cast<Mutation>(lor);
	}else if(mtype == "COR"){

	}else if(mtype == "ROR"){
		RORMut *ror = new RORMut();
		int op, sp, tp;
		ss>>op;
		ss>>colon;
		ss>>sp;
		ss>>colon;
		ss>>tp;
		ror->op = op;
		ror->src_pre = sp;
		ror->tar_pre = tp;
		m = dyn_cast<Mutation>(ror);
	}else if(mtype == "SOR"){
		
	}else if(mtype == "STD"){

	}else if(mtype == "LVR"){
		LVRMut *lvr = new LVRMut();
		int oi, sc, tc;
		ss>>oi;
		ss>>colon;
		ss>>sc;
		ss>>colon;
		ss>>tc;
		lvr->oper_index = oi;
		lvr->src_const = sc;
		lvr->tar_const = tc;
		m = dyn_cast<Mutation>(lvr);
	}else{
		errs()<<"WRONG MUT TYPE !\n";
		exit(-1);
	}
	m->id = id;
	m->type = mtype;
	m->func = func;
	m->index = index;
	return m;
}

/*------------------reserved begin-------------------*/
void DMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char DMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/
