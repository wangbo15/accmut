//===----------------------------------------------------------------------===//
//
// This file decribes the sattic mutation analysis IR instrumenter pass
// 
// Add by Wang Bo. DEC 23, 2015
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/SMAInstrumenter.h"
#include "llvm/Transforms/AccMut/MutUtil.h"


#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include<fstream>
#include<sstream>
#include<string>
#include<cstdlib>
#include<bitset>

#include "clang/AST/Stmt.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;
using namespace std;

using clang::Stmt;

extern std::map<Value*, Stmt*> toStmtMap;

map<Stmt*, bool> mutatedStmt;	 // TODO:: change to std::set
map<Instruction*, int> instToExprID;
vector<Instruction *> implementedList;

ofstream SMAInstrumenter::expr_os;
ofstream SMAInstrumenter::mut_os;


void dumpToStmtMap(Function &F);

void SMAInstrumenter::outputMutation(Instruction *inst, vector<Mutation*>* v){
	int index = 0;
	Function *F = inst->getParent()->getParent();

	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I, index++) {
		if((&*I) == inst){
			break;
		}
	}
	
	for(vector<Mutation*>::iterator it = v->begin(), end = v->end(); it != end; it++){
		if( (*it)->index == index){
			errs()<<"\tEID: "<<instToExprID[inst]<<"\tMID: "<<(*it)->id<<"\tTP: "<<(*it)->type
				<<"\tS_OP: "<<(*it)->src_op<<"\n";
			stringstream ss;
			ss<<instToExprID[inst]<<":"<<(*it)->id<<":"<<(*it)->type<<":"<<(*it)->src_op<<":";
			if(RORMut *ror = dyn_cast<RORMut>(*it)){
				ss<<ror->src_pre<<":"<<ror->tar_pre;
			}else if(LVRMut *lvr = dyn_cast<LVRMut>(*it)){
				ss<<lvr->oper_index<<":"<<lvr->src_const<<":"<<lvr->tar_const;
			}else if(AORMut *aor = dyn_cast<AORMut>(*it)){
				ss<<aor->tar_op;
			}else if(LORMut *lor = dyn_cast<LORMut>(*it)){
				ss<<lor->tar_op;
			}else{
				errs()<<"ERRS @ outputMutation !!! \n";
				exit(0);
			}
			ss<<'\n';
			mut_os<<ss.str();
		}else if((*it)->index > index){// TODO: check
			break;
		}
	}
	mut_os.flush();
}

void SMAInstrumenter::outputExpression(Instruction* inst){
	
	int left = 0, right = 0;
	Instruction * inst0 = dyn_cast<Instruction> (inst->getOperand(0));
	Instruction * inst1 = dyn_cast<Instruction> (inst->getOperand(1));
	if(instToExprID.find(inst0) != instToExprID.end()) {
		left = instToExprID[inst0];
	} 
	if(instToExprID.find(inst1) != instToExprID.end()) {
		right = instToExprID[inst1];
	}
	errs()<<*inst<<"\t"<<instToExprID[inst]<<"\t====>\t"<<left<<"\t"<<right<<"\n";
	stringstream ss;
	ss<<instToExprID[inst]<<" ";
	if(inst->getOpcode() == Instruction::ICmp){
		ICmpInst *cmp = dyn_cast<ICmpInst>(inst);
		ss<<'c'<<" "<<cmp->getPredicate()<<" ";
	}else{
		ss<<'a'<<" "<<inst->getOpcode()<<" ";
	}
	ss<<left<<" "<<right<<'\n';
	expr_os<<ss.str();
	expr_os.flush();
}

SMAInstrumenter::SMAInstrumenter(Module *M) : FunctionPass(ID) {
	this->TheModule = M;
	MutUtil::getAllMutations();
	string home = getenv("HOME");
	stringstream ss;
	ss<<home<<"/tmp/accmut/expr.txt"; 
	expr_os.open(ss.str(), ios::trunc); 
	stringstream ss2;
	ss2<<home<<"/tmp/accmut/mut.txt"; 
	mut_os.open(ss2.str(), ios::trunc); 
}

