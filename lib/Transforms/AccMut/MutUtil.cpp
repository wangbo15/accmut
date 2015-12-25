
#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/AccMut/MutUtil.h"

#include<fstream>
#include<sstream>


using namespace llvm;
using namespace std;

bool MutUtil::allMutsGeted = false;
map<string, vector<Mutation*>*> MutUtil::AllMutsMap;


void MutUtil::dumpAllMuts(){
	map<string, vector<Mutation*>*>::iterator it;
	errs()<<"--------- DUMP ALL MUTATIONS ----------\n";
	for( it = AllMutsMap.begin(); it != AllMutsMap.end(); it++){
		errs()<<"FUNCTION : "<<it->first<<'\n';
		vector<Mutation*>* v = it->second;
		vector<Mutation*>::iterator vit;
		for(vit = v->begin(); vit != v->end(); vit++){
			Mutation *m = (*vit);
			errs()<<"\tID: "<<m->id<<"\tTYPE: "<<m->type<<" \tFUNC: "<<m->func
				<<"\tINDEX: "<<m->index<<'\n';
		}
	}
	errs()<<"--------------- END DUMP -----------------\n";
}	

void MutUtil::getAllMutations(){
	if(allMutsGeted){
		return;
	}
	string buf;
	string path = getenv("HOME");
	path += "/tmp/accmut/mutations.txt";
	
	std::ifstream  fin(path, ios::in); 
	
	if(!fin.is_open()){
		errs()<<"FILE ERROR : mutations.txt @ "<<path<<"\n";
		exit(-1);
	}
	
	while( fin>>buf) { 
		Mutation *m = getMutation(buf);
		//AllMutsVector.push_back(m);
		if(AllMutsMap.count(m->func) == 0){
			AllMutsMap[m->func] = new vector<Mutation*>();
		}
		AllMutsMap[m->func]->push_back(m);
	}
	fin.close();
	allMutsGeted = true;
	dumpAllMuts();
}

Mutation *MutUtil::getMutation(string line){
	stringstream ss;
	ss<<line;
	int id;
	string mtype, func;
	int index;
	ss>>id;
	char colon;
	ss>>colon;
	getline(ss, mtype, ':');
	getline(ss, func, ':');
	ss>>index;
	ss>>colon;
	
	Mutation *m;
	if(mtype == "AOR"){
		AORMut *aor = new AORMut();
		int sop, top;
		ss>>sop;
		ss>>colon;
		ss>>top;
		aor->src_op = sop;
		aor->tar_op = top;
		m = dyn_cast<Mutation>(aor);
	}else if(mtype == "LOR"){
		LORMut *lor = new LORMut();
		int sop, top;
		ss>>sop;
		ss>>colon;
		ss>>top;
		lor->src_op = sop;
		lor->tar_op = top;
		m = dyn_cast<Mutation>(lor);
	}else if(mtype == "COR"){

	}else if(mtype == "ROR"){
		RORMut *ror = new RORMut();
		int op, sp, tp;
		ss>>op;
		ss>>colon;
		ss>>sp;
		ss>>colon;
		ss>>tp;
		ror->op = op;
		ror->src_pre = sp;
		ror->tar_pre = tp;
		m = dyn_cast<Mutation>(ror);
	}else if(mtype == "SOR"){
		
	}else if(mtype == "STD"){
		STDMut *std = new STDMut();
		int op, type;
		ss>>op;
		ss>>colon;
		ss>>type;
		std->op = op;
		std->func_ty = type;
		m = dyn_cast<Mutation>(std);
	}else if(mtype == "LVR"){
		LVRMut *lvr = new LVRMut();
		int op, oi, sc, tc;
		ss>>op;
		ss>>colon;		
		ss>>oi;
		ss>>colon;
		ss>>sc;
		ss>>colon;
		ss>>tc;
		lvr->op = op;
		lvr->oper_index = oi;
		lvr->src_const = sc;
		lvr->tar_const = tc;
		m = dyn_cast<Mutation>(lvr);
	}else{
		errs()<<"WRONG MUT TYPE !\n";
		exit(-1);
	}
	m->id = id;
	m->type = mtype;
	m->func = func;
	m->index = index;
	return m;
}

