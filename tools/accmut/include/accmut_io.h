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
#define ACCMUT_MAX_FILENO (16)
#define MAX_IN_BUF_SIZE (1<<16)

//4M OUTPUT BUF 
#define MAX_STDOUT_BUF_SIZE (1<<22)
#define MAX_STDERR_BUF_SIZE (1<<22)
#define MAX_FILE_BUF_SIZE (1<<22)
#define LINE_BUF_SIZE (4096)
/*************************************************/

# define EOF (-1)

/***********************************************************/
//FOR OUTPUT BUFFER
const char ORACLEDIR[] = "/home/nightwish/workspace/testing/accmut/printtokens/oracle/";

char STDOUT_BUFF[MAX_STDOUT_BUF_SIZE];
char STDERR_BUFF[MAX_STDERR_BUF_SIZE];
size_t CUR_STDOUT = 0;
size_t CUR_STDERR = 0;

char *ORACLEBUFF;
size_t ORACLESIZE = 0;

/***********************************************************/


typedef struct _ACCMUT_FILE{
	int flags;
	int fd;
	char *bufbase;
	char *bufend;
	char *read_cur;
	char *write_cur;
	size_t fsize;
	
}ACCMUT_FILE;


#define DEF_STDFILE(NAME, FD, BUF, MAXSIZE, FLAGS) \
	ACCMUT_FILE NAME = {FLAGS, FD, BUF, (BUF + MAXSIZE), BUF, BUF, 0};

DEF_STDFILE(stdfile_0, 0, NULL, 0, 0);	//unimplemented stdin
DEF_STDFILE(stdfile_1, 1, STDOUT_BUFF, MAX_STDOUT_BUF_SIZE, O_WRONLY);
DEF_STDFILE(stdfile_2, 2, STDERR_BUFF, MAX_STDERR_BUF_SIZE, O_WRONLY);

ACCMUT_FILE* accmut_stdin = &stdfile_0;
ACCMUT_FILE* accmut_stdout = &stdfile_1;
ACCMUT_FILE* accmut_stderr = &stdfile_2;


/*********************** FILE OPTIONS ****************************************/


ACCMUT_FILE* __accmut__fopen(const char *path, const char *mode){

	int omode;
	int oflags = 0;
	switch(*mode++)
	{
		case 'r':
			omode = O_RDONLY;
			break;
		case 'w':
			omode = O_WRONLY;
			oflags = O_CREAT|O_TRUNC;
			break;			
		// case 'a':
		// 	0flags = O_WRONLY | O_APPEND;
		// 	break;
		default:
			fprintf(stderr, "ERROR FOPEN MODE : %s !\n", mode);
			exit(0);			
	}

	// if (m[0] == '+' || (m[0] == 'b' && m[1] == '+')){
	// 	omode = O_RDONLY | O_WRONLY;
	// }


	int _fd = open(path, omode);
	if(_fd < 0){
		return NULL;
	}
	
	ACCMUT_FILE *fp = (ACCMUT_FILE *)malloc(sizeof(ACCMUT_FILE));

	if(omode == O_RDONLY){
		struct stat sb;
		if(fstat(_fd, &sb) == -1){
			return NULL;
		}
		if(sb.st_size > MAX_IN_BUF_SIZE){
			fprintf(stderr, "INPUT FILE : %s IS TOO BIG\n", path);
			return NULL;
		}
		fp->flags = O_RDONLY;
		fp->fd = _fd;
		fp->bufbase = fp->read_cur = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, _fd, 0);
		fp->write_cur = NULL;
		fp->fsize = sb.st_size;
		fp->bufend = fp->bufbase + fp->fsize;
	}else if(omode == O_WRONLY){
		fp->flags = O_WRONLY;
		fp->fd = _fd;
		fp->bufbase = fp->write_cur = (char*) calloc(1, MAX_FILE_BUF_SIZE*(sizeof(char)) );
		fp->read_cur = NULL;
		fp->fsize = 0;
		fp->bufend = fp->bufbase + MAX_FILE_BUF_SIZE*(sizeof(char));
	}

	return fp;
}


int __accmut__fclose(ACCMUT_FILE *fp){
	int status;
	if(fp->flags == O_RDONLY){
		status = munmap(fp->bufbase, fp->fsize);
		status = status & close(fp->fd);
		free(fp);
	}else if(fp->flags == O_WRONLY){
		status = close(fp->fd);
		free(fp->bufbase);
		free(fp);
	}
	return status;
}


int __accmut__fseek(ACCMUT_FILE *fp, size_t offset, int loc){
	//TODO
	return 0;
}


ACCMUT_FILE * __accmut__freopen(const char *path, const char *mode, ACCMUT_FILE *stream){
	//TODO
	return NULL;
}



/*********************** INPUT ****************************************/