#if ACCMUT_STATIC_ANALYSIS_INSTRUMENT_EVAL
bool SMAInstrumenter::runOnFunction(Function & F){
	if(F.getName().startswith("__accmut__")){
		return false;
	}
	if(F.getName().equals("main")){
		#if 0
		Function* finit = TheModule->getFunction("__accmut__sma_init");
		if (!finit) {
			std::vector<Type*>Fty_args;
				FunctionType* Fty = FunctionType::get(Type::getVoidTy(TheModule->getContext()),
													Fty_args,true);
			finit = Function::Create(
			 	Fty,	//Type
				 GlobalValue::ExternalLinkage,	//Linkage
				 "__accmut__sma_init",	//Name
				 TheModule); 
			finit->setCallingConv(CallingConv::C);
		}

		CallInst* call_init = CallInst::Create(finit, "", F.getEntryBlock().begin());
		call_init->setCallingConv(CallingConv::C);
		call_init->setTailCall(false);
		AttributeSet call_init_PAL;
		call_init->setAttributes(call_init_PAL);
		#endif
		return true;
	}

	dumpToStmtMap(F);

	// TODO:

	vector<Mutation*>* v= MutUtil::AllMutsMap[F.getName()];
	
	if(v == NULL || v->size() == 0){
		return false;
	}

	errs()<<"\n######## SMA INSTRUMTNTING EVAL  @"<<F.getName()<<"  ########\n\n";

	unsigned already = implementedList.size();
	
	for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
		Instruction* inst = &(*I);
		if(needInstrument(inst, v)){
			errs()<<"NEED INSTRU  "<<*inst<<'\n';
			implementedList.push_back(inst);
			instToExprID[inst] = implementedList.size();
			outputExpression(inst);
			outputMutation(inst, v);
		}	
	}
	
  	for(unsigned i = already; i < implementedList.size(); ++i) {
		Instruction * inst = implementedList[i];
		
		Function* func_eval = TheModule->getFunction("__accmut__eval_i32");
		std::vector<Value*> paras;
		stringstream ss;
		ss<<(i+1);
		ConstantInt* CurExprValue = ConstantInt::get(TheModule->getContext(), APInt(32, StringRef(ss.str()), 10));
		paras.push_back(CurExprValue);
    		paras.push_back(inst->getOperand(0));
    		paras.push_back(inst->getOperand(1));
		if(inst->getOpcode() == Instruction::ICmp) {
			CallInst* call = CallInst::Create(func_eval, paras, "", inst);
			CastInst* i32_to_i1 = new TruncInst(call, IntegerType::get(TheModule->getContext(), 1), "");
			ReplaceInstWithInst(inst, i32_to_i1);
		}else{
			CallInst* call = CallInst::Create(func_eval, paras, "");
			ReplaceInstWithInst(inst, call);
		}
	}

	return true;
}

#elif ACCMUT_STATIC_ANALYSIS_INSTRUEMENT_MUT

int getTypeMacro(Type *t);

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
					llvm::errs()<<"ERR STORE TYPE @ "<<__FUNCTION__<<" : "<<__LINE__<<"\n";
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
					llvm::errs()<<"NOT A POINTER @ "<<__FUNCTION__<<" : "<<__LINE__<<"\n";
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

bool SMAInstrumenter::runOnFunction(Function & F){
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

	errs()<<"\n######## SMA INSTRUMTNTING MUT  @"<<TheModule->getName()<<"->"<<F.getName()<<"()  ########\n\n";	

	instrument(F, v);
	//test(F);

	return true;
}
#endif

//TYPE BITS OF SIGNATURE
#define CHAR_TP 0
#define SHORT_TP 1
#define INT_TP 2
#define LONG_TP 3

#if 0
#define CHAR_PTR_TP 4
#define SHORT_PTR_TP 5
#define INT_PTR_TP 6
#define LONG_PTR_TP 7
#endif

int getTypeMacro(Type *t){
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
				llvm::errs()<<"TYPE ERROR @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
				exit(0);
		}
	}
	
	#if 0
	else if(t->isPointerTy() && t->getPointerElementType()->isIntegerTy()){
		unsigned width = t->getPointerElementType()->getIntegerBitWidth();
		switch(width){
			case 8:	
				res = CHAR_PTR_TP;
				break;
			case 16:
				res = SHORT_PTR_TP;
				break;
			case 32:
				res = INT_PTR_TP;
				break;
			case 64:
				res = LONG_PTR_TP;
				break;
			default:
				llvm::errs()<<"TYPE ERROR @ "<<__FUNCTION__<<" : "<<__LINE__<<"\n";
				exit(0);
		}					
	}
	#endif
	
	return res;
}

