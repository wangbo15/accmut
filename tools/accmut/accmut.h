#ifndef ACCMUT_H
#define ACCMUT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>

#define MAXMUTNUM 10000
#define MAX_MUTS_PER_INST 32

int MUTSNUM = 0;

//TODO : change to bit set ?
int *I = NULL;	// a set of mutation IDs represented by the current process.
//int ISIZE = 0;

typedef enum MTYPE{
	AOR,
	LOR,
	COR,
	ROR,
	SOR,
	STD,
	LVR
}MType;

typedef struct Mutation{
	MType type;
	int op;
	//for AOR, LOR
	int t_op;
	//for ROR
	int s_pre;
	int t_pre;
	//for STD
	int f_tp;
	//for LVR
	int op_index;
	long s_con;		// TODO: 'long' is enough ?
	long t_con;
}Mutation;

Mutation* ALLMUTS[MAXMUTNUM + 1];
int MUTATION_ID = 0;

void __accmut__init(){
	//get MUTSNUM
	FILE *fp = NULL;
    char numpath[] ="/home/nightwish/tmp/accmut/mutsnum.txt";	// TODO: change to getenv()
	fp = fopen(numpath, "r");
    fscanf(fp, "%d", &MUTSNUM);
    fclose(fp);
	//for *I
	I = (int *)malloc(sizeof(int)*(MUTSNUM + 1));
	if(I == NULL){
		printf("MALLOC ERROR!\n");
		exit(0);
	}
	memset(I, 1, sizeof(int)*(MUTSNUM + 1));
	//ISIZE = MUTSNUM + 1;

	//init ALLMUTS
	char path[]="/home/nightwish/tmp/accmut/mutations.txt";	
	fp = fopen(path, "r");
	if(fp == NULL){
		printf("FILE ERROR: mutation.txt can not open !!!\n");
		exit(0);
	}
	int id;	
	char type[4];
	char buff[50];	
	char useless[20];
	char tail[20];
	while(fgets(buff, 50, fp)){
		printf("%s", buff);
		sscanf(buff, "%d:%3s:%*[^:]:%*[^:]:%s", &id, type, tail);
		printf("%d -- %s --  %s\n", id, type, tail);
		Mutation* m = (Mutation *)malloc(sizeof(Mutation));
		if(!strcmp(type, "AOR")){
			m->type = AOR;
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->op = s_op;
			m->t_op = t_op;

		}else if(!strcmp(type, "LOR")){
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->op = s_op;
			m->t_op = t_op;
		}else if(!strcmp(type, "ROR")){
			int op, s_pre, t_pre;
			sscanf(tail, "%d:%d:%d", &op, &s_pre, &t_pre);
			m->op = op;
			m->s_pre = s_pre;
			m->t_pre = t_pre;
		}else if(!strcmp(type, "STD")){
			int op, f_tp;
 			sscanf(tail, "%d:%d", &op, &f_tp);
			m->op = op;
			m->f_tp=f_tp;
		}else if(!strcmp(type, "LVR")){
			int op, op_i;
			long s_c, t_c;
			sscanf(tail, "%d:%d:%ld:%ld", &op, &op_i, &s_c, &t_c);
			m->op = op;
			m->op_index = op_i;
			m->s_con = s_c;
			m->t_con = t_c;
		}	
		ALLMUTS[id] = m;
	}
    fclose(fp);
	printf("######### INIT END, %d MUTS LOADED ####\n", MUTSNUM);
}

