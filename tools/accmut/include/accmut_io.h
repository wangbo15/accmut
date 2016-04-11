#ifndef ACCMUT_IO_H
#define ACCMUT_IO_H


#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*************************************************/
//INPUT BUF
#define ACCMUT_MAX_FILENO 16
#define IO_BUF_SIZE 8192

//4M OUTPUT BUF 
#define MAXBUFFERSIZE 1<<22
#define MAXMUTNUM 10000
/*************************************************/


/***********************************************************/
//FOR OUTPUT BUFFER
const char ORACLEDIR[] = "/home/nightwish/workspace/testing/accmut/printtokens/oracle/";

char STDBUFF[MAXBUFFERSIZE];
char *ORACLEBUFF;
size_t ORACLESIZE = 0;
size_t CURBUFSIZE = 0;
/***********************************************************/


typedef struct _ACCMUT_INPUT_FILE{
	int fd;
	char *bufbase;
	char *cur;
	char *bufend;
	size_t fsize;
	
}ACCMUT_FILE;

/*********************** INPUT ****************************************/

ACCMUT_FILE* __accmut__fopen(const char *path, const char *mode){
	//TODO: check mode
	if(*mode != 'r' && *(mode+1) != NULL){// only support READ-ONLY mode now
		fprintf(stderr, "ERROR FOPEN MODE : %s !\n", mode);
		exit(0);
	}
	int _fd = open(path, O_RDONLY);
	if(_fd < 0){
		return NULL;
	}
	struct stat sb;
	if(fstat(_fd, &sb) == -1){
		return NULL;
	}
	if(sb.st_size > IO_BUF_SIZE){
		fprintf(stderr, "INPUT FILE : %s IS TOO BIG\n", path);
		return NULL;
	}
	ACCMUT_FILE *fp = (ACCMUT_FILE *)malloc(sizeof(ACCMUT_FILE));
	fp->fd = _fd;
	fp->bufbase = fp->cur = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, _fd, 0);
	fp->fsize = sb.st_size;
	fp->bufend = fp->bufbase + fp->fsize;
	return fp;
}

char* __accmut__fgets(char *buf, int size, ACCMUT_FILE *fp){
	if(size <= 0)
		return NULL;
		
	if(fp->cur - fp->bufbase >= fp->fsize)
		return NULL;
	
	size_t len = size - 1;
	
	char *t = (char*) memchr((void*) fp->cur, '\n', len);
	
	if(t != NULL){
		len = t - fp->cur;
		++t;
		++len;
	}
	
	if(len == 0){
		return NULL;
	}
	
	memcpy((void *) buf , (void *) fp->cur, len);
		
	fp->cur = fp->cur + len;
	
	*(buf + len) = '\0';
	
	return buf;
}

int __accmut__fclose(ACCMUT_FILE *fp){
	int status = munmap(fp->bufbase, fp->fsize);
	status = status & close(fp->fd);
	free(fp);
	return status;
}

int __accmut__fseek(ACCMUT_FILE *fp, size_t offset, int loc){
	return 0;
}


/*********************** OUTPUT ****************************************/

//ORI fprintf
// int __accmut__fprintf(FILE *stream, const char *format, ...){
// 	int ret;
// 	va_list ap;
// 	va_start(ap, format);
// 	ret = vfprintf(stream, format, ap);
// 	va_end(ap);
// 	return 0;
// }

int __accmut__fprintf(FILE *stream, const char *format, ...){
	int ret;
	va_list ap;
	va_start(ap, format);
	char tmp[4096] = {0};
	ret = vsprintf(tmp, format, ap);
	va_end(ap);

	if(ret > 4096){
		perror("BUFFER LINE OVERFLOW !!!!!!!\n");
	}

	int total = CURBUFSIZE + ret;
	if(total >= MAXBUFFERSIZE){
		perror("ACCMUT BUFFER OVERFLOW !!!!!!\n");
		exit(0);
	}
	memcpy((STDBUFF + CURBUFSIZE), tmp, ret*(sizeof(char)) );
	CURBUFSIZE = total;
	return ret;
}


void __accmut__bufferdump();
void __accmut__oracledump();

int __accmut__checkoutput(){
	if(ORACLESIZE != CURBUFSIZE){
		return 1;
	}
	return memcmp(STDBUFF, ORACLEBUFF, CURBUFSIZE);
}

void __accmut__exit_check_output(){
	int res = __accmut__checkoutput();
	if(res != 0){
		fprintf(stderr, "TEST: %d KILL MUT: %d\n", TEST_ID, MUTATION_ID);
		//__accmut__bufferdump();
		
	}

	// if(MUTATION_ID == 0){
	// 	__accmut__oracledump();
	// }
}

void __accmut__bufinit(){
	char path[120];
	sprintf(path, "%st%d", ORACLEDIR, TEST_ID);
	int fd = open(path, O_RDONLY);
	if(fd == -1){
		fprintf(stderr, "ORACLEDIR OPEN ERROR !!!!!!\n");
		fprintf(stderr, "ORACLEDIR PATH : %s\n", path);
	}
	struct stat sb;
	if(fstat(fd, &sb) == -1){
		fprintf(stderr, "fstat ERROR !!!!!!\n");
	}
	ORACLEBUFF = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(ORACLEBUFF == MAP_FAILED){
		fprintf(stderr, "mmap ERROR !!!!!!\n");
	}
	ORACLESIZE = sb.st_size;

	//regist the exit handler function of a process
	if(atexit(__accmut__exit_check_output) != 0){
		fprintf(stderr, "____accmut__exit_check_output REGSITER ERROR\n");
		exit(0);
	}

	// if(atexit(__accmut__exit_time) != 0){
	// 	fprintf(stderr, "__accmut__exit_time REGSITER ERROR\n");
	// }
}

void __accmut__setout(int id){
 	char path[120];
	strcpy(path, getenv("HOME"));
	strcat(path, "/tmp/accmut/output/");
	strcat(path, PROJECT);
	strcat(path, "/t");
	sprintf(path, "%s%d_%d", path, TEST_ID, id);
	//printf("PATH : %s\n", path);
	if (freopen(path, "w", stdout)==NULL){
		fprintf(stderr, "STDOUT REDIR ERR! : %s\n", path);
	}
}

void __accmut__oracledump(){
	fprintf(stderr, "\n********** TID:%d  ORI BUFFER SIZE: %d ***********\n", TEST_ID, ORACLESIZE);
	int i;
	for(i = 0; i < ORACLESIZE; i++){
		fprintf(stderr, "%c", ORACLEBUFF[i]);
	}
	fprintf(stderr, "************ END OF ORACLE ***************\n\n");
}

void __accmut__bufferdump(){
	fprintf(stderr, "\n********** TID:%d  MID:%d  ACCMUT BUF SIZE:%d ***********\n", TEST_ID, MUTATION_ID, CURBUFSIZE);
	int i;
	for(i = 0; i < CURBUFSIZE; i++){
		fprintf(stderr, "%c", STDBUFF[i]);
	}
	fprintf(stderr, "************ END OF ACCMUT BUFFER ***************\n\n");
}


#endif
