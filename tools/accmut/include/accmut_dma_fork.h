#ifndef ACCMUT_DMA_FORK_H
#define ACCMUT_DMA_FORK_H

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

#include <execinfo.h> //add for debug

#include <accmut/accmut_arith_common.h>


/** Added By Shiyqw **/

#define MMPL (64) //MAX MUT NUM PER LOCATION 

int forked_active_set[MMPL];
int forked_active_num;
int default_active_set[MAXMUTNUM + 1];
int recent_set[MMPL];
int recent_num;

long temp_result[MMPL];

typedef struct Eqclass {
    long value;
    int num;
    int mut_id[MMPL];
} Eqclass;

Eqclass eqclass[MMPL];
int eq_num;

//#undef MMPL

// Algorithms for Dynamic mutation anaylsis 

void __accmut__filter__variant(int from, int to) {
    recent_num = 0;
    int i;
    if (MUTATION_ID == 0) {
        recent_set[recent_num++] = 0;
        for(i = from; i <= to; ++i) {
            if (default_active_set[i] == 1) {
                recent_set[recent_num++] = i;
            }
        }
    } else {
        for(i = 0; i < forked_active_num; ++i) {
            if (forked_active_set[i] >= from && forked_active_set[i] <= to) {
                recent_set[recent_num++] = forked_active_set[i];
            }
        }
        if(recent_num == 0) {
            recent_set[recent_num++] = 0;
        }
    }

}

void __accmut__dump_eqclass(){
    int i;
    for(i = 0; i < eq_num; i++){
        printf("%d'th EQCLS  size: %d  : ", i, eqclass[i].num);
        int j;
        for(j = 0; j < eqclass[i].num; j++){
            printf("%d ", eqclass[i].mut_id[j]);
        }
        printf("\n");
    }
}

#define USING_DIVIDE 1

void __accmut__divide__eqclass() {
    eq_num = 0;
    int i;
    for(i = 0; i < recent_num; ++i) {
        long result = temp_result[i];

        // printf("DIVEQ : MID %d -> TMPRES %d\n", recent_set[i], temp_result[i]);

        int j;
        int flag = 0;

        #if USING_DIVIDE
        for(j = 0; j < eq_num; ++j) {
            if(eqclass[j].value == result) {
                eqclass[j].mut_id[eqclass[j].num++] = recent_set[i];
                flag = 1;
                break;
            }
        }
        #endif

        if (flag == 0) {
            eqclass[eq_num].value = result;
            eqclass[eq_num].num = 1;
            eqclass[eq_num].mut_id[0] = recent_set[i];
            ++eq_num;
        }
    }
    // __accmut__dump_eqclass();
}

/* OPTIMIAZTION FOR DIVIDE EQ CLS */
#define DIV_EQ_CMP USING_DIVIDE

#define DIV_EQ_CL_ST USING_DIVIDE

void __accmut__divide__eqclass_cmp(int onlyhas_0, int onlyhas_1) {

    int i;
    if(onlyhas_0 == 0){

        eqclass[0].num = 0;
        eqclass[0].value = 0;

        for(i = 0; i < recent_num; ++i) {
            eqclass[0].mut_id[eqclass[0].num++] = recent_set[i];
        }
        eq_num = 1;
        return;
    }else if(onlyhas_1 == 1){
        
        eqclass[0].num = 0;
        eqclass[0].value = 1;

        for(i = 0; i < recent_num; ++i) {
            eqclass[0].mut_id[eqclass[0].num++] = recent_set[i];
        }
        eq_num = 1;
        return;
    }

    int res_0 = temp_result[0];

    eqclass[0].num = 1;
    eqclass[0].value = res_0;

    eqclass[1].num = 0;
    eqclass[1].value = 1 - res_0;


    for(i = 1; i < recent_num; ++i) {

        // printf("CPM : MID %d -> TMPRES %d\n", recent_set[i], temp_result[i]);

        if(temp_result[i] == res_0){
            eqclass[0].mut_id[eqclass[0].num++] = recent_set[i];
        }else{
            eqclass[1].mut_id[eqclass[1].num++] = recent_set[i];
        }
    }
    eq_num = 2;
    // __accmut__dump_eqclass();
}