int __accmut__state_changed(){
	
	return 0;
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

///////

int __accmut__cal_i32_arith(int op, int a, int b){// TODO:: add float point
	switch(op){
		case 14: return a + b;
		case 16: return a - b;
		case 18: return a * b;
		case 20: return ((unsigned)a) / ((unsigned)b);
		case 21: return a / b;
		case 23: return ((unsigned)a) % ((unsigned)b);
		case 24: return a % b;
		case 26: return a << b;
		case 27: return ((unsigned)a) >> ((unsigned)b);
		case 28: return a >> b;
		case 29: return a & b;
		case 30: return a | b;
		case 31: return a ^ b;
		default:
			printf("ERROR : __accmut__cal_i32_arith !!!\n");
			exit(0);
	}
}

long __accmut__cal_i64_arith(int op, long a, long b){// TODO:: add float point
	switch(op){
		case 14: return a + b;
		case 16: return a - b;
		case 18: return a * b;
		case 20: return ((unsigned long)a) / ((unsigned long)b);
		case 21: return a / b;
		case 23: return ((unsigned long)a) % ((unsigned long)b);
		case 24: return a % b;
		case 26: return a << b;
		case 27: return ((unsigned long)a) >> ((unsigned long)b);
		case 28: return a >> b;
		case 29: return a & b;
		case 30: return a | b;
		case 31: return a ^ b;
		default:
			printf("ERROR : __accmut__cal_i64_arith !!!\n");
			exit(0);
	}
}


int __accmut__fork(int mutid){
	pid_t pid = 0;
	if(MUTATION_ID == 0){//in the father process
		pid = fork();
		if(pid < 0){
			printf("FORK ERROR !!!\n");
			exit(0);
		}else if(pid == 0){
			
		}else{
			MUTATION_ID = mutid;
		}
	}
	return pid;
}

int __accmut__process_i32_arith(int from, int to, int left, int right){
	int ori = __accmut__cal_i32_arith(ALLMUTS[from]->op , left, right);
	//printf("ORIG RESULT : i32 arith %d\n", ori);
	/*int i;
	for(i = from; i <= to; i++){// to + 1 or to ?
		Mutation *m = ALLMUTS[i];
		int mut_res = __accmut__cal_i32_arith(m->t_op, left, right);
		if(ori != mut_res){
			pid_t pid = __accmut__fork(i);
			if(pid > 0){
				return mut_res;
			}
		}
	}*/	
	return ori;
}

long __accmut__process_i64_arith(int from, int to, long left, long right){
	long ori = __accmut__cal_i64_arith(ALLMUTS[from]->op , left, right);
	//printf("ORIG RESULT : i64 arith %ld\n", ori);
	/*int i;
	for(i = from; i <= to; i++){// to + 1 or to ?
		Mutation *m = ALLMUTS[i];
		long mut_res = __accmut__cal_i64_arith(m->t_op, left, right);
		if(ori != mut_res){
			pid_t pid = __accmut__fork(i);
			if(pid > 0){
				return mut_res;
			}
		}
	}*/	
	return ori;
}


int __accmut__cal_i32_bool(int pre, int a, int b){
	switch(pre){
		case 32: return a == b;
		case 33: return a != b;
		case 34: return ((unsigned) a) > ((unsigned) b);
		case 35: return ((unsigned) a) >= ((unsigned) b);
		case 36: return ((unsigned) a) < ((unsigned) b);
		case 37: return ((unsigned) a) <= ((unsigned) b);
		case 38: return a > b;
		case 39: return a >= b;
		case 40: return a < b;
		case 41: return a <= b;
		default:
			printf("ERROR : __accmut_cal_i32_bool !!!\n");
			exit(0);
	}
}

int __accmut__cal_i64_bool(int pre, long a, long b){
	switch(pre){
		case 32: return a == b;
		case 33: return a != b;
		case 34: return ((unsigned long) a) > ((unsigned long) b);
		case 35: return ((unsigned long) a) >= ((unsigned long) b);
		case 36: return ((unsigned long) a) < ((unsigned long) b);
		case 37: return ((unsigned long) a) <= ((unsigned long) b);
		case 38: return a > b;
		case 39: return a >= b;
		case 40: return a < b;
		case 41: return a <= b;
		default:
			printf("ERROR : __accmut__cal_i64_bool !!!\n");
			exit(0);
	}
}

int __accmut__process_i32_cmp(int from, int to, int left, int right){
	int i;
	int len = to - from + 2;//all mutations(from - to + 1) and original code
	int V[len] = {0};// TODO:: change to bit operate
	int VSIZE = 0;
	// filter_variants(V, I)
	if(I[0] > 0){//check if the origninal is enabled for this mutation
		V[0] = 1;
		VSIZE++;
	}
    int tmp;
	for(i = from; i <= to; i++){
		if(I[i] > 0){
			V[i - from + 1] = 1;
            tmp = i; // TODO::check
			VSIZE++;
		}
	}
	if(VSIZE == 1){ // at the orig v or at a certain mut
        if(V[0] == 1){
		    int ori = __accmut__cal_i32_bool(ALLMUTS[from]->s_pre, left, right); // TODO :: right ?
            printf("__accmut__process_i32_cmp ORIG RESULT : %d\n", ori);	
            return ori;
        }else{
            int muted = __accmut__cal_i32_bool(ALLMUTS[tmp]->t_pre, left, right);
            return muted;
        }
	}
	int V_RES[len]; 
    int V_RES_CLS[len];
    if(V[0] == 1){
        V_RES[0] = __accmut__cal_i32_bool(ALLMUTS[from]->s_pre, left, right);// TODO:: orig version can be omitted?
        V_RES_CLS[0] = 0;
    }
    //compute each variant and classcify
	for(i = from; i <= to; i++){
        if(V[i - from + 1] == 0){
            continue;
        }
		Mutation *m = ALLMUTS[i];
		int mut_res = __accmut__cal_i32_bool(m->t_pre, left, right); // TODO: reduction ?
		V_RES[i - from + 1] = mut_res;
        int j;
        for(j = 0; j <= i - from; j++){// TODO ::check
            if(V_RES[j] == mut_res){
                V_RES_CLS[i - from + 1] = j; 
                break;
            }
        }
        if(j == i - from + 1){
            V_RES_CLS[j] = j;
        }
	} 
    //select the minnum as Xcur
    int xcur;
    for(i = 0; i < len; i++){
    	if(V_RES_CLS[i] == i){
    		xcur = i;
    	}
    }
    
	for(i = xcur + 1; i < len; i++){
		if(V_RES_CLS[i] == i){ // the header of a eqiu class
			pid_t pid = fork(); 
			if(pid < 0){
				printf("FORK ERR!!! @__accmut__process_i32_cmp  FROM: %d TO: %d\n", from, to);
			}else if(pid == 0){ // child process
				//filter(I, V)
                memset(I, 0, sizeof(int)*(MUTSNUM + 1));
                I[i + from - 1] = 1;
                int j;
                for(j = i + 1; j < len; j++){
                   if(V_RES_CLS[i] == V_RES_CLS[j]){
                    I[j + from - 1] = 1;
                   } 
                }
				return V_RES[i];
			}else{// fater process
				waitpid(pid); // TODO:: add timer
			} 
		} 
    }
    //
    for(i = xcur + 1; i < len; i++){
    	if(V_RES_CLS[i] == xcur){
    		I[i + from - 1] = 1;	// TODO::check
    	}
    }
    
    return V_RES[xcur];
}

int __accmut__process_i64_cmp(int from, int to, long left, long right){
	long ori = __accmut__cal_i64_bool(ALLMUTS[from]->s_pre, left, right);	
	//printf("__accmut__process_i64_cmp ORIG RESULT : %d\n", ori);
	/*int i;
	for(i = from; i <= to; i++){
		Mutation *m = ALLMUTS[i];
		int mut_res = __accmut__cal_i64_bool(m->t_pre, left, right);
		if(ori != mut_res){
			pid_t pid =	__accmut__fork(i);	
			if(pid > 0){
				return mut_res;
			}
		}
	}*/
	return ori;
}

void __accmut__process_st_i32(int from, int to, int *addr){
	//orig
	*addr = ALLMUTS[from]->s_con;
/*	int i;
	for(i = from; i <= to; i++){
			
	}*/
}

void __accmut__process_st_i64(int from, int to, long *addr){
	//orig	
	*addr = ALLMUTS[from]->s_con;
	/*int i;
	for(i = from; i <= to; i++){
			
	}*/
}

/*
int main(){
	__accmut__init();
	int i;
	for(i = 1; i < MAXMUTNUM+1; i++){
		if(ALLMUTS[i] != NULL ){
			printf("%d-%d\n", i, ALLMUTS[i]->op);
		}
	}
	return 0;
}
*/

#endif
