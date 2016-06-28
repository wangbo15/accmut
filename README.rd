# AccMut
AccMut is a framework of mutation testing for C programs. AccMut implements 
two accelerating methods of mutaiton testing, one is static mutation analysis, 
the other one is dynamic mutation analysis. AccMut is developed integrated into 
the Low Level Virtual Machine (LLVM).


### Install AccMut
 * cd the-root-of-accmut
 * mkdir build
 * ../configure --enable-optimized
 * make

Then Accmut is integrated into the C compiler Clang, which is located at "build
/Release+Asserts/bin/clang".

### Generate the mutation description file.
Use the new compiler at "build/Release+Asserts/bin/clang" to compile the 
program being tested. The mutation description file will be generated in the 
path "$HOME/tmp/accmut/mutations.txt". Make sure the directory has 
already existed. This file contains all mutations generated. Each line represents 
a LLVM-IR level mutation.


