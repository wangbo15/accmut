#ifndef ACCMUT_SMA_EVAL_H
#define ACCMUT_SMA_EVAL_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include <accmut/accmut_arith_common.h>

#define MAXINDEXNUM MAXMUTNUM

#define MAX_COV_TIME 1

int COVERED_LOCATIONS[MAXINDEXNUM];

long ORIRES[MAXINDEXNUM];

long MUTRES[MAXMUTNUM];

int UNSUPORTED[MAXMUTNUM];

int PARENT[MAXMUTNUM];


/**************************** ARITH ***************************************/

int __accmut__process_i32_arith(int from, int to, int left, int right){

	int ori = __accmut__cal_i32_arith(ALLMUTS[to]->sop , left, right);

    //printf("ARI FROM: %d  TO: %d  SPRE : %d  ORI: %d\n", from, to, ALLMUTS[to]->sop, ori);
    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] > 0){
        return ori;
    }
    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = ori;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
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
            case ROV:
            {
                mut_res = __accmut__cal_i32_arith(m->sop , right, left);
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

            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i

    return ori;
}// end __accmut__process_i32_arith

long __accmut__process_i64_arith(int from, int to, long left, long right){
    
    int ori = __accmut__cal_i64_arith(ALLMUTS[to]->sop , left, right);
    
    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] > 0){
        return ori;
    }
    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = ori;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        int mut_res;
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
            case ROV:
            {
                mut_res = __accmut__cal_i64_arith(m->sop , right, left);
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

            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i

    return ori;
}// end __accmut__process_i64_arith


/**************************** ICMP ***************************************/
int __accmut__process_i32_cmp(int from, int to, int left, int right){

    int spre = ALLMUTS[to]->op_1;

    int ori = __accmut__cal_i32_bool(spre , left, right);

    //printf("CMP FROM: %d  TO: %d  SPRE: %d  ORI: %d\n", from, to, ALLMUTS[to]->sop, ori);

    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] > 0){
        return ori;
    }
    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = ori;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        int mut_res;
        switch(m->type){
            case LVR:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i32_bool(spre, m->op_2, right);
                }else{
                    mut_res = __accmut__cal_i32_bool(spre, left, m->op_2);
                }
                break;
            }
            case ROV:
            {
                mut_res = __accmut__cal_i32_bool(spre , right, left);
                break;
            } 
            case ROR:
            {
                mut_res = __accmut__cal_i32_bool(m->op_2, left, right);
                break;
            }
            default:
            {

            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i
    return ori;
}//end __accmut__process_i32_cmp

int __accmut__process_i64_cmp(int from, int to, long left, long right){

    int spre = ALLMUTS[to]->op_1;

    int ori = __accmut__cal_i64_bool(spre , left, right);

    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] > 0){
        return ori;
    }
    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = ori;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        int mut_res;
        switch(m->type){
            case LVR:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i64_bool(spre, m->op_2, right);
                }else{
                    mut_res = __accmut__cal_i64_bool(spre, left, m->op_2);
                }
                break;
            }
            case ROV:
            {
                mut_res = __accmut__cal_i64_bool(spre , right, left);
                break;
            } 
            case ROR:
            {
                mut_res = __accmut__cal_i32_bool(m->op_2, left, right);
                break;
            }
            default:
            {
 
            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i
    return ori;
}// end __accmut__process_i64_cmp

/**************************** CALL and Store ***************************************/

int __accmut__prepare_call(int from, int to, int opnum, ...){
    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] == 0){
        COVERED_LOCATIONS[idx]++;
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


int __accmut__prepare_st_i32(int from, int to, int tobestore, int* addr){
    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] == 0){
        COVERED_LOCATIONS[idx]++;
    }
    *addr = tobestore;
    return 0;
}

int __accmut__prepare_st_i64(int from, int to, long tobestore, long* addr){
    int idx = ALLMUTS[to]->index;
    if(COVERED_LOCATIONS[idx] == 0){
        COVERED_LOCATIONS[idx]++;
    }
    *addr = tobestore;
    return 0;
}

void __accmut__std_store(){/*donothing*/}


/*************************************************************************/

