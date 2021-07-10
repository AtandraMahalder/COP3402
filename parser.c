/*
	Author: Noelle Midkiff
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

symbol *table;
int sym_index;
int error;
int token = 0;

void printtable();
void errorend(int);
void block(lexeme*, int, int);
void constdecl(lexeme*);
void vardecl(lexeme*, int, int);
void procdecl(lexeme*, int);
void statement(lexeme*);
void expression(lexeme*);
void condition(lexeme*);
void expression(lexeme*);
void term(lexeme*);
void factor(lexeme*);

symbol *parse(lexeme *input)
{
	table = malloc(1000 * sizeof(symbol));
	sym_index = 0;
	error = 0;
	table[sym_index].kind = 3;
	strcpy(table[sym_index].name, "main");
	table[sym_index].value = 0;   /* the property in compiler.h file was "val" */
	table[sym_index].level = 0;
	table[sym_index].addr = 0;
	sym_index++;

	block(input, 0, 3);

	if (input[token].type != periodsym)
	{
		errorend(3);
		error = 1;
	}

	if (error)
	{
		free(table);
		return NULL;
	}
	else
	{
		printtable();
		return table;
	}
}

void block(lexeme *input, int lexlevel, int nextaddress)
{
	if (input[token].type == constsym)
		constdecl(input);
	else if (input[token].type == varsym)
		vardecl(input, lexlevel, nextaddress);
	else if (input[token].type == procsym)
		procdecl(input, lexlevel);
	else
	{
		errorend(666);
		error = 1;
		return;
	}

	if (error)
		return;

	statement(input);
}

void constdecl(lexeme *input)
{
	do
	{
		token++;
		if (input[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		token++;
		if (input[token].type != eqlsym)
		{
			errorend(666);
			error = 1;
			return;
		}

		token++;
		if (input[token].type != numbersym)
		{
			errorend(5);
			error = 1;
			return;
		}

		table[sym_index].kind = 1;
		strcpy(table[sym_index].name, input[token - 2].name);
		table[sym_index].value = input[token].value;
		table[sym_index].level = 0;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;
		token++;
	} while (input[token].type == commasym);

	if (input[token].type != semicolonsym)
	{
		errorend(6);
		error = 1;
		return;
	}

	token++;
}

void vardecl(lexeme *input, int lexlevel, int nextaddress)
{
	do
	{
		token++;
		if (input[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		table[sym_index].kind = 2;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].value = 0;
		table[sym_index].addr = nextaddress++;
		table[sym_index].level = lexlevel;
		table[sym_index].mark = 0;
		sym_index++;
		token++;
	} while (input[token].type == commasym);

	if (input[token].type != semicolonsym)
	{
		errorend(6);
		error = 1;
		return;
	}

	token++;
}

void procdecl(lexeme *input, int lexlevel)
{
	while (input[token].type == procsym)
	{
		token++;

		if (input[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		table[sym_index].kind = 3;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].value = 0;
		table[sym_index].level = lexlevel;
		table[sym_index].addr = 0;       // doubtful
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		block(input, lexlevel + 1, 3);

		if (error)
		 	return;

		token++;
	}
}

void statement(lexeme *input)
{
	if (input[token].type == identsym)
	{
		token++;

		if (input[token].type != becomessym)
		{
			errorend(666);
			error = 1;
			return;
		}

		token++;
		expression(input);
	}

	else if (input[token].type == callsym)
	{
		token++;

		if (input[token].type != identsym)
		{
			errorend(14);
			error = 1;
			return;
		}

		token++;
	}

	else if (input[token].type == beginsym)
	{
		token++;
		statement(input);

		if (error)
			return;

		while (input[token].type == semicolonsym)
		{
			token++;
			statement(input);

			if (error)
				return;
		}

		if (input[token].type != endsym)
		{
			errorend(10);
			error = 1;
			return;
		}

		token++;
	}

	else if (input[token].type == ifsym)
	{
		token++;
		condition(input);

		if (error)
			return;

		if (input[token].type != thensym)
		{
			errorend(9);
			error = 1;
			return;
		}

		token++;
		statement(input);
	}

	else if (input[token].type == whilesym)
	{
		token++;
		condition(input);

		if (error)
			return;

		if (input[token].type != dosym)
		{
			errorend(8);
			error = 1;
			return;
		}

		token++;
		statement(input);
	}
}

void condition(lexeme* input) 
{
	if (input[token].type == oddsym)
	{
		token++;
		expression(input);
	}

	else
	{
		expression(input);

		if (input[token].type != eqlsym || input[token].type != neqsym || input[token].type != lessym || input[token].type != leqsym || input[token].type != gtrsym || input[token].type != geqsym)
		{
			errorend(12);
			error = 1;
			return;
		}

		token++;
		expression(input);
	}
}

void expression(lexeme* input)
{
	if (input[token].type == plussym)
		token++;
	
	term(input); 
	
	while (input[token].type == plussym){
		token++;
		term(input);
	}
}


void term(lexeme* input){
	factor(input);

	while (input[token].type == multsym){
		token++;
		factor(input);
	}
}

void factor(lexeme* input){
	if (input[token].type == identsym)
		token++;
	
	else if (input[token].type == numbersym)
		token++;
	
	else if (input[token].type == lparentsym)
	{
		token++;
		expression(input);

		if (input[token].type != rparentsym)
		{
			errorend(13);
			error = 1;
			return;
		}

		token++;
	}

	else
	{
		errorend(666);
		error = 1;
		return;
	}
}

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

void printtable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address\n");
	printf("------------------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr);
}