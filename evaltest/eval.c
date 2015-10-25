#include<stdio.h>
#define MAXNUM 10010
#define UNDEF 0xDEA0BEEF

struct Mutation {
    char type;
    int mut_op;
    int obj;
};

struct Mutation mut_list[MAXNUM];
int fork_list[MAXNUM] = {0};
int fork_index = 0;
int store[MAXNUM];
int store_index = 0;
int mid = 0;

int eval_init() {
    int index = 0;
    int i;
    int id;
    char type;
    char buffer[100];
    while (scanf("%d:%s", &id, buffer) != EOF) {
        printf("%s\n", buffer);
        if(buffer[1] == 'V') {
            type = 'v';
        } else {
            type = buffer[0]+'a'-'A';
        }
        mut_list[index].type = type;
        printf("%c\n", mut_list[index].type);
        i = 4;
        while(buffer[i] != ':') ++i;
        int j;
        int t[4] = {0,0,0,0}, index_t = 0;
        for(j = 0; j<3 || (j<4 && (type=='v'||type=='r')); ++j) {
            ++i;
            int temp = 0;
            while(buffer[i] != ':' && buffer[i] != 0) {
                temp = temp * 10;
                temp += buffer[i++]-'0';
            }
            printf("%d\n", temp);
            t[index_t++] = temp;
        }
        for(j = 0; j < 4; ++j) printf("%d, ", t[j]);
        printf("\n");
        for(j = 0; j < 4; ++j) {
            if(type == 'a' || type == 'l') {
                mut_list[index].mut_op = t[2];
            } else if (type == 'r') {
                mut_list[index].mut_op = t[3];
            } else {
                mut_list[index].mut_op = t[3];
                mut_list[index].obj = t[2];
            }
        }
        ++index;
    }
    return 0;
}

int eval(int start_mid, int end_mid, int orig_op, int left_val, int right_val) {
    int i;
    for(i = start_mid; i != end_mid; ++i) {
        int result;
        if (mut_list[i].type != 'v') {
            result = calc(mut_list[i].mut_op, left_val, right_val);
        } else {
            result = calc(orig_op, left_val, right_val);
        }
        int j;
        for(j = 0; j < store_index; ++j) {
            if(store[j] == result) continue;
            else {
                fork_list[fork_index++] = i;
                store[store_index++] = result;
            }
        }
    }

    return 0;
}

int myFork(int start_mid, int end_mid, int orig_op, int left_val, int right_val) {
    fork_index = 0;
    store_index = 0;
    eval(start_mid, end_mid, orig_op, left_val, right_val);
    int i;
    for(i = 0; i < fork_index; ++i) {
       if(mid != 0) {
           int pid = fork();
           if(pid == 0) {
               // child
               mid = fork_list[i];
               return store[i];
           } else {
               continue;
           }
       }
    }
}

int calc(int op, int a, int b) {

    switch(op) {
        case 8: return a+b;
        case 10: return a-b;
        case 12: return a*b;
        case 14: if(b==0) return UNDEF;
                   else return a/b;
        case 15: if(b==0) return UNDEF;
                   else return a/b;
        case 17: if(b==0) return UNDEF;
                   else return a%b;
        case 18: if(b==0) return UNDEF;
                   else return a%b;
        case 19: return a<<b;
        case 23: return a&b;
        case 24: return a|b;
        case 25: return a^b;

        case 32: return a==b;
        case 33: return a!=b;
        case 34: return (unsigned)a>(unsigned)b;
        case 35: return (unsigned)a>=(unsigned)b;
        case 36: return (unsigned)a<(unsigned)b;
        case 37: return (unsigned)a<=(unsigned)b;
        case 38: return a>b;
        case 39: return a>=b;
        case 40: return a<b;
        case 41: return a<=b;
        default: return UNDEF;
    }
}

int main() {
    eval_init();
}
