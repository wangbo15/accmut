# AccMut
AccMut is a framework of mutation testing for C programs built on the top of the Low Level Virtual Machine (LLVM) version 3.8. AccMut generates mutants on LLVM IR level and integrates some acclerating techniques, such as mutation schemata, split-stream execution and dynamic mutation analysis.

## Overview
AccMut contains three main components: Mutation Generator, Mutation Instrumenter and Runtime Library. AccMut processes mutation analysis by the following steps.
* Compile the source code of the program into LLVM IR. Mutation Generator scans the IR code and generates the description file file of mutants on the LLVM IR code. Note that we can modify the description file to sample the mutants.
* Mutation Instrumenter instrument mutants into the IR code of the program using mutation schemata, according to the description file. Then compile the IR code into object files.
* LLVM links the object files with Runtime Library and obtains an executable file of the program.
* The executable file executes each test on all mutants.

## Directory Layout
AccMut is based on the project LLVM. 

Mutation Generator and  Mutation Instrumenter are implemented in LLVM passes, located in the path `accmut/include/llvm/Transforms/AccMut/` and `accmut/lib/Transforms/AccMut/`. 

Runtime Library has two ways of implementation. For the simple projects, we can use the header files to provide the definations of the instrumented functions. For the large projects, we can provide the static link library. Both of them are located in `accmut/tools/accmut/`.

## Compile AccMut

Accmut has several types of compiling configuration. The config file is in the file `accmut/include/llvm/Transforms/AccMut/Config.h` . We can use the macros to choose which type to build, such as mutation generation and mutation instrumentation.
The compiling commands are shown below.

* `cd the-root-of-accmut`
* `python first_set_up.py` (Root permission is needed during the execution.)
* `mkdir build`
* `../configure --enable-optimized`
* `make`

Note that the add the `--enable-optimized` option will get the release version of LLVM, if you want to get the debug version please remove it. The compiling time of release is much shorter than debug.

After building we can find the executable file at `build
/Release+Asserts/bin/clang`.

Note that currently AccMut can only be built on a 64-bit linux system. We have compiled it successfully on Ubuntu 14 LST and Ubuntu 16 LST.

## Generate the mutation description file.
Before the complation, make sure the switch-on `ACCMUT_GEN_MUT` in `accmut/include/llvm/Transforms/AccMut/Config.h` should be 1, and other switch-ons should be 0. After compiling it, we can get a runable *clang* for mutation generation. It is better to save the mutation generator in some other place, for we will re-complie AccMut for instrumentation. 
Use the *clang* to compile the program being tested. The mutation description file will be generated in the path `$HOME/tmp/accmut/mutations.txt`. Please make sure the directory has already existed. This file contains all mutations generated. Each line represents a LLVM-IR level mutation. 

The mutation file `mutations.txt` follows the rules below:
`MUT_OPERATOR:FUNCTION:INDEX:ORIGINAL_OPERATION_CODE:[MUT_ACTTION | MUT_OPREAND]*`

We can also sample the mutants by modifying this text file.

As we mutate on the LLVM IR level, each IR instruction corresponds to a location. We apply a set of mutation operators on IR
instructions to produce mutants.

## Mutant opreators
AccMut supports the opeartors as below:

|Name         | Description           | Example  |
| :-------------: |:-------------:| :-----:|
|AOR    |  Replace arithmetic operator | `a + b` -> `a − b` |
| LOR | Replace logic operator          | `a & b` -> `a | b`  |
| ROR | Replace relational operator     | `a == b` -> `a >= b`       |
| LVR | Replace literal value           | T -> T+1, T -> 0, T -> 1  |
| COR | Replace bit operator            | `a && b` -> `a || b`       |
| SOR | Replace shift operator          | `a >> b` -> `a << b`       |
| STDC | Delete a call                  | `foo()` -> nop          |
| STDS | Delete a store                 | `a = 5` -> nop          |
| UOI | Insert a unary operation        | `b = a` -> `b = ++a` OR `foo(a)`->`foo(++a)`  |
| ROV | Replace the operation value     | `foo(a,b)` -> `foo(b,a)` OR `a-b`->`b-a`      |
| ABV | Take absolute value     | `foo(a,b)` -> `foo(abs(a),b)` OR `a-b` ->  `abs(a)-b`      |