void __accmut__divide__eqclass_cl_st() {

    if(recent_num == 1){
        eq_num = 1;
        eqclass[0].num = 1;
        eqclass[0].mut_id[0] = recent_set[0];
        return;
    }

    int cur_zero_num = 0;
    int i;
    for(i = 0; i < recent_num; ++i) {

        long result = temp_result[i];

        // printf("CLST : MID %d -> TMPRES %d\n", recent_set[i], temp_result[i]);

        if(result == 0){
            if(cur_zero_num == 0){
                eqclass[0].num = 1;
                eqclass[0].mut_id[0] = recent_set[i];
                cur_zero_num = 1;
            }else{
                eqclass[0].mut_id[cur_zero_num] = recent_set[i];
                cur_zero_num ++;
                eqclass[0].num = cur_zero_num;
            }
        }else{
            int idx = i - cur_zero_num  + 1;
            eqclass[idx].num = 1;
            eqclass[idx].mut_id[0] = recent_set[i];
        }
    }
    if(cur_zero_num > 0){
        eq_num = recent_num - cur_zero_num + 1;
    }else{
        eq_num = 1;
    }

    //__accmut__dump_eqclass();
}



void __accmut__filter__mutants(int from, int to, int classid) {
    /** filter_mutants **/
    int j;
    if(eqclass[classid].mut_id[0] == 0) {
        for(j = from; j <= to; ++j) {
            default_active_set[j] = 0;
        }
        for(j = 0; j < eqclass[classid].num; ++j) {
           default_active_set[eqclass[classid].mut_id[j]] = 1;
        }
    } else {
        forked_active_num = 0;
        for(j = 0; j < eqclass[classid].num; ++j) {
            forked_active_set[forked_active_num++] = eqclass[classid].mut_id[j];
        }
    }
}


FILE* forked_fp;


long __accmut__fork__eqclass(int from, int to) {

    if(eq_num == 1) {
        return eqclass[0].value;
    }

    int result = eqclass[0].value;
    int id = eqclass[0].mut_id[0];
    int i;
    
    /** fork **/
    for(i = 1; i < eq_num; ++i) {

         int pid = 0;

    #if 1
         fprintf(stderr, "#\n");
    #endif

         pid = fork();

         if(pid == 0) {

            // struct timeval tv_cur;

            // gettimeofday(&tv_cur, NULL);

            // long used_sec =  tv_cur.tv_sec - tv_begin.tv_sec;
            // long used_usec = tv_cur.tv_usec - tv_begin.tv_usec;

            // if(used_usec < 0){
            //     used_sec--;
            //     used_usec += 1000000;
            // }

            // ACCMUT_REAL_TICK.it_value.tv_sec -= used_sec;
            // ACCMUT_REAL_TICK.it_value.tv_sec -= used_usec;

            // ACCMUT_PROF_TICK.it_value.tv_sec -= used_sec;
            // ACCMUT_PROF_TICK.it_value.tv_sec -= used_usec;

            // fprintf(stdout, "MODIFIED PROFTIMER: %ld %ld ; REALTIMER: %ld %ld\n", 
            //         ACCMUT_PROF_TICK.it_value.tv_sec ,
            //         ACCMUT_PROF_TICK.it_value.tv_usec, 
            //         ACCMUT_REAL_TICK.it_value.tv_sec , 
            //         ACCMUT_REAL_TICK.it_value.tv_usec);

            int r1 = setitimer(ITIMER_REAL, &ACCMUT_REAL_TICK, NULL); 
            int r2 = setitimer(ITIMER_PROF, &ACCMUT_PROF_TICK, NULL); 

            if(r1 < 0 || r2 < 0){
                ERRMSG("setitimer ERR ");
                exit(1);
            }

            __accmut__filter__mutants(from, to, i);

            if (mprotect((void *)(&MUTATION_ID), PAGESIZE, PROT_READ | PROT_WRITE)) {
                perror("mprotect ERR : PROT_READ | PROT_WRITE");
                exit(errno);
            }

            MUTATION_ID = eqclass[i].mut_id[0];

            if (mprotect((void *)(&MUTATION_ID), PAGESIZE, PROT_READ)) {
                perror("mprotect ERR : PROT_READ");
                exit(errno);
            }

            #if 1
            fprintf(forked_fp, "%d\n", MUTATION_ID);
            fflush(NULL);
            #endif

            return eqclass[i].value;

         } else {

             waitpid(pid, NULL, 0);

            // fprintf(stderr, "FATHER-> MUT: %d , PID: %d\n", MUTATION_ID, getpid());

            // fprintf(stderr, "#\n");

            #if 0

            int nptrs, j;
            void *buffer[100];
            char **strings;
            nptrs = backtrace(buffer, 20);

            fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);

            strings = backtrace_symbols(buffer, nptrs);
            if (strings == NULL) {
                perror("backtrace_symbols");
                exit(EXIT_FAILURE);
            }
            for (j = 0; j < nptrs; j++)
                fprintf(stderr, "%s\n", strings[j]);

            free(strings);

            #endif
         }
    }

    __accmut__filter__mutants(from, to, 0);
    return result;
}// end __accmut__fork__eqclass


