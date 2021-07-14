/*
	COP3402. Project 3.
	Authors: Atandra Mahalder, Emin Mammadzada.
	Date: July 14, 2021.
*/



/*
NOTE: 
When returning from functions, there are 3 cases: returning 0, 1, or 2

0 -- error was detected but message was not printed
1 -- no error
2 -- error was detected and message was printed
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

symbol *table;
int sym_index;
int error;
int token = 0;

void mark(); 					// "delete" (mark as unavailable) local variables when program exits the given prodedure
void identundeclared(char*);	// catch the error associated with undeclared variables
void identconflict(char*, int); // catch the error associated with competing symbol declarations
void printtable();				// print the resulting symbol table
void errorend(int x);			//end the execution of the program and print the corresponding error message


/* Procedures for syntactic classes of the PL/0 language */
void block(lexeme*, int, int);
void constdecl(lexeme*, int);
void vardecl(lexeme*, int, int);
void procdecl(lexeme*, int);
void statement(lexeme*);
int expression(lexeme*);
int condition(lexeme*);
int term(lexeme*);
int factor(lexeme*);

symbol *parse(lexeme *input)
{
	table = malloc(1000 * sizeof(symbol));
	sym_index = 0;
	error = 0;

    //first entry of the table is the main procedure
	table[sym_index].kind = 3;
	strcpy(table[sym_index].name, "main");
	table[sym_index].val = 0;
	table[sym_index].level = 0;
	table[sym_index].addr = 0;
	sym_index++; //move the symbol table pointer forward

	block(input, 0, 3); //calling the block function and passing the lexicographical level and the address in the stack where variables can be stored

	if (error) //free the symbol table and don't return anything if there was an error
	{
		free(table);
		return NULL;
	}

	if (input[token].type != periodsym) //throw an error if the program does not finish with a "." symbol
	{
		errorend(3);
		free(table);
		return NULL;
	}

    //otherwise, print the resulting symbol table and return it
	printtable();
	return table;
}

void identconflict(char *name, int lexlevel)
{
	int i = sym_index - 1;
	while (i >= 0)
	{
        //	if the variable names are same and identifier is not marked 
		// and they are on the same lexicographical level: throw conflicting identifier error
		if (!strcmp(table[i].name, name) && !table[i].mark && table[i].level == lexlevel)
		{
			errorend(1);
			error = 1;
			return;
		}

		i--;
	}
}

void identundeclared(char *name)
{
	int i = sym_index - 1;
	while (i >= 0)
	{
        //if we found the identifier in the symbol table and it's not marked: no errors thrown...
		if (!strcmp(table[i].name, name) && !table[i].mark)
			break;

		i--;
	}

    // ...otherwise, throw undeclared symbol error
	if (i == -1)
	{
		errorend(7);
		error = 1;
		return;
	}
}

void mark()
{
	table[sym_index - 1].mark = 1; //mark the symbol
	int level_deleted = table[sym_index - 1].level; //detect the level of the symbol

	int i = sym_index - 2;

    //mark all other symbols on that lex level
	while (i >= 0)
	{
		if (table[i].level == level_deleted)
			table[i].mark = 1;
		else
			break;

		i--;
	}
}

void block(lexeme *input, int lexlevel, int nextaddress)
{
	if (input[token].type == constsym)	//if the token from lexeme list is const
		constdecl(input, lexlevel);

	if (error) //return to the caller function if an error was thrown when calling constdecl()
		return;

	if (input[token].type == varsym) //if the token from lexeme list is var
		vardecl(input, lexlevel, nextaddress); 

	if (error)	//return to the caller function if an error was thrown when calling vardecl()
		return;

	procdecl(input, lexlevel); //if the token from lexeme list is procedure

	if (error)	//return to the caller function if an error was thrown when calling procdecl()
		return;

	statement(input); 
}

