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
	if(F.getName().equals("main")){/*
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
		call_init->setAttributes(call_init_PAL);*/
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

	errs()<<"######## SMA INSTRUMTNTING MUT  @"<<F.getName()<<"  ########\n";	

	instrument(F, v);

	return true;
}
#endif


void SMAInstrumenter::instrument(Function &F, vector<Mutation*> * v){
	int instrumented_insts = 0;
	
	Function::iterator cur_bb;
	BasicBlock::iterator cur_it;
	
	for(unsigned i = 0; i < v->size(); i++){		
		vector<Mutation*> tmp;
		Mutation* m = (*v)[i];
		tmp.push_back(m);		// TODO: remenber to free
		
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
			ConstantInt* Mut_ID_Const = ConstantInt::get(Type::getInt64Ty(F.getContext()), tmp[j]->id , true);
			SelectMuts->addCase(Mut_ID_Const, Mut_BBs[j]);
		}

		instrumented_insts += 2; //

		//create phi in final BB
		PHINode *phi = PHINode::Create(cur_it->getType(), tmp.size()+1, "mut_phi");
			
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
				}else if(ori_ty->isVoidTy()){
					f = TheModule->getFunction("__accmut__process_call_void");	
					vector<Value*> call_params;
					CallInst* call = CallInst::Create(f, call_params, name.str(), Mut_BBs[j]);
					call->setCallingConv(CallingConv::C);
					call->setTailCall(false);
					AttributeSet attr;			
					call->setAttributes(attr);		
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

	}
}


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

	if(F.getName() != "get_token"){
		return;
	}

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

