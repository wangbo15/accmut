#ifndef ACCMUT_MUT_H
#define ACCMUT_MUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>



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

const char PROJECT[]="printtokens";
int MUTATION_ID = 0;
int TEST_ID = -1;

/** Set Timer **/
//TODO: REAL OR PROF
struct itimerval tick;
const long VALUE_SEC = 0;
const long VALUE_USEC = 2000;
const long INTTERVAL_SEC = 0;
const long INTTERVAL_USEC = 50;

/*************************************************/


void __accmut_handler(int sig){
    fprintf(stdout, "MUT %d TIME OUT!\n", MUTATION_ID);
    exit(0);
}

void __accmut__setout(int id){
 	char path[100];
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


/****************************/

#if ACCMUT_MUTATION_SCHEMATA
	#include<accmut/accmut_sche.h>
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
