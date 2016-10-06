#ifndef ACCMUT_SMA_SCHEM_H
#define ACCMUT_SMA_SCHEM_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <math.h>
#include <stdarg.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>


#include <accmut/accmut_arith_common.h>
/******************************************************/


void __accmut__init(){
	
    gettimeofday(&tv_begin, NULL);
    
    atexit(__accmut__exit_time);

	__accmut__sepcific_timer();

	__accmut__set_sig_handlers();
    
	if(TEST_ID < 0){
		ERRMSG("TEST_ID NOT INIT");
		exit(0);
	}

	__accmut__load_all_muts();


	MUTS_ON = (int *) malloc( (sizeof(int)) * (MUT_NUM + 1) );
	
	int i;


#if ACCMUT_STATIC_ANALYSIS_FORK_CALL
	for(i = 1; i < MUT_NUM + 1; i++){
		*(MUTS_ON + i) = 0;
	}

	char path[128];
	sprintf(path, "%s%s%s/t%d", getenv("HOME"), "/tmp/accmut/input/", PROJECT, TEST_ID);
		
	FILE* fp = fopen(path, "r");
	
	if(fp == 0){
		ERRMSG("SMA FOEPN ERR");
		exit(0);
	}
	int curmut, on_id;
	
	/*
	while(fscanf(fp,"%d:%d", &curmut, &on_id) != EOF){
			if(on_id == -1 || curmut == on_id){
				//fprintf(stderr,"CURMUT: %d, ON_ID: %d\n", curmut, on_id);
				*(MUTS_ON + curmut) = 1;
			}	                	            
    }*/

	while(fscanf(fp,"%d", &curmut) != EOF){
		*(MUTS_ON + curmut) = 1;                	            
    }

    fclose(fp);
#elif ACCMUT_MUTATION_SCHEMATA
	for(i = 0; i < MUT_NUM + 1; i++){
		*(MUTS_ON + i) = 1;
	}
#endif

	static int TOTALFORK = 0;

   	for(i = 1; i < MUT_NUM + 1; i++){

		if(MUTATION_ID == 0 && (MUTS_ON[i] == 1) ){
		
			//fprintf(stderr, "FORK MUT: %d\n", i);
			pid_t pid = fork();
			
			if(pid < 0){
				ERRMSG("fork ERR ");
				exit(0);
			}else if(pid == 0){//child process

	            if (mprotect((void *)(&MUTATION_ID), PAGESIZE, PROT_READ | PROT_WRITE)) {
	                perror("mprotect ERR : PROT_READ | PROT_WRITE");
	                exit(errno);
	            }

				MUTATION_ID = i;
				
	            if (mprotect((void *)(&MUTATION_ID), PAGESIZE, PROT_READ)) {
	                perror("mprotect ERR : PROT_READ");
	                exit(errno);
	            }
				
				//fprintf(stderr, "%d %d\n", TEST_ID, MUTATION_ID);
				
				int r1 = setitimer(ITIMER_REAL, &ACCMUT_REAL_TICK, NULL); 
				
				int r2 = setitimer(ITIMER_PROF, &ACCMUT_PROF_TICK, NULL); 

				if(r1 < 0 || r2 < 0){
					ERRMSG("setitimer ERR ");
					exit(1);
				}				

				break;
			}else{//father process	
				TOTALFORK++;
				int pr = waitpid(pid, NULL, 0);

				if(pr < 0){
					ERRMSG("waitpid ERR ");
					exit(0);
				}			
			}
		}

	}

	free(MUTS_ON);

#if 1
	if(MUTATION_ID == 0){
		fprintf(stdout, "TOTALFORK : %d\n",	TOTALFORK);
	}
#endif

}



