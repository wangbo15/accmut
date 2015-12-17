#include"../accmut.h"

/*
int test_ari(int a, int b){
    int res = a + b;
    res = res << b;
    return res;
}

long test_ari_l(int a, long b){
	return a + b;
}

int test_cmp(int a, int b){
    if(a>b){
        return a;    
    }
    return b;
}

long test_cmp_l(long a, long b){
	if(a == b){
		return a;
	}
	return b;
}

int callee_i32(int a, int b){
    return a + b;    
}

long callee_l(int a, char b){
	return 100 + a;
}

void callee_void(int a){
    
}

int test(int a, int b){
    int c = test_ari(a, b);    
    int d = test_cmp(a, b);
    int e = callee_i32(a, b);
    callee_void(d);
    int f = 4;
    return c + d + e + f;
}

long test_l(long a, long b){
	long c = test_ari_l(a, b);
	long d = test_cmp_l(a, b);
	int e = callee_l(a ,b);
	long f = 10000;
	return c+d+e+f;
}
*/

/*int test(int a, int b){
    int res = a + b;
    res = res << b;
    return res;
}*/

/*int test(int a, int b){
	if(a > b){
		return a;	
	}
	return b;
}*/

int test(int a, int b){
	int c = 3;
	return a+b+c;
}

int main(){
    int a, b;
    //scanf("%d %d", &a, &b);
    a = 1;
    b = 2;
    int res = test(a, b);
    printf("%d\n", res);
	long c, d;
	c = 888;
	d = 999;
	//long res_l = test_l(c, d);
	//printf("%ld\n", res_l);
    return 0;
}

