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
			//__accmut__setout(id);
			
			int r = setitimer(ITIMER_PROF, &tick, NULL); // TODO:ITIMER_REAL?
			
			if(r < 0){
				fprintf(stderr, "SET TIMMER ERR, M_ID : %d\n", id);
				exit(0);
			}
		}else{//father process	
			totalfork++;
			int pr = waitpid(pid, NULL, 0);

			if(pr < 0){
				fprintf(stderr, "WAITPID ERROR !!!!!!\n");
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


	MUTS_ON = (int *) malloc( (sizeof(int)) * (MAX_MUT_NUM + 1) );
	
	int i;
	for(i = 0; i < MAX_MUT_NUM + 1; i++){
		*(MUTS_ON + i) = 1;
	}
    
 //   	for(i = 1; i < MAX_MUT_NUM + 1; i++){
	// 	__accmut__mainfork(i);
	// }
	

	/*if(MUTATION_ID == 0){
		fprintf(stderr, "%d\n",	totalfork);
	}*/
}

//TYPE BITS OF SIGNATURE
#define CHAR_TP 0
#define SHORT_TP 1
#define INT_TP 2
#define LONG_TP 3

typedef struct PrepareCallParam{
	int type;
	unsigned long address;
}PrepareCallParam;

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

	PrepareCallParam params[16];	//max param num limited to 16

	int i;
	for(i = 0; i < opnum; i++){
		short tp_and_idx = va_arg(ap, short);
		int idx = tp_and_idx & 0x00FF;

		params[idx].type = tp_and_idx >> 8;
		params[idx].address = va_arg(ap, unsigned long);

		//fprintf(stderr, "%dth: %d\n", i, *((int*)ptrs[i]));

	}
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
					fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
					exit(0);
				}
			}

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
					fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
					exit(0);
				}
			}

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
							fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
							exit(0);
						}
					}

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
							fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
							exit(0);
						}
					}

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
							fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
						}
					}

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
							fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
							exit(0);
						}
					}					
					break;
				}
				default:
				{
					fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
					exit(0);
				}
			}

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
					fprintf(stderr, "ERR @__accmut__prepare_call. MID: %d\n", MUTATION_ID);
					exit(0);
				}
			}

			break;
		}
		default:
		{
			fprintf(stderr, "M->TYPE ERR @__accmut__prepare_call. MID: %d MTP: %d\n", MUTATION_ID, m->type);
			exit(0);
		}

	}

	return 0;
}

int __accmut__stdcall_i32(){
    return ALLMUTS[MUTATION_ID]->op_2;
}

long __accmut__stdcall_i64(){
	return ALLMUTS[MUTATION_ID]->op_2;
}

void __accmut__stdcall_void(){/*do nothing*/}


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
					fprintf(stderr, "UOI ERR @__accmut__process_i32_arith. MID: %d\n", MUTATION_ID);
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
					fprintf(stderr, "UOI ERR @__accmut__process_i32_arith. MID: %d\n", MUTATION_ID);
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
			fprintf(stderr, "M->TYPE ERR @__accmut__process_i32_arith. MID: %d MTP: %d\n", MUTATION_ID, m->type);
			exit(0);
		}
	}
	
	return mut_res;
}

long __accmut__process_i64_arith(int from, int to, long left, long right){
	long ori = __accmut__cal_i64_arith(ALLMUTS[to]->sop , left, right);
	return ori;
}

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
					fprintf(stderr, "UOI ERR @__accmut__process_i32_cmp. MID: %d\n", MUTATION_ID);
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
					fprintf(stderr, "UOI ERR @__accmut__process_i32_cmp. MID: %d\n", MUTATION_ID);
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
			fprintf(stderr, "M->TYPE ERR @__accmut__cal_i32_bool. MID: %d MTP: %d\n", MUTATION_ID, m->type);
			exit(0);
	}
	
	return mut_res;    
}

int __accmut__process_i64_cmp(int from, int to, long left, long right){

	int s_pre = ALLMUTS[to]->op_1;
	int ori = __accmut__cal_i64_bool(s_pre, left, right);
	return ori;
}


int __accmut__prepare_st_i32(int from, int to, int* tobestore){
	
	if(MUTATION_ID == 0 || MUTATION_ID < from || MUTATION_ID > to){
		return 0;
	}

	Mutation *m = ALLMUTS[MUTATION_ID];

	if(m->type == STD){
		return 1;
	}

	switch(m->type){
		case LVR:
		{
			*tobestore = m->op_2;
			break;
		}
		case UOI:
		{
			int uoi_tp = m->op_2;
			if(uoi_tp == 0){
				*tobestore = *tobestore + 1;
			}else if(uoi_tp == 1){
				*tobestore = *tobestore - 1;
			}else if(uoi_tp == 2){
				*tobestore = 0 - *tobestore;
			}
			break;
		}
		case ABV:
		{
			*tobestore = abs(*tobestore);
			break;
		}
		default:
			fprintf(stderr, "M->TYPE ERR @__accmut__prepare_st_i32. MID: %d MTP: %d\n", MUTATION_ID, m->type);
			exit(0);		
	}

	return 0;
}

int __accmut__prepare_st_i64(int from, int to, long* addr){
	return 0;
}

void __accmut__std_store(){/*donothing*/}



#endif
