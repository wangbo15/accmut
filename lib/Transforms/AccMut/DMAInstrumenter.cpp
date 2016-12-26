//===----------------------------------------------------------------------===//
//
// This file decribes the dynamic mutation analysis IR instrumenter pass
// 
// Add by Wang Bo. OCT 21, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/DMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutUtil.h"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include<fstream>
#include<sstream>
#include<string>
#include<cstdlib>



using namespace llvm;
using namespace std;

#define ERRMSG(msg) llvm::errs()<<(msg)<<" @ "<<__FILE__<<"->"<<__FUNCTION__<<"():"<<__LINE__<<"\n"
		
#define VALERRMSG(it,msg,cp) llvm::errs()<<"\tCUR_IT:\t"<<(*(it))<<"\n\t"<<(msg)<<":\t"<<(*(cp))<<"\n"

DMAInstrumenter::DMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	//getAllMutations(); 
	MutUtil::getAllMutations();
}

static void test(Function &F){
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator cur_it = BB->begin(); cur_it != BB->end(); ++cur_it){
			#if 0
			if(dyn_cast<CallInst>(&*cur_it)){

				//move all constant literal int to repalce to alloca
				for (auto OI = cur_it->op_begin(), OE = cur_it->op_end(); OI != OE; ++OI){
					if(ConstantInt* cons = dyn_cast<ConstantInt>(&*OI)){
						AllocaInst *alloca = new AllocaInst(cons->getType(), "cons_alias", cur_it);
						StoreInst *str = new StoreInst(cons, alloca, cur_it);
						LoadInst *ld = new LoadInst(alloca, "const_load", cur_it);
						*OI = (Value*) ld;
					}
				}
						
				Module* TheModule = F.getParent();
				
				Function* precallfunc = TheModule->getFunction("__accmut__prepare_call");
				std::vector<Value*> params;
				std::stringstream ss;
				ss<<0;
				ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
						APInt(32, StringRef(ss.str()), 10)); 
				params.push_back(from_i32);
				ss.str("");
				ss<<1;
				ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
					APInt(32, StringRef(ss.str()), 10));
				params.push_back(to_i32);
	
				std::bitset<64> signature;
				unsigned char index = 0;
				int record_num = 0;
				//get signature info
				for (auto OI = cur_it->op_begin(), OE = cur_it->op_end(); OI != OE; ++OI, ++index){
					Type* OIType = (dyn_cast<Value>(&*OI))->getType();
					int tp = getTypeMacro(OIType);
					if(tp < 0){
						continue;
					}
					//push type
					ss.str("");
					short tp_and_idx = ((unsigned char)tp)<<8;
					tp_and_idx = tp_and_idx | index;
					ss<<tp_and_idx;
					ConstantInt* ctai = ConstantInt::get(TheModule->getContext(), 
						APInt(16, StringRef(ss.str()), 10)); 
					params.push_back(ctai);
					//now push the idx'th param
					if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){//is a local var
						params.push_back(ld->getPointerOperand());
					}else{
						llvm::errs()<<"ERROR, ONLY FOR LORDINST @ "<<__FUNCTION__<<" : "<<__LINE__<<"\n";
						exit(0);
					}
					record_num++;
				}
				//insert num of param-records
				ss.str("");
				ss<<record_num;
				ConstantInt* rcd = ConstantInt::get(TheModule->getContext(), 
						APInt(32, StringRef(ss.str()), 10)); 
				params.insert( params.begin()+2 , rcd);

				CallInst *pre = CallInst::Create(precallfunc, params, "", cur_it);

				ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(TheModule->getContext()), 0);
			
				ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_NE, pre, zero, "hasstd");

				BasicBlock *cur_bb = cur_it->getParent();
		
				Instruction* oricall = cur_it->clone();
				
				BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "if.end");

				BasicBlock* label_if_then = BasicBlock::Create(TheModule->getContext(), "if.then",cur_bb->getParent(), label_if_end);
				BasicBlock* label_if_else = BasicBlock::Create(TheModule->getContext(), "if.else",cur_bb->getParent(), label_if_end);

				cur_bb->back().eraseFromParent();

				BranchInst::Create(label_if_then, label_if_else, hasstd, cur_bb);
					
				//label_if_then
				label_if_then->getInstList().push_back(oricall);
				BranchInst::Create(label_if_end, label_if_then);	

				//label_if_else
				Function *std_handle;
				if(oricall->getType()->isIntegerTy(32)){
					std_handle = TheModule->getFunction("__accmut__std_i32");
				}else if(oricall->getType()->isIntegerTy(64)){
					std_handle = TheModule->getFunction("__accmut__std_i64");
				}else if(oricall->getType()->isVoidTy()){
					std_handle = TheModule->getFunction("__accmut__std_void");
				}else{
					llvm::errs()<<"ERR CALL TYPE @ "<<__FUNCTION__<<" : "<<__LINE__<<"\n";
					exit(0);
				}
	
				CallInst*  stdcall = CallInst::Create(std_handle, "", label_if_else);
				stdcall->setCallingConv(CallingConv::C);
				stdcall->setTailCall(false);
				AttributeSet stdcallPAL;
				stdcall->setAttributes(stdcallPAL);
				BranchInst::Create(label_if_end, label_if_else);

				//label_if_end
				if(oricall->getType()->isVoidTy()){
					cur_it->removeFromParent();
				}
				else{
					PHINode* call_res = PHINode::Create(IntegerType::get(TheModule->getContext(), 32), 2, "call.phi");
					call_res->addIncoming(oricall, label_if_then);
					call_res->addIncoming(stdcall, label_if_else);
					ReplaceInstWithInst(cur_it, call_res);
				}

				return;

			}
			#endif

			if(StoreInst* st = dyn_cast<StoreInst>(&*cur_it)){

				Module* TheModule = F.getParent();
				
				if(ConstantInt* cons = dyn_cast<ConstantInt>(st->getValueOperand())){
						AllocaInst *alloca = new AllocaInst(cons->getType(), "cons_alias", st);
						StoreInst *str = new StoreInst(cons, alloca, st);
						LoadInst *ld = new LoadInst(alloca, "const_load", st);
						User::op_iterator OI = st->op_begin();
						*OI = (Value*) ld;
				}

				Function* prestfunc;
				if(st->getValueOperand()->getType()->isIntegerTy(32)){
					prestfunc = TheModule->getFunction("__accmut__prepare_st_i32");
				}
				else if(st->getValueOperand()->getType()->isIntegerTy(64)){
					prestfunc = TheModule->getFunction("__accmut__prepare_st_i64");
				}else{
					ERRMSG("ERR STORE TYPE");
					exit(0);
				}
				
				std::vector<Value*> params;
				std::stringstream ss;
				ss<<0;
				ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
						APInt(32, StringRef(ss.str()), 10)); 
				params.push_back(from_i32);
				ss.str("");
				ss<<1;
				ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
					APInt(32, StringRef(ss.str()), 10));
				params.push_back(to_i32);
				
				auto OI = st->op_begin() + 1;
				if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){//is a local var
					params.push_back(ld->getPointerOperand());
				}else if(AllocaInst *alloca = dyn_cast<AllocaInst>(&*OI)){
					params.push_back(alloca);
				}else{
					ERRMSG("NOT A POINTER");
					exit(0);
				}

				CallInst *pre = CallInst::Create(prestfunc, params, "", cur_it);

				ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(TheModule->getContext()), 0);
			
				ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_EQ, pre, zero, "hasstd");
			
				BasicBlock *cur_bb = cur_it->getParent();
				
				Instruction* orist = cur_it->clone();
				
				BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "if.end");
				
				BasicBlock* label_if_then = BasicBlock::Create(TheModule->getContext(), "if.then",cur_bb->getParent(), label_if_end);
				BasicBlock* label_if_else = BasicBlock::Create(TheModule->getContext(), "if.else",cur_bb->getParent(), label_if_end);
				
				cur_bb->back().eraseFromParent();
				
				BranchInst::Create(label_if_then, label_if_else, hasstd, cur_bb);

				//label_if_then
				label_if_then->getInstList().push_back(orist);
				BranchInst::Create(label_if_end, label_if_then);	

				//label_if_else
				Function *std_handle = TheModule->getFunction("__accmut__std_store");
				CallInst*  stdcall = CallInst::Create(std_handle, "", label_if_else);
				stdcall->setCallingConv(CallingConv::C);
				stdcall->setTailCall(false);
				AttributeSet stdcallPAL;
				stdcall->setAttributes(stdcallPAL);
				BranchInst::Create(label_if_end, label_if_else);

				//label_if_end
				cur_it->eraseFromParent();
				return;
			}
		}
	}	
}

