#ifndef ACCMUT_CONFIG_H
#define ACCMUT_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

#include <unistd.h>

#include <stdarg.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>

#include "accmut_async_sig_safe_string.h"

#define ACCMUT_ORI_TEST 0
//SWITCH FOR MUTATION SCHEMATA
#define ACCMUT_MUTATION_SCHEMATA 1
//SWITCH FOR STATIC ANALYSIS
#define ACCMUT_STATIC_ANALYSIS_EVAL 0

#define ACCMUT_STATIC_ANALYSIS_FORK_CALL 0

//SWITCH FOR DYNAMIC ANALYSIS
#define ACCMUT_DYNAMIC_ANALYSIS_FORK 0

#define MAXMUTNUM 0x16000


extern const char PROJECT[];

extern int HOLDER[1024];
#define MUTATION_ID (HOLDER[0])

//#if ACCMUT_DYNAMIC_ANALYSIS_FORK
//	extern int HOLDER[1024] __attribute__((aligned(0x1000)));
//	#define MUTATION_ID (HOLDER[0])
//#else
//	extern int MUTATION_ID;
//#endif

extern int TEST_ID;

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

	#if ACCMUT_STATIC_ANALYSIS_EVAL	
	int location;
	#endif

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


#define MUTFILELINE 128

extern Mutation* ALLMUTS[MAXMUTNUM + 1];
extern int MUT_NUM;

extern struct timeval tv_begin, tv_end;

// #ifndef USING_LIB
// 	#define __real_fprintf fprintf
// #else
// 	int __real_fprintf(FILE *fp, const char *format, ...);
// #endif

// #define ERRMSG(msg) __real_fprintf(stderr, "%s @ %s->%s():%d\tMID: %d\tTID: %d\n", \
// 	msg,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)

// #define ERRMSG2(msg,i) __real_fprintf(stderr, "%s : %d @ %s->%s():%d\tMID: %d\tTID: %d\n", \
// 	msg, i,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)

int __real_fprintf(FILE *fp, const char *format, ...);

#define ERRMSG(msg) __real_fprintf(stderr, "%s @ %s->%s():%d\tMID: %d\tTID: %d\n", \
	msg,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)

#define ERRMSG2(msg,i) __real_fprintf(stderr, "%s : %d @ %s->%s():%d\tMID: %d\tTID: %d\n", \
	msg, i,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)


/************* ALL EXIT HANDLER ***************************/
void __accmut__exit_check_output();

void __accmut__exit_time();

/* The signal handler of time out process */
//void __accmut__handler(int sig);

void __accmut__set_sig_handlers();

void __accmut__sepcific_timer();

void __accmut__exec_inst_nums();

void __accmut__debug(int index);

void __accmut__load_all_muts();



#endif