/*
*must fellow the type seq: (i8 type, i8 index, i64* ptr ...)
*not std ->return 0 , std -> return 1
*/
int __accmut__prepare_call(int from, int to, int opnum, ...){

	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		return 0;
	}

	Mutation *m = ALLMUTS[MUTATION_ID];

	if(m->type == STD){
		return 1;
	}

	va_list ap;
	va_start(ap, opnum);

	PrepareCallParam params[MAX_PARAM_NUM];	//max param num limited to 16

	int i;
	for(i = 0; i < opnum; i++){
		short tp_and_idx = va_arg(ap, short);
		int idx = tp_and_idx & 0x00FF;

		params[idx].type = tp_and_idx >> 8;
		params[idx].address = va_arg(ap, unsigned long);

		//fprintf(stderr, "%dth: %d\n", i, *((int*)ptrs[i]));

	}//end for i
	va_end(ap);

	switch(m->type){
		case LVR:
		{
			int idx = m->op_0;

			switch(params[idx].type){
				case CHAR_TP:
				{
					char *ptr = (char *) params[idx].address;
					*ptr = m->op_2;
					break;
				}
				case SHORT_TP:
				{
					short *ptr = (short *) params[idx].address;
					*ptr = m->op_2;
					break;
				}
				case INT_TP:
				{
					int *ptr = (int *) params[idx].address;
					*ptr = m->op_2;
					break;
				}	
				case LONG_TP:
				{
					long *ptr = (long *) params[idx].address;
					*ptr = m->op_2;
					break;
				}
				default:
				{
					ERRMSG("ERR LVR params[idx].type ");
					exit(0);
				}
			}//end switch(params[idx].type)
			break;
		}
		case UOI:
		{
			int idx = m->op_1;
			int uoi_tp = m->op_2;

			switch(params[idx].type){
				case CHAR_TP:
				{
					char *ptr = (char *) params[idx].address;
					if(uoi_tp == 0){
						*ptr = *ptr + 1;
					}else if(uoi_tp == 1){
						*ptr = *ptr - 1;
					}else if(uoi_tp == 2){
						*ptr = 0 - *ptr;
					}
					break;
				}
				case SHORT_TP:
				{
					short *ptr = (short *) params[idx].address;
					if(uoi_tp == 0){
						*ptr = *ptr + 1;
					}else if(uoi_tp == 1){
						*ptr = *ptr - 1;
					}else if(uoi_tp == 2){
						*ptr = 0 - *ptr;
					}
					break;
				}
				case INT_TP:
				{
					int *ptr = (int *) params[idx].address;
					if(uoi_tp == 0){
						*ptr = *ptr + 1;
					}else if(uoi_tp == 1){
						*ptr = *ptr - 1;
					}else if(uoi_tp == 2){
						*ptr = 0 - *ptr;
					}
					break;
				}		
				case LONG_TP:
				{
					long *ptr = (long *) params[idx].address;
					if(uoi_tp == 0){
						*ptr = *ptr + 1;
					}else if(uoi_tp == 1){
						*ptr = *ptr - 1;
					}else if(uoi_tp == 2){
						*ptr = 0 - *ptr;
					}
					break;
				}
				default:
				{
					ERRMSG("ERR UOI params[idx].type ");
					exit(0);
				}
			}//end switch(params[idx].type)
			break;
		}
		case ROV:
		{
			int idx1 = m->op_1;
			int idx2 = m->op_2;
			//op1
			switch(params[idx1].type){
				case CHAR_TP:
				{
					char *ptr1 = (char *) params[idx1].address;
					//op2
					switch(params[idx2].type){
						case CHAR_TP:
						{
							char *ptr2 = (char *) params[idx2].address;
							char tmp = *ptr1;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case SHORT_TP:
						{
							short *ptr2 = (short *) params[idx2].address;
							char tmp = (char) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						case INT_TP:
						{
							int *ptr2 = (int *) params[idx2].address;
							char tmp = (char) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						case LONG_TP:
						{
							long *ptr2 = (long *) params[idx2].address;
							char tmp = (char) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						default:
						{
							ERRMSG("ERR ROV params[idx2].type ");
							exit(0);
						}
					}//end switch(params[idx2].type)
					break;
				}
				case SHORT_TP:
				{
					short *ptr1 = (short *) params[idx1].address;
					//op2
					switch(params[idx2].type){
						case CHAR_TP:
						{
							char *ptr2 = (char *) params[idx2].address;
							char tmp = (short)*ptr1;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case SHORT_TP:
						{
							short *ptr2 = (short *) params[idx2].address;
							short tmp = *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						case INT_TP:
						{
							int *ptr2 = (int *) params[idx2].address;
							short tmp = (short) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						case LONG_TP:
						{
							long *ptr2 = (long *) params[idx2].address;
							short tmp = (short) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						default:
						{
							ERRMSG("ERR ROV params[idx2].type ");
							exit(0);
						}
					}//end switch(params[idx2].type)
					break;
				}
				case INT_TP:
				{
					int *ptr1 = (int *) params[idx1].address;
					//op2
					switch(params[idx2].type){
						case CHAR_TP:
						{
							char *ptr2 = (char *) params[idx2].address;
							char tmp = (char)*ptr1;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case SHORT_TP:
						{
							short *ptr2 = (short *) params[idx2].address;
							short tmp = *ptr2;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case INT_TP:
						{
							int *ptr2 = (int *) params[idx2].address;
							int tmp = *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						case LONG_TP:
						{
							long *ptr2 = (long *) params[idx2].address;
							int tmp = (int) *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						default:
						{
							ERRMSG("ERR ROV params[idx2].type ");
							exit(0);
						}
					}//end switch(params[idx2].type)
					break;
				}
				case LONG_TP:
				{
					long *ptr1 = (long *) params[idx1].address;
					//op2
					switch(params[idx2].type){
						case CHAR_TP:
						{
							char *ptr2 = (char *) params[idx2].address;
							char tmp = (char)*ptr1;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case SHORT_TP:
						{
							short *ptr2 = (short *) params[idx2].address;
							short tmp = (short) *ptr1;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case INT_TP:
						{
							int *ptr2 = (int *) params[idx2].address;
							int tmp = (int)*ptr2;
							*ptr1 = *ptr2;
							*ptr2 = tmp;
							break;
						}
						case LONG_TP:
						{
							long *ptr2 = (long *) params[idx2].address;
							long tmp = *ptr2;
							*ptr2 = *ptr1;
							*ptr1 = tmp;
							break;
						}
						default:
						{
							ERRMSG("ERR ROV params[idx2].type ");
							exit(0);
						}
					}//end switch(params[idx2].type)				
					break;
				}
				default:
				{
					ERRMSG("ERR ROV params[idx1].type ");
					exit(0);
				}
			}//end switch(params[idx1].type)
			break;
		}
		case ABV:
		{
			int idx = m->op_0;

			switch(params[idx].type){
				case CHAR_TP:
				{
					char *ptr = (char *) params[idx].address;
					*ptr = abs(*ptr);
					break;					
				}
				case SHORT_TP:
				{
					short *ptr = (short *) params[idx].address;
					*ptr = abs(*ptr);
					break;
				}
				case INT_TP:
				{
					int *ptr = (int *) params[idx].address;
					*ptr = abs(*ptr);
					break;
				}
				case LONG_TP:
				{
					long *ptr = (long *) params[idx].address;
					*ptr = abs(*ptr);
					break;
				}
				default:
				{
					ERRMSG("ERR ABV params[idx].type ");
					exit(0);
				}
			}//end switch(params[idx].type)
			break;
		}
		default:
		{
			ERRMSG("m->type ERR ");
			exit(0);
		}
	}//end switch(m->type)
	return 0;
}//end __accmut__prepare_call

int __accmut__stdcall_i32(){
    return ALLMUTS[MUTATION_ID]->op_2;
}

long __accmut__stdcall_i64(){
	return ALLMUTS[MUTATION_ID]->op_2;
}

void __accmut__stdcall_void(){/*do nothing*/}

//TODO::
//char *__accmut__stdcall_pt(){return 0;}


int __accmut__process_i32_arith(int from, int to, int left, int right){

	int ori = __accmut__cal_i32_arith(ALLMUTS[to]->sop , left, right);
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
	
	int mut_res;

	switch(m->type){
		case LVR:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i32_arith(m->sop, m->op_2, right);
			}else{
				mut_res = __accmut__cal_i32_arith(m->sop, left, m->op_2);
			}
			break;
		}
		case UOI:
		{
			if(m->op_1 == 0){
				int u_left;
				if(m->op_2 == 0){
					u_left = left + 1;
				}else if(m->op_2 == 1){
					u_left = left - 1;
				}else if(m->op_2 == 2){
					u_left = 0 - left;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i32_arith(m->sop, u_left, right);
			}else{
				int u_right;
				if(m->op_2 == 0){
					u_right = right + 1;
				}else if(m->op_2 == 1){
					u_right = right - 1;
				}else if(m->op_2 == 2){
					u_right = 0 - right;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i32_arith(m->sop, left, u_right);
			}
			break;
		}
		case ROV:
		{
			mut_res = __accmut__cal_i32_arith(m->sop , right, left);
			break;
		}
		case ABV:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i32_arith(m->sop, abs(left), right);
			}else{
				mut_res = __accmut__cal_i32_arith(m->sop, left, abs(right) );
			}
			break;
		}		
		case AOR:
		case LOR:
		{
			mut_res = __accmut__cal_i32_arith(m->op_0, left, right);
			break;
		}

		default:
		{
			ERRMSG("m->type ERR ");
			exit(0);
		}
	}//end switch
	return mut_res;
}//end __accmut__process_i32_arith

long __accmut__process_i64_arith(int from, int to, long left, long right){

	long ori = __accmut__cal_i64_arith(ALLMUTS[to]->sop , left, right);
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
	
	long mut_res;

	switch(m->type){
		case LVR:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i64_arith(m->sop, m->op_2, right);
			}else{
				mut_res = __accmut__cal_i64_arith(m->sop, left, m->op_2);
			}
			break;
		}
		case UOI:
		{
			if(m->op_1 == 0){
				long u_left;
				if(m->op_2 == 0){
					u_left = left + 1;
				}else if(m->op_2 == 1){
					u_left = left - 1;
				}else if(m->op_2 == 2){
					u_left = 0 - left;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i64_arith(m->sop, u_left, right);
			}else{
				long u_right;
				if(m->op_2 == 0){
					u_right = right + 1;
				}else if(m->op_2 == 1){
					u_right = right - 1;
				}else if(m->op_2 == 2){
					u_right = 0 - right;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i64_arith(m->sop, left, u_right);
			}
			break;
		}
		case ROV:
		{
			mut_res = __accmut__cal_i64_arith(m->sop , right, left);
			break;
		}
		case ABV:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i64_arith(m->sop, abs(left), right);
			}else{
				mut_res = __accmut__cal_i64_arith(m->sop, left, abs(right) );
			}
			break;
		}		
		case AOR:
		case LOR:
		{
			mut_res = __accmut__cal_i64_arith(m->op_0, left, right);
			break;
		}

		default:
		{
			ERRMSG("m->type ERR ");
			exit(0);
		}
	}//end switch
	return mut_res;
}//end __accmut__process_i64_arith

int __accmut__process_i32_cmp(int from, int to, int left, int right){

	int s_pre = ALLMUTS[to]->op_1;

	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		int ori = __accmut__cal_i32_bool(s_pre , left, right);	
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
	
	int mut_res;

	switch(m->type){	
		case LVR:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i32_bool(s_pre, m->op_2, right);
			}else{
				mut_res = __accmut__cal_i32_bool(s_pre, left, m->op_2);
			}
			break;
		}
		case UOI:
		{
			if(m->op_1 == 0){
				int u_left;
				if(m->op_2 == 0){
					u_left = left + 1;
				}else if(m->op_2 == 1){
					u_left = left - 1;
				}else if(m->op_2 == 2){
					u_left = 0 - left;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i32_bool(s_pre, u_left, right);
			}else{
				int u_right;
				if(m->op_2 == 0){
					u_right = right + 1;
				}else if(m->op_2 == 1){
					u_right = right - 1;
				}else if(m->op_2 == 2){
					u_right = 0 - right;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i32_bool(s_pre, left, u_right);
			}			
			break;
		}
		case ROV:
		{
			mut_res = __accmut__cal_i32_bool(s_pre , right, left);
			break;
		}
		case ABV:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i32_bool(s_pre, abs(left), right);
			}else{
				mut_res = __accmut__cal_i32_bool(s_pre, left, abs(right) );
			}
			break;
		}
		case ROR:
		{
			mut_res = __accmut__cal_i32_bool(m->op_2, left, right);
			break;
		}
		default:
			ERRMSG("m->type ERR ");
			exit(0);
	}//end switch
	
	return mut_res;
}//end __accmut__process_i32_cmp

int __accmut__process_i64_cmp(int from, int to, long left, long right){
	
	int s_pre = ALLMUTS[to]->op_1;

	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		int ori = __accmut__cal_i64_bool(s_pre , left, right);	
		return ori;
	}
	
	Mutation *m = ALLMUTS[MUTATION_ID];
	
	int mut_res;

	switch(m->type){	
		case LVR:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i64_bool(s_pre, m->op_2, right);
			}else{
				mut_res = __accmut__cal_i64_bool(s_pre, left, m->op_2);
			}
			break;
		}
		case UOI:
		{
			if(m->op_1 == 0){
				long u_left;
				if(m->op_2 == 0){
					u_left = left + 1;
				}else if(m->op_2 == 1){
					u_left = left - 1;
				}else if(m->op_2 == 2){
					u_left = 0 - left;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i32_bool(s_pre, u_left, right);
			}else{
				long u_right;
				if(m->op_2 == 0){
					u_right = right + 1;
				}else if(m->op_2 == 1){
					u_right = right - 1;
				}else if(m->op_2 == 2){
					u_right = 0 - right;
				}else{
					ERRMSG("UOI ERR ");
					exit(0);
				}
				mut_res = __accmut__cal_i64_bool(s_pre, left, u_right);
			}			
			break;
		}
		case ROV:
		{
			mut_res = __accmut__cal_i64_bool(s_pre , right, left);
			break;
		}
		case ABV:
		{
			if(m->op_0 == 0){
				mut_res = __accmut__cal_i64_bool(s_pre, abs(left), right);
			}else{
				mut_res = __accmut__cal_i64_bool(s_pre, left, abs(right) );
			}
			break;
		}
		case ROR:
		{
			mut_res = __accmut__cal_i64_bool(m->op_2, left, right);
			break;
		}
		default:
			ERRMSG("m->type ERR ");
			exit(0);
	}//end switch

	return mut_res;
}//end __accmut__process_i64_cmp


int __accmut__prepare_st_i32(int from, int to, int tobestore, int *addr){
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		*addr = tobestore;
		return 0;
	}

	Mutation *m = ALLMUTS[MUTATION_ID];

	if(m->type == STD){
		return 1;
	}

	switch(m->type){
		case LVR:
		{
			tobestore = m->op_2;
			break;
		}
		case UOI:
		{
			int uoi_tp = m->op_2;
			if(uoi_tp == 0){
				tobestore = tobestore + 1;
			}else if(uoi_tp == 1){
				tobestore = tobestore - 1;
			}else if(uoi_tp == 2){
				tobestore = 0 - tobestore;
			}
			break;
		}
		case ABV:
		{
			tobestore = abs(tobestore);
			break;
		}
		default:
			ERRMSG("m->type ERR ");
			exit(0);		
	}//end switch(m->type)
	*addr = tobestore;
	return 0;
}

int __accmut__prepare_st_i64(int from, int to, long tobestore, long* addr){
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		*addr = tobestore;
		return 0;
	}

	Mutation *m = ALLMUTS[MUTATION_ID];

	if(m->type == STD){
		return 1;
	}

	switch(m->type){
		case LVR:
		{
			tobestore = m->op_2;
			break;
		}
		case UOI:
		{
			int uoi_tp = m->op_2;
			if(uoi_tp == 0){
				tobestore = tobestore + 1;
			}else if(uoi_tp == 1){
				tobestore = tobestore - 1;
			}else if(uoi_tp == 2){
				tobestore = 0 - tobestore;
			}
			break;
		}
		case ABV:
		{
			tobestore = abs(tobestore);
			break;
		}
		default:
			ERRMSG("m->type ERR ");
			exit(0);		
	}//end switch(m->type)
	*addr = tobestore;
	return 0;
}

void __accmut__std_store(){/*donothing*/}


#endif
