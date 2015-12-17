//===----------------------------------------------------------------------===//
//
 // This file implements mutation generation
// 
 // Add by Wang Bo. OCT 19, 2015
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

#include "llvm/Transforms/AccMut/MutationGen.h"

#include <sstream>
#include<cstdlib>

using namespace llvm;
using namespace std;

ofstream  MutationGen::ofresult; 

//the generating mutation's id 
int mutation_id = 1;

#define ARITH_OP_NUM 7
#define LOGIC_OP_NUM 6

unsigned arith_opcodes[ARITH_OP_NUM] = {Instruction::Add, Instruction::Sub, Instruction::Mul , Instruction::UDiv,
	Instruction::SDiv , Instruction::URem , Instruction::SRem}; 
unsigned logic_opcodes[LOGIC_OP_NUM] = {Instruction::Shl, Instruction::LShr, Instruction::AShr, Instruction::And,
	Instruction::Or, Instruction::Xor};

MutationGen::MutationGen(Module *M) : FunctionPass(ID) {
	string home = getenv("HOME");
	stringstream ss;
	ss<<home<<"/tmp/accmut/mutations.txt"; 
	ofresult.open(ss.str(), ios::trunc); // TODO: init just once? 
	this->TheModule = M;
}

bool MutationGen::runOnFunction(Function &F) {
	//omit main function and the functions with "__accmut__" prefix
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		return false;
	}
	errs()<<"====GENEARTING MUTATION FOR : "<<F.getName()<<" ========\n";
	genMutationFile(F);
	return false;
}

void MutationGen::genMutationFile(Function & F){
	int index = 0;
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI, index++){

			unsigned opc = BI->getOpcode();

			if(opc == 32){// omit ALLOCA		
				continue;
			}
			
			genLVR(BI, F.getName(), index);
			
			switch(opc){
				case Instruction::Add:
				case Instruction::Sub:
				case Instruction::Mul:
				case Instruction::UDiv:
				case Instruction::SDiv:
				case Instruction::URem:
				case Instruction::SRem:{
					genAOR(BI, F.getName(), index);
					break;
				}
				case Instruction::ICmp:{
					genROR(BI, F.getName(), index);
					break;
				}
				case Instruction::Shl:
				case Instruction::LShr:
				case Instruction::AShr:
				case Instruction::And:
				case Instruction::Or:
				case Instruction::Xor:{
					genLOR(BI, F.getName(), index);
					break;
				}
				case Instruction::Call:
					genSTD(BI, F.getName(), index);
					break;
				// TODO: genSTD for store
				default:{
					
				}					
			}
			
		}
	}
	ofresult.flush();
}

void MutationGen::genAOR(Instruction *inst, StringRef fname, int index){
	for(unsigned i = 0; i < ARITH_OP_NUM ; i++){
		if(arith_opcodes[i] == inst->getOpcode()){
			continue;
		}
		std::stringstream ss;
		ss<<mutation_id<<":AOR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<arith_opcodes[i]<<'\n';
		mutation_id++;
		ofresult<<ss.str();
	}
}

void MutationGen::genROR(Instruction *inst,StringRef fname, int index){
	const ICmpInst * CI  = dyn_cast<ICmpInst>(inst);
	unsigned short predicate = CI->getPredicate();
	bool isPointerCmp = false;
	for(int i = 0; i < (int) (CI->getNumOperands()); i++){// if is pointer type , only change "!=" to "==" , "==" to "!="
		if(CI->getOperand(i)->getType()->getTypeID() == Type::PointerTyID){
			isPointerCmp = true;
			break;
		}
	}
	if(isPointerCmp){
		if(predicate == CmpInst::ICMP_EQ){
			std::stringstream ss;
			ss<<mutation_id<<":ROR:"<<std::string(fname)<<":"<<index<<":"<<inst->getOpcode()<<":";
			ss<<predicate<<":"<<CmpInst::ICMP_NE<<'\n';
			mutation_id++;
			ofresult<<ss.str();
		}else if(predicate == CmpInst::ICMP_NE){
			std::stringstream ss;
			ss<<mutation_id<<":ROR:"<<std::string(fname)<<":"<<index<<":"<<inst->getOpcode()<<":";
			ss<<predicate<<":"<<CmpInst::ICMP_EQ<<'\n';
			ofresult<<ss.str();
			mutation_id++;
		}						
	}else{
		for(unsigned short i = CmpInst::FIRST_ICMP_PREDICATE; i <= CmpInst::LAST_ICMP_PREDICATE; i++){
			if(i == predicate)
				continue;
			std::stringstream ss;
			ss<<mutation_id<<":ROR:"<<std::string(fname)<<":"<<index<<":"
				<<inst->getOpcode()<<":"<<predicate<<":"<<i<<'\n';
			mutation_id ++;
			ofresult<<ss.str();
		}
	}
}

