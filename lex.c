/*
	This is the lex skeleton for the UCF Summer 2021 Systems Software Project
	Implement the function lexanalyzer, add as many functions and global
	variables as desired, but do not alter printerror or printtokens.
	Include your name (and your partner's name) in this comment in order to
	earn the points for compiling
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"


lexeme *list;
int lex_index;

void printerror(int type);
void printtokens();

lexeme *lexanalyzer(char *input)
{
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;

	char reserved[][12] = {"const", "var", "procedure", "call", "if", "then",
						"else", "while", "do", "begin", "end", "read", "write",
						"odd"};

	int buff = 0;
	while (input[buff])
	{
		if (iscntrl(input[buff]) || iscntrl(input[buff]) == ' ')
		{
			buff++;
			continue;
		}

		// check if the token is an identifier or a keyword
		if (isalpha(input[buff]))
		{
			char varname[12];
			varname[0] = input[buff];

			// reading in identifier or keyword
			int i = 1;
			buff++;
			while (i < 12 && (isalpha(input[buff]) || isdigit(input[buff])))
			{
				varname[i] = input[buff];
				buff++;
				i++;
			}

			// if identifier or keyword has excessive length
			if (i == 12)
			{
				printerror(4);
				return NULL;
			}

			varname[i] = '\0';

			i = 0;
			while (i < 14)
			{
				// if the variable is a keyword
				if (!strcmp(varname, reserved[i]))
				{
					switch(i)
					{
						case 0: list[lex_index++].type = constsym;
								break;
						case 1: list[lex_index++].type = varsym;
								break;
						case 2: list[lex_index++].type = procsym;
								break;
						case 3: list[lex_index++].type = callsym;
								break;
						case 4: list[lex_index++].type = ifsym;
								break;
						case 5: list[lex_index++].type = thensym;
								break;
						case 6: list[lex_index++].type = elsesym;
								break;
						case 7: list[lex_index++].type = whilesym;
								break;
						case 8: list[lex_index++].type = dosym;
								break;
						case 9: list[lex_index++].type = beginsym;
								break;
						case 10: list[lex_index++].type = endsym;
								break;
						case 11: list[lex_index++].type = readsym;
								 break;
						case 12: list[lex_index++].type = writesym;
								 break;
						case 13: list[lex_index++].type = oddsym;
								 break;
					}

					break;
				}

				i++;
			}

			// if the variable is a keyword continue
			if (i < 14)
				continue;

			// if the variable is an identifier
			list[lex_index].type = identsym;
			strcpy(list[lex_index].name, varname);
			lex_index++;
			continue;
		}

		// check if the token is a numerical value
		if (isdigit(input[buff]))
		{
			char number[5];
			number[0] = input[buff];

			// fetching the whole number or checking if its an invalid identifier
			int i = 1;
			buff++;
			while (i < 5 && input[buff])
			{
				if (isalpha(input[buff]))
				{
					printerror(2);
					return NULL;
				}

				if (!isdigit(input[buff]))
					break;

				number[i] = input[buff];
				i++;
				buff++;
			}

			// checking if the number exceeds the maximum length
			if (i == 5)
			{
				printerror(3);
				return NULL;
			}

			list[lex_index].type = numbersym;
			list[lex_index].value = 0;
			for (int j = 0; j < i; ++j)
				list[lex_index].value = 10*list[lex_index].value + number[j] - 48;

			lex_index++;
			continue;
		}

		if (input[buff] == '/')
		{
			if (input[buff + 1] != '*')
			{
				list[lex_index++].type = slashsym;
				buff++;
				continue;
			}
			else
			{
				int endofcomment = 0;
				while (input[buff])
				{
					if (input[buff] == '*' && input[buff + 1] == '/')
					{
						endofcomment = 1;
						break;
					}

					buff++;
				}

				if (!endofcomment)
				{
					printerror(5);
					return NULL;
				}

				buff += 2;
				continue;
			}
		}

		if (input[buff] == '%')
		{
			list[lex_index++].type = modsym;
			buff++;
			continue;
		}

		if (input[buff] == '*')
		{
			list[lex_index++].type = multsym;
			buff++;
			continue;
		}

		if (input[buff] == '+')
		{
			list[lex_index++].type = plussym;
			buff++;
			continue;
		}

		if (input[buff] == '-')
		{
			list[lex_index++].type = minussym;
			buff++;
			continue;
		}

		if (input[buff] == '(')
		{
			list[lex_index++].type = lparentsym;
			buff++;
			continue;
		}

		if (input[buff] == ')')
		{
			list[lex_index++].type = rparentsym;
			buff++;
			continue;
		}

		if (input[buff] == ',')
		{
			list[lex_index++].type = commasym;
			buff++;
			continue;
		}

		if (input[buff] == '.')
		{
			list[lex_index++].type = periodsym;
			buff++;
			continue;
		}

		if (input[buff] == ';')
		{
			list[lex_index++].type = semicolonsym;
			buff++;
			continue;
		}

		if (input[buff] == '=' || input[buff] == ':')
		{
			if (input[buff + 1] == '=')
			{
				if (input[buff] == '=')
					list[lex_index++].type = eqlsym;
				else
					list[lex_index++].type = becomessym;

				buff += 2;
				continue;
			}
			else
			{
				printerror(1);
				return NULL;
			}
		}

		if (input[buff] == '<')
		{
			if (input[buff + 1] == '>' || input[buff + 1] == '=')
			{
				if (input[buff + 1] == '>')
					list[lex_index++].type = neqsym;
				else
					list[lex_index++].type = leqsym;

				buff+=2;
				continue;
			}
			else
			{
				list[lex_index++].type = lessym;
				buff++;
				continue;
			}
		}

		if (input[buff] == '>')
		{
			if (input[buff + 1] == '=')
			{
				list[lex_index++].type = geqsym;
				buff += 2;
				continue;
			}
			else
			{
				list[lex_index++].type = gtrsym;
				buff++;
				continue;
			}
		}

		printerror(1);
		return NULL;
	}

	printtokens();
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case lessym:
				printf("%11s\t%d", "<", lessym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case slashsym:
				printf("%11s\t%d", "/", slashsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparentsym:
				printf("%11s\t%d", "(", lparentsym);
				break;
			case rparentsym:
				printf("%11s\t%d", ")", rparentsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case becomessym:
				printf("%11s\t%d", ":=", becomessym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
