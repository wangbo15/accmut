//===----------------------------------------------------------------------===//
//
// This file provides utils to load $HOME/tmp/accmut/mutations.txt to AllMutsMap
// 
// Add by Wang Bo. DEC 22, 2015
//
//===----------------------------------------------------------------------===//

#ifndef ACCMUT_MUT_UTIL_H
#define ACCMUT_MUT_UTIL_H

#include "llvm/Transforms/AccMut/Mutation.h"
#include "llvm/Transforms/AccMut/Config.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>




#include<map>
#include<vector>
#include<string>


using namespace std;
using namespace llvm;

class MutUtil{
public:
	static map<string, vector<Mutation*>* > AllMutsMap;
	static void getAllMutations();
	static void dumpAllMuts();
	static BasicBlock::iterator getLocation(Function &F, int instrumented_insts, int index);
    static int getOperandPtrDimension(Value* v);
private:
	static bool allMutsGeted;
	static Mutation * getMutation(string line, int id);
};


#endif
