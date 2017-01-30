#ifndef ACCMUT_ARITH_COMMON_H
#define ACCMUT_ARITH_COMMON_H


int __accmut__cal_i32_arith(int op, int a, int b);

long __accmut__cal_i64_arith(int op, long a, long b);

int __accmut__cal_i32_bool(int pre, int a, int b);

int __accmut__cal_i64_bool(int pre, long a, long b);


#endif
