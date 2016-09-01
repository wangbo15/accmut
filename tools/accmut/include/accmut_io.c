#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include <unistd.h>

#include "accmut_io.h"
#include "accmut_exitcode.h"

/*************************************************/

#define ACCMUT_IO_DEBUG 1

//INPUT BUF
#define ACCMUT_MAX_FILENO (32)
#define MAX_IN_BUF_SIZE (1<<22)

//4M OUTPUT BUF 
#define MAX_STDOUT_BUF_SIZE (1<<22)
#define MAX_STDERR_BUF_SIZE (1<<22)
#define MAX_FILE_BUF_SIZE (1<<22)
#define LINE_BUF_SIZE (4096)
/*************************************************/

# define EOF (-1)


//extern int MUTATION_ID;
extern int TEST_ID;
extern const char PROJECT[];

/***********************************************************/
//FOR OUTPUT BUFFER
static const char ORACLEDIR[] = "";

static char STDIN_BUFF[LINE_BUF_SIZE];
static char STDOUT_BUFF[MAX_STDOUT_BUF_SIZE];
static char STDERR_BUFF[MAX_STDERR_BUF_SIZE];
static size_t CUR_STDOUT = 0;
static size_t CUR_STDERR = 0;

static char *ORACLEBUFF;
static size_t ORACLESIZE = 0;

/***********************************************************/


#define DEF_STDFILE(NAME, FD, BUF, MAXSIZE, FLAGS) \
	ACCMUT_FILE NAME = {FLAGS, FD, BUF, (BUF + MAXSIZE), BUF, BUF, 0}

DEF_STDFILE(stdfile_0, 0, STDIN_BUFF, LINE_BUF_SIZE, 0);	//unimplemented stdin
DEF_STDFILE(stdfile_1, 1, STDOUT_BUFF, MAX_STDOUT_BUF_SIZE, O_WRONLY);
DEF_STDFILE(stdfile_2, 2, STDERR_BUFF, MAX_STDERR_BUF_SIZE, O_WRONLY);

static ACCMUT_FILE* accmut_stdin = &stdfile_0;
static ACCMUT_FILE* accmut_stdout = &stdfile_1;
static ACCMUT_FILE* accmut_stderr = &stdfile_2;

//map stdio fd to its accmut buffer
static ACCMUT_FILE* STDFD_TO_ACC[ACCMUT_MAX_FILENO] = {0};



// #define ERRMSG(msg) __real_fprintf(stderr, "%s @ %s->%s():%d\tMID: %d\tTID: %d\n", \
// 	msg,__FILE__, __FUNCTION__, __LINE__, MUTATION_ID, TEST_ID)


#define CHECK_FILE(FILE, RET) \
	if ((FILE) == NULL) { return RET; } \
	else { if (((FILE)->_IO_file_flags & _IO_MAGIC_MASK) != _IO_MAGIC) \
	  { return RET; }}


/*********************** FILE OPTIONS ****************************************/


FILE* __wrap_fopen(const char *path, const char *mode){
	
	FILE* fp = fopen(path, mode);

	if(fp == NULL){
		return NULL;
	}

	setvbuf(fp, NULL, _IONBF, 0);

	int _fd = fileno(fp);

	ACCMUT_FILE *acc_fp = (ACCMUT_FILE *)malloc(sizeof(ACCMUT_FILE));

	if(acc_fp == NULL){
		ERRMSG("malloc ACCMUT_FILE ERR");
		exit(FOPEN_ERR);
	}

	if(fp->_flags & _IO_NO_WRITES){//the readonly file

		struct stat sb;
		if(fstat(_fd, &sb) == -1){
			ERRMSG("fstat ERR");
			exit(ENV_ERR);
		}
		if(sb.st_size > MAX_IN_BUF_SIZE){
			#if ACCMUT_IO_DEBUG
				char msg[128] = "INPUT FILE TOO BIG:";
				strcat(msg, path);
				ERRMSG(msg);
			#endif
			return NULL;
		}
		acc_fp->flags = O_RDONLY;
		acc_fp->fd = _fd;
		acc_fp->bufbase = acc_fp->read_cur = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, _fd, 0);
		acc_fp->write_cur = NULL;
		acc_fp->fsize = sb.st_size;
		acc_fp->bufend = acc_fp->bufbase + acc_fp->fsize;

	}else if(fp->_flags & _IO_NO_READS){

		acc_fp->flags = O_WRONLY;
		acc_fp->fd = _fd;
		acc_fp->bufbase = acc_fp->write_cur = (char*) calloc(1, MAX_FILE_BUF_SIZE*(sizeof(char)) );
		acc_fp->read_cur = NULL;
		acc_fp->fsize = 0;
		acc_fp->bufend = acc_fp->bufbase + MAX_FILE_BUF_SIZE*(sizeof(char));
	}

	STDFD_TO_ACC[_fd] = acc_fp;

	return fp;
}

static void __accmut__reset_stdfp(ACCMUT_FILE *fp){
	fp->read_cur = fp->write_cur = fp->bufbase;
	fp->fsize = 0;
}

