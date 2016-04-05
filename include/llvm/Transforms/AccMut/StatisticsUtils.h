//===----------------------------------------------------------------------===//
//
 // This file include some utils for statistic
// 
 // Add by Wang Bo. April 1, 2016
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_STATISTICS_UTILS_H
#define ACCMUT_STATISTICS_UTILS_H


#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"


#include <vector>
#include <map>
#include <string>


using namespace llvm;
using namespace std;

class ExecInstNums: public FunctionPass{
public:
	static char ID;// Pass identification, replacement for typeid
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;
	virtual bool runOnFunction(Function &F);
	ExecInstNums(Module *M);	
private:
	Module *TheModule;
};


#endif