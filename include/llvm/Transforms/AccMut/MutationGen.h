//===----------------------------------------------------------------------===//
//
 // This file implements the mutation generator pass
// 
 // Add by Wang Bo. OCT 19, 2015
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_MUTATION_GEN_H
#define ACCMUT_MUTATION_GEN_H

#include "llvm/Transforms/AccMut/Mutation.h"
#include "llvm/Transforms/AccMut/Config.h"

#include "llvm/IR/Module.h"

#include <fstream>

using namespace llvm;

class MutationGen: public FunctionPass{
public:
	static char ID;// Pass identification, replacement for typeid
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;

	Module *TheModule;
	static std::ofstream  ofresult; 
	MutationGen(Module *M);
	virtual bool runOnFunction(Function &F);
	static void genMutationFile(Function & F);
private:
	static void genAOR(Instruction *inst,StringRef fname, int index);
	static void genLOR(Instruction *inst, StringRef fname, int index);
	static void genCOR();
	static void genROR(Instruction *inst,StringRef fname, int index);
	static void genSOR();
	static void genSTD(Instruction *inst,StringRef fname, int index);
	static void genLVR(Instruction *inst, StringRef fname, int index);
};


#endif