int __wrap_fclose(FILE *fp){
	if(fp == NULL){
		fclose(fp);
		return EOF;
	}

	int _fd = fileno(fp);

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[_fd];

	if(fp == stdout || fp == stderr || fp == stdin){
		__accmut__reset_stdfp(acc_fp);
		STDFD_TO_ACC[_fd] = NULL;
	}else{
		
		#if ACCMUT_IO_DEBUG
			if(acc_fp == NULL){
				ERRMSG("acc_fp SHOULD NOT BE NULL");
				exit(ILL_STATE_ERR);
			}
		#endif

		if(acc_fp->flags & O_RDONLY){
			int status = munmap(acc_fp->bufbase, acc_fp->fsize);
			if(status < 0){
				ERRMSG("munmap ERROR");
				exit(ENV_ERR);
			}
			free(acc_fp);
			STDFD_TO_ACC[_fd] = NULL;
		}else if(acc_fp->flags & O_WRONLY){
			free(acc_fp->bufbase);
			free(acc_fp);
		}

	}
	return fclose(fp);
}

#if 0

//TODO: unmodified
int __wrap_feof(FILE *fp){

	if(fp == NULL){
		return EOF;
	}
	if((fp->flags & O_RDONLY) == 0){
		#if ACCMUT_IO_DEBUG
		fprintf(stderr, "OPLY SUPPORT O_RDONLY MODE @__wrap_feof\n");
		#endif
		return EOF;
	}

	int result = (fp->flags & _IO_EOF_SEEN) != 0;
	return result;
}

int __wrap_fseek(FILE *fp, size_t offset, int loc){
	//TODO
	return 0;
}

int __wrap_ferror(FILE *fp){
	//TODO:
	return 0;
}

int __wrap_fileno(FILE *fp){
	return fileno(fp);
}

//TODO:
FILE * __wrap_freopen(const char *path, const char *mode, FILE *fp){

	int fd = fileno(fp);

	if(fd < 0){
		return NULL;
	}





	ACCMUT_FILE * newfp = __wrap_fopen(path, mode);
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


	if(fp->fd == 1){
		accmut_stdout = newfp;
	}else if(fp->fd == 2){
		accmut_stderr = newfp;
	}else{
		fprintf(stderr, "FREOPEN ERROR @__wrap_freopen. TID: %d, MUT: %d, fd: %d\n", TEST_ID, MUTATION_ID, fp->fd);
	}

	return newfp;
}

#endif

/*********************** POSIX FILE ****************************************/
int __wrap_unlink(const char *pathname){

	if(MUTATION_ID == 0){// only main process can unlink the tmp file
		return unlink(pathname);
	}else{
		return 0;
	}
}

/*********************** INPUT ****************************************/

