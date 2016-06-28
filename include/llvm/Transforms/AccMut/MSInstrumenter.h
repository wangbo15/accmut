//===----------------------------------------------------------------------===//
//
// This file decribes the static mutation schemata IR instrumenter pass
// 
// Add by Wang Bo. DEC 27, 2015
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_MS_INSTRUMENTER_H
#define ACCMUT_MS_INSTRUMENTER_H

#include "llvm/Transforms/AccMut/Mutation.h"
#include "llvm/Transforms/AccMut/Config.h"
#include "llvm/IR/Module.h"

#include <vector>
#include <map>
#include <string>

using namespace llvm;
using namespace std;


class MSInstrumenter: public FunctionPass{
public:
	static char ID;// Pass identification, replacement for typeid
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnFunction(Function &F);
	MSInstrumenter(Module *M);
private:	
	void filtMutsByIndex(Function &F, vector<Mutation*>* v);
	int instrument(Function &F, vector<Mutation *>* v, int index, int mut_from, int mut_to, int instrumented_insts);
	Module *TheModule;
};

#endif

