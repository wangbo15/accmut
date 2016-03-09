#ifndef ACCMUT_SMA_EVAL_H
#define ACCMUT_SMA_EVAL_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAXSIZE 2000
#define MAXCALLTIME 200
///?????
#define UNDEF 0xBEEFDEAD

typedef struct Expression {
    int id;
    char type;
    int op;
    int larg;
    int rarg;
    int isTop;	// TODO:: rename to isButton ?
    int root;
}Expression;

typedef struct Mutation {
    int exp;
    int id;
    char type[4];
    int src;
    int tar;
    int index;// for LVR index
    int flag;
}Mutation;

int EXP_COUNT = 0;
int MUT_COUNT = 0;

Expression exps[MAXSIZE];
Mutation muts[MAXSIZE];

int parent[MAXSIZE];
int flag[MAXSIZE];

int eval_time[MAXSIZE]; // The eval times of an expression
int eval_value[MAXSIZE][MAXSIZE][MAXCALLTIME]; // The eval value of an expression at a mutation each time


int __accmut__cal_i32_arith(int op, int a, int b){// TODO:: add float point
	switch(op){
		case 14: return a + b;
		case 16: return a - b;
		case 18: return a * b;
		case 20: {
			if(b == 0)
				return UNDEF;
			else
				return ((unsigned)a) / ((unsigned)b);	
			}
		case 21: {
			if(b==0)
				return UNDEF;
			else
				return a / b;
			}
		case 23: {
			if(b==0)
				return UNDEF;
			else
				return ((unsigned)a) % ((unsigned)b);
			}
		case 24: {
			if(b==0)
				return UNDEF;
			else
				return a % b;
			}
		case 26: return a << b;
		case 27: return ((unsigned)a) >> ((unsigned)b);
		case 28: return a >> b;
		case 29: return a & b;
		case 30: return a | b;
		case 31: return a ^ b;
		default:
			fprintf(stderr, "ERROR : __accmut__cal_i32_arith !!!\n");
			exit(0);
	}
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
			fprintf(stderr, "ERROR : __accmut_cal_i32_bool !!!\n");
			exit(0);
	}
}

int __accmut__eval_i32(int id, int left, int right){
    int origin_op = exps[id].op;
    char tp =  exps[id].type;
    int lid = exps[id].larg;
    int rid = exps[id].rarg;
    int ori;
    if(tp == 'a'){
    	ori = __accmut__cal_i32_arith(origin_op, left, right); //opcode, left, right
    }else if(tp == 'c'){
    	ori = __accmut__cal_i32_bool(origin_op, left, right); // pre, left, right
    }else{
    	fprintf(stderr, "ERROR TYPE --> EXPRID: %d TP: %c \n", id, tp);
    	exit(0);
    }
    int mid;
	int lval, rval;
    for(mid = 1; mid <= MUT_COUNT; mid++){
		if(lid == 0) {
			lval = left;
		}else{
			lval = eval_value[lid][mid][eval_time[lid]-1]; 
		}
		if(rid == 0) {
			rval = right;
		} else {
			rval = eval_value[rid][mid][eval_time[rid]-1];
		}    
		int new_value = UNDEF;
		if(muts[mid].exp == id){		
			if(!strcmp(muts[mid].type, "LVR")){
				int tar = muts[mid].tar;
				if(muts[mid].index == 0){// first operand
					if(tp == 'a'){
						new_value = __accmut__cal_i32_arith(origin_op, tar, rval); //opcode, tar, right
					}else if(tp == 'c'){
						new_value = __accmut__cal_i32_bool(origin_op, tar, rval); // pre, tar, right
					}else{
						fprintf(stderr, "ERROR TYPE --> EXPRID: %d TP: %c \n", id, tp);
						exit(0);
					}
				}else{//second operand
					if(tp == 'a'){
						new_value = __accmut__cal_i32_arith(origin_op, lval, tar); //opcode, left, tar
					}else if(tp == 'c'){
						new_value = __accmut__cal_i32_bool(origin_op, lval, tar); // pre, left, tar
					}else{
						fprintf(stderr, "ERROR TYPE --> EXPRID: %d TP: %c \n", id, tp);
						exit(0);
					}				
				}
			}else{
				int tar = muts[mid].tar;
				if(tp == 'a'){
					new_value = __accmut__cal_i32_arith(tar, lval, rval); //opcode, left, right
				}else if(tp == 'c'){
					new_value = __accmut__cal_i32_bool(tar, lval, rval); // pre, left, right
				}else{
					fprintf(stderr, "ERROR TYPE --> EXPRID: %d TP: %c \n", id, tp);
					exit(0);
				}				
			}
			
				
		}else{	//?????
			if(tp == 'a'){
				new_value = __accmut__cal_i32_arith(origin_op, lval, rval); //opcode, left, right
			}else if(tp == 'c'){
				new_value = __accmut__cal_i32_bool(origin_op, lval, rval); // pre, left, right
			}else{
				fprintf(stderr, "ERROR TYPE --> EXPRID: %d TP: %c \n", id, tp);
				exit(0);
			}					
		}
		
		//fprintf(stderr, "EXPR: %d\tMID: %d\tTAR: %d\tTIME: %d\tL: %d\tR: %d\tNWVALUE: %d\n", id, mid, muts[mid].tar, eval_time[id], lval, rval,  new_value);
        eval_value[id][mid][eval_time[id]] = new_value; // set new value of a mutation
    }//end for
    
    eval_value[id][0][eval_time[id]] = ori;
    if(eval_time[id] < MAXCALLTIME-1){
    	eval_time[id] += 1;
    }
	return ori;
}

