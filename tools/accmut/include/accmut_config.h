#ifndef ACCMUT_MUT_CONFIG_H
#define ACCMUT_MUT_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdarg.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <fcntl.h>

#define ACCMUT_GEN_MUT 0

//SWITCH FOR MUTATION SCHEMATA
#define ACCMUT_MUTATION_SCHEMATA 1

//SWITCH FOR STATIC ANALYSIS
#define ACCMUT_STATIC_ANALYSIS_EVAL 0
#define ACCMUT_STATIC_ANALYSIS_FORK_INLINE 0
#define ACCMUT_STATIC_ANALYSIS_FORK_CALL 0

//SWITCH FOR DYNAMIC ANALYSIS
#define ACCMUT_DYNAMIC_ANALYSIS_FORK 0



/*************************************************/

const char PROJECT[]="printtokens";
const char ORACLEDIR[] = "/home/nightwish/workspace/testing/accmut/printtokens/oricle/";
int MUTATION_ID = 0;
int TEST_ID = -1;

/** Set Timer **/
//TODO: REAL OR PROF
struct itimerval tick;
const long VALUE_SEC = 0;
const long VALUE_USEC = 500;
const long INTTERVAL_SEC = 0;
const long INTTERVAL_USEC = 50;

/*************************************************/

//4M buffer 
#define MAXBUFFERSIZE 1<<22

char STDBUFF[MAXBUFFERSIZE];
char *ORACLEBUFF;
size_t ORACLESIZE = 0;
size_t CURBUFSIZE = 0;

void __accmut__oracledump();
void __accmut__bufferdump();

int __accmut__checkoutput(){
	if(ORACLESIZE != CURBUFSIZE){
		return 1;
	}
	return memcmp(STDBUFF, ORACLEBUFF, CURBUFSIZE);
}

void __accmt__exit(){
	int res = __accmut__checkoutput();
	if(res != 0){
		fprintf(stderr, "CONSTRACT %d %d\n", MUTATION_ID, res);
		__accmut__bufferdump();
		if(MUTATION_ID == 0){
			__accmut__oracledump();
		}
	}
}

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
		fprintf(stderr, "PATH : %s\n", path);
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

	if(atexit(__accmt__exit) != 0){
		fprintf(stderr, "__accmt__exit REGSITER ERROR\n");
	}
}


void __accmut__handler(int sig){
    fprintf(stdout, "MUT %d TIME OUT!\n", MUTATION_ID);	// TODO::stdout or stderr
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

/****************************/

#if ACCMUT_MUTATION_SCHEMATA
	#include<accmut/accmut_schem.h>
#endif

/****************************/
 
#if ACCMUT_STATIC_ANALYSIS_EVAL
	#include<accmut/accmut_sma_eval.h>
#endif

/****************************/

#if ACCMUT_STATIC_ANALYSIS_FORK_INLINE
	#include<accmut/accmut_sma_fork.h>
#endif

/****************************/

#if ACCMUT_STATIC_ANALYSIS_FORK_CALL
	#include<accmut/accmut_sma_callfork.h>
#endif

/****************************/

#if ACCMUT_DYNAMIC_ANALYSIS_FORK
	#include<accmut/accmut_dma_fork.h>
#endif

/****************************/


#endif
