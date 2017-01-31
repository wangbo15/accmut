#include "accmut_config.h"
#include "accmut_exitcode.h"

#if ACCMUT_STATIC_ANALYSIS_EVAL
#include <math.h>
#endif


#define __real_fprintf fprintf
#define __wrap_fopen fopen
#define __wrap_fclose fclose

const char PROJECT[]="tcas";

//#if ACCMUT_DYNAMIC_ANALYSIS_FORK
	int HOLDER[1024] __attribute__((aligned(0x1000))) = {0};
	#define MUTATION_ID (HOLDER[0])
//#else
//	int MUTATION_ID = 0;
//#endif

int TEST_ID = -1;

/** Set Timer **/
struct itimerval ACCMUT_PROF_TICK;
struct itimerval ACCMUT_REAL_TICK;


struct timeval tv_begin, tv_end;
//struct rusage usage_fbegin, usage_fmid, usage_fend;
//struct rusage usage_cbegin, usage_cend;

/**********************************************************/
//FOR STATISTICS
unsigned long long EXEC_INSTS = 0;
/**********************************************************/

#define MUTFILELINE 128

Mutation* ALLMUTS[MAXMUTNUM + 1];
int MUT_NUM;
int *MUTS_ON;



/************* ALL EXIT HANDLER ***************************/
void __accmut__exit_check_output();

void __accmut__exit_time(){

#if 0
	__accmut__filedump(stdout);
#endif

#if 1
	if(MUTATION_ID != 0){

		return;
	}
#endif 

	gettimeofday(&tv_end, NULL);

	long real_sec =  tv_end.tv_sec - tv_begin.tv_sec;
	long real_usec = tv_end.tv_usec - tv_begin.tv_usec;

	if(real_usec < 0){
		real_sec--;
		real_usec += 1000000;
	}


#if ACCMUT_STATIC_ANALYSIS_EVAL
	char* path = "evaltime";
#else
	char *path = "timeres";
#endif

	int fd = open(path, O_WRONLY | O_CREAT | O_APPEND);

	char res[128] = {0};

	__accmut__strcat(res, __accmut__itoa(TEST_ID, 10));
	__accmut__strcat(res, "\t");

	__accmut__strcat(res, __accmut__itoa(real_sec, 10));
	__accmut__strcat(res, "\t");

	__accmut__strcat(res, __accmut__itoa(real_usec, 10));
	__accmut__strcat(res, "\n");

	write(fd, res, __accmut__strlen(res));
	close(fd);

	//__accmut__filedump(accmut_stdout);
}

#define ACCMUT_MUTE 1

static void __accmut__omitdump__handler(int sig){

	int exitcd = 0;

#if ACCMUT_MUTE

	switch(sig){
		case SIGSEGV:
			exitcd = SIGSEGV_ERR;
			break;
		case SIGABRT: 
			exitcd = SIGABRT_ERR;
			break;
		case SIGFPE:
			exitcd = SIGFPE_ERR;
			break;
		default:
			break;
	}

#else

	char *stderrpath = "/dev/stderr";

	int fd = open(stderrpath, O_WRONLY);

	char msg[128] = {0};

	switch(sig){
		case SIGSEGV:
			exitcd = SIGSEGV_ERR;
			__accmut__strcat(msg, "SIGSEGV: ");
			break;
		case SIGABRT: 
			exitcd = SIGABRT_ERR;
			__accmut__strcat(msg, "SIGABRT: ");
			break;
		case SIGFPE:
			exitcd = SIGFPE_ERR;
			__accmut__strcat(msg, "SIGFPE: ");
			break;
		default:
			ERRMSG("UNHANDLED SIGNAL ");
			break;
	}
	__accmut__strcat(msg, __accmut__itoa(TEST_ID, 10));
	__accmut__strcat(msg, "\t");

	__accmut__strcat(msg, __accmut__itoa(MUTATION_ID, 10));
	__accmut__strcat(msg, "\n");

	write(fd, msg, __accmut__strlen(msg));

#endif

#if 1
	_exit(exitcd);
	// kill(getpid(), SIGKILL);
#else
    signal(SIGABRT, SIG_DFL);
#endif
}

