#ifndef ACCMUT_SMA_CALLFORK_H
#define ACCMUT_SMA_CALLFORK_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#include <accmut/accmut_arith_common.h>
/******************************************************/


int totalfork = 0;


void __accmut__mainfork(int id){
		
	if(MUTATION_ID == 0 && MUTS_ON[id]){
	
		//fprintf(stderr, "FORK MUT: %d\n", id);
		pid_t pid = fork();
		
		if(pid < 0){
			fprintf(stderr, "FORK ERR!\n");
			exit(0);
		}else if(pid == 0){//child process
			MUTATION_ID = id;
			
			//TODO : set_out
			__accmut__setout(id);
			
			int r = setitimer(ITIMER_PROF, &tick, NULL); // TODO:ITIMER_REAL?
			
			if(r < 0){
				fprintf(stderr, "SET TIMMER ERR, M_ID : %d\n", id);
				exit(0);
			}
		}else{//father process	
			totalfork++;
			int pr = waitpid(pid, NULL, 0);
			if(pr < 0){
				fprintf(stderr, "WAITPID ERROR\n");
			}			
		}
	}

}

void __accmut__init(){

    __accmut__sepcific_timer();
	

    tick.it_value.tv_sec = VALUE_SEC;  // sec
    tick.it_value.tv_usec = VALUE_USEC; // u sec.
    tick.it_interval.tv_sec = INTTERVAL_SEC;
    tick.it_interval.tv_usec =  INTTERVAL_USEC;

    signal(SIGPROF, __accmut__handler);
    
	if(TEST_ID < 0){
		fprintf(stderr, "TEST_ID NOT INIT\n");
		exit(0);
	}

	__accmut__load_all_muts();

	char numpath[100];
	strcpy(numpath, getenv("HOME"));
	strcat(numpath, "/tmp/accmut/mutsnum.txt");
	fp = fopen(numpath, "r");
	if(fp == NULL){
		fprintf(stderr, "FILE mutsnum.txt ERR\n");
		exit(0);
	}
	fscanf(fp, "%d", &MAX_MUT_NUM);	
	MUTS_ON = (int *) malloc( (sizeof(int)) * (MAX_MUT_NUM + 1) );
	
	int i;
	for(i = 0; i < MAX_MUT_NUM + 1; i++){
		*(MUTS_ON + i) = 1;
	}
	
/*
    fprintf(stderr, "\n-----------------------------------------\n");
    for(i = 0; i < MAX_MUT_NUM + 1; i++){
		fprintf(stderr, "%d\n", *(MUTS_ON + i));
	}*/
	
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/input/printtokens/t");
	sprintf(path, "%s%d", path, TEST_ID);
	
	//fprintf(stderr, "PATH : %s\n", path);
	
	fp = fopen(path, "r");
	
	int curmut, on_id;
	int killed = 0;
	while(fscanf(fp,"%d:%d", &curmut, &on_id) != EOF){
			if(curmut != on_id){
				//fprintf(stderr,"CURMUT: %d, ON_ID: %d\n", curmut, on_id);
				*(MUTS_ON + curmut) = 0;
				killed++;
			}				                	            
    }
    fclose(fp);
    
/*
    fprintf(stderr, "\n-----------------------------------------\n");
    for(i = 0; i < MAX_MUT_NUM + 1; i++){
		fprintf(stderr, "%d:%d\n", i, *(MUTS_ON + i));
	}
*/

   	for(i = 1; i < MAX_MUT_NUM + 1; i++){
		__accmut__mainfork(i);
	}
	
	// if(MUTATION_ID == 0){
	// 	fprintf(stderr, "%d\n",	totalfork);
	// }
	
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


int __accmut__process_i32_arith(int from, int to, int left, int right){

	int ori = __accmut__cal_i32_arith(ALLMUTS[to]->op , left, right);
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
	
	int mut_res;
	if(m->type == AOR || m->type == LOR){
		mut_res = __accmut__cal_i32_arith(m->t_op, left, right);
	}else if(m->type == LVR){
	
		if(m->op_index == 0)
			mut_res = __accmut__cal_i32_arith(m->op, m->t_con, right);
		else
			mut_res = __accmut__cal_i32_arith(m->op, left, m->t_con);
			
	}else{
		fprintf(stderr, "M->TYPE ERR @__accmut__process_i32_arith. MID: %d MTP: %d\n", MUTATION_ID, m->type);
		exit(0);		
	}
	
	return mut_res;
}

long __accmut__process_i64_arith(int from, int to, long left, long right){

	long ori = __accmut__cal_i64_arith(ALLMUTS[to]->op , left, right);
	return ori;
}

int __accmut__process_i32_cmp(int from, int to, int left, int right){
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		//fprintf(stderr, "F: %d, T: %d M: %d --------1\n", from, to, MUTATION_ID);
		//fprintf(stderr, "%d \n", ALLMUTS[to]->s_pre);
		int ori = __accmut__cal_i32_bool(ALLMUTS[to]->s_pre , left, right);	
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
		
	int mut_res;
	if(m->type == ROR){
		//fprintf(stderr, "F: %d, T: %d M: %d --------2\n", from, to, MUTATION_ID);
		mut_res = __accmut__cal_i32_bool(m->t_pre, left, right);
		
	}else if(m->type == LVR){
	
		if(m->op_index == 0){
			//fprintf(stderr, "F: %d, T: %d M: %d --------3\n", from, to, MUTATION_ID);
			mut_res = __accmut__cal_i32_bool(ALLMUTS[to]->s_pre, m->t_con, right);
		}
		else{
			//fprintf(stderr, "F: %d, T: %d M: %d --------4\n", from, to, MUTATION_ID);
			mut_res = __accmut__cal_i32_bool(ALLMUTS[to]->s_pre, left, m->t_con);
		}

			
	}else{
		fprintf(stderr, "M->TYPE ERR @__accmut__cal_i32_bool. MID: %d MTP: %d\n", MUTATION_ID, m->type);
		exit(0);
	}
	
	return mut_res;    
}

int __accmut__process_i64_cmp(int from, int to, long left, long right){
	long ori = __accmut__cal_i64_bool(ALLMUTS[to]->s_pre , left, right);
	return ori;
}

void __accmut__process_st_i32(int from, int to, int *addr){

	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		int ori = ALLMUTS[to]->s_con;
		*addr = ori;
		return;
	}
	Mutation *m = ALLMUTS[MUTATION_ID];
	if(m->type == LVR){
		*addr = m->t_con;
		return;
	}else{
		fprintf(stderr, "M->TYPE ERR @__accmut__process_st_i32. MID: %d MTP: %d\n", MUTATION_ID, m->type);
		exit(0);
	}
	
}

void __accmut__process_st_i64(int from, int to, long *addr){
	long ori = ALLMUTS[to]->s_con;
	*addr = ori;
}



#endif
