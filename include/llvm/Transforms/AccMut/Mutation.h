//===----------------------------------------------------------------------===//
//
 // This file defines the data structure of mutations
// 
 // Add by Wang Bo. OCT 19, 2015
//
//===----------------------------------------------------------------------===//


#ifndef ACCMUT_MUTATION_H
#define ACCMUT_MUTATION_H

#include "llvm/Support/Casting.h"

#include "llvm/Transforms/AccMut/Config.h"

#include<string>
#include<iostream>
#include<fstream> 
#include<vector>

class Mutation{
public:
	enum MutationKind{
		MK_AOR,
		MK_LOR,
		MK_COR,
		MK_ROR,
		MK_SOR,
		MK_STD,
		MK_LVR,
		MK_UOI,
		MK_ROV,
		MK_ABV
	};
public:	
	int id;
	std::string func; 
	int index;
	std::string type;
	int src_op;
private:
	MutationKind Kind;
public:
	MutationKind getKind() const{ return Kind;}
	Mutation(){}
	Mutation(MutationKind K) : Kind(K){}
    void dump(){
        llvm::errs()<<"\tID: "<<this->id<<"\tTYPE: "<<this->type<<" \tFUNC: "<<this->func
				<<"\tINDEX: "<<this->index<<"\tS_OP: "<<this->src_op<<'\n';
    }
};

//AOR
//Arithmetic operator replacement : a + b ---> a - b
class AORMut : public Mutation{
public:
	int tar_op;
	AORMut() : Mutation(MK_AOR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_AOR;
	}	
};


//LOR
//Logic Operator Replacement : a and b ---> a or b, a >> b ---> a << b
class LORMut : public Mutation{
public:
	int tar_op;
	LORMut() : Mutation(MK_LOR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_LOR;
	}		
};

//COR
//Conditional Operator Replacement, e.g. a || b ---> a && b, which is reduced into ROR and LOR
class CORMut : public Mutation{
public:
	CORMut() : Mutation(MK_COR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_COR;
	}
};

//ROR
//Relational Operator Replacement : (a == b) ---> (a >= b)
class RORMut : public Mutation{
public:
	int src_pre;
	int tar_pre;
	RORMut() : Mutation(MK_ROR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_ROR;
	}	
};

//SOR
//Shift Operator Replacement, a >> b ---> a << b, is included by LOR
class SORMut : public Mutation{
public:
	SORMut() : Mutation(MK_SOR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_SOR;
	}		
};

//STD
//Statement Deletion Operator: Delete (omit) a single statement, foo(a,b) ---> <no-op>
class STDMut : public Mutation{
public:
	int func_ty;
	STDMut() : Mutation(MK_STD){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_STD;
	}		
};

//Literal Value Replacement : 0 ---> 1
class LVRMut : public Mutation{
public:
	int oper_index;
	int src_const;
	int tar_const;
	LVRMut() : Mutation(MK_LVR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_LVR;
	}		
};

//Unary Operation Insertion: b = a; ---> a++; b = a;
class UOIMut : public Mutation{
public:
	int oper_index;
	int ury_tp; //inc:0, dec:1 or negation:2
	UOIMut() : Mutation(MK_UOI){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_UOI;
	}		
};

//Relacement of Operation Value: foo(a, b) ---> foo(b, a)
class ROVMut : public Mutation{
public:
    int op1;
    int op2;
    ROVMut() : Mutation(MK_ROV){}
    static bool classof(const Mutation *M) {
        return M->getKind() == MK_ROV;
    }       
};

//Relace a Value to its Absolute Value: a + b ---> abs(a) + b
class ABVMut : public Mutation{
public:
    int oper_index;
    ABVMut() : Mutation(MK_ABV){}
    static bool classof(const Mutation *M) {
        return M->getKind() == MK_ABV;
    }       
};

#endif