/* The signal handler of time out process */
static void __accmut__timeout_handler(int sig){
	#if 0
	int fd = 2;

	char msg[128] = {0};

	__accmut__strcat(msg, "TIMEOUT: ");
	__accmut__strcat(msg, __accmut__itoa(TEST_ID, 10));
	__accmut__strcat(msg, "\t");

	__accmut__strcat(msg, __accmut__itoa(MUTATION_ID, 10));
	__accmut__strcat(msg, "\n");

	write(fd, msg, __accmut__strlen(msg));
	#endif

#if 0
    // _exit(TIMEOUT_ERR);
    kill(getpid(), SIGKILL);
#else
    exit(TIMEOUT_ERR);
#endif
    
}

void __accmut__set_sig_handlers(){

	signal(SIGPROF, __accmut__timeout_handler);

	signal(SIGALRM, __accmut__timeout_handler);

    signal(SIGSEGV, __accmut__omitdump__handler);

    signal(SIGABRT, __accmut__omitdump__handler);

    signal(SIGFPE, __accmut__omitdump__handler);

}


#define INTTERVAL_SEC (0)
#define INTTERVAL_USEC (50)

#define DEFAULT_SEC (0)
#define DEFAULT_USEC (5000)
#define REAL_TIMES (1.5)

void __accmut__sepcific_timer(){
	long v_sec = 0;
	long v_usec = 0;

	char path[128] = {0};
	sprintf(path, "%s%s%s/%d", getenv("HOME"), "/tmp/accmut/oritime/", PROJECT, TEST_ID);
	FILE * fp = fopen(path, "r");
	if(fp == NULL){
		#if 0
		__real_fprintf(stderr, "WARNING : ORI TIME FILE DOSE NOT EXISIT : %s\n", path);
		#endif
		//if the ori time file does not exisit, use the default timer value.
		v_sec = DEFAULT_SEC;
		v_usec = DEFAULT_USEC;
	}else{
		fscanf(fp, "%ld", &v_sec);
		fscanf(fp, "%ld", &v_usec);
		fclose(fp);
	}
	
    ACCMUT_PROF_TICK.it_value.tv_sec = v_sec;  // sec
    ACCMUT_PROF_TICK.it_value.tv_usec = v_usec; // u sec.
    ACCMUT_PROF_TICK.it_interval.tv_sec = INTTERVAL_SEC;
    ACCMUT_PROF_TICK.it_interval.tv_usec =  INTTERVAL_USEC;

    
	ACCMUT_REAL_TICK.it_value.tv_sec = v_sec * REAL_TIMES;  // sec
    ACCMUT_REAL_TICK.it_value.tv_usec = v_usec * REAL_TIMES; // u sec.
    ACCMUT_REAL_TICK.it_interval.tv_sec = INTTERVAL_SEC * REAL_TIMES;
    ACCMUT_REAL_TICK.it_interval.tv_usec =  INTTERVAL_USEC * REAL_TIMES;

    #if 0
    __real_fprintf(stderr, "PROFTIMER: %ld %ld ; REALTIMER: %ld %ld\n", 
    				ACCMUT_PROF_TICK.it_value.tv_sec ,
    				ACCMUT_PROF_TICK.it_value.tv_usec = v_usec , 
    				ACCMUT_REAL_TICK.it_value.tv_sec , 
    				ACCMUT_REAL_TICK.it_value.tv_usec);
    #endif
}


void __accmut__exec_inst_nums(){
	// fprintf(stderr, "0");
	EXEC_INSTS++;
}


void __accmut__debug(int index){
	
}


