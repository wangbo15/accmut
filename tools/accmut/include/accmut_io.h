#ifndef ACCMUT_MUT_IO_H
#define ACCMUT_MUT_IO_H


#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define ACCMUT_MAX_FILENO 16
#define IO_BUF_SIZE 8192


typedef struct _ACCMUT_INPUT_FILE{
	int fd;
	char *bufbase;
	char *cur;
	char *bufend;
	size_t fsize;
	
}ACCMUT_FILE;



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

#endif
