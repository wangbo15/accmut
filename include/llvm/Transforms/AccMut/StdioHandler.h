//===----------------------------------------------------------------------===//
//
 // This file to replace stido function calls to accmut corresponding calls
// 
 // Add by Wang Bo. AUG 24, 2016
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_STDIO_HANDLER_H
#define ACCMUT_STDIO_HANDLER_H

#include "llvm/Transforms/AccMut/Config.h"

#include "llvm/IR/Module.h"

#include <fstream>

using namespace llvm;

class StdioHandlerPass: public FunctionPass{
public:
	static char ID;// Pass identification, replacement for typeid
	virtual void getAnalysisUsage(AnalysisUsage &AU) const;

	Module *TheModule;
	StdioHandlerPass(Module *M);
	virtual bool runOnFunction(Function &F);
};


#endif

