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
void errorend(int x);
void block(lexeme*, int, int);
void constdecl(lexeme*);
void vardecl(lexeme*, int, int);
void procdecl(lexeme*, int);
void statement(lexeme*, int, int);

symbol *parse(lexeme *input)
{
	table = malloc(1000 * sizeof(symbol));
	sym_index = 0;
	error = 0;
	table[sym_index].kind = 3;
	strcpy(table[sym_index].name, "main");
	table[sym_index].value = 0;
	table[sym_index].level = 0;
	table[sym_index].addr = 0;
	sym_index++;

	block(input, 0, 3);

	if (token == lex_index || input[token].type != periodsym)
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
	if (error)
		return;

	if (token == lex_index)
		return;

	if (input[token].type == constsym)
		constdecl(input, lexlevel, nextaddress);
	else if (input[token].type == varsym)
		vardecl(input, lexlevel, nextaddress);
	else if (input[token].type == procsym)
		procdecl(input, lexlevel, nextaddress);

	token++;
	statement(input, lexlevel, nextaddress);
}

void constdecl(lexeme *input)
{
	if (error)
		return;

	token++;

	do
	{
		if (token == lex_index || input[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		token++;
		if (token == lex_index || input[token].type != eqlsym)
		{
			errorend(666);
			error = 1;
			return;
		}

		token++;
		if (token == lex_index || input[token].type != numbersym)
		{
			errorend(5);
			error = 1;
			return;
		}

		table[sym_index].kind = 1;
		table[sym_index].name = input[token - 2].name;
		table[sym_index].value = input[token].value;
		table[sym_index].level = 0;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;
		token++;
	} while (token < lex_index && input[token].type == commasym);

	if (token == lex_index || input[token].type != semicolonsym)
	{
		errorend(6);
		error = 1;
		return;
	}
	token++;
}

void vardecl(lexeme *input, int lexlevel, int nextaddress)
{
	if (error)
		return;

	token++;

	do
	{
		if (token == lex_index || input[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		table[sym_index].kind = 2;
		table[sym_index].name = input[token].name;
		table[sym_index].value = 0;
		table[sym_index].addr = nextaddress++;
		table[sym_index].level = lexlevel;
		table[sym_index].mark = 0;
		sym_index++;
		token++;
	} while (token < lex_index && input[token].type == commasym);

	if (token == lex_index || input[token].type != semicolonsym)
	{
		errorend(6);
		error = 1;
		return;
	}

	token++;
}

void procdecl(lexeme *input, int lexlevel)
{
	if (error)
		return;

	while (token < lex_index && input[token].type == procsym)
	{
		token++;

		if (token == lex_index || input[token] != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		table[sym_index].kind = 3;
		table[sym_index].name = input[token].name;
		table[sym_index].value = 0;
		table[sym_index].level = lexlevel;
		table[sym_index].addr = 0;       // doubtful
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		block(input, lexlevel + 1, 3);
		token++;
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
