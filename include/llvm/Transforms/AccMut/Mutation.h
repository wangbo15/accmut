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
		MK_LVR
	};
public:	
	int id;
	std::string func; 
	int index;
	std::string type;
private:
	MutationKind Kind;
public:
	MutationKind getKind() const{ return Kind;}
	Mutation(){}
	Mutation(MutationKind K) : Kind(K){}
};

//AOR
//Arithmetic operator replacement : a + b ---> a - b
class AORMut : public Mutation{
public:
	int src_op;
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
	int src_op;
	int tar_op;
	LORMut() : Mutation(MK_LOR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_LOR;
	}		
};

//COR
//Conditional Operator Replacement, e.g. a || b ---> a && b, which is reduced into ROR
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
	int op;
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
	int op;
	int func_ty;
	STDMut() : Mutation(MK_STD){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_STD;
	}		
};

//Literal Value Replacement : 0 ---> 1
class LVRMut : public Mutation{
public:
	int op;
	int oper_index;
	int src_const;
	int tar_const;
	LVRMut() : Mutation(MK_LVR){}
	static bool classof(const Mutation *M) {
		return M->getKind() == MK_LVR;
	}		
};


#endif