bool DMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		return true;
	}
	vector<Mutation*>* v= MutUtil::AllMutsMap[F.getName()];
	
	if(v == NULL || v->size() == 0){
		return false;
	}

	errs()<<"\n######## DMA INSTRUMTNTING MUT  @"<<TheModule->getName()<<"->"<<F.getName()<<"()  ########\n\n";	

	instrument(F, v);
	//test(F);

	return true;
}


//TYPE BITS OF SIGNATURE
#define CHAR_TP 0
#define SHORT_TP 1
#define INT_TP 2
#define LONG_TP 3

static int getTypeMacro(Type *t){
	int res = -1;
	if(t->isIntegerTy()){
		unsigned width = t->getIntegerBitWidth();
		switch(width){
			case 8:
				res = CHAR_TP;
				break;
			case 16:
				res = SHORT_TP;
				break;
			case 32:
				res = INT_TP;
				break;
			case 64:
				res = LONG_TP;
				break;
			default:
				ERRMSG("OMMITNG PARAM TYPE");
				llvm::errs()<<*t<<'\n';
				//exit(0);
		}
	}	
	return res;
}

static bool isHandledCoveInst(Instruction *inst){
	return inst->getOpcode() == Instruction::Trunc 
		|| inst->getOpcode() == Instruction::ZExt 
		|| inst->getOpcode() == Instruction::SExt 
		|| inst->getOpcode() == Instruction::BitCast ;
	// TODO:: PtrToInt, IntToPtr, AddrSpaceCast and float related Inst are not handled
}