char* __accmut__fgets(char *buf, int size, ACCMUT_FILE *fp){
	if(size <= 0)
		return NULL;
		
	if(fp->read_cur - fp->bufbase >= fp->fsize)
		return NULL;
	
	size_t len = size - 1;
	
	char *t = (char*) memchr((void*) fp->read_cur, '\n', len);
	
	if(t != NULL){
		len = t - fp->read_cur;
		++t;
		++len;
	}
	
	if(len == 0){
		return NULL;
	}
	
	memcpy((void *) buf , (void *) fp->read_cur, len);
		
	fp->read_cur = fp->read_cur + len;
	
	*(buf + len) = '\0';
	
	return buf;
}



/*********************** OUTPUT ****************************************/

int __accmut__fputc(int c, ACCMUT_FILE *fp){

	if(fp->write_cur >= fp->bufend){
		fprintf(stderr, "OUTPUT OVERFLOW @ __accmut__fputc\n");
		//TODO: need something to fflush
		exit(0);
	}
	fp->fsize++;
	*(fp->write_cur) = c;
	(fp->write_cur)++;
	return (unsigned char) c;
}
//putc implemented as a macro 
#define __accmut__putc(c, f) ( __accmut__fputc(c, f) )


int __accmut__fputs(const char* s, ACCMUT_FILE *fp){
	int result = EOF;
	int c;
	if(fp->write_cur + len < fp->bufend){
		while(c = *s++){
			__accmut__putc(c, fp);
		}
		result = 1;
	}
	return result;
}

int __accmut__puts(const char* s){
	int result = EOF;
	int c;
	if(fp->write_cur + len < fp->bufend){
		while(c = *s++){
			__accmut__putc(c, accmut_stdout);
		}
		__accmut__putc(c, accmut_stdout);
		result = 1;
	}
	return result;
}


//ORI fprintf
// int __accmut__fprintf(FILE *stream, const char *format, ...){
// 	int ret;
// 	va_list ap;
// 	va_start(ap, format);
// 	ret = vfprintf(stream, format, ap);
// 	va_end(ap);
// 	return 0;
// }

// int __accmut__fprintf(FILE *stream, const char *format, ...){
// 	int ret;
// 	va_list ap;
// 	va_start(ap, format);
// 	char tmp[LINE_BUF_SIZE] = {0};
// 	ret = vsprintf(tmp, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
// 	va_end(ap);

// 	if(ret > LINE_BUF_SIZE){
// 		fprintf(stderr, "BUFFER LINE OVERFLOW !!!!!!!\n");
// 	}

// 	int total = CUR_STDOUT + ret;
// 	if(total >= MAX_STDOUT_BUF_SIZE){
// 		fprintf(stderr, "ACCMUT BUFFER OVERFLOW !  MUT: %d\n", MUTATION_ID);
// 		exit(0);
// 	}
// 	memcpy((STDOUT_BUFF + CUR_STDOUT), tmp, ret*(sizeof(char)) );
// 	CUR_STDOUT = total;
// 	return ret;
// }

int __accmut__fprintf(ACCMUT_FILE *stream, const char *format, ...){
	int ret;
	// va_list ap;
	// va_start(ap, format);
	// ret = vsprintf(tmp, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
	// va_end(ap);

	// if(ret > LINE_BUF_SIZE){
	// 	fprintf(stderr, "BUFFER LINE OVERFLOW !!!!!!!\n");
	// }

	// int total = CUR_STDOUT + ret;
	// if(total >= MAX_STDOUT_BUF_SIZE){
	// 	fprintf(stderr, "ACCMUT BUFFER OVERFLOW !  MUT: %d\n", MUTATION_ID);
	// 	exit(0);
	// }
	// memcpy((STDOUT_BUFF + CUR_STDOUT), tmp, ret*(sizeof(char)) );
	// CUR_STDOUT = total;
	return ret;
}

/*********************** BUF UTILS ****************************************/

void __accmut__filedump(ACCMUT_FILE *fp);
void __accmut__oracledump();

int __accmut__checkoutput(){
	if(ORACLESIZE != CUR_STDOUT){
		return 1;
	}
	return memcmp(STDOUT_BUFF, ORACLEBUFF, CUR_STDOUT);
}

void __accmut__exit_check_output(){
	int res = __accmut__checkoutput();
	if(res != 0){
		fprintf(stderr, "TEST: %d KILL MUT: %d\n", TEST_ID, MUTATION_ID);
		//__accmut__filedump();
		
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
	fprintf(stderr, "\n************ END OF ORACLE ***************\n\n");
}

void __accmut__filedump(ACCMUT_FILE *fp){
	fprintf(stderr, "\n********** TID:%d  MID:%d  FD:%d  SIZE:%d ***********\n", \
		TEST_ID, MUTATION_ID, fp->fd, fp->fsize);
	int i;
	for(i = 0; i < fp->fsize; i++){
		fprintf(stderr, "%c", *(fp->bufbase + i) );
	}
	fprintf(stderr, "\n************ END OF ACCMUT_FILE ***************\n\n");
}


#endif
