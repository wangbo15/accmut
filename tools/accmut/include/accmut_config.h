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

#define ACCMUT_GEN_MUT 0

//SWITCH FOR MUTATION SCHEMATA
#define ACCMUT_MUTATION_SCHEMATA 0

//SWITCH FOR STATIC ANALYSIS
#define ACCMUT_STATIC_ANALYSIS_EVAL 0
#define ACCMUT_STATIC_ANALYSIS_FORK_INLINE 0
#define ACCMUT_STATIC_ANALYSIS_FORK_CALL 0

//SWITCH FOR DYNAMIC ANALYSIS
#define ACCMUT_DYNAMIC_ANALYSIS_FORK 1



/*************************************************/

//4M buffer 
#define MAXBUFFERSIZE 1<<22
#define MAXMUTNUM 10000

/*************************************************/


const char PROJECT[]="printtokens";
const char ORACLEDIR[] = "/home/nightwish/workspace/testing/accmut/printtokens/oracle/";
int MUTATION_ID = 0;
int TEST_ID = -1;

/** Set Timer **/
//TODO: REAL OR PROF
struct itimerval tick;
long VALUE_SEC = 0;
long VALUE_USEC = 0;
long INTTERVAL_SEC = 0;
long INTTERVAL_USEC = 5;

struct timeval tv_begin, tv_end;
struct rusage usage_fbegin, usage_fmid,usage_fend;
struct rusage usage_cbegin, usage_cend;

/*************************************************/
//FOR STATISTICS
unsigned long long EXEC_INSTS = 0;


/*************************************************/

//FOR BUFFER
char STDBUFF[MAXBUFFERSIZE];
char *ORACLEBUFF;
size_t ORACLESIZE = 0;
size_t CURBUFSIZE = 0;

/*************************************************/


void __accmut__oracledump();
void __accmut__bufferdump();

int __accmut__checkoutput(){
	if(ORACLESIZE != CURBUFSIZE){
		return 1;
	}
	return memcmp(STDBUFF, ORACLEBUFF, CURBUFSIZE);
}

void __accmut__exit_check(){
	int res = __accmut__checkoutput();
	if(res != 0){
		fprintf(stderr, "TEST: %d KILL MUT: %d\n", TEST_ID, MUTATION_ID);
		//__accmut__bufferdump();
		
	}

	// if(MUTATION_ID == 0){
	// 	__accmut__oracledump();
	// }
}

void __accmut__exit_time(){
	gettimeofday(&tv_end, NULL);
	double interval = (double)(tv_end.tv_sec - tv_begin.tv_sec) + ((double)(tv_end.tv_usec - tv_begin.tv_usec))/1000000;
	// fprintf(stderr, "%d\t%lf\n", MUTATION_ID, interval);
	fprintf(stderr, "%d\n", MUTATION_ID);
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

//ORI fprintf
// int __accmut__fprintf(FILE *stream, const char *format, ...){
// 	int ret;
// 	va_list ap;
// 	va_start(ap, format);
// 	ret = vfprintf(stream, format, ap);
// 	va_end(ap);
// 	return 0;
// }

int __accmut__fprintf(FILE *stream, const char *format, ...){
	int ret;
	va_list ap;
	va_start(ap, format);
	char tmp[4096] = {0};
	ret = vsprintf(tmp, format, ap);
	va_end(ap);

	if(ret > 4096){
		perror("BUFFER LINE OVERFLOW !!!!!!!\n");
	}

	int total = CURBUFSIZE + ret;
	if(total >= MAXBUFFERSIZE){
		perror("ACCMUT BUFFER OVERFLOW !!!!!!\n");
		exit(0);
	}
	memcpy((STDBUFF + CURBUFSIZE), tmp, ret*(sizeof(char)) );
	CURBUFSIZE = total;
	return ret;
}

void __accmut__bufinit(){
	char path[120];
	sprintf(path, "%st%d", ORACLEDIR, TEST_ID);
	int fd = open(path, O_RDONLY);
	if(fd == -1){
		fprintf(stderr, "ORACLEDIR OPEN ERROR !!!!!!\n");
		fprintf(stderr, "ORACLEDIR PATH : %s\n", path);
	}
	struct stat sb;
	if(fstat(fd, &sb) == -1){
		fprintf(stderr, "fstat ERROR !!!!!!\n");
	}
	ORACLEBUFF = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(ORACLEBUFF == MAP_FAILED){
		fprintf(stderr, "mmap ERROR !!!!!!\n");
	}
	ORACLESIZE = sb.st_size;

	//regist the exit handler function of a process
	if(atexit(__accmut__exit_check) != 0){
		fprintf(stderr, "__accmut__exit_check REGSITER ERROR\n");
	}

	// if(atexit(__accmut__exit_time) != 0){
	// 	fprintf(stderr, "__accmut__exit_time REGSITER ERROR\n");
	// }
}


void __accmut__handler(int sig){
    // fprintf(stderr, "%s", MUTSTXT[MUTATION_ID]);
    // fprintf(stderr, "%d\n", MUTATION_ID);	// TODO::stdout or stderr
    // fprintf(stderr, "%d %d\n", TEST_ID, MUTATION_ID);
    exit(0);
}

void __accmut__setout(int id){
 	char path[120];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/output/");
	strcat(path, PROJECT);
	strcat(path, "/t");
	sprintf(path, "%s%d_%d", path, TEST_ID, id);
	//printf("PATH : %s\n", path);
	if (freopen(path, "w", stdout)==NULL){
		fprintf(stderr, "STDOUT REDIR ERR! : %s\n", path);
	}
}

void __accmut__oracledump(){
	fprintf(stderr, "\n********** TID:%d  ORI BUFFER SIZE: %d ***********\n", TEST_ID, ORACLESIZE);
	int i;
	for(i = 0; i < ORACLESIZE; i++){
		fprintf(stderr, "%c", ORACLEBUFF[i]);
	}
	fprintf(stderr, "************ END OF ORACLE ***************\n\n");
}

void __accmut__bufferdump(){
	fprintf(stderr, "\n********** TID:%d  MID:%d  ACCMUT BUF SIZE:%d ***********\n", TEST_ID, MUTATION_ID, CURBUFSIZE);
	int i;
	for(i = 0; i < CURBUFSIZE; i++){
		fprintf(stderr, "%c", STDBUFF[i]);
	}
	fprintf(stderr, "************ END OF ACCMUT BUFFER ***************\n\n");
}


void __accmut__sepcific_timer(){
	char path[100];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/oritime.txt");
	FILE * fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "ORI TIME CANNOT OPEN : %s\n", path);
		exit(0);
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
