#ifndef ACCMUT_IO_H
#define ACCMUT_IO_H


#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "accmut_config.h"

/*************************************************/

//INPUT BUF
#define MAX_IN_BUF_SIZE (1<<22)

//4M OUTPUT BUF 
#define MAX_STDOUT_BUF_SIZE (1<<22)
#define MAX_STDERR_BUF_SIZE (1<<22)
#define MAX_FILE_BUF_SIZE (1<<22)
#define LINE_BUF_SIZE (4096)
/*************************************************/

# define EOF (-1)


typedef struct _ACCMUT_FILE{
	int flags;
	int fd;
	char *bufbase;
	char *bufend;
	char *read_cur;
	char *write_cur;
	size_t fsize;	//only for input file
	
}ACCMUT_FILE;


// extern ACCMUT_FILE* accmut_stdin;
// extern ACCMUT_FILE* accmut_stdout;
// extern ACCMUT_FILE* accmut_stderr;


/*********************** FILE OPTIONS ****************************************/

FILE* __wrap_fopen(const char *path, const char *mode);

int __wrap_fclose(FILE *fp);

int __wrap_feof(FILE *fp);

int __wrap_fseek(FILE *fp, size_t offset, int loc);

int __wrap_ferror(FILE *fp);

int __wrap_fileno(FILE *fp);

FILE * __wrap_freopen(const char *path, const char *mode, FILE *fp);

/*********************** POSIX FILE ****************************************/
int __wrap_unlink(const char *pathname);

/*********************** INPUT ****************************************/

char* __wrap_fgets(char *buf, int size, FILE *fp);

int __wrap_getc(FILE *fp);

size_t __wrap_fread(void *buf, size_t size, size_t count, FILE *fp);

/*********************** OUTPUT ****************************************/

int __wrap_fputc(int c, FILE *fp);

//TODO
#define __accmut__putc(c, f) ( __wrap_fputc(c, f) )

int __wrap_fputs(const char* s, FILE *fp);

int __wrap_puts(const char* s);

//int __wrap_fprintf(FILE *fp, const char *format, ...);
int __wrap_fprintf(FILE *fp, const char *format, ...);



int __wrap_printf(const char *format, ...);

size_t __wrap_fwrite(const void *buf, size_t size, size_t count, FILE *fp);



/*********************** BUF UTILS ****************************************/

void __accmut__init_stdstream();

void __accmut__oracledump();

int __accmut__checkoutput();

void __accmut__exit_check_output();

void __accmut__oracal_bufinit();

void __accmut__setout(int id);

void __accmut__oracledump();

void __accmut__filedump(FILE *fp);


#endif