void SMAInstrumenter::instrument(Function &F, vector<Mutation*> * v){

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

/*		for(auto I = tmp.begin(); I != tmp.end(); I++){
			(*I)->dump();
		}*/
		
		cur_it =  getLocation(F, instrumented_insts, tmp[0]->index);
		
		cur_bb = cur_it->getParent();

		int mut_from, mut_to;
		mut_from = tmp.front()->id;
		mut_to = tmp.back()->id;

		llvm::errs()<<"CURRENT_INST (FROM: "
			<<mut_from<<"\tTO: "<<mut_to<<")\t"<<*cur_it<<"\n";
		
		if(dyn_cast<CallInst>(&*cur_it)){
			//move all constant literal int to repalce to alloca
			for (auto OI = cur_it->op_begin(), OE = cur_it->op_end(); OI != OE; ++OI){
				if(ConstantInt* cons = dyn_cast<ConstantInt>(&*OI)){
					AllocaInst *alloca = new AllocaInst(cons->getType(), "cons_alias", cur_it);
					StoreInst *str = new StoreInst(cons, alloca, cur_it);
					LoadInst *ld = new LoadInst(alloca, "const_load", cur_it);
					*OI = (Value*) ld;
					instrumented_insts += 3;//add 'alloca', 'store' and 'load'
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
				//now push the pointer of idx'th param
				if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){//is a local var
					params.push_back(ld->getPointerOperand());
				}else if(AllocaInst *alloca = dyn_cast<AllocaInst>(&*OI)){// a param of the F, fetch it by alloca
					params.push_back(alloca);
				}else{
					llvm::errs()<<"NOT A POINTER @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
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
			pre->setCallingConv(CallingConv::C);
			pre->setTailCall(false);
			AttributeSet preattrset;
			pre->setAttributes(preattrset);
			
			ConstantInt* zero = ConstantInt::get(Type::getInt32Ty(TheModule->getContext()), 0);
			
			ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_EQ, pre, zero, "hasstd");
			
			BasicBlock *cur_bb = cur_it->getParent();
			
			Instruction* oricall = cur_it->clone();
			
			BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "if.end");
			
			BasicBlock* label_if_then = BasicBlock::Create(TheModule->getContext(), "if.then",cur_bb->getParent(), label_if_end);
			BasicBlock* label_if_else = BasicBlock::Create(TheModule->getContext(), "if.else",cur_bb->getParent(), label_if_end);
			
			cur_bb->back().eraseFromParent();
			
			BranchInst::Create(label_if_then, label_if_else, hasstd, cur_bb);
				
			//label_if_then
			//move the loadinsts of params into if_then_block
			for (auto OI = oricall->op_begin(), OE = oricall->op_end() - 1; OI != OE; ++OI){
				if(LoadInst *ld = dyn_cast<LoadInst>(&*OI)){
					ld->removeFromParent();
					label_if_then->getInstList().push_back(ld);
				}else{
					llvm::errs()<<"NOT A LoadInst @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
					exit(0);
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
				llvm::errs()<<"ERR CALL TYPE @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
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
				cur_it->eraseFromParent();
				instrumented_insts += 6;
			}
			else{
				PHINode* call_res = PHINode::Create(IntegerType::get(TheModule->getContext(), 32), 2, "call.phi");
				call_res->addIncoming(oricall, label_if_then);
				call_res->addIncoming(stdcall, label_if_else);
				ReplaceInstWithInst(cur_it, call_res);
				instrumented_insts += 7;
			}

		}
		
		else if(StoreInst* st = dyn_cast<StoreInst>(&*cur_it)){
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
				llvm::errs()<<"ERR STORE TYPE @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
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
				params.push_back(ld->getPointerOperand());
			}else if(AllocaInst *alloca = dyn_cast<AllocaInst>(&*addr)){
				params.push_back(alloca);
			}else{
				llvm::errs()<<"NOT A POINTER @ "<<__FUNCTION__<<"() : "<<__LINE__<<"\n";
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
		
			ICmpInst *hasstd = new ICmpInst(cur_it, ICmpInst::ICMP_EQ, pre, zero, "hasstd");
		
			BasicBlock *cur_bb = cur_it->getParent();
						
			BasicBlock* label_if_end = cur_bb->splitBasicBlock(cur_it, "if.end");
			
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
					llvm::errs()<<"TYPE ERROR @ instrument() ArithInst\n";
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
					llvm::errs()<<"TYPE ERROR @ instrument() ICmpInst\n";			
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

#if 0
void SMAInstrumenter::instrument(Function &F, vector<Mutation*> * v){
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

		errs()<<"MUT "<<tmp[0]->id<<" ~ "<<tmp[tmp.size()-1]->id<<
			" @ "<<tmp[0]->index<<"\t"<<*cur_it<<"\n";
			
		cur_bb = cur_it->getParent();
		
		stringstream name;
		name<<"FINAL_BB_OF_INST_"<<tmp[0]->index;

		BasicBlock *FINAL_END = cur_bb->splitBasicBlock(cur_it, name.str());

		vector<BasicBlock*> Mut_BBs;

		//create each BB from tmp[0] 
		name.str("");
		name<<"M_BB_"<< tmp[0]->id;
		Mut_BBs.push_back( BasicBlock::Create(F.getContext(),  name.str(), &F, FINAL_END) );	 
		for(unsigned j = 1; j < tmp.size() ; j++){
			name.str("");
			name<<"M_BB_"<< tmp[j]->id;
			Mut_BBs.push_back( BasicBlock::Create(F.getContext(),  name.str(), &F, FINAL_END ) );	 
		}
///
		name.str("");
		name<<"ORI_OF_INST_"<< tmp[0]->index;
		BasicBlock *ORI_BB = BasicBlock::Create(F.getContext(), name.str(), &F, FINAL_END );

		//modify the BEGINNING BB		
		Value *M_ID = TheModule->getGlobalVariable("MUTATION_ID");
				
		LoadInst *Load_M_ID = new LoadInst(M_ID, "m_id.gv", cur_bb->getTerminator());
				
		cur_bb->back().eraseFromParent();
		
		//add switch
		SwitchInst *SelectMuts =  SwitchInst::Create(Load_M_ID, ORI_BB, tmp.size(), cur_bb);
		for(unsigned j = 0; j < Mut_BBs.size(); j++){
			ConstantInt* Mut_ID_Const = ConstantInt::get(Type::getInt32Ty(F.getContext()), tmp[j]->id , true);
			SelectMuts->addCase(Mut_ID_Const, Mut_BBs[j]);
		}

		instrumented_insts += 2; //

		if(cur_it->getType()->isVoidTy()){
			
			for(unsigned j = 0; j < tmp.size(); j++){
				Mutation *m1 = tmp[j];
				if(LVRMut *lvr = dyn_cast<LVRMut>(m1)){
					name.str("");
					name<<"lvr_mut_"<<lvr->id;
					Value *cons_change = ConstantInt::get( cur_it->getOperand(lvr->oper_index)->getType() , lvr->tar_const); 
					Instruction *mut_inst = cur_it->clone();	
					//mut_inst->setName(name.str()); 	//BUG HERE, can not set name to void type !!
					mut_inst->setOperand(lvr->oper_index, cons_change);
					Mut_BBs[j]->getInstList().push_front(mut_inst);
					BranchInst::Create(FINAL_END, Mut_BBs[j]);				
					instrumented_insts += 2;
				}else if(STDMut* std = dyn_cast<STDMut>(m1)){	
					Function *f;
					f = TheModule->getFunction("__accmut__process_call_void");	
					vector<Value*> call_params;
					CallInst* call = CallInst::Create(f, call_params, "", Mut_BBs[j]);
					call->setCallingConv(CallingConv::C);
					call->setTailCall(false);
					AttributeSet attr;			
					call->setAttributes(attr);		
					BranchInst::Create(FINAL_END, Mut_BBs[j]);
					instrumented_insts += 2;
				}else{
					errs()<<"MUTATION TYPE ERR @ SMAInstrumenter::instrument() \n";
					exit(0);
				}							
			}
			//instrument origin bb
			Instruction *ori_inst = cur_it->clone();	
			name.str("");
			name<<"ori_inst";		
			//ori_inst->setName(name.str());  //BUG HERE, can not set name to void type !!
			
			ORI_BB->getInstList().push_front(ori_inst);		// TODO:: why needn't setParent() ??		
			BranchInst::Create(FINAL_END, ORI_BB);
			instrumented_insts += 2;
			
		}else{
		
			//create phi in final BB
			PHINode *phi = PHINode::Create(cur_it->getType(), tmp.size()+1, "mut_phi");// TODO:: STD for void 

			//instrument each mutation BB		
			for(unsigned j = 0; j < tmp.size(); j++){
				Mutation *m1 = tmp[j];

				if(AORMut* aor = dyn_cast<AORMut>(m1)){
					Instruction::BinaryOps opcode = Instruction::BinaryOps(aor->tar_op) ;	
					name.str("");
					name<<"aor_mut_"<<aor->id;				
					Value *mut_inst = BinaryOperator::Create(opcode, cur_it->getOperand(0), cur_it->getOperand(1), name.str(), Mut_BBs[j]);
					phi->addIncoming(mut_inst, Mut_BBs[j]); 	// TODO::check the upper limit of phinode
					BranchInst::Create(FINAL_END, Mut_BBs[j]);
					instrumented_insts += 2;				
				}else if(RORMut* ror = dyn_cast<RORMut>(m1)){
					name.str("");
					name<<"ror_mut_"<<ror->id;		
					CmpInst::Predicate predicate = CmpInst::Predicate(ror->tar_pre);				
					Value *mut_icmp = new ICmpInst(*Mut_BBs[j], predicate, cur_it->getOperand(0), cur_it->getOperand(1), name.str());
					phi->addIncoming(mut_icmp, Mut_BBs[j]); 
					BranchInst::Create(FINAL_END, Mut_BBs[j]);
					instrumented_insts += 2;
				}else if(LORMut* lor = dyn_cast<LORMut>(m1)){
					Instruction::BinaryOps opcode = Instruction::BinaryOps(lor->tar_op) ;	
					name.str("");
					name<<"lor_mut_"<<lor->id;				
					Value *mut_inst = BinaryOperator::Create(opcode, cur_it->getOperand(0), cur_it->getOperand(1), name.str(), Mut_BBs[j]);
					phi->addIncoming(mut_inst, Mut_BBs[j]); 	// TODO::check the upper limit of phinode
					BranchInst::Create(FINAL_END, Mut_BBs[j]);
					instrumented_insts += 2;						
				}else if(LVRMut *lvr = dyn_cast<LVRMut>(m1)){
					name.str("");
					name<<"lvr_mut_"<<lvr->id;					
					Value *cons_change = ConstantInt::get( cur_it->getOperand(lvr->oper_index)->getType() , lvr->tar_const); 
					Instruction *mut_inst = cur_it->clone();	
					mut_inst->setName(name.str());
					mut_inst->setOperand(lvr->oper_index, cons_change);
					Mut_BBs[j]->getInstList().push_front(mut_inst);
					phi->addIncoming(mut_inst, Mut_BBs[j]); 
					BranchInst::Create(FINAL_END, Mut_BBs[j]);
					instrumented_insts += 2;
				}else if(STDMut* std = dyn_cast<STDMut>(m1)){
					name.str("");
					name<<"std_mut_"<<std->id;		
					Function *f;
					Type* ori_ty = cur_it->getType();
					if(ori_ty->isIntegerTy(32)){
						f = TheModule->getFunction("__accmut__process_call_i32");	
						vector<Value*> call_params;
						CallInst* call = CallInst::Create(f, call_params, name.str(), Mut_BBs[j]);
						call->setCallingConv(CallingConv::C);
						call->setTailCall(false);
						AttributeSet attr;			
						call->setAttributes(attr);
						phi->addIncoming(call, Mut_BBs[j]); 	// TODO::check the upper limit of phinode
						BranchInst::Create(FINAL_END, Mut_BBs[j]);
						instrumented_insts += 2;		
					}else if(ori_ty->isIntegerTy(64)){
						f = TheModule->getFunction("__accmut__process_call_i64");
						vector<Value*> call_params;					
						CallInst* call = CallInst::Create(f, call_params, name.str(), Mut_BBs[j]);
						call->setCallingConv(CallingConv::C);
						call->setTailCall(false);
						AttributeSet attr;			
						call->setAttributes(attr);		
						phi->addIncoming(call, Mut_BBs[j]); 	// TODO::check the upper limit of phinode
						BranchInst::Create(FINAL_END, Mut_BBs[j]);
						instrumented_insts += 2;	
					}else{
						llvm::errs()<<"TYPE ERROR @ SMAInstrumenter::instrument() CallInst\n";			
						exit(0);
					}			
				}else{
					errs()<<"MUTATION TYPE ERR @ SMAInstrumenter::instrument() \n";
					exit(0);
				}			
			}

			//instrument origin bb
			Instruction *ori_inst = cur_it->clone();	
			name.str("");
			name<<"ori_inst";		
			ori_inst->setName(name.str());
			
			phi->addIncoming(ori_inst, ORI_BB);
			ORI_BB->getInstList().push_front(ori_inst);		// TODO:: why needn't setParent() ??		
			BranchInst::Create(FINAL_END, ORI_BB);
			instrumented_insts += 2;

			//modify the final BB
			ReplaceInstWithInst(cur_it, phi);
		}//end of if( cur_it->getType()->isVoidType() ) else
	}
	
}
#endif

BasicBlock::iterator SMAInstrumenter::getLocation(Function &F, int instrumented_insts, int index){
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

bool SMAInstrumenter::hasMutation(Instruction *inst, vector<Mutation*>* v){
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

bool SMAInstrumenter::needInstrument(Instruction *I, vector<Mutation*>* v){
	// TODO:: only implement instuctions within i32 type now. 
	for(unsigned i = 0; i < I->getNumOperands(); i++){//only mutation for instructions with i32 or i64 operands
		Type* t = I->getOperand(i)->getType();
		if( !t->isIntegerTy(32) ){
			return false;
		}
	}
	
	if(dyn_cast<BinaryOperator>(I) ){
		if(hasMutation(I, v)){
			Stmt* curStmt = toStmtMap[I];
			mutatedStmt[curStmt] = true;
			return true;
		}
		Stmt* curStmt = toStmtMap[I];
		if(mutatedStmt[curStmt]){
			return true;
		}
	}else if(dyn_cast<ICmpInst>(I)){
		// do not support pointer type's icmp, e.g. " fp == NULL " 
		if(I->getOperand(0)->getType()->isIntegerTy() && I->getOperand(1)->getType()->isIntegerTy()){
			if(hasMutation(I, v)){
				Stmt* curStmt = toStmtMap[I];
				mutatedStmt[curStmt] = true;
				return true;
			}
			Stmt* curStmt = toStmtMap[I];
			if(mutatedStmt[curStmt]){
				return true;
			}
		}
	}

	/*
	if(dyn_cast<StoreInst>(I)){// TODO:: store ?? e.g. 'b = a+b/c'
		
	}
	if(dyn_cast<CallInst>(I)){// TODO:: CallInst
		
	}*/

	return false;
}


void dumpToStmtMap(Function &F){

	errs()<<"\n\n  ############  FUNCTION : "<<F.getName()<<"  ########\n\n\n";

/*	if(F.getName() != "get_token"){
		return;
	}*/

	int index = 0;
	
	for(Function::iterator FI = F.begin(); FI != F.end(); ++FI){
		BasicBlock *BB = FI;
		for(BasicBlock::iterator BI = BB->begin(); BI != BB->end(); ++BI, index++){
			if(index != 82)	
				continue;
			errs()<<'\n';
			errs()<<*(BI)<<"  ----> \n\n";
			toStmtMap[&(*BI)]->dump();
			errs()<<"\n\n";
		}
	}
}

/*------------------reserved begin-------------------*/
void SMAInstrumenter::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char SMAInstrumenter::ID = 0;
/*-----------------reserved end --------------------*/

