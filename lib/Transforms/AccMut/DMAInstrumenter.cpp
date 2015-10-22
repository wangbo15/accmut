//===----------------------------------------------------------------------===//
//
 // This file decribes the dynamic mutation analysis IR instrumenter pass
// 
 // Add by Wang Bo. OCT 21, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/AccMut/DMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutationGen.h"

using namespace llvm;
using namespace std;

vector<Mutation> DMAInstrumenter::all_muts;
ifstream DMAInstrumenter::fs;

DMAInstrumenter::DMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	fs.open(MutationGen::mutation_filepath, ios::in);
	getMutations();
}

bool DMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		// TODO: intrument main
		return false;
	}
	
	return false;
}

void DMAInstrumenter::getMutations(){
	string line;
	while(!fs.eof()){
		fs>>line;
		
	}
	fs.close();
}

/*------------------reserved begin-------------------*/
void DMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char DMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/