char* __wrap_fgets(char *buf, int size, FILE *fp){

	if(size <= 0)
		return NULL;

	int fd = fileno(fp);

	if(fd < 0){
		return NULL;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	if(acc_fp->read_cur - acc_fp->bufbase >= acc_fp->fsize){
		#if ACCMUT_IO_DEBUG
			ERRMSG("READ OVERFLOW");
		#endif
		acc_fp->flags |= _IO_EOF_SEEN;
		return NULL;
	}
	
	size_t len = size - 1;
	
	char *t = (char*) memchr((void*) acc_fp->read_cur, '\n', len);
	
	if(t != NULL){
		len = t - acc_fp->read_cur;
		++t;
		++len;
	}
	
	if(len == 0){
		return NULL;
	}
	
	memcpy((void *) buf , (void *) acc_fp->read_cur, len);
		
	acc_fp->read_cur = acc_fp->read_cur + len;
	
	*(buf + len) = '\0';
	
	return buf;
}

int __wrap_getc(FILE *fp){

	int fd = fileno(fp);

	if(fd < 0){
		return EOF;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	if(acc_fp->read_cur - acc_fp->bufbase >= acc_fp->fsize){
		#if ACCMUT_IO_DEBUG
			ERRMSG("READ OVERFLOW");		
		#endif
		acc_fp->flags |= _IO_EOF_SEEN;
		return EOF;
	}

	return  *((unsigned char *) acc_fp->read_cur++);
}

size_t __wrap_fread(void *buf, size_t size, size_t count, FILE *fp){

	int fd = fileno(fp);
	
	if(fd < 0){
		return 0;
	}

	_IO_size_t bytes_requested = size * count;

	if (bytes_requested == 0)
    	return 0;
   
	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

    char *s = buf;

    if(acc_fp->read_cur + bytes_requested > acc_fp->bufend){

  		#if ACCMUT_IO_DEBUG
			ERRMSG("READ OVERFLOW");
		#endif
		
    	memcpy(s, acc_fp->read_cur, acc_fp->bufend - acc_fp->read_cur);
    	int res = (acc_fp->bufend - acc_fp->read_cur)/size;
    	acc_fp->read_cur = acc_fp->bufend;
    	acc_fp->flags |= _IO_EOF_SEEN;
    	return res;
    }
    memcpy(s, acc_fp->read_cur, bytes_requested);
    acc_fp->read_cur += bytes_requested;
    return count;
}

/*********************** OUTPUT ****************************************/

int __wrap_fputc(int c, FILE *fp){

	int fd = fileno(fp);

	if(fd < 0){
		return EOF;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	if(acc_fp->write_cur >= acc_fp->bufend){
		#if ACCMUT_IO_DEBUG
			ERRMSG("WRITE OVERFLOW");
		#endif
		//TODO: need something to fflush
		return EOF;
	}
	// fp->fsize++;
	*(acc_fp->write_cur) = c;
	(acc_fp->write_cur)++;
	return (unsigned char) c;
}
//putc implemented as a macro 
// TODO: check for __wrap
#define __wrap_putc(c, f) ( __wrap_fputc(c, f) )


int __wrap_fputs(const char* s, FILE *fp){

	int fd = fileno(fp);

	if(fd < 0){
		return EOF;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	int result = EOF;
	size_t len = strlen(s);
	if(acc_fp->write_cur + len >= acc_fp->bufend){
		#if ACCMUT_IO_DEBUG
			ERRMSG("WRITE OVERFLOW");
		#endif
	}else{
		memcpy(acc_fp->write_cur, s, len);
		acc_fp->write_cur += len;
		// fp->fsize += len;
		result = 1;	
	}
	return result;
}

int __wrap_puts(const char* s){

	size_t len = strlen(s);

	int result = EOF;

	if( (accmut_stdout->write_cur + len + 1) >= accmut_stdout->bufend){
		#if ACCMUT_IO_DEBUG
			ERRMSG("accmut_stdout WRITE OVERFLOW");
		#endif
	}else{
		memcpy(accmut_stdout->write_cur, s, len);		
		accmut_stdout->write_cur += len;
		*accmut_stdout->write_cur = '\n';
		accmut_stdout->write_cur++;
		result = 1;	
	}
	return result;
}

int __wrap_fprintf(FILE *fp, const char *format, ...){

	// ERRMSG("ERRMSG\n");
	// return __real_fprintf(stderr, "__wrap_fprintf !!!!!!!\n");


	int fd = fileno(fp);

	if(fd < 0){
		return EOF;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsprintf(acc_fp->write_cur, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
	va_end(ap);

	int max;
	switch(fd){
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

	if((acc_fp->write_cur - acc_fp->bufbase) + ret  > max){
		#if ACCMUT_IO_DEBUG
			ERRMSG("WRITE OVERFLOW");
		#endif
		return 0;
	}

	acc_fp->write_cur += ret;

	return ret;
}

int __wrap_printf(const char *format, ...){
	int ret;
	va_list ap;
	va_start(ap, format);
	ret = vsprintf(accmut_stdout->write_cur, format, ap);	//TODO:: use (STDOUT_BUFF + CUR_STDOUT) instead of tmp
	va_end(ap);

	if((accmut_stdout->write_cur - accmut_stdout->bufbase) + ret  > MAX_STDOUT_BUF_SIZE){
		#if ACCMUT_IO_DEBUG
			ERRMSG("WRITE OVERFLOW");
		#endif
		return 0;
	}

	accmut_stdout->write_cur += ret;
	return ret;
}

size_t __wrap_fwrite(const void *buf, size_t size, size_t count, FILE *fp){

	int fd = fileno(fp);

	if(fd < 0){
		return EOF;
	}

	ACCMUT_FILE* acc_fp = STDFD_TO_ACC[fd];

	#if ACCMUT_IO_DEBUG
		if(acc_fp == NULL){
			ERRMSG("acc_fp SHOULD NOT BE NULL");
			exit(ILL_STATE_ERR);
		}
	#endif

	int request = size*count;
	if(acc_fp->write_cur + request > acc_fp->bufend){
		#if ACCMUT_IO_DEBUG
			ERRMSG("WRITE OVERFLOW");
		#endif
		return 0;
	}
	char *s = (char *) buf;
	memcpy(acc_fp->write_cur, s, request);
	acc_fp->write_cur += request;
	return count;
}



/*********************** BUF UTILS ****************************************/
void __accmut__init_stdstream(){
	STDFD_TO_ACC[0] = accmut_stdin;
	STDFD_TO_ACC[1] = accmut_stdout;
	STDFD_TO_ACC[2] = accmut_stderr;
}



void __accmut__filedump(ACCMUT_FILE *fp);

void __accmut__oracledump();

int __accmut__checkoutput(){
	if(ORACLESIZE != CUR_STDOUT){
		return 1;
	}
	return memcmp(STDOUT_BUFF, ORACLEBUFF, CUR_STDOUT);
}

#define fprintf __real_fprintf

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
		exit(ENV_ERR);
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
		exit(ILL_STATE_ERR);
	}

	size_t sz;
	if(fp->flags == O_RDONLY){
		sz = fp->fsize;
	}else if(fp->flags == O_WRONLY){
		sz = fp->write_cur - fp->bufbase;
	}else{
		fprintf(stderr, "ERROR FILE FLAGS @ __accmut__filedump\n");
		exit(ILL_STATE_ERR);
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