static bool pushPreparecallParam(std::vector<Value*>& params, int index, Value *OI, Module *TheModule){
	Type* OIType = (dyn_cast<Value>(&*OI))->getType();
	int tp = getTypeMacro(OIType);
	if(tp < 0){
		return false;
	}
	
	std::stringstream ss;
	//push type
	ss.str("");
	short tp_and_idx = ((unsigned char)tp)<<8;
	tp_and_idx = tp_and_idx | index;
	ss<<tp_and_idx;
	ConstantInt* c_t_a_i = ConstantInt::get(TheModule->getContext(), 
		APInt(16, StringRef(ss.str()), 10)); 

	//now push the pointer of idx'th param
	if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){//is a local var
		Value *ptr_of_ld = ld->getPointerOperand();
		//if the pointer of loadInst dose not point to an integer
		if(SequentialType *t = dyn_cast<SequentialType>(ptr_of_ld->getType())){
			if(! t->getElementType()->isIntegerTy()){// TODO: for i32** 
				ERRMSG("WARNNING ! Trying to push a none-i32* !! ");
				return false;
			}
		}
		params.push_back(c_t_a_i);
		params.push_back(ptr_of_ld);
	}else if(AllocaInst *alloca = dyn_cast<AllocaInst>(&*OI)){// a param of the F, fetch it by alloca
		params.push_back(c_t_a_i);
		params.push_back(alloca);
	}else if(GetElementPtrInst *ge = dyn_cast<GetElementPtrInst>(&*OI)){
		// TODO: test
		params.push_back(c_t_a_i);
		params.push_back(ge);
	}
	// TODO:: for Global Pointer ?!
	else{
		ERRMSG("CAN NOT GET A POINTER");
		Value *v = dyn_cast<Value>(&*OI);
		llvm::errs()<<"\tCUR_OPREAND:\t";
		v->dump();
		exit(0);					
	}
	return true;
}

