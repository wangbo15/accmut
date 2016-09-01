//===----------------------------------------------------------------------===//
//
 // This file to replace stido function calls to accmut corresponding calls
// 
 // Add by Wang Bo. AUG 24, 2016
//
//===----------------------------------------------------------------------===//


#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>

#include "llvm/Transforms/AccMut/StdioHandler.h"

#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace llvm;
using namespace std;

//string handling_function[] = {};

StdioHandlerPass::StdioHandlerPass(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
}


static bool isStdioFunc(Function *f){
	return false;
}

bool StdioHandlerPass::runOnFunction(Function &F) {

	//omit main function and the functions with "__accmut__" prefix
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		return false;
	}
	llvm::errs()<<"\n\t Stdio Handler Passing : "<<TheModule->getName()<<" -> "<<F.getName()<<"() \n";

	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI){
			if(BI->getOpcode() != Instruction::Call){
				continue;
			}
			
		}

	}

	return false;
}




/*------------------reserved begin-------------------*/
void StdioHandlerPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char StdioHandlerPass::ID = 0;
/*-----------------reserved end --------------------*/