void __accmut__init(){
	//load expr.txt
	char path[100];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/expr.txt");
	FILE * file = fopen(path, "r");
	if(file == NULL){
		fprintf(stderr, "FILE ERR : %s\n", path);
		exit(0);
	}
	int count = 1;
    while(fscanf(file, "%d %c %d %d %d\n", &exps[count].id, &exps[count].type, &exps[count].op, &exps[count].larg, &exps[count].rarg) != EOF) {
    	++count;
    }
    EXP_COUNT = count - 1;
    fclose(file);
    //load mut.txt
    strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/mut.txt");
	file = fopen(path, "r");
	if(file == NULL){
		fprintf(stderr, "FILE ERR : %s\n", path);
		exit(0);
	}
	char buff[50];	
	char tail[20];
	count = 1;
	while(fgets(buff, 50, file)){
		sscanf(buff, "%d:%d:%3s:%s", &muts[count].exp, &muts[count].id, muts[count].type, tail);
		//fprintf(stderr, "TYPE : %s\tTAIL %s\n", muts[count].type, tail);
		if(!strcmp(muts[count].type, "AOR")){
			sscanf(tail, "%d:%d", &muts[count].src, &muts[count].tar);
		}else if(!strcmp(muts[count].type, "LVR")){
			sscanf(tail, "%*d:%d:%d:%d", &muts[count].index, &muts[count].src, &muts[count].tar);
		}else if(!strcmp(muts[count].type, "ROR")){
			sscanf(tail, "%*d:%d:%d", &muts[count].src, &muts[count].tar);
		}else if(!strcmp(muts[count].type, "LOR")){
			sscanf(tail, "%d:%d", &muts[count].src, &muts[count].tar);
		}else{
			fprintf(stderr, "MUT TYPE ERR @  __accmut__sma_init() : %s\n", muts[count].type);
			exit(0);
		}
		count++;
	}
	MUT_COUNT = count - 1;
	fclose(file);
	//
	int i;
    for(i = 1; i <= EXP_COUNT; ++i) {
        exps[i].isTop = 1;
    }
    for(i = 1; i <= EXP_COUNT; ++i) {
        int lid = exps[i].larg;
        int rid = exps[i].rarg;
        if(lid != 0) {
            exps[lid].isTop = 0;
        }
        if(rid != 0) {
            exps[rid].isTop = 0;
        }
    }
    /*
    fprintf(stderr, "---------------- DUMP INIT ------------\n");
    for(i = 1; i <= EXP_COUNT; ++i) {
        fprintf(stderr, "EXPR: %d %c %d %d %d \n", exps[i].id, exps[i].type, exps[i].op, exps[i].larg, exps[i].rarg); 
    }
    for(i = 1; i <= MUT_COUNT; ++i) {
        fprintf(stderr, "MUT: %d %d %s %d %d %d\n", muts[i].exp, muts[i].id, muts[i].type, muts[i].src, muts[i].tar, muts[i].index);
    }
    fprintf(stderr, "EXP_COUNT: %d MID_CNT: %d\n", EXP_COUNT, MUT_COUNT);
    fprintf(stderr, "---------------- DUMP INIT END ------------\n");*/
}

int __accmut__equal_mut(mid1, mid2){
	int eid;
	for(eid = 1; eid <= EXP_COUNT; eid++){
		if(!exps[eid].isTop){
			continue; // only compare top expression
		}
		int k;
		for(k = 0; k < eval_time[eid]; k++){
            if(eval_value[eid][mid1][k] != eval_value[eid][mid2][k]) {
                return 0;
            }			
		}
	}
	return 1;
}

void __accmut__init_set(){
    int i;
    for(i = 0; i <= MUT_COUNT; ++i) {
        parent[i] = i;
        flag[i] = 0;
    }
}

void __accmut__union_set(int i, int j) {
    parent[j] = i;
}

int __accmut__find_set(int k) {
    while(parent[k] != k) {
        k = parent[k];
    }
    return k;
}

void __accmut__output_set() {
    //fprintf(stderr, "SET NUM %d\n", MUT_COUNT);
    int i;
    for(i = 0; i <= MUT_COUNT; ++i) {
        int root = __accmut__find_set(i);
        fprintf(stderr, "%d:%d\n", muts[i].id, muts[root].id);
        flag[root] = 1;
    }
}

void __accmut__eval_analysis(){
	__accmut__init_set();
	int i, j;
    for(i = 0; i < MUT_COUNT; ++i) {
        for(j = i+1; j <= MUT_COUNT; ++j) {
            if(__accmut__equal_mut(i, j)) {
                __accmut__union_set(i, j);
            }
        }
    }
	__accmut__output_set();
}

#endif

