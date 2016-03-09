#ifndef ACCMUT_SMA_FORK_H
#define ACCMUT_SMA_FORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>


int MAX_MUT_NUM;
int *MUTS_ON;


void __accmut__mainfork(int id){
	
	//fprintf(stderr, "MID: %d ID: %d ON: %d\n", MUTATION_ID, id , MUTS_ON[id]);

	if(MUTATION_ID == 0 && MUTS_ON[id]){
		pid_t pid = fork();
		
		if(pid < 0){
			fprintf(stderr, "FORK ERR!\n");
			exit(0);
		}else if(pid == 0){//child process
			MUTATION_ID = id;
			
			//TODO : set_out
			__accmut__setout(id);
			
			int r = setitimer(ITIMER_REAL, &tick, NULL); // TODO:ITIMER_REAL?
			
			if(r < 0){
				fprintf(stderr, "SET TIMMER ERR, M_ID : %d\n", id);
				exit(0);
			}
		}else{//father process
		/*
			int pr;
			while( (pr = waitpid(pid, NULL, 0)) > 0){
				
			}
			
			if(pr < 0){
				fprintf(stderr, "WAITPID ERROR\n");
			}*/
			
			
			int pr = waitpid(pid, NULL, 0);
			if(pr < 0){
				fprintf(stderr, "WAITPID ERROR\n");
			}
		}
	}
}

void __accmut__init(){
	if(TEST_ID < 0){
		fprintf(stderr, "TEST_ID NOT INIT\n");
		exit(0);
	}
	FILE *fp;
	char path[100];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/mutsnum.txt");
	fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "FILE mutsnum.txt ERR\n");
		exit(0);
	}
	fscanf(fp, "%d", &MAX_MUT_NUM);
	//printf("PATH: %s\n", path);
	//printf("%d\n", MAX_MUT_NUM);
	
	MUTS_ON = (int *) malloc( (sizeof(int)) * (MAX_MUT_NUM + 1) );
	
	int i;
	for(i = 0; i < MAX_MUT_NUM + 1; i++){
		*(MUTS_ON + i) = 1;
	}
	
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/input/printtokens/t");
	sprintf(path, "%s%d", path, TEST_ID);
	//printf("PATH: %s\n", path);
	
	fp = fopen(path, "r");
	
	if(fp == NULL){
		fprintf(stderr, "FILE muts_on_str ERR : %s\n", path);
		exit(0);		
	}
	
	int curmut, on_id;
	int killed = 0;
	while(fscanf(fp,"%d:%d", &curmut, &on_id) != EOF){
			if(curmut != on_id){
				*(MUTS_ON + on_id) = 0;
				killed++;
			}				                	            
    }
    fclose(fp);
	
	/*for(i = 0; i < MAX_MUT_NUM + 1; i++){
		printf("%d : %d \n", i, *(MUTS_ON + i));
	}*/
	//printf("TOTAL : %d, KILLED: %d\n", MAX_MUT_NUM+1, killed);
	
    tick.it_value.tv_sec = VALUE_SEC;  // sec
    tick.it_value.tv_usec = VALUE_USEC; // u sec.
    tick.it_interval.tv_sec = INTTERVAL_SEC;
    tick.it_interval.tv_usec =  INTTERVAL_USEC;

    signal(SIGPROF, __accmut__handler);  // TODO:: signal(SIGALRM, __accmut__handler);  
    
    // set out
	__accmut__setout(0);
    //
    for(i = 1; i < MAX_MUT_NUM + 1; i++){
		__accmut__mainfork(i);
	}
    
}


//////
int __accmut__process_call_i32(){
    return 0;
}

long __accmut__process_call_i64(){
	return 0;
}

void __accmut__process_call_void(){

}



#endif
