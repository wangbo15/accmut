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
#define ACCMUT_MUTATION_SCHEMATA 1

//SWITCH FOR STATIC ANALYSIS
#define ACCMUT_STATIC_ANALYSIS_EVAL 0
#define ACCMUT_STATIC_ANALYSIS_FORK_INLINE 0
#define ACCMUT_STATIC_ANALYSIS_FORK_CALL 0

//SWITCH FOR DYNAMIC ANALYSIS
#define ACCMUT_DYNAMIC_ANALYSIS_FORK 0


//for DMA FORK
#define MAXMUTNUM 32768

#define PAGESIZE 4096

// const char PROJECT[]="printtokens";
const char PROJECT[]="flex";

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

#include <accmut/accmut_mut.h>

#include <accmut/accmut_io.h>

Mutation* ALLMUTS[MAXMUTNUM + 1];
int MAX_MUT_NUM;
int *MUTS_ON;



/************* ALL EXIT HANDLER ***************************/
void __accmut__exit_check_output();

void __accmut__exit_time(){

	if(MUTATION_ID != 0){
		return;
	}

	gettimeofday(&tv_end, NULL);

	double interval = (double)(tv_end.tv_sec - tv_begin.tv_sec) + ((double)(tv_end.tv_usec - tv_begin.tv_usec))/1000000;

#if 0
	long real_sec =  tv_end.tv_sec - tv_begin.tv_sec;
	long real_usec = tv_end.tv_usec - tv_begin.tv_usec;

	fprintf(stderr, "%ld\t%ld\n", real_sec, real_usec);
#endif
	
#if 1
	FILE* timefile = fopen("timeres", "a");
	if(timefile == 0){
		fprintf(stderr, "FILE OPEN ERROR @__accmut__exit_time\n");
	}
	fprintf(timefile, "ATEXIT %d\t%lf\n", TEST_ID, interval);	
	fclose(timefile);
#else
	// fprintf(stderr, "%d %d\n", TEST_ID, MUTATION_ID);
	fprintf(stderr, "ATEXIT %d\t%lf\n", TEST_ID, interval);
#endif

}

void __accmut__exit_preciese_time(){
	gettimeofday(&tv_end, NULL);
	long real_sec =  tv_end.tv_sec - tv_begin.tv_sec;
	long real_usec = tv_end.tv_usec - tv_begin.tv_usec;

	if(MUTATION_ID == 0){
		
		getrusage(RUSAGE_SELF, &usage_fend);

		fprintf(stderr, "%d\t%ld %ld\t%ld %ld\t%ld %ld\n", MUTATION_ID, real_sec, real_usec,
			usage_fend.ru_utime.tv_sec,  usage_fend.ru_utime.tv_usec, 
			usage_fend.ru_stime.tv_sec, usage_fend.ru_stime.tv_sec);
	}else{

		getrusage(RUSAGE_SELF, &usage_cend);

		long child_u_sec = usage_cend.ru_utime.tv_sec - usage_cbegin.ru_utime.tv_sec;
		long child_u_usec = usage_cend.ru_utime.tv_usec - usage_cbegin.ru_utime.tv_usec;
		long child_s_sec = usage_cend.ru_stime.tv_sec - usage_cbegin.ru_stime.tv_sec;
		long child_s_usec = usage_cend.ru_stime.tv_usec - usage_cbegin.ru_stime.tv_usec;

		fprintf(stderr, "%d\t%ld %ld\t%ld %ld\t%ld %ld\n", MUTATION_ID, real_sec, real_usec, 
			child_u_sec, child_u_usec, child_s_sec, child_s_usec);
	}

}

/* The signal handler of time out process */
void __accmut__handler(int sig){
    // fprintf(stderr, "%s", MUTSTXT[MUTATION_ID]);
    // fprintf(stderr, "%d\n", MUTATION_ID);	// TODO::stdout or stderr
    // fprintf(stderr, "%d %d\n", TEST_ID, MUTATION_ID);
    exit(0);
}

void __accmut__sepcific_timer(){
	char path[100];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/oritime.txt");
	FILE * fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "ORI TIME FILE DOSE NOT EXISIT : %s\n", path);
		//if the ori time file does not exisit, use the default timer value.
		VALUE_SEC = 0;
		VALUE_USEC = 120;
		//exit(0);
	}
	//TODO::optimize
	int i;
	for(i = 1; i <= TEST_ID; i++){
		fscanf(fp, "%d", &VALUE_SEC);
		fscanf(fp, "%d", &VALUE_USEC);
	}
	fclose(fp);
	// fprintf(stderr, " ~~~~~ %ld %ld\n", VALUE_SEC, VALUE_USEC);
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
		//fprintf(stderr, "%s", buff);
		sscanf(buff, "%3s:%*[^:]:%*[^:]:%s", type, tail);

		//fprintf(stderr, "%d -- %s --  %s\n", id, type, tail);
		Mutation* m = (Mutation *)malloc(sizeof(Mutation));
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
	MAX_MUT_NUM = id - 1;

	#if 0
	int i;
	for(i = 1; i <= MAX_MUT_NUM; i++){
		Mutation *m = ALLMUTS[i];
		fprintf(stderr, "");
	}
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
	#include<accmut/accmut_sma_callfork.h>
#endif


#if ACCMUT_DYNAMIC_ANALYSIS_FORK
	#include<accmut/accmut_dma_fork.h>
#endif

/****************************************************************/


#endif
