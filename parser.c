/*
	Author: Noelle Midkiff
	Modified: Atandra Mahalder
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

symbol *table;
lexeme *lex;
int token;
int sym_index;
int error;

void identconflict(char*, int);
void identundeclared(char*);
void mark(int);
void printtable();
void errorend(int x);

void block(int, int);
void constdecl(int);
void const_decl(int);
void vardecl(int, int);
void var_decl(int, int);
void procdecl(int);
void statement();
void state_ment();
void condition();
void expression();
void term();
void te_rm();
void factor();
void fac_tor();

symbol *parse(lexeme *input)
{
	table = malloc(1000 * sizeof(symbol));
	sym_index = 0;
	error = 0;

	lex = input;
	token = 0;

	if (lex[token].type != constsym && lex[token].type != varsym && lex[token].type != procsym
		&& lex[token].type !=identsym && lex[token].type != callsym && lex[token].type != beginsym
		&& lex[token].type != ifsym && lex[token].type != whilesym && lex[token].type != readsym
		&& lex[token].type != writesym && lex[token].type != periodsym)
	{
		errorend(2);
		free(table);
		return NULL;
	}

	table[sym_index].kind = 3;
	strcpy(table[sym_index].name, "main");
	table[sym_index].val = 0;
	table[sym_index].level = 0;
	table[sym_index].addr = 0;
	table[sym_index].mark = 0;
	sym_index++;

	block(0, 3);

	if(error)
	{
		free(table);
		return NULL;
	}

	if (lex[token].type != periodsym)
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

void identconflict(char *name, int level)
{
	int i = sym_index - 1;

	while (i >= 0)
	{
		if (!strcmp(table[i].name, name) && table[i].level == level && !table[i].mark)
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
			return;

		i--;
	}

	errorend(7);
	error = 1;
}

void mark(int level)
{
	int i = sym_index - 1;

	while (i >= 0)
	{
		if (table[i].level == level)
			table[i].mark = 1;

		i--;
	}
}

void block(int level, int nextaddress)
{
	if (lex[token].type == constsym || lex[token].type == varsym || lex[token].type == procsym
		|| lex[token].type == identsym || lex[token].type == callsym || lex[token].type == beginsym
		|| lex[token].type == ifsym || lex[token].type == whilesym || lex[token].type == readsym
		|| lex[token].type == writesym)
	{
		constdecl(level);
		if (error)
			return;

		vardecl(level, nextaddress);
		if (error)
			return;

		procdecl(level);
		if (error)
			return;

		statement();
	}
	else if (lex[token].type == semicolonsym || lex[token].type == periodsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void constdecl(int level)
{
	if (lex[token].type == constsym)
	{
		token++;
		if (lex[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(lex[token].name, level);
		if (error)
			return;

		token++;
		if (lex[token].type != becomessym)
		{
			errorend(2);
			error = 1;
			return;
		}

		token++;
		if (lex[token].type != numbersym)
		{
			errorend(5);
			error = 1;
			return;
		}

		table[sym_index].kind = 1;
		strcpy(table[sym_index].name, lex[token - 2].name);
		table[sym_index].val = lex[token].value;
		table[sym_index].level = level;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		const_decl(level);
		if (error)
			return;

		if (lex[token].type != semicolonsym)
		{
			errorend(6);
			error = 1;
			return;
		}

		token++;
	}
	else if (lex[token].type == varsym || lex[token].type == procsym || lex[token].type == callsym
			|| lex[token].type == identsym || lex[token].type == beginsym || lex[token].type == ifsym
			|| lex[token].type == whilesym || lex[token].type == readsym || lex[token].type == writesym
			|| lex[token].type == periodsym || lex[token].type == semicolonsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void const_decl(int level)
{
	if (lex[token].type == commasym)
	{
		token++;
		if (lex[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(lex[token].name, level);
		if (error)
			return;

		token++;
		if (lex[token].type != becomessym)
		{
			errorend(2);
			error = 1;
			return;
		}

		token++;
		if (lex[token].type != numbersym)
		{
			errorend(5);
			error = 1;
			return;
		}

		table[sym_index].kind = 1;
		strcpy(table[sym_index].name, lex[token - 2].name);
		table[sym_index].val = lex[token].value;
		table[sym_index].level = level;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		const_decl(level);
	}
	else if (lex[token].type == semicolonsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void vardecl(int level, int nextaddress)
{
	if (lex[token].type == varsym)
	{
		token++;
		if (lex[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(lex[token].name, level);
		if (error)
			return;

		table[sym_index].kind = 2;
		strcpy(table[sym_index].name, lex[token].name);
		table[sym_index].val = 0;
		table[sym_index].level = level;
		table[sym_index].addr = nextaddress++;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		var_decl(level, nextaddress);
		if (error)
			return;

		if (lex[token].type != semicolonsym)
		{
			errorend(6);
			error = 1;
			return;
		}

		token++;
	}
	else if (lex[token].type == procsym || lex[token].type == callsym
			|| lex[token].type == identsym || lex[token].type == beginsym || lex[token].type == ifsym
			|| lex[token].type == whilesym || lex[token].type == readsym || lex[token].type == writesym
			|| lex[token].type == periodsym || lex[token].type == semicolonsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void var_decl(int level, int nextaddress)
{
	if (lex[token].type == commasym)
	{
		token++;
		if (lex[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(lex[token].name, level);
		if (error)
			return;

		table[sym_index].kind = 2;
		strcpy(table[sym_index].name, lex[token].name);
		table[sym_index].val = 0;
		table[sym_index].level = level;
		table[sym_index].addr = nextaddress++;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		var_decl(level, nextaddress);
	}
	else if (lex[token].type == semicolonsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void procdecl(int level)
{
	if (lex[token].type == procsym)
	{
		token++;
		if (lex[token].type != identsym)
		{
			errorend(4);
			error = 1;
			return;
		}

		identconflict(lex[token].name, level);
		if (error)
			return;

		table[sym_index].kind = 3;
		strcpy(table[sym_index].name, lex[token].name);
		table[sym_index].val = 0;
		table[sym_index].level = level;
		table[sym_index].addr = 0;
		table[sym_index].mark = 0;
		sym_index++;

		token++;
		if (lex[token].type != semicolonsym)
		{
			errorend(6);
			error = 1;
			return;
		}

		token++;
		block(level + 1, 3);
		if (error)
			return;

		mark(level + 1);

		if (lex[token].type != semicolonsym)
		{
			errorend(6);
			error = 1;
			return;
		}

		token++;
		procdecl(level);
	}
	else if (lex[token].type == callsym
			|| lex[token].type == identsym || lex[token].type == beginsym || lex[token].type == ifsym
			|| lex[token].type == whilesym || lex[token].type == readsym || lex[token].type == writesym
			|| lex[token].type == periodsym || lex[token].type == semicolonsym)
		return;
	else
	{
		errorend(2);
		return;
	}
}

void statement()
{
	switch(lex[token].type)
	{
		case identsym:	identundeclared(lex[token].name);
						if (error)
							return;

						token++;
						if (lex[token].type != becomessym)
						{
							errorend(2);
							error = 1;
							return;
						}

						token++;
						expression();
						break;

		case callsym:	token++;
						if (lex[token].type != identsym)
						{
							errorend(14);
							error = 1;
							return;
						}

						identundeclared(lex[token].name);
						token++;
						break;

		case beginsym:	token++;
						statement();
						if (error)
							return;

						state_ment();
						if (error)
							return;

						if (lex[token].type != endsym)
						{
							errorend(10);
							error = 1;
							return;
						}

						token++;
						break;

		case ifsym:		token++;
						condition();
						if (error)
							return;

						if (lex[token].type != thensym)
						{
							errorend(9);
							error = 1;
							return;
						}

						token++;
						statement();
						if (error)
							return;

						token++;
						if (lex[token].type == elsesym)
						{
							token++;
							statement();
						}

						break;

		case whilesym:	token++;
						condition();
						if (error)
							return;

						if (lex[token].type != dosym)
						{
							errorend(8);
							error = 1;
							return;
						}

						token++;
						statement();
						break;

		case readsym:	token++;
						if (lex[token].type != identsym)
						{
							errorend(14);
							error = 1;
							return;
						}

						identundeclared(lex[token].name);
						token++;
						break;

		case writesym: 	token++;
						expression();
						break;

		case semicolonsym: break;

		case periodsym:	break;

		case endsym: break;

		case elsesym: break;

		default:	errorend(2);
				 	error = 1;
	}
}

void state_ment()
{
	switch(lex[token].type)
	{
		case semicolonsym: 	token++;
							statement();
							if (error)
								return;

							state_ment();
							break;

		case endsym:		break;

		default:			errorend(10);
							error = 1;
	}
}

void condition()
{
	if (lex[token].type == oddsym)
	{
		token++;
		expression();
	}
	else if (lex[token].type == plussym || lex[token].type == minussym || lex[token].type == identsym
			|| lex[token].type == numbersym || lex[token].type == lparentsym)
	{
		expression();
		if (error)
			return;

		if (lex[token].type != eqlsym && lex[token].type != neqsym && lex[token].type != lessym
			&& lex[token].type != leqsym && lex[token].type != gtrsym && lex[token].type != geqsym)
		{
			errorend(12);
			error = 1;
			return;
		}

		expression();
	}
	else
	{
		errorend(11);
		error = 1;
	}
}

void expression()
{
	if (lex[token].type == plussym || lex[token].type == minussym || lex[token].type == identsym
		||lex[token].type == numbersym || lex[token].type == lparentsym)
	{
		if (lex[token].type == plussym || lex[token].type == minussym)
			token++;

		term();
		if (error)
			return;

		te_rm();
	}
	else
	{
		errorend(2);
		error = 1;
	}
}

void term()
{
	if (lex[token].type == identsym || lex[token].type == numbersym || lex[token].type == lparentsym)
	{
		factor();
		if (error)
			return;

		fac_tor();
	}
	else
	{
		errorend(2);
		error = 1;
	}
}

void te_rm()
{
	if (lex[token].type == plussym || lex[token].type == minussym)
	{
		token++;
		term();
		if (error)
			return;

		te_rm();
	}

	else if (lex[token].type == thensym || lex[token].type == dosym || lex[token].type == semicolonsym
			 || lex[token].type == periodsym || lex[token].type == rparentsym || lex[token].type == endsym
			 || lex[token].type == elsesym || lex[token].type == eqlsym || lex[token].type == neqsym
			 || lex[token].type == lessym || lex[token].type == leqsym || lex[token].type == gtrsym
			 || lex[token].type == geqsym)
		return;
	else
	{
		errorend(2);
		error = 1;
	}
}

void factor()
{
	switch(lex[token].type)
	{
		case identsym: 	identundeclared(lex[token].name);
						token++;
						break;

		case numbersym:	token++;
						break;

		case lparentsym:	token++;
							expression();
							if (error)
								break;

							if (lex[token].type != rparentsym)
							{
								errorend(13);
								error = 1;
								return;
							}

							token++;
							break;

		default:	errorend(2);
					error = 1;
	}
}

void fac_tor()
{
	if (lex[token].type == multsym || lex[token].type == slashsym || lex[token].type == modsym)
	{
		token++;
		factor();
		if (error)
			return;

		fac_tor();
	}
	else if (lex[token].type == thensym || lex[token].type == dosym || lex[token].type == semicolonsym
			 || lex[token].type == periodsym || lex[token].type == rparentsym || lex[token].type == endsym
			 || lex[token].type == elsesym || lex[token].type == eqlsym || lex[token].type == neqsym
			 || lex[token].type == lessym || lex[token].type == leqsym || lex[token].type == gtrsym
			 || lex[token].type == geqsym)
		return;
	else
	{
		errorend(2);
		error = 1;
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
	printf("--------------------------------------------\n");
	for (i = 0; i < sym_index; i++)
		printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr);
}
