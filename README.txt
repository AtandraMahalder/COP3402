Authors: Emin Mammadzada, Atandra Mahalder
Date: 28 July 2021

To run the program, we need a unix environment. Before compilation ensure that driver.c, lex.o, parser.o and compiler.h
are in the same directory as codegen.c. To compile open the terminal, navigate to the directory containing codegen.c and
enter the command: gcc driver.c codegen.c lex.o parser.o -lm.

This will generate an executable file: a.out
To execute the program use: ./a.out [inputfile]
where the [inputfile] is a file that contains source program written in PL/0, which will be first tokenized,
then get its syntax checked by the parser and finally produce assembly instructions which we can run on our 
virtual machine.