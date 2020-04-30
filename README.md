# Description of Systems Projects

# Proj1 - Bit:
File: bits.c

The purpose of this assignment is to become more familiar with bit-level representations of integers
and floating point numbers. This is done by solving a series of programming “puzzles.” Many of these
puzzles are quite artificial, but they cause the programmer to think much more about bits in working their way
through them.

# Proj2 - Bomb:
File: bomb.c

The purpose of this assignment is to master machine language type commands and understand how programs work by
walking through the disassembled binary of the bomb program file. 
A binary bomb is a program that consists of a sequence of phases. Each phase expects you to type a particular 
string on stdin. If you type the correct string, then the phase is defused and the bomb proceeds to the next phase. 
Otherwise, the bomb explodes by printing "BOOM!!!" and then terminating. The bomb is defused when every phase has 
been defused. Using a debugger to step through the disassembled binary of the bomb file, discover the secret 
strings that can be used to defuse the bomb at each phase.

# Proj3 - Buffer Overflow:
File: exploit.txt

This assignment involves generating a total of five attacks on two programs having different security
vulnerabilities. Outcomes you will gain from this lab include:
• Learning different ways that attackers can exploit security vulnerabilities when programs
do not safeguard themselves well enough against buffer overflows.
• A better understanding of how to write programs that are more secure,
as well as some of the features provided by compilers and operating systems to make programs
less vulnerable.
• A deeper understanding of the stack and parameter-passing mechanisms of x86-64
machine code.
• A deeper understanding of how x86-64 instructions are encoded.
• More experience with debugging tools such as gdb and objdump.

# Proj4 - Shell:
File: tsh.c

The purpose of this assignment is to become more familiar with the concepts of process control and
signalling. This is done by writing a simple Unix shell program from scratch that supports job control.

# Proj5 - Cache:
This lab is designed to help understand the impact that cache memories can have on the performance of C programs.
The lab consists of two parts. 

In the first part is to write a small C program from scratch (about 200–300 lines)
that simulates the behavior of a cache memory. 

In the second part, the task is to optimize a small matrix
transpose function, with the goal of minimizing the number of cache misses.

# Proj6 - Proxy:
In this lab, the task was to write a simple HTTP proxy that caches web objects. 

For the first part of the lab, the task was to set up the proxy to accept incoming connections, read and 
parse requests, forward requests to web servers, read the servers’ responses, and forward those responses 
to the corresponding clients. This first part involves learning about basic HTTP operation and how to use 
sockets to write programs that communicate over network connections. 

In the second part, the task was to upgrade the proxy to deal with multiple concurrent connections. This
introduces concurrency, a crucial systems concept into the program. 

In the third and last part, the task was to add caching to your proxy using a simple main memory cache of 
recently accessed web content.