/** End Added **/
extern int MUT_NUM;

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

    int i;
    for(i = 0; i <= MUT_NUM; ++i){
        default_active_set[i] = 1;
    }

    

    char path[256];
    sprintf(path, "%s/tmp/accmut/input/%s/t%d", getenv("HOME"), PROJECT, TEST_ID);  

    forked_fp = fopen(path ,"w");
    // FILE* forked_fp = stderr;

    if(forked_fp == NULL){
        char msg[128] = "FOPEN ERR: ";
        strcat(msg, path);
        ERRMSG(msg);
        exit(1);
    }

}


/**************************** ARITH ***************************************/
int __accmut__process_i32_arith(int from, int to, int left, int right){

	int ori = __accmut__cal_i32_arith(ALLMUTS[to]->sop , left, right);

    __accmut__filter__variant(from, to);

    int i;
    // for(i = 0; i < recent_num; ++i) {
    //     printf("\trecent_set[%d] -> %d \n", i, recent_set[i]);
    // }

    // generate recent_set
    for(i = 0; i < recent_num; ++i) {
        if(recent_set[i] == 0) {
            temp_result[i] = ori;
            continue;
        }
        Mutation *m = ALLMUTS[recent_set[i]];
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
        temp_result[i] = mut_res;

    }//end for i

    if(recent_num == 1) {
        if(MUTATION_ID < from || MUTATION_ID > to) {
            return ori;
        }
        return temp_result[0];
    }

    /* divide */
    __accmut__divide__eqclass();

    // printf("EQNUM: %d\n", eq_num);
    // for (int i = 0; i < eq_num; ++i)
    // {
    //     printf("EQCLS %d -> val: %d, num: %d, mid: %d\n", i, eqclass[i].value, eqclass[i].num, eqclass[i].mut_id[0]);
    // }


    /* fork */
    int result = __accmut__fork__eqclass(from, to);

    return result;

}// end __accmut__process_i32_arith

long __accmut__process_i64_arith(int from, int to, long left, long right){

	long ori = __accmut__cal_i64_arith(ALLMUTS[to]->sop , left, right);

    __accmut__filter__variant(from, to);

    // generate recent_set
    int i;
    for(i = 0; i < recent_num; ++i) {
        if(recent_set[i] == 0) {
            temp_result[i] = ori;
            continue;
        }
        Mutation *m = ALLMUTS[recent_set[i]];
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
        temp_result[i] = mut_res;
    }//end for i

    if(recent_num == 1) {
        if(MUTATION_ID < from || MUTATION_ID > to) {
            return ori;
        }
        return temp_result[0];
    }

    /* divide */
    __accmut__divide__eqclass();

    /* fork */
    long result = __accmut__fork__eqclass(from, to);    //TODO:: i64 -> long, 2016.8.2

    return result;

}// end __accmut__process_i64_arith


