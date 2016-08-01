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
#include <cstdlib>
#include <ctime>

using namespace llvm;
using namespace std;

ofstream  MutationGen::ofresult; 


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
	ofresult.open(ss.str(), ios::app); 
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
	errs()<<"\n\t GENEARTING MUTATION FOR : "<<TheModule->getName()<<" -> "<<F.getName()<<"() \n";
	genMutationFile(F);
	return false;
}

void MutationGen::genMutationFile(Function & F){
	int index = 0;
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI, index++){

			unsigned opc = BI->getOpcode();

			if( !((opc >= 14 && opc <= 31) || opc == 34 || opc == 52 || opc == 55) ){// omit alloca and getelementptr		
				continue;
			}
			/**************************/
		#if 0	
			if(opc == 55){		// TODO:: disable STD first
				continue;
			}
		#endif
			
			switch(opc){
				case Instruction::Add:
				case Instruction::Sub:
				case Instruction::Mul:
				case Instruction::UDiv:
				case Instruction::SDiv:
				case Instruction::URem:
				case Instruction::SRem:{
					genLVR(BI, F.getName(), index);
					genUOI(BI, F.getName(), index);
					genROV(BI, F.getName(), index);
					genABV(BI, F.getName(), index);					
					genAOR(BI, F.getName(), index);
					break;
				}
				case Instruction::ICmp:{
					genLVR(BI, F.getName(), index);
					genUOI(BI, F.getName(), index);	
					genROV(BI, F.getName(), index);
					genABV(BI, F.getName(), index);			
					genROR(BI, F.getName(), index);
					break;
				}
				case Instruction::Shl:
				case Instruction::LShr:
				case Instruction::AShr:
				case Instruction::And:
				case Instruction::Or:
				case Instruction::Xor:{
					// TODO: to support i32 and i64 first
					if(! (BI->getType()->isIntegerTy(32) || BI->getType()->isIntegerTy(64))){
						continue;
					}
					genLVR(BI, F.getName(), index);
					genUOI(BI, F.getName(), index);
					genROV(BI, F.getName(), index);
					genABV(BI, F.getName(), index);					
					genLOR(BI, F.getName(), index);
					break;
				}
				case Instruction::Call:
				{
					StringRef name = cast<CallInst>(BI)->getCalledFunction()->getName();
					if(name.startswith("llvm")){//omit llvm inside functions
						continue;
					}
					genLVR(BI, F.getName(), index);
					genUOI(BI, F.getName(), index);
					genROV(BI, F.getName(), index);
					genABV(BI, F.getName(), index);					
					genSTDCall(BI, F.getName(), index);
					break;
				}
				case Instruction::Store:{
					genLVR(BI, F.getName(), index);
					genUOI(BI, F.getName(), index);
					genABV(BI, F.getName(), index);	
					genSTDStore(BI, F.getName(), index);
					break;
				}	
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
		ss<<"AOR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<arith_opcodes[i]<<'\n';
		ofresult<<ss.str();
		ofresult.flush();
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
			ss<<"ROR:"<<std::string(fname)<<":"<<index<<":"<<inst->getOpcode()<<":";
			ss<<predicate<<":"<<CmpInst::ICMP_NE<<'\n';
			ofresult<<ss.str();
		}else if(predicate == CmpInst::ICMP_NE){
			std::stringstream ss;
			ss<<"ROR:"<<std::string(fname)<<":"<<index<<":"<<inst->getOpcode()<<":";
			ss<<predicate<<":"<<CmpInst::ICMP_EQ<<'\n';
			ofresult<<ss.str();
		}						
	}else{
		for(unsigned short i = CmpInst::FIRST_ICMP_PREDICATE; i <= CmpInst::LAST_ICMP_PREDICATE; i++){
			if(i == predicate)
				continue;
			std::stringstream ss;
			ss<<"ROR:"<<std::string(fname)<<":"<<index<<":"
				<<inst->getOpcode()<<":"<<predicate<<":"<<i<<'\n';
			ofresult<<ss.str();
		}
	}
	ofresult.flush();
}

void MutationGen::genLOR(Instruction *inst, StringRef fname, int index){
	for(unsigned i = 0; i < LOGIC_OP_NUM ; i++){
		if(logic_opcodes[i] == inst->getOpcode()){
			continue;
		}
		std::stringstream ss;
		ss<<"LOR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<logic_opcodes[i]<<'\n';
		ofresult<<ss.str();
		ofresult.flush();
	}	
}

void MutationGen::genSTDCall(Instruction * inst, StringRef fname, int index){

	CallInst *call = cast<CallInst>(inst);

	Function *fun = call->getCalledFunction();

	if(fun->getName().startswith("llvm")){
		return;
	}
	
	Type *t = call->getCalledValue()->getType();

	FunctionType* ft = cast<FunctionType>(cast<PointerType>(t)->getElementType());

	Type *tt = ft->getReturnType();
	//tt->dump();
	
	if(tt->isIntegerTy(32)){
		//1. if the func returns a int32 val, let it be 0, 1 or a random number
		//errs()<<"IT IS A 32 !!\n";
		std::stringstream ss;
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<32<<":0\n";
		
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<32<<":1\n";		
		//srand((int)time(0));
		//int random = rand();
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<32<<":"<<-1<<"\n";
		
		ofresult<<ss.str();
		ofresult.flush();
	}else if(tt->isVoidTy()){
		//2. if the func returns void, subsitute @llvm.donothing for the func
		//errs()<<"IT IS A VOID !!\n";
		std::stringstream ss;
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<0<<'\n';
		ofresult<<ss.str();
		ofresult.flush();
	}else if(tt->isIntegerTy(64)){
		//1. if the func returns a int64 val, let it be 0, 1 or a random number
		//errs()<<"IT IS A 64 !!\n";
		std::stringstream ss;
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<64<<":0\n";

		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<64<<":1\n";
		
		//srand((int)time(0));
		//int random = rand();
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<64<<":"<<-1<<"\n";
		
		ofresult<<ss.str();	
		ofresult.flush();
	}
	
}

