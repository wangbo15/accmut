
#CC = gcc
CC = $(accmut)/build/Release+Asserts/bin/clang

CFLAFS = -Wall
#CFLAGS = -Wall -g
#WRAP_FLAGS = -fno-builtin-fprintf -Wl,--wrap=fprintf -Wl,--wrap=fopen

DEFI = -D ACCMUT_STATIC_ANALYSIS_EVAL



#SCHEMATA_AR_OBJ = accmut_config.o accmut_arith_common.o accmut_async_sig_safe_string.o accmut_io.o accmut_schem.o
SCHEMATA_AR_OBJ = accmut_config.o accmut_arith_common.o accmut_async_sig_safe_string.o accmut_schem.o

EVAL_AR_OBJ = accmut_config.o accmut_arith_common.o accmut_async_sig_safe_string.o accmut_io.o accmut_sma_eval.o

DMA_AR_OBJ = accmut_config.o accmut_arith_common.o accmut_async_sig_safe_string.o accmut_io.o accmut_dma_fork.o


sche_ar: libamsche.a

eval: libameval.a

dma: libamdma.a

libamsche.a: $(SCHEMATA_AR_OBJ)
	ar -rcs $@ $^

libameval.a: $(EVAL_AR_OBJ)
	ar -rcs $@ $^

libamdma.a: $(DMA_AR_OBJ)
	ar -rcs $@ $^

accmut_config.o: accmut_config.c accmut_config.h accmut_async_sig_safe_string.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_arith_common.o: accmut_arith_common.c accmut_arith_common.h accmut_config.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_async_sig_safe_string.o: accmut_async_sig_safe_string.c accmut_async_sig_safe_string.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_io.o: accmut_io.c accmut_io.h accmut_config.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_schem.o: accmut_schem.c accmut_process.h accmut_io.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_sma_eval.o: accmut_sma_eval.c accmut_process.h accmut_io.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<

accmut_dma_fork.o: 	accmut_dma_fork.c accmut_process.h accmut_io.h accmut_exitcode.h
	$(CC) $(CFLAGS) -c $<
		
		
.PHONY: clean sche_ar
clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
