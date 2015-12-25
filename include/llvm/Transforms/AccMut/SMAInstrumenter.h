//===----------------------------------------------------------------------===//
//
 // This file decribes the dynamic mutation analysis IR instrumenter pass
// 
 // Add by Wang Bo. DEC 21, 2015
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_SMA_INSTRUMENTER_H
#define ACCMUT_SMA_INSTRUMENTER_H

#include "llvm/Transforms/AccMut/Mutation.h"
#include "llvm/Transforms/AccMut/Config.h"

#include <vector>
#include <map>
#include <string>

using namespace llvm;
using namespace std;


class SMAInstrumenter: public FunctionPass{
public:
	static char ID;// Pass identification, replacement for typeid
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnFunction(Function &F);
	SMAInstrumenter(Module *M);
private:	
	void filtMutsByIndex(Function &F, vector<Mutation*>* v);
	int instrument(Function &F, int index, int mut_from, int mut_to, int instrumented_insts);
	BasicBlock::iterator getLocation(Function &F, int instrumented_insts, int index);
	Module *TheModule;
};

#endif
