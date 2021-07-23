/*
	Author:
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

instruction *code;
int code_index;
int token;
int sym_index;
int lexlevel;
lexeme *lex;
symbol *table;

void emit(int, int, int);
int seek(char*);
void mark(int);
void printcode();

void block();
void constdecl();
void const_decl();
void vardecl();
void var_decl();
void procdecl();
void statement();
void state_ment();
void condition();
void expression();
void term();
void te_rm();
void factor();
void fac_tor();


instruction *generate_code(lexeme *tokens, symbol *symbols)
{
	code = malloc(500 * sizeof(instruction));
	code_index = 0;

	lexlevel = 0;
	token = 0;
	sym_index = 1;
	lex = tokens;
	table = symbols;

	block();

	emit(9, 0, 3);

	printcode();
	return code;
}

void block()
{
	// should this statement be here?
	int jmp_ins_index = code_index;
	emit(7, 0, 0);

	constdecl();

	int old_sym_index = sym_index;

	vardecl();

	int num_var = sym_index - old_sym_index;

	procdecl();

	code[jmp_ins_index].m = code_index;

	if (num_var)
		emit(6, 0, num_var + 3);

	statement();
}

void constdecl()
{
	if (lex[token].type == constsym)
	{
		table[sym_index].mark = 0;
		sym_index++;

		token += 4;
		const_decl();
		token++;
	}
}

void const_decl()
{
	if (lex[token].type == commasym)
	{
		table[sym_index].mark = 0;
		sym_index++;

		token += 4;
		const_decl();
	}
}

void vardecl()
{
	if (lex[token].type == varsym)
	{
		table[sym_index].mark = 0;
		sym_index++;

		token += 2;
		var_decl();
		token++;
	}
}

void var_decl()
{
	if (lex[token].type == commasym)
	{
		table[sym_index].mark = 0;
		sym_index++;

		token += 2;
		var_decl();
	}
}

void procdecl()
{
	if (lex[token].type == procsym)
	{
		table[sym_index].val = code_index;
		table[sym_index].mark = 0;
		sym_index++;

		token += 3;

		lexlevel++;
		block();
		lexlevel--;

		mark(lexlevel + 1);

		emit(2, 0, 0);

		token++;
		procdecl();
	}
}

void statement()
{
	int identaddress, old_code_index;

	switch(lex[token].type)
	{
		case identsym:	identaddress = seek(lex[token].name);

						token += 2;
						expression();

						emit(4, lexlevel - table[identaddress].level, table[identaddress].addr);

						break;

		case callsym:	token++;
						identaddress = seek(lex[token].name);

						emit(5, lexlevel - table[identaddress].level, table[identaddress].val);

						token++;
						break;

		case beginsym:	token++;

						statement();
						state_ment();

						token++;
						break;

		case ifsym:		token++;
						condition();

						old_code_index = code_index;
						emit(8, 0, 0);

						token++;
						statement();

						code[old_code_index].m = code_index;

						if (lex[token].type == elsesym)
						{
							code[old_code_index].m = code_index + 1;
							old_code_index = code_index;
							emit(7, 0, 0);

							token++;
							statement();

							code[old_code_index].m = code_index;
						}

						break;

		case whilesym:	token++;
						int jmpaddress = code_index;
						condition();

						old_code_index = code_index;
						emit(8, 0, 0);

						token++;
						statement();
						emit(7, 0, jmpaddress);
						code[old_code_index].m = code_index;
						break;

		case readsym:	token++;
						emit(9, 0, 2);

						identaddress = seek(lex[token].name);
						emit(4, lexlevel - table[identaddress].level, table[identaddress].addr);

						token++;
						break;

		case writesym: 	token++;
						expression();
						emit(9, 0, 1);

						break;

		default: break;
	}
}

void state_ment()
{
	switch(lex[token].type)
	{
		case semicolonsym: 	token++;
							statement();
							state_ment();
							break;

		default:			break;
	}
}

void condition()
{
	if (lex[token].type == oddsym)
	{
		token++;
		expression();
		emit(2, 0, 6);

		return;
	}

	expression();

	int relop = lex[token].type;

	token++;
	expression();

	switch(relop)
	{
		case eqlsym:	emit(2, 0, 8);
							break;
		case neqsym:	emit(2, 0, 9);
						break;
		case lessym:	emit(2, 0, 10);
							break;
		case leqsym:	emit(2, 0, 11);
							break;
		case gtrsym:	emit(2, 0, 12);
							break;
		case geqsym:	emit(2, 0, 13);
							break;
		default:		break;
	}
}

void expression()
{
	if (lex[token].type == plussym || lex[token].type == minussym)
	{
		int firstop = lex[token].type;
		token++;
		term();

		if (firstop == minussym)
			emit(2, 0, 1);
	}
	else
		term();

	te_rm();
}

void term()
{
	factor();
	fac_tor();
}

void te_rm()
{
	if (lex[token].type == plussym || lex[token].type == minussym)
	{
		int operation = lex[token].type;
		token++;
		term();

		if (operation == plussym)
			emit(2, 0, 2);
		else
			emit(2, 0, 3);

		te_rm();
	}
}

void factor()
{
	int identaddress;

	switch(lex[token].type)
	{
		case identsym: 	identaddress = seek(lex[token].name);

						if (table[identaddress].kind == 1)
							emit(1, 0, table[identaddress].val);
						else
							emit(3, lexlevel - table[identaddress].level, table[identaddress].addr);

						token++;
						break;

		case numbersym:	emit(1, 0, lex[token].value);

						token++;
						break;

		case lparentsym:	token++;
							expression();
							token++;
							break;

		default:	break;
	}
}

void fac_tor()
{
	if (lex[token].type == multsym || lex[token].type == slashsym || lex[token].type == modsym)
	{
		int operation = lex[token].type;
		token++;
		factor();

		if (operation == multsym)
			emit(2, 0, 4);
		else if (operation == slashsym)
			emit(2, 0, 5);
		else
			emit(2, 0, 7);

		fac_tor();
	}
}

void emit(int opcode, int l, int m)
{
	code[code_index].opcode = opcode;
	code[code_index].l = l;
	code[code_index].m = m;
	code_index++;
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

int seek(char *name)
{
	int i = sym_index - 1;
	while (i >= 0)
	{
		if (!strcmp(table[i].name, name) && !table[i].mark)
			return i;

		i--;
	}

	return i;
}

void printcode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < code_index; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
}