void MutationGen::genLOR(Instruction *inst, StringRef fname, int index){
	for(unsigned i = 0; i < LOGIC_OP_NUM ; i++){
		if(logic_opcodes[i] == inst->getOpcode()){
			continue;
		}
		std::stringstream ss;
		ss<<mutation_id<<":LOR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<logic_opcodes[i]<<'\n';
		mutation_id++;
		ofresult<<ss.str();
	}	
}

void MutationGen::genSTD(Instruction * inst, StringRef fname, int index){

	CallInst *call = cast<CallInst>(inst);
	
	Type *t = call->getCalledValue()->getType();

	FunctionType* ft = cast<FunctionType>(cast<PointerType>(t)->getElementType());

	Type *tt = ft->getReturnType();
	//tt->dump();
	
	if(tt->isIntegerTy(32)){
		//1. if the func returns a int32 val, let it be 0
		//errs()<<"IT IS A 32 !!\n";
		std::stringstream ss;
		ss<<mutation_id<<":STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<32<<'\n';
		mutation_id++;
		ofresult<<ss.str();
	}else if(tt->isVoidTy()){
		//2. if the func returns void, subsitute @llvm.donothing for the func
		//errs()<<"IT IS A VOID !!\n";
		std::stringstream ss;
		ss<<mutation_id<<":STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<0<<'\n';
		mutation_id++;
		ofresult<<ss.str();
	}else if(tt->isIntegerTy(64)){
		//1. if the func returns a int64 val, let it be 0
		//errs()<<"IT IS A 64 !!\n";
		std::stringstream ss;
		ss<<mutation_id<<":STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<64<<'\n';
		mutation_id++;
		ofresult<<ss.str();		
	}
}

// for a numeral value T, LVR will generate 4 mut: 0, 1, -1, T+1, T-1 
void MutationGen::genLVR(Instruction *inst, StringRef fname, int index){
	int num = inst->getNumOperands();
	for(int i = 0; i < num; i++){
		if (const ConstantInt *CI = dyn_cast<ConstantInt>(inst->getOperand(i))){

				//CI->dump();
			
				std::stringstream ss;
				if(CI->isZero()){
					// TODO: how to confirm the int is signed or unsigned 
					// 0 -> 1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<0<<":"<<1<<'\n';
					mutation_id ++;
					// 0 -> -1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<0<<":"<<-1<<'\n';
					mutation_id ++;
					ofresult<<ss.str();
				}else if(CI->isOne()){
					// 1 -> 0
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<0<<'\n';
					mutation_id ++;	
					// 1 -> -1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<-1<<'\n';
					mutation_id ++;					
					// 1 -> 2
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<2<<'\n';
					mutation_id ++;
					ofresult<<ss.str();		
				}else if(CI->isMinusOne()){
					// -1 -> 0
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<0<<'\n';
					mutation_id ++;		
					// -1 -> 1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<1<<'\n';
					mutation_id ++;						
					// -1 -> -2
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<-2<<'\n';
					mutation_id ++;
					ofresult<<ss.str();		
				}else if(CI->equalsInt((unsigned) -2)){
					// -2 -> 0
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<0<<'\n';
					mutation_id ++;							
					// -2 -> 1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<1<<'\n';
					mutation_id ++;				
					// -2 -> -1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<-1<<'\n';
					mutation_id ++;		
					// -2 -> -3
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<-3<<'\n';
					mutation_id ++;
					ofresult<<ss.str();						
				}else if(CI->equalsInt(2)){
					// 2 -> 0
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<0<<'\n';
					mutation_id ++;							
					// 2 -> 1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<1<<'\n';
					mutation_id ++;						
					// 2 -> -1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<-1<<'\n';
					mutation_id ++;						
					// 2 -> 3
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<3<<'\n';
					mutation_id ++;
					ofresult<<ss.str();						
				}else{
				// T -> 0
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<0<<'\n';
					mutation_id ++;;					
				// T -> 1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<1<<'\n';
					mutation_id ++;					
				// T -> -1
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<-1<<'\n';
					mutation_id ++;				
				// T -> T+1
					int bigger = (int)*(CI->getValue().getRawData()) + 1;
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<bigger<<'\n';
					mutation_id ++;					
				// T -> T-1
					int smaller = (int)*(CI->getValue().getRawData()) -1;
					ss<<mutation_id<<":LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<smaller<<'\n';
					mutation_id ++;
					ofresult<<ss.str();	
				}
				
		}

	}
}

/*------------------reserved begin-------------------*/
void MutationGen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char MutationGen::ID = 0;
/*-----------------reserved end --------------------*/

