//===----------------------------------------------------------------------===//
//
 // This file decribes the static mutation analysis IR instrumenter pass
// 
 // Add by Wang Bo. DEC 21, 2015
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_SMA_INSTRUMENTER_H
#define ACCMUT_SMA_INSTRUMENTER_H

#include "llvm/Transforms/AccMut/Mutation.h"
#include "llvm/Transforms/AccMut/Config.h"
#include "llvm/IR/Module.h"

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
	bool hasMutation(Instruction *I, vector<Mutation*>* v);
	bool needInstrument(Instruction *I, vector<Mutation*>* v);
	static void outputExpression(Instruction* inst);
	static void outputMutation(Instruction *inst, vector<Mutation*>* v);
	
	void instrument(Function &F, vector<Mutation*> * v);
	BasicBlock::iterator getLocation(Function &F, int instrumented_insts, int index);
	Module *TheModule;
	static ofstream  expr_os;
	static ofstream  mut_os;
};

#endif
