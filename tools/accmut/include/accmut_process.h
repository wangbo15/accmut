#ifndef ACCMUT_PROCESS_H
#define ACCMUT_PROCESS_H


#define PAGESIZE (4096)

#define MAX_PARAM_NUM (16)

/****************** PREPARE CALL **************************/
//TYPE BITS OF SIGNATURE
#define CHAR_TP 0
#define SHORT_TP 1
#define INT_TP 2
#define LONG_TP 3

typedef struct PrepareCallParam{
	int type;
	unsigned long address;
}PrepareCallParam;
/**********************************************************/

void __accmut__init(void);

int __accmut__prepare_call(int from, int to, int opnum, ...);

int __accmut__stdcall_i32(void);

long __accmut__stdcall_i64(void);

void __accmut__stdcall_void(void);

//char *__accmut__stdcall_pt();

int __accmut__process_i32_arith(int from, int to, int left, int right);

long __accmut__process_i64_arith(int from, int to, long left, long right);

int __accmut__process_i32_cmp(int from, int to, int left, int right);

int __accmut__process_i64_cmp(int from, int to, long left, long right);

int __accmut__prepare_st_i32(int from, int to, int tobestore, int *addr);

int __accmut__prepare_st_i64(int from, int to, long tobestore, long* addr);

void __accmut__std_store(void);

#endif