void __accmut__load_all_muts(){
    char path[256];
    strcpy(path, getenv("HOME"));
    strcat(path, "/tmp/accmut/mutations.txt");
	FILE *fp = fopen(path, "r");
	if(fp == NULL){
		ERRMSG("mutation.txt OPEN ERR");
		exit(FOPEN_ERR);
	}
	int id = 1;	
	char type[4];
	char buff[MUTFILELINE];	
	char tail[40];
	
	#if ACCMUT_STATIC_ANALYSIS_EVAL
	int idx;
	int cur_loc = 1;	//begin from 1, not 0
	int pre_idx = -1;
	char pre_func[64] = {0};
	#endif

	while(fgets(buff, MUTFILELINE, fp)){

		#if ACCMUT_STATIC_ANALYSIS_EVAL
			char func[64] = {0};
			sscanf(buff, "%3s:%[^:]:%d:%s", type, func, &idx, tail);

			int is_in_loop = 0;
			if(idx < 0){
				idx = 0 - idx;
				is_in_loop = 1;
			}

			Mutation* m = (Mutation *)malloc(sizeof(Mutation));

			if((strcmp(pre_func, func)) != 0 || idx != pre_idx){
				cur_loc++;
			}
			pre_idx = idx;
			strcpy(pre_func, func);
			if(is_in_loop)
				m->location = 0 - cur_loc;
			else
				m->location = cur_loc;
		#else
			sscanf(buff, "%3s:%*[^:]:%*[^:]:%s", type, tail);
			Mutation* m = (Mutation *)malloc(sizeof(Mutation));	
		#endif

		if(!strcmp(type, "AOR")){
			m->type = AOR;
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->sop = s_op;
			m->op_0 = t_op;
		}else if(!strcmp(type, "LOR")){
			m->type = LOR;
			int s_op, t_op;
			sscanf(tail, "%d:%d", &s_op, &t_op);
			m->sop = s_op;
			m->op_0 = t_op;
		}else if(!strcmp(type, "ROR")){
			m->type = ROR;
			int op, s_pre, t_pre;
			sscanf(tail, "%d:%d:%d", &op, &s_pre, &t_pre);
			m->sop = op;
			m->op_1 = s_pre;
			m->op_2 = t_pre;
		}else if(!strcmp(type, "STD")){
			m->type = STD;
			int op, f_tp, retval;
			if(strlen(tail) == 4){//return void 
				sscanf(tail, "%d:%d", &op, &f_tp);
				m->sop = op;	//must be 0
				m->op_1 = f_tp;
			}else{//return i32 or i64
				sscanf(tail, "%d:%d:%d", &op, &f_tp, &retval);
				m->sop = op;
				m->op_1 = f_tp;	//32, or 64
				m->op_2 = retval;
			}
		}else if(!strcmp(type, "LVR")){
			m->type = LVR;
			int op, op_i;
			long s_c, t_c;
			sscanf(tail, "%d:%d:%ld:%ld", &op, &op_i, &s_c, &t_c);
			m->sop = op;
			m->op_0 = op_i;
			m->op_1 = s_c;
			m->op_2 = t_c;
		}else if(!strcmp(type, "UOI")){
			m->type = UOI;
			int op, op_i, tp;
			sscanf(tail, "%d:%d:%d", &op, &op_i, &tp);
			m->sop = op;
			m->op_1 = op_i;
			m->op_2 = tp;
		}else if(!strcmp(type, "ROV")){
			m->type = ROV;
			int op, op1, op2;
			sscanf(tail, "%d:%d:%d", &op, &op1, &op2);
			m->sop = op;
			m->op_1 = op1;
			m->op_2 = op2;
		}else if(!strcmp(type, "ABV")){
			m->type = ABV;
			int op, op_i;
			sscanf(tail, "%d:%d", &op, &op_i);
			m->sop = op;
			m->op_0 = op_i;
		}else{
			__real_fprintf(stderr, "ERROR MUT TYPE: %d:%s\n", id, buff);
			exit(MUT_TP_ERR);
		}
		ALLMUTS[id] = m;
		id++;
	}
	MUT_NUM = id - 1;

	#if 0
	__real_fprintf(stderr, "\n----------------- DUMP ALL MUTS ------------------\n");
	__real_fprintf(stderr, "TOTAL MUTS NUM : %d\n", MUT_NUM);
	int i;
	for(i = 1; i <= MUT_NUM; i++){
		Mutation *m = ALLMUTS[i];
	
		#if ACCMUT_STATIC_ANALYSIS_EVAL
			__real_fprintf(stderr, "MUT %d => LOC: %d , TP: %d , SOP: %d , OP0 : %d , OP1 : %d , OP2 : %d\n",
				i, m->location, m->type, m->sop, m->op_0, m->op_1, m->op_2);
		#else
			__real_fprintf(stderr, "MUT %d => TP: %d , SOP: %d , OP0 : %d , OP1 : %d , OP2 : %d\n",
				i, m->type, m->sop, m->op_0, m->op_1, m->op_2);
		#endif
	}
	__real_fprintf(stderr, "----------------- END DUMP ALL MUTS ---------------\n\n");

	#endif
}

