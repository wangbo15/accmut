typedef enum MTYPE{
	AOR, /* 0 */
	LOR, /* 1 */
	COR, /* 2 */
	ROR, /* 3 */
	SOR, /* 4 */
	STD, /* 5 */
	LVR, /* 6 */
	UOI, /* 7 */
	ROV, /* 8 */
	ABV  /* 9 */
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
