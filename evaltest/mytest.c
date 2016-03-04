#include<stdio.h>

extern int mid;

int main() {
    eval_init();
    int a, i, x;
    a = 1;
    x = 0;
    for(i = 0; i < 2; ++i) {
        x += myFork(1,8,8,a,a); 
        ++a;
    }
    printf("%d: x = %d\n", mid, x);
    return 0;
} 