void constdecl(lexeme *input, int lexlevel)
{
	do
	{
		token++;    //get the next token
		if (input[token].type != identsym)  //if the token after "const" is not identifier: throw error and return to the caller
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(input[token].name, lexlevel); //check if there are any identifier conflicts

		if (error)  //return to the caller if there was competing symbol declarations error
			return;

		token++;    //get next token

		// if there is no "becomes" operator,
		//then throw an error "consts must be assigned a value at declarations"
		if (input[token].type != becomessym)	
		{
			errorend(5); 
			error = 1;
			return;
		}

		token++; //get next token

		// if there is no number following "const",
		//then throw an error "consts must be assigned a value at declarations"
		if (input[token].type != numbersym)
		{
			errorend(5);
			error = 1;
			return;
		}

		//if no errors detected, fill the symbol table with consts seperated by comma
		table[sym_index].kind = 1;
		strcpy(table[sym_index].name, input[token - 2].name);
		table[sym_index].val = input[token].value;
		table[sym_index].level = lexlevel;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;	//move the symbol table pointer forward
		token++;		//get next token
	} while (input[token].type == commasym);

	if (input[token].type != semicolonsym)	//if symbol declarations are not ending with a semicolon, then throw an error
	{
		errorend(6);
		error = 1;
		return;
	}

	token++;	//get next token
}

void vardecl(lexeme *input, int lexlevel, int nextaddress)
{
	do
	{
		token++; //get next token
		if (input[token].type != identsym) //if the token after "var" is not identifier: throw error and return to the caller
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(input[token].name, lexlevel); //check if there are any identifier conflicts

		if (error) //return to the caller if there was competing symbol declarations error
			return;


		//if no errors detected, fill the symbol table with vars seperated by comma
		table[sym_index].kind = 2;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].val = 0;
		table[sym_index].addr = nextaddress++;
		table[sym_index].level = lexlevel;
		table[sym_index].mark = 0;
		sym_index++;	//move the symbol table pointer forward
		token++;		//get next token
	} while (input[token].type == commasym);

	if (input[token].type != semicolonsym) //if symbol declarations are not ending with a semicolon, then throw an error
	{
		errorend(6);
		error = 1;
		return;
	}

	token++; //get next token
}

void procdecl(lexeme *input, int lexlevel)
{
	while (input[token].type == procsym)
	{
		token++; //get next token
		if (input[token].type != identsym)  //if the token after "procedure" is not identifier: throw error and return to the caller
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(input[token].name, lexlevel); //check if there are any identifier conflicts

		if (error) //return to the caller if there was competing symbol declarations error
			return;

		//if no errors detected, fill the symbol table with procedure data
		table[sym_index].kind = 3;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].val = 0;
		table[sym_index].level = lexlevel;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;	//move the symbol table pointer forward
		token++;		//get next token

		if (input[token].type != semicolonsym) //if symbol declarations are not ending with a semicolon, then throw an error
		{
			errorend(6); 
			error = 1;
			return;
		}

		token++; //get next token
		block(input, lexlevel + 1, 3);	//create a new prodecure block with a new lexicographical level and new AR

		if (error)	//return to the caller if there was an error when creating a new block
		 	return;

		mark();	//mark all the variables at this variable before entering procedure block 

		if (input[token].type != semicolonsym)	//if symbol declarations are not ending with a semicolon, then throw an error
		{
			errorend(6);
			error = 1;
			return;
		}

		token++; //get next token
	}
}

