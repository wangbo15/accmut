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


//SWITCH FOR MUTATION SCHEMATA
#define ACCMUT_MUTATION_SCHEMATA 0
//SWITCH FOR STATIC ANALYSIS
#define ACCMUT_STATIC_ANALYSIS_EVAL 0
#define ACCMUT_STATIC_ANALYSIS_FORK_INLINE 0
#define ACCMUT_STATIC_ANALYSIS_FORK_CALL 1

//SWITCH FOR DYNAMIC ANALYSIS
#define ACCMUT_DYNAMIC_ANALYSIS_FORK 0


//for DMA FORK
#define MAXMUTNUM 32768

#define PAGESIZE 4096


const char PROJECT[]="test";


#if ACCMUT_DYNAMIC_ANALYSIS_FORK
	int HOLDER[1024] __attribute__((aligned(0x1000))) = {0};
	#define MUTATION_ID (HOLDER[0])
#else
	int MUTATION_ID = 0;
#endif

int TEST_ID = -1;

/** Set Timer **/
//TODO: REAL OR PROF
struct itimerval tick;
long VALUE_SEC = 0;
long VALUE_USEC = 0;
long INTTERVAL_SEC = 0;
long INTTERVAL_USEC = 5;

struct timeval tv_begin, tv_end;
struct rusage usage_fbegin, usage_fmid, usage_fend;
struct rusage usage_cbegin, usage_cend;

/**********************************************************/
//FOR STATISTICS
unsigned long long EXEC_INSTS = 0;
/**********************************************************/

#define MUTFILELINE 128

#include "accmut_mut.h"

#include "accmut_io.h"

#include "accmut_async_sig_safe_string.h"

Mutation* ALLMUTS[MAXMUTNUM + 1];
int MUT_NUM;
int *MUTS_ON;

#define MAX_PARAM_NUM 16 

/****************** PREPARE CALL **************************/
//TYPE BITS OF SIGNATURE
#define CHAR_TP 0
#define SHORT_TP 1
#define INT_TP 2
#define LONG_TP 3

typedef struct PrepareCallParam{
	int type;
	unsigned long address;
}PrepareCallParam;
/**********************************************************/


#define ERRMSG(msg) fprintf(stderr, "%s @ %s->%s():%d\tMID: %d\tTID: %d\n", \
	msg,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)

/************* ALL EXIT HANDLER ***************************/
void __accmut__exit_check_output();

void __accmut__exit_time(){

	if(MUTATION_ID != 0){
		return;
	}

	gettimeofday(&tv_end, NULL);

	long real_sec =  tv_end.tv_sec - tv_begin.tv_sec;
	long real_usec = tv_end.tv_usec - tv_begin.tv_usec;

#if ACCMUT_STATIC_ANALYSIS_EVAL
	char* path = "evaltime";
#else
	char *path = "timeres";
#endif

	int fd = open(path, O_WRONLY | O_CREAT | O_APPEND);

	char res[128] = {0};

	__accmut__strcat(res, __accmut__itoa(TEST_ID, 10));
	__accmut__strcat(res, "\t");

	__accmut__strcat(res, __accmut__itoa(real_sec, 10));
	__accmut__strcat(res, "\t");

	__accmut__strcat(res, __accmut__itoa(real_usec, 10));
	__accmut__strcat(res, "\n");

	write(fd, res, __accmut__strlen(res));
	close(fd);
}


/* The signal handler of time out process */
void __accmut__handler(int sig){
    // fprintf(stderr, "%s", MUTSTXT[MUTATION_ID]);
    // fprintf(stderr, "%d\n", MUTATION_ID);	// TODO::stdout or stderr
    // fprintf(stderr, "%d %d\n", TEST_ID, MUTATION_ID);
    exit(0);
}

void __accmut__sepcific_timer(){
	char path[128] = {0};
	sprintf(path, "%s%s%d", getenv("HOME"), "/tmp/accmut/oritime/", TEST_ID);
	FILE * fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "WARNING : ORI TIME FILE DOSE NOT EXISIT : %s\n", path);
		//if the ori time file does not exisit, use the default timer value.
		VALUE_SEC = 0;
		VALUE_USEC = 5000;
		return;
	}
	fscanf(fp, "%d", &VALUE_SEC);
	fscanf(fp, "%d", &VALUE_USEC);
	fclose(fp);
	// fprintf(stderr, " ~~~~~ %ld %ld\n", VALUE_SEC, VALUE_USEC);
	// exit(0);
}


void __accmut__exec_inst_nums(){
	// fprintf(stderr, "0");
	EXEC_INSTS++;
}


void __accmut__debug(int index){
	// if(MUTATION_ID == 0)
	// 	fprintf(stderr, "~~~~~ %d\n", index);
	if(index == 2){
		perror("UNLINK ERR: ");
	}
}


