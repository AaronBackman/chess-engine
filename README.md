# Chess Engine
## What is this?
This is a chess program capable of playing chess at the same level as a master-level human. It is written in C programming language, for speed and efficiency.
It implements the UCI (universal chess interface) protocol, so it can be connected to most desktop chess GUIs
## How to use it?
The C source files must first be compiled, with gcc, for example  
gcc -O3 -pthread *.c -lpthread -o engine  
./engine  