void statement(lexeme *input)
{
	if (input[token].type == identsym) //if the token is an identifier
	{
		identundeclared(input[token].name);	//check if the given identifier was declared or not

		if (error)	//throw an error if it was not declared
			return;

		token++; //get next token otherwise

		if (input[token].type != becomessym) //if there is no "becomes" symbol, throw unrecognized statement form error
		{
			errorend(2);
			error = 1;
			return;
		}

		token++; //get next token

		int ret = expression(input);	//call expression

		if (ret == 0)	//if there was an error and no message was printed -- print unrecognized statement form error message and return to the caller
		{
			errorend(2);
			return;
		}
		else if (ret == 2) //if there was an error and it was handled -- return to the caller
			return;
	}

	else if (input[token].type == callsym) //if the token is "call"
	{
		token++;	//get next token

		identundeclared(input[token].name);	//check if the procedure to be called was declared or not

		if (error)	//return if the procedure was not declared
			return;

		if (input[token].type != identsym) //if the token is not an identifier, throw "call and read Must Be Followed By an Identifier" error
		{
			errorend(14);
			error = 1;
			return;
		}

		token++; //get next token
	}

	else if (input[token].type == beginsym)	//if the token is "begin"
	{
		token++; //get next token
		statement(input);

		if (error) //if there was an error with statement, return to the caller
			return;

		while (input[token].type == semicolonsym) //get next token and call statement while token is ";"
		{
			token++;
			statement(input);

			if (error)	//return if an error was detected
				return;
		}

		if (input[token].type != endsym)	//if begin is not followed by "end": throw an error and return
		{
			errorend(10);
			error = 1;
			return;
		}

		token++; //get next token
	}

	else if (input[token].type == ifsym)	//if the token is "if"
	{
		token++; //get next token

		// checking if there is a condition following "if"
		int ret = condition(input);

		if (ret == 0) //if there is no condition and error message was not printed -  print the error and return to the caller
		{
			errorend(11);
			return;
		}
		else if (ret == 2) //if there is no condition and error message was printed - return to the caller
			return;

		if (input[token].type != thensym) //if "if" is not followed by "then" - throw an error and return to the caller
		{
			errorend(9);
			error = 1;
			return;
		}

		token++; //get next token
		statement(input);

		if (error) //if there was an error when calling statement, return to the caller
			return;

		if (input[token].type == elsesym)	//if the token is "else", get next token and call statement
		{
			token++;
			statement(input);
		}
	}

	else if (input[token].type == whilesym)	//if the token is "while"
	{
		token++; //get next token

		// checking if there is a condition following "while"
		int ret = condition(input);

		if (ret == 0) //if there is no condition and error message was not printed -  print the error and return to the caller
		{
			errorend(11);
			error = 1;
			return;
		}
		else if (ret == 2) //if there is no condition and error message was printed - return to the caller
			return;

		if (input[token].type != dosym) //if while is not followed by "do" - throw an error and return to the caller
		{
			errorend(8);
			error = 1;
			return;
		}

		token++; //get next token
		statement(input);
	}

	else if (input[token].type == readsym) //if the token is read
	{
		token++; //get next tokdn

		identundeclared(input[token].name); //check if the identifier was declared or not

		if (error) //if identifier was not declared - return to the caller
			return;

		//if the next token is not an identifier - 
		// throw "call and read Must Be Followed By an Identifier" error
		if (input[token].type != identsym) 
		{
			errorend(14);
			error = 1;
			return;
		}

		token++; //get next token
	}

	else if (input[token].type == writesym) //if the token is write
	{
		token++; //get next token
		int ret = expression(input);

		if (ret == 0) //if there was an error calling expression and the message was not printed - print the error message
			errorend(2);
	}
}

int condition(lexeme* input)
{
	if (input[token].type == oddsym) //if the token  is "odd"
	{
		token++; //get next token
		int ret = expression(input);

		if (ret == 0) //if there was an error calling expression and message was not printed -- return 0
			return 0;
		else if (ret == 2) //if there was an error calling expression and message was printed -- return 2
			return 2;

		return 1; //return true otherwise
	}

	int exp = expression(input);

	if (exp == 0) //if there was an error calling expression and message was not printed -- return 0
		return 0;
	else if (exp == 2) //if there was an error calling expression and message was printed -- return 2
		return 2;

	//if conditions do not contain relational operators, throw error and return
	if (input[token].type != eqlsym && input[token].type != neqsym && input[token].type != lessym && input[token].type != leqsym && input[token].type != gtrsym && input[token].type != geqsym)
	{
		errorend(12);
		error = 1;
		return 2;
	}

	token++; //get next token

	exp = expression(input);

	if (exp == 0)
		return 0;
	else if (exp == 2)
		return 2;

	//return true if there are no errors with condition
	return 1;
}

