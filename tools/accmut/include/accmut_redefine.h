#ifndef ACCMUT_REDEFINE_H
#define ACCMUT_REDEFINE_H

#ifdef USE_ACCMUT_IO

    /****** FILE DATA STRUCTURE ******/
    #define FILE ACCMUT_FILE
	#define stdin accmut_stdin
	#define stdout accmut_stdout
	#define stderr accmut_stderr

    /****** FILE OPTIONS ******/
    #define fopen(a,b) __accmut__fopen(a,b)
    #define fclose(a) __accmut__fclose(a)
    #define fseek(a,b,c) __accmut__fseek(a,b,c)
    #define ferror(a) __accmut__ferror(a)
    #define fileno(a) __accmut__fileno(a)
    #define freopen(a,b,c) __accmut__freopen(a,b,c)

    /****** POSIX FILE ******/
    #define unlink(a,b) __accmut__unlink(a,b)

    /****** INPUT ******/
    #define fget(a,b,c) __accmut__fgets(a,b,c)
    #define getc(a) __accmut__getc(a)
    #define fread(a,b,c,d) __accmut__fread(a,b,c,d)
    
    /****** OUTPUT ******/
    #define fputc(a,b) __accmut__fputc(a,b)
    #define putc(a,b) __accmut__putc(a,b)
    #define puts(a) __accmut__puts(a)
    #define fwrite(a,b,c,d) __accmut__fwrite(a,b,c,d)
	#define fprintf(...) __accmut__fprintf(__VA_ARGS__)
	#define printf(...) __accmut__printf(__VA_ARGS__)

#endif  //USE_ACCMUT_IO

#endif  //ACCMUT_REDEFINE_H