void __accmut__load_all_muts(){
    char path[256];
    strcpy(path, getenv("HOME"));
    strcat(path, "/tmp/accmut/mutations.txt");
	FILE *fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "FILE ERROR: mutation.txt CAN NOT OPEN !!! PATH: %s\n", path);
		exit(0);
	}
	int id = 1;	
	char type[4];
	char buff[MUTFILELINE];	
	char tail[40];
	while(fgets(buff, MUTFILELINE, fp)){

		#if ACCMUT_STATIC_ANALYSIS_EVAL
			int idx;
			sscanf(buff, "%3s:%*[^:]:%d:%s", type, &idx, tail);
			Mutation* m = (Mutation *)malloc(sizeof(Mutation));		
			m->index = idx;
		#else
			sscanf(buff, "%3s:%*[^:]:%*[^:]:%s", type, tail);
			Mutation* m = (Mutation *)malloc(sizeof(Mutation));	
		#endif

		if(!strcmp(type, "AOR")){
			m->type = AOR;
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->sop = s_op;
			m->op_0 = t_op;
		}else if(!strcmp(type, "LOR")){
			m->type = LOR;
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->sop = s_op;
			m->op_0 = t_op;
		}else if(!strcmp(type, "ROR")){
			m->type = ROR;
			int op, s_pre, t_pre;
			sscanf(tail, "%d:%d:%d", &op, &s_pre, &t_pre);
			m->sop = op;
			m->op_1 = s_pre;
			m->op_2 = t_pre;
		}else if(!strcmp(type, "STD")){
			m->type = STD;
			int op, f_tp, retval;
			if(strlen(tail) == 4){//return void 
				sscanf(tail, "%d:%d", &op, &f_tp);
				m->sop = op;	//must be 0
				m->op_1 = f_tp;
			}else{//return i32 or i64
				sscanf(tail, "%d:%d:%d", &op, &f_tp, &retval);
				m->sop = op;
				m->op_1 = f_tp;	//32, or 64
				m->op_2 = retval;
			}
		}else if(!strcmp(type, "LVR")){
			m->type = LVR;
			int op, op_i;
			long s_c, t_c;
			sscanf(tail, "%d:%d:%ld:%ld", &op, &op_i, &s_c, &t_c);
			m->sop = op;
			m->op_0 = op_i;
			m->op_1 = s_c;
			m->op_2 = t_c;
		}else if(!strcmp(type, "UOI")){
			m->type = UOI;
			int op, op_i, tp;
			sscanf(tail, "%d:%d:%d", &op, &op_i, &tp);
			m->sop = op;
			m->op_1 = op_i;
			m->op_2 = tp;
		}else if(!strcmp(type, "ROV")){
			m->type = ROV;
			int op, op1, op2;
			sscanf(tail, "%d:%d:%d", &op, &op1, &op2);
			m->sop = op;
			m->op_1 = op1;
			m->op_2 = op2;
		}else if(!strcmp(type, "ABV")){
			m->type = ABV;
			int op, idx;
			sscanf(tail, "%d:%d", &op, &idx);
			m->sop = op;
			m->op_0 = idx;
		}else{
			fprintf(stderr, "ERROR MUT TYPE: %d:%s\n", id, buff);
			exit(0);
		}
		ALLMUTS[id] = m;
		id++;
	}
	MUT_NUM = id - 1;

	#if 1
	fprintf(stderr, "\n----------------- DUMP ALL MUTS ------------------\n");
	fprintf(stderr, "TOTAL MUTS NUM : %d\n", MUT_NUM);
	int i;
	for(i = 1; i <= MUT_NUM; i++){
		Mutation *m = ALLMUTS[i];
	
		#if ACCMUT_STATIC_ANALYSIS_EVAL
			fprintf(stderr, "MUT %d => IDX: %d , TP: %d , SOP: %d , OP0 : %d , OP1 : %d , OP2 : %d\n",
				i, m->index, m->type, m->sop, m->op_0, m->op_1, m->op_2);
		#else
			fprintf(stderr, "MUT %d => TP: %d , SOP: %d , OP0 : %d , OP1 : %d , OP2 : %d\n",
				i, m->type, m->sop, m->op_0, m->op_1, m->op_2);
		#endif
	}
	fprintf(stderr, "----------------- END DUMP ALL MUTS ---------------\n\n");

	#endif
}

/****************************************************************/


//#include<accmut/accmut_io.h>

#if ACCMUT_MUTATION_SCHEMATA
	#include<accmut/accmut_schem.h>
#endif

 
#if ACCMUT_STATIC_ANALYSIS_EVAL
	#include<accmut/accmut_sma_eval.h>
#endif


#if ACCMUT_STATIC_ANALYSIS_FORK_INLINE
	#include<accmut/accmut_sma_fork.h>
#endif


#if ACCMUT_STATIC_ANALYSIS_FORK_CALL
	#include<accmut/accmut_schem.h>
#endif


#if ACCMUT_DYNAMIC_ANALYSIS_FORK
	#include<accmut/accmut_dma_fork.h>
#endif

/****************************************************************/


#endif
