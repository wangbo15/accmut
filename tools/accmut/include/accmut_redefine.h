//#ifndef ACCMUT_REDEFINE_H
//#define ACCMUT_REDEFINE_H

/********** unimplemented of stdio.h *****************/

#define UNIMPL *UNIMPLEMENTED_STDIO_H*

/*************** STDIO.H *******************/
//Formatted Input/Output functions
//#define fprintf UNIMPL
//#define fscanf UNIMPL
//#define printf UNIMPL
#define scanf UNIMPL
//#define sprintf UNIMPL
//#define sscanf UNIMPL
#define vfprintf UNIMPL
#define vprintf UNIMPL
#define vsprintf UNIMPL

//File Operation functions
//#define fclose UNIMPL
//#define fflush UNIMPL
//#define fopen UNIMPL
//#define freopen UNIMPL
#define remove UNIMPL
#define rename UNIMPL
#define setbuf UNIMPL
#define setvbuf UNIMPL
#define tmpfile UNIMPL
#define tmpnam UNIMPL

//Character Input/Output functions
#define fgetc UNIMPL
//#define fgets UNIMPL
//#define fputc UNIMPL
//#define fputs UNIMPL
//#define getc UNIMPL
#define getchar UNIMPL
#define gets UNIMPL
//#define putc UNIMPL
#define putchar UNIMPL
//#define puts UNIMPL
//#define ungetc UNIMPL


//Block Input/Output functions
#define fread UNIMPL
//#define fwrite UNIMPL

//File Positioning functions
#define fgetpos UNIMPL
#define fseek UNIMPL
#define fsetpos UNIMPL
#define ftell UNIMPL
#define rewind UNIMPL

//Error Handling functions
#define clearerr UNIMPL
#define feof UNIMPL
//#define ferror UNIMPL
//#define perror UNIMPL

/*************** END OF STDIO.H *************/

//TODO::
/*************** UNISTD.H *******************/



//#ifdef USE_ACCMUT_IO

/****** FILE DATA STRUCTURE ******/
#undef FILE
#undef stdin
#undef stdout
#undef stderr


#define FILE ACCMUT_FILE
#define stdin accmut_stdin
#define stdout accmut_stdout
#define stderr accmut_stderr

/****** FILE OPTIONS ******/
#define fopen(a,b) __accmut__fopen(a,b)
#define fclose(a) __accmut__fclose(a)
//#define fseek(a,b,c) __accmut__fseek(a,b,c)
#define ferror(a) __accmut__ferror(a)
#define fileno(a) __accmut__fileno(a)
#define freopen(a,b,c) __accmut__freopen(a,b,c)
#define feof(a) __accmut__feof(a)


/****** POSIX FILE ******/
#define unlink(a) __accmut__unlink(a)

/****** INPUT ******/
#define fgets(a,b,c) __accmut__fgets(a,b,c)
#define getc(a) __accmut__getc(a)
#define fread(a,b,c,d) __accmut__fread(a,b,c,d)
#define ungetc(a,b) __accmut__ungetc(a,b)
#define fscanf(...) __accmut__fscanf(__VA_ARGS__)


/****** OUTPUT ******/
#define fputc(a,b) __accmut__fputc(a,b)
#define putc(a,b) __accmut__putc(a,b)
#define puts(a) __accmut__puts(a)
#define fputs(a,b) __accmut__fputs(a,b)
#define fwrite(a,b,c,d) __accmut__fwrite(a,b,c,d)
#define fprintf(...) __accmut__fprintf(__VA_ARGS__)
#define printf(...) __accmut__printf(__VA_ARGS__)

#define perror(a) __accmut__perror(a)
#define fflush(a) __accmut__fflush(a)


//#endif  //USE_ACCMUT_IO

//#endif  //ACCMUT_REDEFINE_H