void MutationGen::genSTDStore(Instruction * inst, StringRef fname, int index){
	StoreInst *st = cast<StoreInst>(inst);
	Type * t = st->getValueOperand()->getType();
	if(t->isIntegerTy(32) || t->isIntegerTy(64)){
		std::stringstream ss;
		ss<<"STD:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()
			<< ":"<<0<<'\n';
		ofresult<<ss.str();
		ofresult.flush();
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
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<0<<":"<<1<<'\n';
					// 0 -> -1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<0<<":"<<-1<<'\n';
					ofresult<<ss.str();
				}else if(CI->isOne()){
					// 1 -> 0
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<0<<'\n';
					// 1 -> -1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<-1<<'\n';			
					// 1 -> 2
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<1<<":"<<2<<'\n';
					ofresult<<ss.str();		
				}else if(CI->isMinusOne()){
					// -1 -> 0
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<0<<'\n';
					// -1 -> 1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<1<<'\n';					
					// -1 -> -2
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-1<<":"<<-2<<'\n';
					ofresult<<ss.str();		
				}else if(CI->equalsInt((unsigned) -2)){
					// -2 -> 0
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<0<<'\n';					
					// -2 -> 1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<1<<'\n';
					// -2 -> -1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<-1<<'\n';
					// -2 -> -3
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<-2<<":"<<-3<<'\n';
					ofresult<<ss.str();						
				}else if(CI->equalsInt(2)){
					// 2 -> 0
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<0<<'\n';					
					// 2 -> 1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<1<<'\n';
					// 2 -> -1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<-1<<'\n';					
					// 2 -> 3
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<2<<":"<<3<<'\n';
					ofresult<<ss.str();						
				}else{
				// T -> 0
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<0<<'\n';				
				// T -> 1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<1<<'\n';
				// T -> -1
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<-1<<'\n';			
				// T -> T+1
					int bigger = (int)*(CI->getValue().getRawData()) + 1;
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<bigger<<'\n';				
				// T -> T-1
					int smaller = (int)*(CI->getValue().getRawData()) -1;
					ss<<"LVR:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<":"<<CI->getValue().toString(10, true)<<":"<<smaller<<'\n';
					ofresult<<ss.str();	
				}
				ofresult.flush();
		}

	}
}

void MutationGen::genUOI(Instruction *inst, StringRef fname, int index){
	for(unsigned i = 0; i < inst->getNumOperands(); i++){
		Type* t = inst->getOperand(i)->getType();
		if( t->isIntegerTy(32) || t->isIntegerTy(64)){
			std::stringstream ss;
			ss<<"UOI:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
				<<i<<":"<<"0\n";	//inc

			ss<<"UOI:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
				<<i<<":"<<"1\n";	//dec
			
			ss<<"UOI:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
				<<i<<":"<<"2\n";	//neg
			ofresult<<ss.str();	
		}
	}	
	ofresult.flush();
}

void MutationGen::genROV(Instruction *inst, StringRef fname, int index){
	int opc = inst->getOpcode();
	if( opc == Instruction::Add || opc == Instruction::Mul){
		return;//there is no need to exchange the oprand of a commutative instruction
	}
	unsigned uperbound = inst->getNumOperands();
	
	if(isa<CallInst>(inst)){//omit the callee of the CallInst
		uperbound--;
	}
	
	for(unsigned i = 0; i < uperbound; i++){
		Type* ti = inst->getOperand(i)->getType();

		bool isIntpt = false;
/**** for int pointer types
		if(ti->isPointerTy()){
			if(ti->getPointerElementType()->isIntegerTy()){
				llvm::errs()<<*ti<<" ----->> "<<*ti->getPointerElementType()<<"\n";
				isIntpt = true;
			}
		}
*/		
		if( !(ti->isIntegerTy(32) || ti->isIntegerTy(64) || isIntpt) ){
			continue;
		}
		for(unsigned j = i+1; j < uperbound; j++){
			Type* tj = inst->getOperand(j)->getType();

			if(ti->getTypeID() != tj->getTypeID()){//only switch the same type
				continue;
			}
			
			isIntpt = false;
			/*
			if(tj->isPointerTy() && tj->getPointerElementType()->isIntegerTy() ){
				isIntpt = true;;
			}
			*/
			if(!(tj->isIntegerTy(32) || tj->isIntegerTy(64) || isIntpt) ){
				continue;
			}

			std::stringstream ss;
			ss<<"ROV:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
				<<i<<":"<<j<<"\n";
			ofresult<<ss.str();	
		}
	}	
	ofresult.flush();
}

void MutationGen::genABV(Instruction *inst, StringRef fname, int index){

	for(unsigned i = 0; i < inst->getNumOperands(); i++){
		Type* t = inst->getOperand(i)->getType();
		if( t->isIntegerTy(32) || t->isIntegerTy(64)){
			std::stringstream ss;
			ss<<"ABV:"<<std::string(fname)<<":"<<index<< ":"<<inst->getOpcode()<<":"
						<<i<<"\n";
			ofresult<<ss.str();	
		}
	}	
	ofresult.flush();	
}



/*------------------reserved begin-------------------*/
void MutationGen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char MutationGen::ID = 0;
/*-----------------reserved end --------------------*/

