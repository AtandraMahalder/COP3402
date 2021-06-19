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
int findsymb(char *reseved);

lexeme *lexanalyzer(char *input)
{
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;

	char reserved[][12] = {"const", "var", "procedure", "call", "if", "then",
						"else", "while", "do", "begin", "end", "read", "write",
						"odd"};

	FILE *fp = fopen(input, "r");

	char name[12];
	char buffer;
	while (fscanf(fp, "%c", &buffer) != EOF)
	{
		if (iscntrl(buffer)) continue;

		// check if the token is an identifier or keyword
		if (isalpha(buffer))
		{
			name[0] = buffer;
			int i = 1;
			while (i < 12 && fscanf(fp, "%c", &buffer))
			{
				if (isalpha(buffer) || isdigit(buffer))
					name[i++] = buffer;

				else break;
			}

			// if the length of the token is more than 12
			if (i == 12)
			{
				printerror(4);
				return NULL;
			}

			// if the identifier or keyword is valid convert it to a string
			name[i] = '\0';

			// check if the token is a keyword
			i = 0;
			while (i < 14)
			{
				if (!strcmp(reserved[i], name))
				{
					list[lex_index++].type = findsymb(reserved[i]);
					break;
				}

				i++;
			}

			if (i < 14)
			{
				fseek(fp, -1, SEEK_CUR);
				continue;
			}

			list[lex_index].type = 32;
			strcpy(list[lex_index].name, name);
			lex_index++;
			fseek(fp, -1, SEEK_CUR);
			continue;
		}

		// check if the token is a numerical value
		if (isdigit(buffer))
		{
			char numb[5];
			numb[0] = buffer;
			int i = 1;

			while (i < 5 && fscanf(fp, "%c", &buffer))
			{
				if (isdigit(buffer))
					numb[i++] = buffer;
				else if (isalpha(buffer))
				{
					printerror(2);
					return NULL;
				}
				else break;
			}

			if (i == 5)
			{
				printerror(3);
				return NULL;
			}

			list[lex_index].type = 33;
			list[lex_index].value = 0;
			for (int j = 0; j < i; ++j)
				list[lex_index].value = 10*list[lex_index].value + numb[i] - '0';

			lex_index++;
			fseek(fp, -1, SEEK_CUR);
			continue;
		}

		if (buffer == '/')
		{
			if (fscanf(fp, "%c", &buffer) != EOF)
			{
				if (buffer == '*')
				{
					int endofcomment = 0;
					while (fscanf(fp, "%c", &buffer) != EOF)
					{
						if (buffer == '*')
						{
							if (fscanf(fp, "%c", &buffer) == EOF)
							{
								printerror(5);
								return NULL;
							}
							else if (buffer == '/')
							{
								endofcomment = 1;
								break;
							}
							else fseek(fp, -1, SEEK_CUR);
						}
					}

					if (!endofcomment)
					{
						printerror(5);
						return NULL;
					}

					else continue;
				}
				else
				{
					list[lex_index++].type = 10;
					fseek(fp, -1, SEEK_CUR);
					continue;
				}
			}
			else
			{
				list[lex_index++].type = 10;
				fseek(fp, -1, SEEK_CUR);
				fclose(fp);
				return list;
			}
		}

		if (buffer == '%')
		{
			list[lex_index++].type = 8;
			continue;
		}

		if (buffer == ',')
		{
			list[lex_index++].type = 15;
			continue;
		}

		if (buffer == '.')
		{
			list[lex_index++].type = 16;
			continue;
		}

		if (buffer == ';')
		{
			list[lex_index++].type = 17;
			continue;
		}

		if (buffer == '-')
		{
			list[lex_index++].type = 12;
			continue;
		}

		if (buffer == ')')
		{
			list[lex_index++].type = 14;
			continue;
		}

		if (buffer == '(')
		{
			list[lex_index++].type = 13;
			continue;
		}

		if (buffer == '+')
		{
			list[lex_index++].type = 11;
			continue;
		}

		if (buffer == '*')
		{
			list[lex_index++].type = 9;
			continue;
		}

		if (buffer == '=')
		{
			if (fscanf(fp, "%c", &buffer) != EOF && buffer == '=')
			{
				list[lex_index++].type = 2;
				continue;
			}
			else
			{
				printerror(1);
				return NULL;
			}
		}

		if (buffer == ':')
		{
			if (fscanf(fp, "%c", &buffer) != EOF && buffer == '=')
			{
				list[lex_index++].type = 18;
				continue;
			}
			else
			{
				printerror(1);
				return NULL;
			}
		}

		if (buffer == '<')
		{
			if (fscanf(fp, "%c", &buffer) != EOF)
			{
				if (buffer == '>')
				{
					list[lex_index++].type = 3;
					continue;
				}
				if (buffer == '=')
				{
					list[lex_index++].type = 7;
					continue;
				}
				else
				{
					list[lex_index++].type = 4;
					fseek(fp, -1, SEEK_CUR);
					continue;
				}
			}
			else
			{
				list[lex_index++].type = 4;
				fseek(fp, -1, SEEK_CUR);
				fclose(fp);
				return list;
			}
		}

		if (buffer == '>')
		{
			if (fscanf(fp, "%c", &buffer) != EOF)
			{
				if (buffer == '=')
				{
					list[lex_index++].type = 7;
					continue;
				}
				else
				{
					list[lex_index++].type = 6;
					fseek(fp, -1, SEEK_CUR);
					continue;
				}
			}
			else
			{
				list[lex_index++].type = 6;
				fseek(fp, -1, SEEK_CUR);
				fclose(fp);
				return list;
			}
		}
	}

	fclose(fp);
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

int findsymb(char *reseved)
{
	if (!strcmp(reseved, "odd"))
		return 1;

	if (!strcmp(reseved, "begin"))
		return 19;

	if (!strcmp(reseved, "end"))
		return 20;

	if (!strcmp(reseved, "if"))
		return 21;

	if (!strcmp(reseved, "then"))
		return 22;

	if (!strcmp(reseved, "else"))
		return 23;

	if (!strcmp(reseved, "while"))
		return 24;

	if (!strcmp(reseved, "do"))
		return 25;

	if (!strcmp(reseved, "call"))
		return 26;

	if (!strcmp(reseved, "write"))
		return 27;

	if (!strcmp(reseved, "read"))
		return 28;

	if (!strcmp(reseved, "const"))
		return 29;

	if (!strcmp(reseved, "var"))
		return 30;

	if (!strcmp(reseved, "procedure"))
		return 31;
}