/**************************** ICMP ***************************************/
int __accmut__process_i32_cmp(int from, int to, int left, int right){

    int s_pre = ALLMUTS[to]->op_1;

	int ori = __accmut__cal_i32_bool(s_pre , left, right);

    __accmut__filter__variant(from, to);

// printf("FROM : %d, TO: %d, MID: %d, ORI: %d\n", from, to, MUTATION_ID, ori);

// for (int i = 0; i < recent_num; ++i)
// {
//     printf("recent_set[%d]: %d\n", i, recent_set[i]);
// }

    // generate recent_set

    int onlyhas_1 = 1;
    int onlyhas_0 = 0;

    int i;
    for(i = 0; i < recent_num; ++i){
        if(recent_set[i] == 0) {
            temp_result[i] = ori;

            onlyhas_1 &= ori;
            onlyhas_0 |= ori;

            continue;
        }

        Mutation *m = ALLMUTS[recent_set[i]];
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
                        ERRMSG("UOI ERR");
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
        temp_result[i] = mut_res;

        onlyhas_1 &= mut_res;
        onlyhas_0 |= mut_res;
    }//end for i

    if(recent_num == 1) {
        if(MUTATION_ID < from || MUTATION_ID > to) {
            return ori;
        }
        return temp_result[0];
    }

    /* divide */

    #if DIV_EQ_CMP
        __accmut__divide__eqclass_cmp(onlyhas_0, onlyhas_1);
    #else
        __accmut__divide__eqclass();
    #endif

    /* fork */
    int result = __accmut__fork__eqclass(from, to);

    return result;
}//end __accmut__process_i32_cmp

int __accmut__process_i64_cmp(int from, int to, long left, long right){

    int s_pre = ALLMUTS[to]->op_1;

    int ori = __accmut__cal_i64_bool(s_pre , left, right);

    __accmut__filter__variant(from, to);

    // generate recent_set

    int onlyhas_1 = 1;
    int onlyhas_0 = 0;

    int i;
    for(i = 0; i < recent_num; ++i){
        if(recent_set[i] == 0) {
            temp_result[i] = ori;

            onlyhas_1 &= ori;
            onlyhas_0 |= ori;

            continue;
        }

        Mutation *m = ALLMUTS[recent_set[i]];
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
                    mut_res = __accmut__cal_i64_bool(s_pre, u_left, right);
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
        temp_result[i] = mut_res;

        onlyhas_1 &= mut_res;
        onlyhas_0 |= mut_res;
    }//end for i

    if(recent_num == 1) {
        if(MUTATION_ID < from || MUTATION_ID > to) {
            return ori;
        }
        return temp_result[0];
    }

    /* divide */
    #if DIV_EQ_CMP
        __accmut__divide__eqclass_cmp(onlyhas_0, onlyhas_1);
    #else
        __accmut__divide__eqclass();
    #endif

    /* fork */
    int result = __accmut__fork__eqclass(from, to);

    return result;
}// end __accmut__process_i64_cmp

/**************************** CALL ***************************************/
int __accmut__apply_call_mut(Mutation* m, PrepareCallParam* params){

    if(m->type == STD){
        return 1;
    }

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
            ERRMSG("ERR m->type ");
            exit(0);
        }
    }//end switch(m->type)
    return 0;

}