## Instrument the mutants into the C program.
Note that the current *clang* of AccMut should be built under the configuration  `ACCMUT_DYNAMIC_ANALYSIS_INSTRUEMENT` in `accmut/include/llvm/Transforms/AccMut/Config.h` is 1 and others are 0.

### Imlementation for Mutation Instrumenter
Mutation Instrumenter modifies the IR according to the description file. According to the type of an IR instruction,
Mutation Instrumenter has different instrument strategies. For the arithmetic-based IRs, Mutation Instrumenter just
replace the IR instruction with corresponding process function. For `store` and `call`, Mutation Instrumenter
adds process functions before the mutated location and modifies the control flow to perform skipping the location.

#### Illustrating example
We are given a fragment C code :
```
a=b-foo(c,d)
```
All the variables are int.
Note that LLVM IR is a three-address, SSA form code, so the source IRs contain two locations and the pseudo code shows below.
```
int res = foo(c,d)     //LOCATION 0
a = b-res	             //LOCATION 1
```
Assuming we have three mutants on the two locations, an AOR
(`a=b+res`) at location 1, an ROV (`res=foo(d,c)`) at
location 0, and an STDC (`res=UNINIT`),
Mutation Instrumenter modifies the IRs as below.
```
//int prepare_call(int location_id,int number_and_tpyes_of_paras, ...);
int IS_STDC = prepare_call(0, FLAG, &c, &d)
int res;
if(! IS_STDC){
  res = foo(c,d)
}
//int process_arith(int loc_id,int left,int right);
a = process_arith(1, b, res)
```
For the call instruction, Mutation Instrumenter adds a process function before and passes the pointers of `c` and `d` to perform switching their values as the ROV(Line 2) and adds control flow to skip the call as the STDC (Line 3 o 6). Note that `prepare_call` is a variable parameter function, so it can handle functions with different parameter numbers. LLVM IR is well typed, we can get the number and the types of the parameters statically and generate a bit vector `FLAG` to encapsulate the information.

For the arithmetic instruction, Mutation Instrumenter directly replaces it with the corresponding process function, passing the original operands(Line 8). 

Moreover, in Mutation Instrumenter, `store` can be treated as a special condition of `call`, that is, a function with two parameters, a value and a printer.

For the ROV, the `prepare_call` will check whether `c` equals to `d` by dereferencing their pointers. If `c` equals to `d`, the mutant is equivalence modulo the current state, otherwise AccMut will conservatively consider the states are not equivalent and will fork a process, then switch the values of `c` and `d` utilizing the pointers.
For the STDC, `prepare_call` treats it as mutant can not be tried and directly fork a new process and return 1 to `IS_STDC`, so that the control flow can skip the call.

#### Details for arithmetic instructions
Arithmetic mutants are based on the LLVM-IRs whose result can be directly computed, such as arithmetic instructions and icmp instructions. We can apply AOR, LOR, ROR, LVR, COR, SOR, UOI, ROV, ABV on these LLVM-IRs. 

AccMut directly replace the mutated location to our process function. For example,
`%add = add nsw i32 %0, %1` will be transformed into `%add = call i32 @__accmut__process_i32_arith(i32 MUT_BEGIN_ID, i32 MUT_END_ID, i32 %0, i32 %1)`.
    
And `%cmp = icmp ne i32 %3, %4` will be transformed into `%cmp = call i32 @__accmut__process_i32_cmp(i32 MUT_BEGIN_ID, i32 MUT_END_ID, i32 %3, i32 %4)`. We can pre-compute all the results of the mutants in the process functions.

#### Details for side-effects instructions
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

##Publication
*Faster mutation analysis via equivalence modulo states*
    Bo Wang, Yingfei Xiong, Yangqingwei Shi, Lu Zhang and Dan Hao,     ISSTA 2017. [[download]][issta17].
    
[issta17]: http://sei.pku.edu.cn/~xiongyf04/papers/ISSTA17.pdf


## Acknowledgements
Thanks for the excellent project LLVM. Our code follows the GPL license. 
