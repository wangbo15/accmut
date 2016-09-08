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

#define MUT_TP_ERR (2)

/**************************** ARITH ***************************************/

int __accmut__process_i32_arith(int from, int to, int left, int right){

    int ori = __accmut__cal_i32_arith(ALLMUTS[to]->sop , left, right);

    //printf("ARI FROM: %d  TO: %d  SPRE : %d  ORI: %d\n", from, to, ALLMUTS[to]->sop, ori);
    int idx = ALLMUTS[to]->location;
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
        int mut_res = i;
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
            case ABV:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i32_arith(m->sop, abs(left), right);
                }else{
                    mut_res = __accmut__cal_i32_arith(m->sop, left, abs(right));
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
                        exit(MUT_TP_ERR);
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
                        exit(MUT_TP_ERR);

                    }
                    mut_res = __accmut__cal_i32_arith(m->sop, left, u_right);
                }
                break;                
            }
              default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i

    return ori;
}// end __accmut__process_i32_arith

long __accmut__process_i64_arith(int from, int to, long left, long right){
    
    int ori = __accmut__cal_i64_arith(ALLMUTS[to]->sop , left, right);
    
    int idx = ALLMUTS[to]->location;
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
        long mut_res = i;
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
            case ABV:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i64_arith(m->sop, labs(left), right);
                }else{
                    mut_res = __accmut__cal_i64_arith(m->sop, left, labs(right));
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
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i64_arith(m->sop, u_left, right);
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
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i64_arith(m->sop, left, u_right);
                }
                break;                
            }            
            default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
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

    int idx = ALLMUTS[to]->location;
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
        int mut_res = i;
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
                        ERRMSG("UOI ERR");
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i32_bool(spre, u_left, right);
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
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i32_bool(spre, left, u_right);
                }           
                break;
            }
            case ABV:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i32_bool(spre, abs(left), right);
                }else{
                    mut_res = __accmut__cal_i32_bool(spre, left, abs(right) );
                }
                break;
            }
            default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i
    return ori;
}//end __accmut__process_i32_cmp

int __accmut__process_i64_cmp(int from, int to, long left, long right){

    int spre = ALLMUTS[to]->op_1;

    int ori = __accmut__cal_i64_bool(spre , left, right);

    int idx = ALLMUTS[to]->location;
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
        long mut_res = i;
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
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i64_bool(spre, u_left, right);
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
                        exit(MUT_TP_ERR);
                    }
                    mut_res = __accmut__cal_i64_bool(spre, left, u_right);
                }       
                break;
            }
            case ABV:
            {
                if(m->op_0 == 0){
                    mut_res = __accmut__cal_i64_bool(spre, labs(left), right);
                }else{
                    mut_res = __accmut__cal_i64_bool(spre, left, labs(right) );
                }
                break;
            }
            default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
            }
        }//end switch
        MUTRES[i] = mut_res;
    }//end for i
    return ori;
}// end __accmut__process_i64_cmp

/**************************** CALL and Store ***************************************/