/*
* must fellow the type seq: (i8 type, i8 index, i64* ptr ...)
* not std ->return 0 , std -> return 1
*/
int __accmut__prepare_call(int from, int to, int opnum, ...){

    __accmut__filter__variant(from, to);

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

    // for(i = 0; i < recent_num; i++){
    //     printf("recent_num : %d -> %d @ %d\n", i, recent_set[i], getpid());
    // }

    for(i = 0; i < recent_num; i++){

        if(recent_set[i] == 0) {
            temp_result[i] = 0;
            continue;
        }

        Mutation *m = ALLMUTS[recent_set[i]];

        int mut_res = recent_set[i];

        switch(m->type){
            case UOI:
            {
                int idx = m->op_1;
                int uoi_tp = m->op_2;

                switch(params[idx].type){
                    case CHAR_TP:
                    {
                        char *ptr = (char *) params[idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = 0;
                        }
                        break;
                    }
                    case SHORT_TP:
                    {
                        short *ptr = (short *) params[idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = 0;
                        }
                        break;
                    }
                    case INT_TP:
                    {
                        int *ptr = (int *) params[idx].address;

                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = 0;
                        }
                        break;
                    }       
                    case LONG_TP:
                    {
                        long *ptr = (long *) params[idx].address;
                        if(uoi_tp == 2 && (*ptr == 0) ){
                            mut_res = 0;
                        }
                        break;
                    }
                    default:
                    {
                        ERRMSG("UOI params[idx].type ");
                        exit(0);
                    }
                }//end switch(params[idx].type)
                break;
            }//end case UOI
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
                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[idx2].address;
                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }                            
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[idx2].type ");
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

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }                                 
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[idx2].type ");
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

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[idx2].type ");
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

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case SHORT_TP:
                            {
                                short *ptr2 = (short *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            case INT_TP:
                            {
                                int *ptr2 = (int *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }                                    
                                break;
                            }
                            case LONG_TP:
                            {
                                long *ptr2 = (long *) params[idx2].address;

                                if(*ptr1 == *ptr2){
                                    mut_res = 0;
                                }
                                break;
                            }
                            default:
                            {
                                ERRMSG("ROV params[idx2].type ");
                                exit(0);
                            }
                        }//end switch(params[idx2].type)                
                        break;
                    }
                    default:
                    {
                        ERRMSG("ROV params[idx1].type ");
                        exit(0);
                    }
                }//end switch(params[idx1].type)
                break;
            }//end case ROV
            case ABV:
            {
                int idx = m->op_0;

                switch(params[idx].type){
                    case CHAR_TP:
                    {
                        char *ptr = (char *) params[idx].address;
                        if(*ptr >= 0){
                            mut_res = 0;
                        }
                        break;                  
                    }
                    case SHORT_TP:
                    {
                        short *ptr = (short *) params[idx].address;
                        if(*ptr >= 0){
                            mut_res = 0;
                        }
                        break;
                    }
                    case INT_TP:
                    {
                        int *ptr = (int *) params[idx].address;
                        if(*ptr >= 0){
                            mut_res = 0;
                        }
                        break;
                    }
                    case LONG_TP:
                    {
                        long *ptr = (long *) params[idx].address;
                        if(*ptr >= 0){
                            mut_res = 0;
                        }
                        break;
                    }
                    default:
                    {
                        ERRMSG("ABV params[idx].type ");
                        exit(0);
                    }
                }//end switch(params[idx].type)
                break;
            }//end case ABV
        }//end switch(m->type)

        temp_result[i] = mut_res;

    } // end for i


    if(recent_num == 1) {
        if(MUTATION_ID < from || MUTATION_ID > to){
            return 0;
        }
        
        int tmpmid = temp_result[0];    //TODO :: CHECK !

        // printf("MID %d tmpmid %d\n", MUTATION_ID, tmpmid);

        Mutation *m = ALLMUTS[tmpmid];

        if( tmpmid != 0 && m->type == STD){
            return 1;
        }else{
            return __accmut__apply_call_mut(m, &params);
        }
    }

   /* divide */
    #if DIV_EQ_CL_ST
    __accmut__divide__eqclass_cl_st();
    #else
    __accmut__divide__eqclass();
    #endif
    // printf("eq_num : %d\n", eq_num);
    // for (int i = 0; i < eq_num; ++i)
    // {
    //     printf("EQCLS %d -> val: %d, num: %d, mid: %d\n", i, eqclass[i].value, eqclass[i].num, eqclass[i].mut_id[0]);
    // }

    /* fork */
    __accmut__fork__eqclass(from, to);

    /* apply the represent mutant*/

    // printf("CURR_MID: %d\n", MUTATION_ID);


    if(MUTATION_ID == 0)
        return 0;

    return __accmut__apply_call_mut(ALLMUTS[MUTATION_ID], &params);

}// end __accmut__prepare_call

int __accmut__stdcall_i32(){
    return ALLMUTS[MUTATION_ID]->op_2;
}

long __accmut__stdcall_i64(){
    return ALLMUTS[MUTATION_ID]->op_2;
}

void __accmut__stdcall_void(){/*do nothing*/}


/******************************** STORE ***********************************/
void __accmut__std_store(){/*donothing*/}

int __accmut__apply_store_mut(Mutation*m , long tobestore, unsigned long addr, int is_i32){

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

    if(is_i32 == 1){
        int* ptr = (int*) addr;
        *ptr = tobestore;
    }else{
        long* ptr = (long*) addr;
        *ptr = tobestore;
    }
    return 0;
}

