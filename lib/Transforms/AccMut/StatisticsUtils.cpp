
#include "llvm/Transforms/AccMut/StatisticsUtils.h"

ExecInstNums::ExecInstNums(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
}

bool ExecInstNums::runOnFunction(Function & F){
	if( F.getName().equals("main")){	//F.getName().startswith("__accmut__") ||
		return false;
	}

	if( F.getName().startswith("__accmut__exec_inst_nums") ){
		return false;
	}

	Function *f = TheModule->getFunction("__accmut__exec_inst_nums");// 
	
	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		Instruction* inst = &(*I);
		if(inst->getOpcode() == Instruction::PHI){
			continue;
		}
		CallInst* call = CallInst::Create(f, "", inst);
		call->setCallingConv(CallingConv::C);
		call->setTailCall(false);
		AttributeSet call_PAL;
		call->setAttributes(call_PAL);
	}
	return true;
}



/*------------------reserved begin-------------------*/
void ExecInstNums::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char ExecInstNums::ID = 0;
/*-----------------reserved end --------------------*/