int __accmut__prepare_call(int from, int to, int opnum, ...){

    int idx = ALLMUTS[to]->location;
    if(COVERED_LOCATIONS[idx] > 0){
        return 0;
    }

    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = idx;

    va_list ap;
    va_start(ap, opnum);

    PrepareCallParam params[MAX_PARAM_NUM]; //max param num limited to 16

    int i;
    for(i = 0; i < opnum; i++){
        short tp_and_idx = va_arg(ap, short);
        int idx = tp_and_idx & 0x00FF;

        params[idx].type = tp_and_idx >> 8;
        params[idx].address = va_arg(ap, unsigned long);

        //fprintf(stderr, "%dth para: %d\n", i, *((int*)params[idx].address));

    }//end for i
    va_end(ap);

    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        long mut_res = 0 - i;

        switch(m->type){
            case UOI:
            {
                int op_idx = m->op_1;
                int uoi_tp = m->op_2;
                
                switch(params[op_idx].type){
                    case CHAR_TP:
                    {
                        char *ptr = (char *) params[op_idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = idx;
                        }
                        break;
                    }
                    case SHORT_TP:
                    {
                        short *ptr = (short *) params[op_idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = idx;
                        }
                        break;
                    }
                    case INT_TP:
                    {
                        int *ptr = (int *) params[op_idx].address;

                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = idx;
                        }
                        break;
                    }       
                    case LONG_TP:
                    {
                        long *ptr = (long *) params[op_idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = idx;
                        }
                        break;
                    }
                    default:
                    {
                        ERRMSG("UOI params[op_idx].type ");
                        exit(MUT_TP_ERR);
                    }
                }//end switch(params[idx].type)
                break;
            }//end case UOI
            case ROV:
            {
                int op_idx1 = m->op_1;
                int op_idx2 = m->op_2;
                //op1
                switch(params[op_idx1].type){
                    case CHAR_TP:
                    {
                        char *ptr1 = (char *) params[op_idx1].address;
                        //op2
                        switch(params[op_idx2].type){
                            case CHAR_TP:
                            {
                                char *ptr2 = (char *) params[op_idx2].address;
                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[op_idx2].address;
                                if(*ptr1 == *ptr2){
                                   mut_res = idx;
                                }                            
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[op_idx2].type ");
                                exit(MUT_TP_ERR);
                            }
                        }//end switch(params[idx2].type)
                        break;
                    }
                    case SHORT_TP:
                    {
                        short *ptr1 = (short *) params[op_idx1].address;
                        //op2
                        switch(params[op_idx2].type){
                            case CHAR_TP:
                            {
                                char *ptr2 = (char *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }                                 
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[op_idx2].type ");
                                exit(MUT_TP_ERR);
                            }
                        }//end switch(params[idx2].type)
                        break;
                    }
                    case INT_TP:
                    {
                        int *ptr1 = (int *) params[op_idx1].address;
                        //op2
                        switch(params[op_idx2].type){
                            case CHAR_TP:
                            {
                                char *ptr2 = (char *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[op_idx2].type ");
                                exit(MUT_TP_ERR);
                            }
                        }//end switch(params[idx2].type)
                        break;
                    }
                    case LONG_TP:
                    {
                        long *ptr1 = (long *) params[op_idx1].address;
                        //op2
                        switch(params[op_idx2].type){
                            case CHAR_TP:
                            {
                                char *ptr2 = (char *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                   mut_res = idx;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }                                    
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[op_idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = idx;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[op_idx2].type ");
                                exit(MUT_TP_ERR);
                            }
                        }//end switch(params[idx2].type)                
                        break;
                    }
                    default:
                    {
                        ERRMSG("ROV params[op_idx1].type ");
                        exit(MUT_TP_ERR);
                    }
                }//end switch(params[idx1].type)
                break;
            }//end case ROV
            case ABV:
            {
                int op_idx = m->op_0;

                switch(params[op_idx].type){
                    case CHAR_TP:
                    {
                        char *ptr = (char *) params[op_idx].address;
                        if(*ptr >= 0){
                            mut_res = idx;
                        }
                        break;                  
                    }
                    case SHORT_TP:
                    {
                        short *ptr = (short *) params[op_idx].address;
                        if(*ptr >= 0){
                            mut_res = idx;
                        }
                        break;
                    }
                    case INT_TP:
                    {
                        int *ptr = (int *) params[op_idx].address;
                        if(*ptr >= 0){
                            mut_res = idx;
                        }
                        break;
                    }
                    case LONG_TP:
                    {
                        long *ptr = (long *) params[op_idx].address;
                        if(*ptr >= 0){
                            mut_res = idx;
                        }
                        break;
                    }
                    default:
                    {
                        ERRMSG("ABV params[op_idx].type ");
                        exit(MUT_TP_ERR);
                    }
                }//end switch(params[idx].type)
                break;
            }//end case ABV
            default:
            {
                ERRMSG("ERR MUT TYPE ");
                exit(MUT_TP_ERR);
            }
        }//end switch(m->type)

        MUTRES[i] = mut_res;

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
    
    *addr = tobestore;

    int idx = ALLMUTS[to]->location;
    if(COVERED_LOCATIONS[idx] > 0){
        return 0;
    }

    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = idx;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        int mut_res = 0 - i;
        
        switch(m->type){
            case UOI:
            {
                int uoi_tp = m->op_2;
                if(tobestore == 0 && uoi_tp == 2){
                    mut_res = idx;
                }
                break;
            }//end case UOI
            case ABV:
            {
                if(tobestore >= 0){//TODO::check signed or unsigned are the same
                    mut_res = idx;
                }
                break;
            }//end case ABV
            default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
            }
        }//end switch(m->type)

        MUTRES[i] = mut_res;
    }
    
    return 0;
}

