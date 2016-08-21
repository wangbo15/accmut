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

#define ACCMUT_IO_DEBUG 0

//INPUT BUF
#define ACCMUT_MAX_FILENO (16)
#define MAX_IN_BUF_SIZE (1<<22)

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
	size_t fsize;	//only for input file
	
}ACCMUT_FILE;


#define DEF_STDFILE(NAME, FD, BUF, MAXSIZE, FLAGS) \
	ACCMUT_FILE NAME = {FLAGS, FD, BUF, (BUF + MAXSIZE), BUF, BUF, 0}

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
	int oprot = 0666;

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
			fprintf(stderr, "ERROR FOPEN MODE: %s ! TID: %d, MUT: %d\n", mode, TEST_ID, MUTATION_ID);
			exit(0);			
	}

	// if (m[0] == '+' || (m[0] == 'b' && m[1] == '+')){
	// 	omode = O_RDONLY | O_WRONLY;
	// }


	int _fd = open(path, omode | oflags, oprot);

	if(_fd < 0){
		#if ACCMUT_IO_DEBUG
			fprintf(stdout, "OPEN ERROR, PATH: \"%s\", MODE: %d  @__accmut__fopen. TID: %d , MUT: %d\n", path, omode, TEST_ID, MUTATION_ID);
		#endif
		return NULL;
	}
	
	ACCMUT_FILE *fp = (ACCMUT_FILE *)malloc(sizeof(ACCMUT_FILE));

	if(fp == NULL)
			return NULL;

	if(omode == O_RDONLY){

		struct stat sb;
		if(fstat(_fd, &sb) == -1){
			return NULL;
		}
		if(sb.st_size > MAX_IN_BUF_SIZE){
			#if ACCMUT_IO_DEBUG
				fprintf(stderr, "INPUT FILE : %s IS TOO BIG. TID: %d, MUT: %d\n", path, TEST_ID, MUTATION_ID);
			#endif
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
	int status = 0;
	if(fp->flags == O_RDONLY){
		status = munmap(fp->bufbase, fp->fsize);
		status = status & close(fp->fd);
		free(fp);
	}else if(fp->flags == O_WRONLY){
		
		// if(fp == accmut_stdout || fp == accmut_stderr){
		// 	fp->bufend = fp->bufbase;	// TODO
		// 	status = close(fp->fd);
		// 	return status;
		// }

		status = close(fp->fd);

		//fprintf(stderr, "  Close : %d  %d  %d\n", fp->fd, status, MUTATION_ID);

		free(fp->bufbase);
		free(fp);
	}
	return status;
}


int __accmut__fseek(ACCMUT_FILE *fp, size_t offset, int loc){
	//TODO
	return 0;
}

int __accmut__ferror(ACCMUT_FILE *fp){
	//TODO:
	return 0;
}

int __accmut__fileno(ACCMUT_FILE *fp){
	return fp->fd;
}

ACCMUT_FILE * __accmut__freopen(const char *path, const char *mode, ACCMUT_FILE *fp){
	ACCMUT_FILE * newfp = __accmut__fopen(path, mode);
	if(newfp == NULL){
		return NULL;
	}

	int status = close(fp->fd);

	if(fp->fd > 2){//not stdin, stdout, stderr
		if(fp->flags == O_RDONLY){
			status = munmap(fp->bufbase, fp->fsize);
			status = status & close(fp->fd);
		}else if(fp->flags == O_WRONLY){
			status = close(fp->fd);
			free(fp->bufbase);
		}
	}

	if(status < 0){
		return NULL;
	}

	// fp->flags = newfp->flags;
	// fp->fd = newfp->fd;
	// fp->bufbase = newfp->bufbase;
	// fp->write_cur = newfp->write_cur;
	// fp->read_cur = newfp->read_cur;
	// fp->bufend = newfp->bufend;
	// fp->fsize = newfp->fsize;

	if(fp->fd == 1){
		accmut_stdout = newfp;
	}else if(fp->fd == 2){
		accmut_stderr = newfp;
	}else{
		fprintf(stderr, "FREOPEN ERROR @__accmut__freopen. TID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
	}

	return newfp;
}

/*********************** POSIX FILE ****************************************/
int __accmut__unlink(const char *pathname){

	if(MUTATION_ID == 0){// only main process can unlink the tmp file
		return unlink(pathname);
	}else{
		return 0;
	}
}

/*********************** INPUT ****************************************/

char* __accmut__fgets(char *buf, int size, ACCMUT_FILE *fp){
	if(size <= 0)
		return NULL;
		
	if(fp->read_cur - fp->bufbase >= fp->fsize){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "READ OVERFLOW @ __accmut__fgets, TID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
		#endif
		return NULL;
	}
	
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

int __accmut__getc(ACCMUT_FILE *fp){
	if(fp->read_cur - fp->bufbase >= fp->fsize){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "READ OVERFLOW @ __accmut__getc, TID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
		#endif
		return NULL;
	}

	return  *((unsigned char *) fp->read_cur++);
}

size_t __accmut__fread(void *buf, size_t size, size_t count, ACCMUT_FILE *fp){
	_IO_size_t bytes_requested = size * count;
	if (bytes_requested == 0)
    	return 0;

    //fprintf(stderr, "%d %d %d\n", fp->fd, bytes_requested, fp->bufend - fp->read_cur);
   
    char *s = buf;

    if(fp->read_cur + bytes_requested > fp->bufend){
		// fprintf(stderr, "READ OVERFLOW @ __accmut__fread\n");
    	memcpy(s, fp->read_cur, fp->bufend - fp->read_cur);
    	int res = (fp->bufend - fp->read_cur)/size;
    	fp->read_cur = fp->bufend;
    	//fprintf(stderr, "%s\n", s);
    	return res;
    }
    memcpy(s, fp->read_cur, bytes_requested);
    fp->read_cur += bytes_requested;
    return count;
}

/*********************** OUTPUT ****************************************/

int __accmut__fputc(int c, ACCMUT_FILE *fp){

	if(fp->write_cur >= fp->bufend){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "OUTPUT OVERFLOW @ __accmut__fputc, ID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
		#endif
		//TODO: need something to fflush
		return EOF;
	}
	// fp->fsize++;
	*(fp->write_cur) = c;
	(fp->write_cur)++;
	return (unsigned char) c;
}
//putc implemented as a macro 
#define __accmut__putc(c, f) ( __accmut__fputc(c, f) )


int __accmut__fputs(const char* s, ACCMUT_FILE *fp){
	int result = EOF;
	size_t len = strlen(s);
	if(fp->write_cur + len >= fp->bufend){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "OUTPUT OVERFLOW @ __accmut__fputs, TID: %d, MUT: %d\n", TEST_ID, MUTATION_ID);
		#endif
	}else{
		memcpy(fp->write_cur, s, len);
		fp->write_cur += len;
		// fp->fsize += len;
		result = 1;	
	}
	return result;
}

int __accmut__puts(const char* s){
	int result = EOF;
	size_t len = strlen(s);
	if( (accmut_stdout->write_cur + len + 1) >= accmut_stdout->bufend){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "STDOUT OVERFLOW @ __accmut__puts, TID: %d, MUT: %d\n", TEST_ID, MUTATION_ID);
		#endif	
	}else{
		memcpy(accmut_stdout->write_cur, s, len);		
		accmut_stdout->write_cur += len;
		*accmut_stdout->write_cur = '\n';
		accmut_stdout->write_cur++;
		//*accmut_stdout->write_cur = '\0';
		//accmut_stdout->write_cur++;
		// accmut_stdout->fsize += len + 1;
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

int __accmut__fprintf(ACCMUT_FILE *fp, const char *format, ...){

	if(fp == NULL){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "NULL ACCMUT FILE  !!! @__accmut__fprintf, TID: %d, MUT: %d\n", TEST_ID, MUTATION_ID);
		#endif
		return 0;
	}

	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsprintf(fp->write_cur, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
	va_end(ap);

	int max;
	switch(fp->fd){
		case 0:
			max = 0;
			break;
		case 1:
			max = MAX_STDOUT_BUF_SIZE;
			break;
		case 2:
			max = MAX_STDERR_BUF_SIZE;
			break;
		default:
			max = MAX_FILE_BUF_SIZE;
			break;
	}

	if((fp->write_cur - fp->bufbase) + ret  > max){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "ACCMUT BUFFER OVERFLOW !  @__accmut__fprintf. TID:%d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
		#endif
		return 0;
	}

	fp->write_cur += ret;
	return ret;
}

int __accmut__printf(const char *format, ...){
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsprintf(accmut_stdout->write_cur, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
	va_end(ap);

	if((accmut_stdout->write_cur - accmut_stdout->bufbase) + ret  > MAX_STDOUT_BUF_SIZE){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "ACCMUT STDOUT BUF OVERFLOW !  @__accmut__printf, TID: %d, MUT: %d\n", TEST_ID, MUTATION_ID);
		#endif
		return 0;
	}

	accmut_stdout->write_cur += ret;
	return ret;
}

size_t __accmut__fwrite(const void *buf, size_t size, size_t count, ACCMUT_FILE *fp){
	int request = size*count;
	if(fp->write_cur + request > fp->bufend){
		#if ACCMUT_IO_DEBUG
			fprintf(stderr, "ACCMUT WRITE OVERFLOW !  @__accmut__fwrite, TID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
		#endif
		return 0;
	}
	char *s = buf;
	memcmp(fp->write_cur, s, request);
	fp->write_cur += request;
	return count;
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

void __accmut__oracal_bufinit(){
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

	if(fp == NULL){
		fprintf(stderr, "NULL ACCMUT_FILE !!! @__accmut__filedump\n");
		exit(0);
	}

	size_t sz;
	if(fp->flags == O_RDONLY){
		sz = fp->fsize;
	}else if(fp->flags == O_WRONLY){
		sz = fp->write_cur - fp->bufbase;
	}else{
		fprintf(stderr, "ERROR FILE FLAGS @ __accmut__filedump\n");
		exit(0);
	}

	fprintf(stderr, "\n********** TID:%d  MID:%d  FD:%d  SIZE:%d ***********\n", \
		TEST_ID, MUTATION_ID, fp->fd, sz);

	int i;
	for(i = 0; i < sz; i++){
		fprintf(stderr, "%c", *(fp->bufbase + i) );
	}
	
	fprintf(stderr, "\n**** END **** TID:%d  MID:%d  FD:%d ***************\n\n", \
		TEST_ID, MUTATION_ID, fp->fd);
}


#endif