int __accmut__prepare_st_i32(int from, int to, int tobestore, int *addr){

    __accmut__filter__variant(from, to);

    Mutation *m;

    int i;

    // for(i = 0; i < recent_num; ++i){
    //     printf("recent_set[%d]: %d, PID: %d\n", i, recent_set[i], getpid());
    // }

    for(i = 0; i < recent_num; ++i) {

        if(recent_set[i] == 0) {
            temp_result[i] = 0;
            continue;
        }

        m = ALLMUTS[recent_set[i]];

        int mut_res = recent_set[i];

        switch(m->type){
            case UOI:
            {
                int uoi_tp = m->op_2;
                if(tobestore == 0 && uoi_tp == 2){
                    mut_res = 0;
                }
                break;
            }//end case UOI
            case ABV:
            {
                if(tobestore >= 0){//TODO::check signed or unsigned are the same
                    mut_res = 0;
                }
                break;
            }//end case ABV
        }//end switch(m->type)

        temp_result[i] = mut_res;
        // printf("temp_result[%d] = %d\n", i, temp_result[i]);

    } // end for i

    if(recent_num == 1) {

        if(MUTATION_ID < from || MUTATION_ID > to){
            *addr = tobestore;
            return 0;
        }
        
        int tmpmid = temp_result[0];

        m = ALLMUTS[tmpmid];

        if( tmpmid != 0 && m->type == STD){
            return 1;
        }else{
            return __accmut__apply_store_mut(m, tobestore, addr, 1);
        }
    }


   /* divide */
    #if DIV_EQ_CL_ST
    __accmut__divide__eqclass_cl_st();
    #else
    __accmut__divide__eqclass();
    #endif


    // for (int i = 0; i < eq_num; ++i)
    // {
    //     printf("EQCLS %d -> val: %d, num: %d, mid: %d\n", i, eqclass[i].value, eqclass[i].num, eqclass[i].mut_id[0]);
    // }


    /* fork */
    __accmut__fork__eqclass(from, to);

    if(MUTATION_ID == 0){
        *addr = tobestore;
        return 0;
    }


    // printf("CURR_MID: %d   PID: %d\n", MUTATION_ID, getpid());


    /* apply the mutation */
    m = ALLMUTS[MUTATION_ID];

    if(m->type == STD){
        return 1;
    }

    return __accmut__apply_store_mut(m, tobestore, addr, 1);
}


int __accmut__prepare_st_i64(int from, int to, long tobestore, long *addr){
    
    __accmut__filter__variant(from, to);

    Mutation *m;

    int i;

    // for(i = 0; i < recent_num; ++i){
    //     printf("recent_set[%d]: %d, PID: %d\n", i, recent_set[i], getpid());
    // }

    for(i = 0; i < recent_num; ++i) {

        if(recent_set[i] == 0) {
            temp_result[i] = 0;
            continue;
        }

        m = ALLMUTS[recent_set[i]];

        int mut_res = recent_set[i];

        switch(m->type){
            case UOI:
            {
                int uoi_tp = m->op_2;
                if(tobestore == 0 && uoi_tp == 2){
                    mut_res = 0;
                }
                break;
            }//end case UOI
            case ABV:
            {
                if(tobestore >= 0){//TODO::check signed or unsigned are the same
                    mut_res = 0;
                }
                break;
            }//end case ABV
        }//end switch(m->type)

        temp_result[i] = mut_res;
        // printf("temp_result[%d] = %d\n", i, temp_result[i]);

    } // end for i

    if(recent_num == 1) {

        if(MUTATION_ID < from || MUTATION_ID > to){
            *addr = tobestore;
            return 0;
        }
        
        int tmpmid = temp_result[0];

        m = ALLMUTS[tmpmid];

        if( tmpmid != 0 && m->type == STD){
            return 1;
        }else{
            return __accmut__apply_store_mut(m, tobestore, addr, 0);
        }
    }


   /* divide */
    #if DIV_EQ_CL_ST
    __accmut__divide__eqclass_cl_st();
    #else
    __accmut__divide__eqclass();
    #endif

    // for (int i = 0; i < eq_num; ++i)
    // {
    //     printf("EQCLS %d -> val: %d, num: %d, mid: %d\n", i, eqclass[i].value, eqclass[i].num, eqclass[i].mut_id[0]);
    // }


    /* fork */
    __accmut__fork__eqclass(from, to);

    if(MUTATION_ID == 0){
        *addr = tobestore;
        return 0;
    }


    // printf("CURR_MID: %d   PID: %d\n", MUTATION_ID, getpid());


    /* apply the mutation */
    m = ALLMUTS[MUTATION_ID];

    if(m->type == STD){
        return 1;
    }

    return __accmut__apply_store_mut(m, tobestore, addr, 0);
}// end for __accmut__prepare_st_i64

#endif