void DMAInstrumenter::instrument(Function &F, vector<Mutation*> * v){

	int instrumented_insts = 0;
	
	Function::iterator cur_bb;
	BasicBlock::iterator cur_it;

	for(unsigned i = 0; i < v->size(); i++){
		vector<Mutation*> tmp;
		Mutation* m = (*v)[i];
		tmp.push_back(m);
		//collect all mutations of one instruction
		for(unsigned j = i + 1; j < v->size(); j++){
			Mutation* m1 = (*v)[j];
			if(m1->index == m->index){
				tmp.push_back(m1);
				i = j;		// TODO: check
			}else{
				break;
			}
		}
		
		cur_it =  getLocation(F, instrumented_insts, tmp[0]->index);

		cur_bb = cur_it->getParent();

		int mut_from, mut_to;
		mut_from = tmp.front()->id;
		mut_to = tmp.back()->id;

		if(tmp.size() >= MAX_MUT_NUM_PER_LOCATION){
			ERRMSG("TOO MANY MUTS ");
			llvm::errs()<<"CUR_INST: "<<tmp.front()->index<<"\t(FROM: "
				<<mut_from<<"\tTO: "<<mut_to<<")\t"<<*cur_it<<"\n";
			exit(0);
		}
		
		llvm::errs()<<"CUR_INST: "<<tmp.front()->index<<"\t(FROM: "
			<<mut_from<<"\tTO: "<<mut_to<<")\t"<<*cur_it<<"\n";
		
		if(dyn_cast<CallInst>(&*cur_it)){
			//move all constant literal and SSA value to repalce to alloca, e.g foo(a+5)->b = a+5;foo(b)
			for (auto OI = cur_it->op_begin(), OE = cur_it->op_end(); OI != OE; ++OI){
				if(ConstantInt* cons = dyn_cast<ConstantInt>(&*OI)){
					AllocaInst *alloca = new AllocaInst(cons->getType(), (cons->getName().str()+".alias"), cur_it);
					StoreInst *str = new StoreInst(cons, alloca, cur_it);
					LoadInst *ld = new LoadInst(alloca, (cons->getName().str()+".ld"), cur_it);
					*OI = (Value*) ld;
					instrumented_insts += 3;//add 'alloca', 'store' and 'load'
				}
				else if(Instruction* oinst = dyn_cast<Instruction>(&*OI)){
					if(oinst->isBinaryOp() || 
						(oinst->getOpcode() == Instruction::Call) || 
						isHandledCoveInst(oinst) ||
						(oinst->getOpcode() == Instruction::PHI) ||
						(oinst->getOpcode() == Instruction::Select) ){
						AllocaInst *alloca = new AllocaInst(oinst->getType(), (oinst->getName().str()+".ptr"), cur_it);
						StoreInst *str = new StoreInst(oinst, alloca, cur_it);
						LoadInst *ld = new LoadInst(alloca, (oinst->getName().str()+".ld"), cur_it);
						*OI = (Value*) ld;
						instrumented_insts += 3;
						
					}
				}
				
			}

			Function* precallfunc = TheModule->getFunction("__accmut__prepare_call");
			std::vector<Value*> params;
			std::stringstream ss;
			ss<<mut_from;
			ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
					APInt(32, StringRef(ss.str()), 10)); 
			params.push_back(from_i32);
			ss.str("");
			ss<<mut_to;
			ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
				APInt(32, StringRef(ss.str()), 10));
			params.push_back(to_i32);

			int index = 0;
			int record_num = 0;
			std::vector<int> pushed_param_idx;
			
			//get signature info
			for (auto OI = cur_it->op_begin(), OE = cur_it->op_end() - 1; OI != OE; ++OI, ++index){

				Value *v = dyn_cast<Value>(&*OI);
				bool succ = pushPreparecallParam(params, index, v, TheModule);

				if(succ){					
					pushed_param_idx.push_back(index);
					record_num++;
				}
				else{
					//ERRMSG("---- WARNNING : PUSH PARAM FAILURE ");
					//VALERRMSG(cur_it,"CUR_OPRAND",v);
				}

			}

			if(! pushed_param_idx.empty()){
				llvm::errs()<<"---- PUSH PARAM : ";
				for(auto it = pushed_param_idx.begin(), ie = pushed_param_idx.end(); it != ie; ++it){
					llvm::errs()<<*it<<"'th\t";
				}
				llvm::errs()<<"\n";
			}
			
			//insert num of param-records
			ss.str("");
			ss<<record_num;
			ConstantInt* rcd = ConstantInt::get(TheModule->getContext(), 
					APInt(32, StringRef(ss.str()), 10)); 
			params.insert( params.begin()+2 , rcd);

			CallInst *pre = CallInst::Create(precallfunc, params, "", cur_it);
			pre->setCallingConv(CallingConv::C);
			pre->setTailCall(false);
			AttributeSet preattrset;
			pre->setAttributes(preattrset);
			
			ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(TheModule->getContext()), 0);
			
			ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_EQ, pre, zero, "hasstd.call");
			
			BasicBlock *cur_bb = cur_it->getParent();
			
			Instruction* oricall = cur_it->clone();
			
			BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "stdcall.if.end");
			
			BasicBlock* label_if_then = BasicBlock::Create(TheModule->getContext(), "stdcall.if.then",cur_bb->getParent(), label_if_end);
			BasicBlock* label_if_else = BasicBlock::Create(TheModule->getContext(), "stdcall.if.else",cur_bb->getParent(), label_if_end);
			
			cur_bb->back().eraseFromParent();
			
			BranchInst::Create(label_if_then, label_if_else, hasstd, cur_bb);
				
			//label_if_then
			//move the loadinsts of params into if_then_block
			index = 0;
			for (auto OI = oricall->op_begin(), OE = oricall->op_end() - 1; OI != OE; ++OI, ++index){

				//only move pushed parameters
				if(find(pushed_param_idx.begin(), pushed_param_idx.end(), index) == pushed_param_idx.end()){
					continue;
				}
				
				if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){
					ld->removeFromParent();
					label_if_then->getInstList().push_back(ld);
				}else if(Constant *con = dyn_cast<Constant>(&*OI)){
					// TODO::  test
					continue;
				}else if(GetElementPtrInst *ge = dyn_cast<GetElementPtrInst>(&*OI)){
					// TODO: test
					ge->removeFromParent();
					label_if_then->getInstList().push_back(ge);
				}else{
					// TODO:: check
					// TODO:: instrumented_insts !!!
					Instruction *coversion = dyn_cast<Instruction>(&*OI);
					if(isHandledCoveInst(coversion)){
						Instruction* op_of_cov = dyn_cast<Instruction>(coversion->getOperand(0));
						if(dyn_cast<LoadInst>(&*op_of_cov) || dyn_cast<AllocaInst>(&*op_of_cov)){
							op_of_cov->removeFromParent();
							label_if_then->getInstList().push_back(op_of_cov);
							coversion->removeFromParent();
							label_if_then->getInstList().push_back(coversion);
						}else{
							ERRMSG("CAN MOVE GET A POINTER INTO IF.THEN");
							Value *v = dyn_cast<Value>(&*OI);
							VALERRMSG(cur_it,"CUR_OPRAND",v);
							exit(0);
						}
					}else{
						ERRMSG("CAN MOVE GET A POINTER INTO IF.THEN");
						Value *v = dyn_cast<Value>(&*OI);
						VALERRMSG(cur_it,"CUR_OPRAND",v);
						exit(0);
					}						
				}
			}
			label_if_then->getInstList().push_back(oricall);
			BranchInst::Create(label_if_end, label_if_then);	
			
			//label_if_else
			Function *std_handle;
			if(oricall->getType()->isIntegerTy(32)){
				std_handle = TheModule->getFunction("__accmut__stdcall_i32");
			}else if(oricall->getType()->isIntegerTy(64)){
				std_handle = TheModule->getFunction("__accmut__stdcall_i64");
			}else if(oricall->getType()->isVoidTy()){
				std_handle = TheModule->getFunction("__accmut__stdcall_void");
			}else{
				ERRMSG("ERR CALL TYPE ");
				oricall->dump();
				oricall->getType()->dump();
				exit(0);
			}//}else if(oricall->getType()->isPointerTy()){
			
			CallInst*  stdcall = CallInst::Create(std_handle, "", label_if_else);
			stdcall->setCallingConv(CallingConv::C);
			stdcall->setTailCall(false);
			AttributeSet stdcallPAL;
			stdcall->setAttributes(stdcallPAL);
			BranchInst::Create(label_if_end, label_if_else);
			
			//label_if_end
			if(oricall->getType()->isVoidTy()){
				cur_it->eraseFromParent();
				instrumented_insts += 6;
			}
			else{
				PHINode* call_res = PHINode::Create(oricall->getType(), 2, "call.phi");
				call_res->addIncoming(oricall, label_if_then);
				call_res->addIncoming(stdcall, label_if_else);
				ReplaceInstWithInst(cur_it, call_res);
				instrumented_insts += 7;
			}

		}
		
		else if(StoreInst* st = dyn_cast<StoreInst>(&*cur_it)){
			// TODO:: add or call inst?
			if(ConstantInt* cons = dyn_cast<ConstantInt>(st->getValueOperand())){
					AllocaInst *alloca = new AllocaInst(cons->getType(), "cons_alias", st);
					StoreInst *str = new StoreInst(cons, alloca, st);
					LoadInst *ld = new LoadInst(alloca, "const_load", st);
					User::op_iterator OI = st->op_begin();
					*OI = (Value*) ld;
					instrumented_insts += 3;
			}

			Function* prestfunc;
			if(st->getValueOperand()->getType()->isIntegerTy(32)){
				prestfunc = TheModule->getFunction("__accmut__prepare_st_i32");
			}
			else if(st->getValueOperand()->getType()->isIntegerTy(64)){
				prestfunc = TheModule->getFunction("__accmut__prepare_st_i64");
			}else{
				ERRMSG("ERR STORE TYPE ");
				VALERRMSG(cur_it, "CUR_TYPE", st->getValueOperand()->getType());
				exit(0);
			}
			
			std::vector<Value*> params;
			std::stringstream ss;
			ss<<mut_from;
			ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
					APInt(32, StringRef(ss.str()), 10)); 
			params.push_back(from_i32);
			ss.str("");
			ss<<mut_to;
			ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
				APInt(32, StringRef(ss.str()), 10));
			params.push_back(to_i32);

			Value* tobestored = dyn_cast<Value>(st->op_begin());
			params.push_back(tobestored);
			
			auto addr = st->op_begin() + 1;// the pointer of the storeinst
			if(LoadInst *ld = dyn_cast<LoadInst>(&*addr)){//is a local var
				params.push_back(ld);
			}else if(AllocaInst *alloca = dyn_cast<AllocaInst>(&*addr)){
				params.push_back(alloca);
			}else if(Constant *con = dyn_cast<Constant>(&*addr)){
				params.push_back(con);
			}else if(GetElementPtrInst *gete = dyn_cast<GetElementPtrInst>(&*addr)){
				params.push_back(gete);
			}else{
				ERRMSG("NOT A POINTER ");
				cur_it->dump();
				Value *v = dyn_cast<Value>(&*addr);
				v->dump();
				exit(0);
			}
			CallInst *pre = CallInst::Create(prestfunc, params, "", cur_it);
			pre->setCallingConv(CallingConv::C);
			pre->setTailCall(false);
			AttributeSet attrset;
			pre->setAttributes(attrset);

			ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(TheModule->getContext()), 0);
		
			ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_EQ, pre, zero, "hasstd.st");
		
			BasicBlock *cur_bb = cur_it->getParent();
						
			BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "stdst.if.end");
			
			BasicBlock* label_if_else = BasicBlock::Create(TheModule->getContext(), "std.st",cur_bb->getParent(), label_if_end);
			
			cur_bb->back().eraseFromParent();
			
			BranchInst::Create(label_if_end, label_if_else, hasstd, cur_bb);


			//label_if_else
			Function *std_handle = TheModule->getFunction("__accmut__std_store");
			CallInst*  stdcall = CallInst::Create(std_handle, "", label_if_else);
			stdcall->setCallingConv(CallingConv::C);
			stdcall->setTailCall(false);
			AttributeSet stdcallPAL;
			stdcall->setAttributes(stdcallPAL);
			BranchInst::Create(label_if_end, label_if_else);

			//label_if_end
			cur_it->eraseFromParent();	
			instrumented_insts += 4;
		}
		else{
			// FOR ARITH INST
			if(cur_it->getOpcode() >= Instruction::Add && 
				cur_it->getOpcode() <= Instruction::Xor){ 
				Type* ori_ty = cur_it->getType();
				Function* f_process;
				if(ori_ty->isIntegerTy(32)){
					f_process = TheModule->getFunction("__accmut__process_i32_arith");
				}else if(ori_ty->isIntegerTy(64)){
					f_process = TheModule->getFunction("__accmut__process_i64_arith");
				}else{
					ERRMSG("ArithInst TYPE ERROR ");
					cur_it->dump();
					llvm::errs()<<*ori_ty<<"\n";
					// TODO:: handle i1, i8, i64 ... type
					exit(0);
				}

				std::vector<Value*> int_call_params;
				std::stringstream ss;
				ss<<mut_from;
				ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10)); 
				int_call_params.push_back(from_i32);
				ss.str("");
				ss<<mut_to;
				ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10));
				int_call_params.push_back(to_i32);
				int_call_params.push_back(cur_it->getOperand(0));
				int_call_params.push_back(cur_it->getOperand(1));
				CallInst *call = CallInst::Create(f_process, int_call_params);
				ReplaceInstWithInst(cur_it, call);

				
			}
			// FOR ICMP INST
			else if(cur_it->getOpcode() == Instruction::ICmp){
				Function* f_process;
				
				if(cur_it->getOperand(0)->getType()->isIntegerTy(32)){
					f_process = TheModule->getFunction("__accmut__process_i32_cmp");
				}else if(cur_it->getOperand(0)->getType()->isIntegerTy(64)){
					f_process = TheModule->getFunction("__accmut__process_i64_cmp");
				}else{
					ERRMSG("ICMP TYPE ERROR ");	
					exit(0);
				}
				
				std::vector<Value*> int_call_params;
				std::stringstream ss;
				ss<<mut_from;
				ConstantInt* from_i32= ConstantInt::get(TheModule->getContext(), 
						APInt(32, StringRef(ss.str()), 10)); 
				int_call_params.push_back(from_i32);
				ss.str("");
				ss<<mut_to;
				ConstantInt* to_i32= ConstantInt::get(TheModule->getContext(),
					APInt(32, StringRef(ss.str()), 10));
				int_call_params.push_back(to_i32);
				int_call_params.push_back(cur_it->getOperand(0));
				int_call_params.push_back(cur_it->getOperand(1));
				CallInst *call = CallInst::Create(f_process, int_call_params, "", cur_it);
				CastInst* i32_conv = new TruncInst(call, IntegerType::get(TheModule->getContext(), 1), "");

				instrumented_insts++;
				
				ReplaceInstWithInst(cur_it, i32_conv);
			}
		}
		
	}
}

#undef CHAR_TP
#undef SHORT_TP
#undef INT_TP
#undef LONG_TP

BasicBlock::iterator DMAInstrumenter::getLocation(Function &F, int instrumented_insts, int index){
	int cur = 0;
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI, cur++){
			if(index + instrumented_insts == cur ){
				return BI;
			}
		}
	}
	return F.back().end();
}

bool DMAInstrumenter::hasMutation(Instruction *inst, vector<Mutation*>* v){
	int index = 0;
	Function *F = inst->getParent()->getParent();

	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I, index++) {
		if((&*I) == inst){
			break;
		}
	}

	//errs()<<" ~~~~~ INDEX : "<<index<<"\n";
	for(vector<Mutation*>::iterator it = v->begin(), end = v->end(); it != end; it++){
		if( (*it)->index == index){
			return true;
		}else if((*it)->index > index){// TODO: check
			return false;
		}
	}
	return false;
}




/*------------------reserved begin-------------------*/
void DMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char DMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/