int __accmut__prepare_st_i64(int from, int to, long tobestore, long* addr){

    *addr = tobestore;

    int idx = ALLMUTS[to]->location;
    if(COVERED_LOCATIONS[idx] > 0){
        return 0;
    }

    COVERED_LOCATIONS[idx]++;
    ORIRES[idx] = idx;

    int i;
    for(i = from; i <= to; ++i) {

        if(UNSUPORTED[i] > 0){
            continue;
        }

        Mutation *m = ALLMUTS[i];
        long mut_res = 0 - i;
        
        switch(m->type){
            case UOI:
            {
                int uoi_tp = m->op_2;
                if(tobestore == 0 && uoi_tp == 2){
                    mut_res = idx;
                }
                break;
            }//end case UOI
            case ABV:
            {
                if(tobestore >= 0){//TODO::check signed or unsigned are the same
                    mut_res = idx;
                }
                break;
            }//end case ABV
            default:
            {
                ERRMSG("ERR MUT TYPE");
                exit(MUT_TP_ERR);
            }
        }//end switch(m->type)

        MUTRES[i] = mut_res;
    }
    
    return 0;
}

void __accmut__std_store(){/*donothing*/}


/*************************************************************************/


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

    int i , j;
    
    // for(i = 1; i <= MUT_NUM; i++){
    //     int loci = ALLMUTS[i]->location;

    //     printf("%d => LOC: %d , COVED: %d , ORES: %d, MUTRES: %d, UNSUP: %d\n", 
    //         i, loci, COVERED_LOCATIONS[loci], ORIRES[loci], MUTRES[i], UNSUPORTED[i]);
    // }


    char path[256];

    sprintf(path, "%s/tmp/accmut/input/%s/t%d", getenv("HOME"), PROJECT, TEST_ID);  

    FILE* fp = fopen(path ,"w");
    // FILE* fp = stderr;

    if(fp == NULL){
        char msg[128] = "FOPEN ERR: ";
        strcat(msg, path);
        ERRMSG(msg);
        exit(1);
    }

    for(i = 1; i <= MUT_NUM; ++i) {

        int loci = ALLMUTS[i]->location;

        if(COVERED_LOCATIONS[loci] == 0){
            continue;
        }

        for(j = i; j <= MUT_NUM; j++){
            if(ALLMUTS[j]->location != loci){
                break;
            }
        }

        j--;

        printf("--- I : %d J : %d\n",i, j);

        int k;

        for(k = i; k <= j; k++){
            printf("%d ORI: %ld , MUT: %ld, UNSUP: %d\n", k, ORIRES[loci], MUTRES[k], UNSUPORTED[k]);
        }

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
        int loci = ALLMUTS[i]->location;

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
    int total_coved = 0;
    for(i = 1; i <= MUT_NUM; i++){
        int loci = ALLMUTS[i]->location;
        if(COVERED_LOCATIONS[loci] == 0){
            continue;
        }else{
            total_coved++;
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
    fprintf(stderr, "TOTAL COVED: %d\n", total_coved);
    fprintf(stderr, "COVED BUT UNSUPORTED: %d\n", cbu);
    fprintf(stderr, "PARENT NUM: %d\n", pnum + cbu);

}

#define CALLSOP 55
#define STSOP 34

void __accmut__init(){

    gettimeofday(&tv_begin, NULL);

    atexit(__accmut__exit_time);
    atexit(__accmut__output_set);

   // __accmut__init_stdstream();

    if(TEST_ID < 0){
        ERRMSG("TEST_ID NOT INIT");
        exit(0);
    }

    __accmut__load_all_muts();

    int i;

    for(i = 1; i <= MUT_NUM; i++){// removing muts within a loop
        if(ALLMUTS[i]->location < 0){
            UNSUPORTED[i] = 1;
            ALLMUTS[i]->location = 0 - ALLMUTS[i]->location;
        }
    }

    for(i = 1; i <= MUT_NUM; i++){

        if(ALLMUTS[i]->sop == STSOP || ALLMUTS[i]->sop == CALLSOP){
            if((ALLMUTS[i]->type == STD) || (ALLMUTS[i]->type == LVR)){
                UNSUPORTED[i] = 1;
            }else if(ALLMUTS[i]->type == UOI){
                if(ALLMUTS[i]->op_2 != 2){
                    UNSUPORTED[i] = 1;
                }
            }
        }
          
        // else if((ALLMUTS[i]->type != LVR) && (ALLMUTS[i]->type != ROV) && (ALLMUTS[i]->type != AOR)
        //         && (ALLMUTS[i]->type != LOR) && (ALLMUTS[i]->type != ROR)){
        //     UNSUPORTED[i] = 1;
        // }
    }

}

#undef STSOP
#undef CALLSOP
/*************************************************************************/
#endif

