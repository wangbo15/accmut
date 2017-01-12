# AccMut
AccMut is a framework of mutation testing for C programs built on the top of the Low Level Virtual Machine (LLVM) version 3.8. AccMut generates mutants on LLVM IR level and integrates some acclerating techniques, such as mutation schemata, split-stream execution and dynamic mutation analysis.

## Compile AccMut

Accmut has several types of compiling configuration. The config file is in the file `accmut/include/llvm/Transforms/AccMut/Config.h` . We can use the macros to choose which type to build, such as mutation generation and mutation instrumentation.
The compiling commands are shown below.

* cd the-root-of-accmut
* mkdir build
* ../configure --enable-optimized
* make

After building we can find the executable file at `build
/Release+Asserts/bin/clang`.

Note that currently AccMut can only be built on a 64-bit linux system. We have compiled it successfully on Ubuntu 14 LST and Ubuntu 16 LST.

## Generate the mutation description file.
Before the complation, make sure the switch-on `ACCMUT_GEN_MUT` in `accmut/include/llvm/Transforms/AccMut/Config.h` should be 1, and other switch-ons should be 0. After compiling it, we can get a runable *clang* for mutation generation. It is better to save the mutation generator in some other place, for we will re-complie AccMut for instrumentation. 
Use the *clang* to compile the program being tested. The mutation description file will be generated in the path *$HOME/tmp/accmut/mutations.txt*. Please make sure the directory has already existed. This file contains all mutations generated. Each line represents a LLVM-IR level mutation. 

The mutation file `mutations.txt` follows the rules below:
`MUT_OPERATOR:FUNCTION:INDEX:ORIGINAL_OPERATION_CODE:[MUT_ACTTION | MUT_OPREAND]`

##Mutant opreators
AccMut supports the opeartors as below:
| Name | Description | Example  |
| :-------------: |:-------------:| :-----:|
| AOR | Replace arithmetic operator     | `a + b` → `a − b`        |
| LOR | Replace logic operator          | `a & b` → `a | b`         |
| ROR | Replace relational operator     | `a == b` → `a >= b`       |
| LVR | Replace literal value           | T → T +1, T → 0, T → 1  |
| COR | Replace bit operator            | `a && b` → `a \|\| b`       |
| SOR | Replace shift operator          | `a >> b` → `a << b`       |
| STDC | Delete a call                  | `foo()` → nop          |
| STDS | Delete a store                 | `a = 5` → nop          |
| UOI | Insert a unary operation        | `b = a` → `b = ++a` OR `foo(a)`->`foo(++a)`      |
| ROV | Replace the operation value     | `foo(a,b)` → `foo(b,a)` OR `a-b`->`b-a`      |
| ABV | Take absolute value     | `foo(a,b)` → `foo(abs(a),b)` OR `a-b` ->  `abs(a)-b`      |



## Instrument the mutants into the C program.
Note that the current *clang* of AccMut should be built under the configuration  `ACCMUT_DYNAMIC_ANALYSIS_INSTRUEMENT` in `accmut/include/llvm/Transforms/AccMut/Config.h` is 1 and others are 0.

###Imlementation for instrumentation

####For arithmetic instructions
Arithmetic mutants are based on the LLVM-IRs whose result can be directly computed, such as arithmetic instructions and icmp instructions. We can apply AOR, LOR, ROR, LVR, COR, SOR, UOI, ROV, ABV on these LLVM-IRs. 

AccMut directly replace the mutated location to our process function. For example,
`%add = add nsw i32 %0, %1` will be transformed into `%add = call i32 @__accmut__process_i32_arith(i32 MUT_BEGIN_ID, i32 MUT_END_ID, i32 %0, i32 %1)`.
    
And `%cmp = icmp ne i32 %3, %4` will be transformed into `%cmp = call i32 @__accmut__process_i32_cmp(i32 MUT_BEGIN_ID, i32 MUT_END_ID, i32 %3, i32 %4)`. We can pre-compute all the results of the mutants in the process functions.

####For side-effects instructions
Some LLVM-IRs cannot be directly computed their results, and they may have side-effects, such as store and call instructions. We can apply STDC, STDS, UOI, ROV, ABV on these IRs. It is much complex to handle them.

For example, a method invocation:
```
  %1 = load i32, i32* %a.addr, align 4
  %call = call i32 @add(i32 %1, i32 2)
```
will be transfered into a sequence of IRs as below:
```
  %1 = call i32 (i32, i32, i32, ...) @__accmut__prepare_call(i32 1, i32 16, i32 2, i16 512, i32* %a.addr, i16 513, i32* %cons_alias)
  %hasstd = icmp eq i32 %1, 0
  br i1 %hasstd, label %if.then, label %if.else

if.then:                                          ; preds = %for.body
  %2 = load i32, i32* %a.addr, align 4
  %const_load = load i32, i32* %cons_alias
  %3 = call i32 @add(i32 %2, i32 %const_load)
  br label %if.end

if.else:                                          ; preds = %for.body
  %4 = call i32 @__accmut__stdcall_i32()
  br label %if.end
```
AccMut pass the pointers of the parameters to the function `__accmut__prepare_call`, in which we can judge the mutants' types and apply them. For example, we can exange the pointees' value of the 2 pointers to implement an ROV mutant. And if the current mutant is STDC, `__accmut__prepare_call` will fork a new process then directly return 0. The new process will go into the `if.else` branch, in which there is just a dummy function call. By this way, we can handle mutants on `call` and `store`.


##Acknowledgements
Thanks for the excellent project LLVM. Our code follows the GPL license. 
