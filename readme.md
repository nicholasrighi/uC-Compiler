# Project overview

This is a compiler that implements a subset of C called [uC](https://www.it.uu.se/katalog/aleji304/CompilersProject/uc.html). The compiler takes a valid uC program as an input and produces x86_64 code that can be assembled and linked into an executable. Despite x86 allowing you to operate on memory directly, the compiler treates x86 as a RISC machine, so the only operations done to memory are the loading/storing of values. All other operations require loading the value into a register, operating on that value, and then storing the resulting value back to memory. 

# Installation
  
`git clone URL`
`cd URL`
`make `

# Usage

`./compiler <optional flags> -f <input file path> -o <output file name>`

The optional flags are

* `-d   Prints the AST to the debugging file `
* `-s   Prints the symbol table to the debugging file`
* `-t   Enable bison output trace`

# Compiler internals

The compiler has four main stages

1.  Flex/Bison parsing. Determines if the input is a valid uC program, and if so constructs the corresponding AST
2.  AST checking with the various visitor classes. This phase of the compiler ensures that the input program declares variables
    before they're used, that all control flow paths for non void functions have valid return statements, and that types are used/assigned 
    correctly. 
3.  The last visitor to run is the intermediate code generator. This generates pseudo assembly for the input program, and prints the pseudo assembly
    to the debug file. 
4.  The final stage to run is the register allocator. This takes the intermediate code generated in the previous step and generates corresponding x86_64 assembly code. The 
    allocator allocates free registers until none are avaliable, then spills to memory the value that will be used furthest in the future. Of the 16 general purpose registers, all
    but `rax` and `r15` are avaliable for general allocation. `rax` is saved specifically for the return value, and `r15` is reserved for use in conditional statements. 

# Areas for improvement
