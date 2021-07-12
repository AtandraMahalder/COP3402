/*
	Date: 11 July, 2021
	Authors: Atandra Mahalder, Emin Mammadzada
*/

// figure out how to handle errors correctly
// program works and detects errors but prints wrong
// error message in many cases
// may have to change function return type to handle that
// return different integers to show what error message
// has been printed:
// 0 if there is an error and no message has been printed
// 1 if there is no error
// 2 if there is an error and a mesage has been printed

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

symbol *table;
int sym_index;
int error;
int token = 0;

void mark();
void identundeclared(char*);
void identconflict(char*, int);
void printtable();
void errorend(int x);

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
	table[sym_index].kind = 3;
	strcpy(table[sym_index].name, "main");
	table[sym_index].val = 0;
	table[sym_index].level = 0;
	table[sym_index].addr = 0;
	sym_index++;

	block(input, 0, 3);

	if (error)
	{
		free(table);
		return NULL;
	}

	if (input[token].type != periodsym)
	{
		errorend(3);
		free(table);
		return NULL;
	}

	printtable();
	return table;
}

void identconflict(char *name, int lexlevel)
{
	int i = sym_index - 1;
	while (i >= 0)
	{
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
		if (!strcmp(table[i].name, name) && !table[i].mark)
			break;

		i--;
	}

	if (i == -1)
	{
		errorend(7);
		error = 1;
		return;
	}
}

void mark()
{
	table[sym_index - 1].mark = 1;
	int level_deleted = table[sym_index - 1].level;

	int i = sym_index - 2;
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
	if (input[token].type == constsym)
		constdecl(input, lexlevel);

	if (error)
		return;

	if (input[token].type == varsym)
		vardecl(input, lexlevel, nextaddress);

	if (error)
		return;

	procdecl(input, lexlevel);

	if (error)
		return;

	statement(input);
}

void constdecl(lexeme *input, int lexlevel)
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

		identconflict(input[token].name, lexlevel);

		if (error)
			return;

		token++;
		if (input[token].type != becomessym)
		{
			errorend(5); //doubt
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
		table[sym_index].val = input[token].value;
		table[sym_index].level = lexlevel;
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

		identconflict(input[token].name, lexlevel);
		if (error)
			return;

		table[sym_index].kind = 2;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].val = 0;
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

		identconflict(input[token].name, lexlevel);

		if (error)
			return;

		table[sym_index].kind = 3;
		strcpy(table[sym_index].name, input[token].name);
		table[sym_index].val = 0;
		table[sym_index].level = lexlevel;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		if (input[token].type != semicolonsym)
		{
			errorend(6); //doubt
			error = 1;
			return;
		}

		token++;
		block(input, lexlevel + 1, 3);

		if (error)
		 	return;

		mark();

		if (input[token].type != semicolonsym)
		{
			errorend(6); //doubt
			error = 1;
			return;
		}

		token++;
	}
}

void statement(lexeme *input)
{
	if (input[token].type == identsym)
	{
		identundeclared(input[token].name);

		if (error)
			return;

		token++;
		if (input[token].type != becomessym)
		{
			errorend(2); // doubt
			error = 1;
			return;
		}

		token++;

		int ret = expression(input);

		if (ret == 0)
		{
			errorend(2);
			return;
		}
		else if (ret == 2)
			return;
	}

	else if (input[token].type == callsym)
	{
		token++;

		identundeclared(input[token].name);

		if (error)
			return;

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

		// checking for error 11
		int ret = condition(input);

		if (ret == 0)
		{
			errorend(11);
			return;
		}
		else if (ret == 2)
			return;

		if (input[token].type != thensym)
		{
			errorend(9);
			error = 1;
			return;
		}

		token++;
		statement(input);

		if (error)
			return;

		if (input[token].type == elsesym)
		{
			token++;
			statement(input);
		}
	}

	else if (input[token].type == whilesym)
	{
		token++;

		// checking for error 11
		int ret = condition(input);

		if (ret == 0)
		{
			errorend(11);
			error = 1;
			return;
		}
		else if (ret == 2)
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

	else if (input[token].type == readsym)
	{
		token++;

		identundeclared(input[token].name);

		if (error)
			return;

		if (input[token].type != identsym)
		{
			errorend(14);
			error = 1;
			return;
		}

		token++;
	}

	else if (input[token].type == writesym)
	{
		token++;
		int ret = expression(input);

		if (ret == 0)
			errorend(2);
	}
}

int condition(lexeme* input)
{
	if (input[token].type == oddsym)
	{
		token++;
		int ret = expression(input);

		if (ret == 0)
			return 0;
		else if (ret == 2)
			return 2;

		return 1;
	}

	int exp = expression(input);

	if (exp == 0)
		return 0;
	else if (exp == 2)
		return 2;

	// apply De-Morgan's law. Think of the conditions when there is a relational
	// operator and negate that condition to generate error condition
	if (input[token].type != eqlsym && input[token].type != neqsym && input[token].type != lessym && input[token].type != leqsym && input[token].type != gtrsym && input[token].type != geqsym)
	{
		errorend(12);
		error = 1;
		return 2;
	}

	token++;

	exp = expression(input);

	if (exp == 0)
		return 0;
	else if (exp == 2)
		return 2;

	//change for error 11
	return 1;
}

int expression(lexeme* input)
{
	if (input[token].type == plussym || input[token].type == minussym)
		token++;

	int t = term(input);

	if (t == 0)
		return 0;
	else if (t == 2)
		return 2;

	while (input[token].type == plussym || input[token].type == minussym)
	{
		token++;
		t = term(input);

		if (t == 0)
			return 0;
		else if (t == 2)
			return 2;
	}

	return 1;
}


int term(lexeme* input)
{
	int f = factor(input);

	if (f == 0)
		return 0;
	else if (f == 2)
		return 2;

	while (input[token].type == multsym || input[token].type == slashsym || input[token].type == modsym)
	{
		token++;
		f = factor(input);

		if (f == 0)
			return 0;
		else if (f == 2)
			return 2;
	}

	return 1;
}

int factor(lexeme* input)
{
	if (input[token].type == identsym)
	{
		identundeclared(input[token].name);

		if (error)
			return 2;

		token++;

		return 1;
	}

	else if (input[token].type == numbersym)
	{
		token++;
		return 1;
	}

	else if (input[token].type == lparentsym)
	{
		token++;
		int exp = expression(input);

		if (exp == 0)
			return 0;
		else if (exp == 2)
			return 2;

		if (input[token].type != rparentsym)
		{
			errorend(13);
			error = 1;
			return 2;
		}

		token++;

		return 1;
	}

	return 0;
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
	printf("--------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr);
}
