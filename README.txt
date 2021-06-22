Authors: Emin Mammadzada, Atandra Mahalder
Date: 22 June 2021

To run the program, we need a unix environment. Before compilation ensure that driver.c, lex.c and compiler.h
are in the same directory. To compile open the terminal, navigate to the directory containing driver.c and
enter the command: gcc driver.c lex.c -lm
This will generate an executable file: a.out
To execute the program use: ./a.out [inputfile]
where the [inputfile] is a file that contains source program written in PL/0, which needs to be tokenized.
