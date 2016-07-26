typedef enum MTYPE{
	AOR,
	LOR,
	COR,
	ROR,
	SOR,
	STD,
	LVR,
	UOI,
	ROV,
	ABV
}MType;

typedef struct Mutation{
	MType type;

	//src operand, for all muts
	int sop;

	//AOR,LOR->t_op
	//LVR,ABV->index; 
	int op_0;	

	//ROR->s_pre & t_pre
	//LVR->src_const & tar_const
	//ROV->op1 & op2
	//STD->func_type & retval
	//UOI->idx & tp
	long op_1;
	long op_2;
}Mutation;