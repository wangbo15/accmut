#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>


char ori_mut_path[256];
char mut_path[256];

FILE* ori_mut_file;
FILE* mut_file;

char comile_cmd[256] = "/home/nightwish/code/git/accmut/release/Release+Asserts/bin/clang print_tokens.c -o print_tokens.exe";
char test_cmd[256] = "../scripts/100.sh";

void load_ori_mut_file(){
	strcpy(ori_mut_path, getenv("HOME"));
	strcat(ori_mut_path, "/tmp/accmut/printtokens.txt");

	strcpy(mut_path, getenv("HOME"));
	strcat(mut_path, "/tmp/accmut/mutations.txt");
	
	ori_mut_file = fopen(ori_mut_path, "r");
	if(ori_mut_file == NULL){
		fprintf(stderr, "OPEN ERR !! PATH: %s\n", ori_mut_path);
		exit(0);
	}
}

int main(){
	struct timeval tv_begin, tv_end;
	
	gettimeofday(&tv_begin, NULL);

	load_ori_mut_file();
	char buff[256];	
	int cur_mut_id = 0;
	while(fgets(buff, 256, ori_mut_file)){
		cur_mut_id++;
		mut_file = fopen(mut_path, "w");
		if(mut_file == NULL){
			fprintf(stderr, "OPEN ERR !! PATH: %s\n", mut_path);
			exit(0);
		}
		fprintf(mut_file, "%s", buff);
		fclose(mut_file);
		
		system(comile_cmd);
		
		system(test_cmd);
		
		//if(cur_mut_id == 100)
		//	break;
	}
	
	
	gettimeofday(&tv_end, NULL);
	long real_sec =  tv_end.tv_sec - tv_begin.tv_sec;
	long real_usec = tv_end.tv_usec - tv_begin.tv_usec;

	if(real_usec < 0){
		real_sec--;
		real_usec += 1000000;
	}
	
	fprintf(stderr, "TOTAL TIME: %ld %ld\n", real_sec, real_usec);
	return 0;
}