int expression(lexeme* input)
{
	if (input[token].type == plussym || input[token].type == minussym) //if token is addition symbol, get next token
		token++;
 
	int t = term(input); //call the term and check if there are errors (both with and without printed message)

	if (t == 0)
		return 0;
	else if (t == 2)
		return 2;

	while (input[token].type == plussym || input[token].type == minussym) //get the next token and call term while token is addition symbol
	{
		token++;
		t = term(input);

		if (t == 0) 
			return 0;
		else if (t == 2)
			return 2;
	}

	return 1; //return true is there is no error with expression
}


int term(lexeme* input)
{
	int f = factor(input);

	if (f == 0) // if there was an error with factor and message was not printed -- return 0
		return 0;
	else if (f == 2) // if there was an error with factor and message was printed -- return 2
		return 2;

	while (input[token].type == multsym || input[token].type == slashsym || input[token].type == modsym) //get the next token and call factor while token is mult symbol
	{
		token++;
		f = factor(input);

		if (f == 0)
			return 0;
		else if (f == 2)
			return 2;
	}

	return 1; //return true is there is no error with term
}

int factor(lexeme* input)
{
	if (input[token].type == identsym) //if the token is identifier
	{
		identundeclared(input[token].name); //check whether identifier was declared or not

		if (error) // if there was an error, return 2 -- error was detected and the message was printed (inside the identundeclared function)
			return 2;

		token++; //get next token

		return 1; //return true if there is no error
	}

	else if (input[token].type == numbersym) //get next token and return true if the next token is number
	{
		token++; 
		return 1;
	}

	else if (input[token].type == lparentsym) //if the token is "(", get next token and call expression
	{
		token++;
		int exp = expression(input);

		if (exp == 0) //if expression had errors and message was not printed : return 0
			return 0;
		else if (exp == 2) //if expression had errors and message was printed : return 2
			return 2;

		if (input[token].type != rparentsym) //if "(", does not have matching ")", throw corresponding error and return to the caller
		{
			errorend(13);
			error = 1;
			return 2;
		}

		token++; //get next token

		return 1; //return true if there is no error
	}

	error = 1; //return with error otherwise
	return 0;
}


//error printing function
void errorend(int x)
{
	switch (x)
	{
		case 1:
			printf("Parser Error: Competing Symbol Declarations\n");
			break;
		case 2:
			printf("Parser Error: Unrecognized Statement Form\n");
			break;
		case 3:
			printf("Parser Error: Programs Must Close with a Period\n");
			break;
		case 4:
			printf("Parser Error: Symbols Must Be Declared with an Identifier\n");
			break;
		case 5:
			printf("Parser Error: Constants Must Be Assigned a Value at Declaration\n");
			break;
		case 6:
			printf("Parser Error: Symbol Declarations Must Be Followed By a Semicolon\n");
			break;
		case 7:
			printf("Parser Error: Undeclared Symbol\n");
			break;
		case 8:
			printf("Parser Error: while Must Be Followed By do\n");
			break;
		case 9:
			printf("Parser Error: if Must Be Followed By then\n");
			break;
		case 10:
			printf("Parser Error: begin Must Be Followed By end\n");
			break;
		case 11:
			printf("Parser Error: while and if Statements Must Contain Conditions\n");
			break;
		case 12:
			printf("Parser Error: Conditions Must Contain a Relational-Operator\n");
			break;
		case 13:
			printf("Parser Error: ( Must Be Followed By )\n");
			break;
		case 14:
			printf("Parser Error: call and read Must Be Followed By an Identifier\n");
			break;
		default:
			printf("Implementation Error: Unrecognized Error Code\n");
			break;
	}

}

//print the symbol table
void printtable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address\n");
	printf("--------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr);
}