static void __accmut__init_set(){
    int i;
    for(i = 0; i <= MUT_NUM; ++i) {
        PARENT[i] = i;
    }
}

static void __accmut__union_set(int i, int j) {
    PARENT[j] = i;
}

static int __accmut__find_set(int k) {
    while(PARENT[k] != k) {
        k = PARENT[k];
    }
    return k;
}

static void __accmut__output_set() {
    __accmut__init_set();

    // int i , j;
    // for(i = 1; i <= MUT_NUM; i++){
    //     int loci = ALLMUTS[i]->index;

    //     printf("%d => LOC: %d , COVED: %d , ORES: %d, MUTRES: %d, UNSUP: %d\n", 
    //         i, loci, COVERED_LOCATIONS[loci], ORIRES[loci], MUTRES[i], UNSUPORTED[i]);
    // }


	char path[256];

	sprintf(path, "%s/tmp/accmut/input/%s/t%d", getenv("HOME"), PROJECT, TEST_ID);	

	FILE* fp = fopen(path ,"w");
    // FILE* fp = stderr;

	if(fp == NULL){
		ERRMSG("FOPEN ERR");
		exit(1);
	}

    for(i = 1; i <= MUT_NUM; ++i) {
        int loci = ALLMUTS[i]->index;

        if(COVERED_LOCATIONS[loci] == 0){
            continue;
        }

        for(j = i; j <= MUT_NUM; j++){
            if(ALLMUTS[j]->index != loci)
                break;
        }

        j--;

        // printf("--- I : %d J : %d\n",i, j);

        int k;

        for(k = i; k <= j; k++){
            if(UNSUPORTED[k] == 1){
                continue;
            }
            if(MUTRES[k] == ORIRES[loci]){
                __accmut__union_set(0, k);
                continue;
            }
            int m;
            for(m = k+1; m <= j; m++){
                if(UNSUPORTED[k] == 1){
                    continue;
                }
                if(MUTRES[m] == MUTRES[k]){
                    __accmut__union_set(k, m);
                }
            }
        }

        i = j;
    }

    fprintf(fp, "0:0\n");

    for(i = 1; i <= MUT_NUM; ++i) {
        int loci = ALLMUTS[i]->index;

        if(COVERED_LOCATIONS[loci] == 0){
            continue;
        }
        if(UNSUPORTED[i] == 1){
            fprintf(fp, "%d:-1\n", i);
        }else{
            int p = __accmut__find_set(i);
            fprintf(fp, "%d:%d\n", i, p);            
        }
    }

    fclose(fp);


    int cbu = 0;
    int pnum = 0;
    for(i = 1; i <= MUT_NUM; i++){
        int loci = ALLMUTS[i]->index;
        if(COVERED_LOCATIONS[loci] == 0){
            continue;
        }
        if(UNSUPORTED[i] == 1){
            cbu++;
            continue;
        }
        int p = __accmut__find_set(i);
        if(p == i){
            pnum++;
        }
    }
    fprintf(stderr, "########## SMA EVAL END ##########\n");
    fprintf(stderr, "TOTAL MUT: %d\n", MUT_NUM);
    fprintf(stderr, "COVED BUT UNSUPORTED: %d\n", cbu);
    fprintf(stderr, "PARENT NUM: %d\n", pnum + cbu);

}

#define CALLSOP 55
#define STSOP 34

void __accmut__init(){

    gettimeofday(&tv_begin, NULL);

    atexit(__accmut__exit_time);
    atexit(__accmut__output_set);

    if(TEST_ID < 0){
        ERRMSG("TEST_ID NOT INIT");
        exit(0);
    }

    __accmut__load_all_muts();

    int i;
    for(i = 1; i <= MUT_NUM; i++){
        if(ALLMUTS[i]->sop == STSOP || ALLMUTS[i]->sop == CALLSOP){
            UNSUPORTED[i] = 1;
        }else if((ALLMUTS[i]->type != LVR) && (ALLMUTS[i]->type != ROV) && (ALLMUTS[i]->type != AOR)
                && (ALLMUTS[i]->type != LOR) && (ALLMUTS[i]->type != ROR)){
            UNSUPORTED[i] = 1;
        }
    }

}

#undef STSOP
#undef CALLSOP
/*************************************************************************/

#endif

