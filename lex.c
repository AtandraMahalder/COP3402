/*
	Created by Atandra Mahalder, Emin Mammadzada
	Date: June 22, 2021
	COP3402 
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"


lexeme *list;
int lex_index;

//function to print error messages in case the scanner encounters lexical errors
void printerror(int type);

//function to print the tokens in both list and table formats
void printtokens();

//function to read in the program, tokenize it, and store in an array
lexeme *lexanalyzer(char *input)
{
	//array to store the lexeme list
	list = malloc(500 * sizeof(lexeme));
	lex_index = 0;

	//list of keywords of the PL/0 language
	char reserved[][12] = {"const", "var", "procedure", "call", "if", "then",
						"else", "while", "do", "begin", "end", "read", "write",
						"odd"};

	int buff = 0;

	//read contents of the input file
	while (input[buff])
	{
		//check if the character is a whitespace and ignore it if it is
		if (iscntrl(input[buff]) || iscntrl(input[buff]) == ' ')
		{
			buff++;
			continue;
		}

		// check if the token is an identifier or a keyword
		if (isalpha(input[buff]))
		{
			//temporary storage to store the possible keyword or identifier
			char varname[12];
			varname[0] = input[buff];

			// reading in an identifier or keyword
			int i = 1;
			buff++;
			while (i < 12 && (isalpha(input[buff]) || isdigit(input[buff])))
			{
				varname[i] = input[buff];
				buff++;
				i++;
			}

			// if identifier or keyword has excessive length
			if (i == 12 && (isalpha(input[buff]) || isdigit(input[buff])))
			{
				printerror(4);
				return NULL;
			}

			//add terminating character if keyword or identifier is valid
			varname[i] = '\0';

			i = 0;
			while (i < 14)
			{
				/* check if the variable is a keyword
				and determine its type based on the enum values provided
				in the compiler.h file
				*/ 

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

			// if the variable is a keyword continue, since we assigned its type inside of switch statement already
			if (i < 14)
				continue;

			// if the variable is an identifier
			list[lex_index].type = identsym;

			//store the name of the identifier
			strcpy(list[lex_index].name, varname);

			//move the lexeme list array index forward
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

			//read the number as long as it is less than 5 digits long
			while (i < 5 && input[buff])
			{
				/*check if the character is letter and throw invalid identifier error,
				since letters cannot be part of numerical values 
				*/
				if (isalpha(input[buff]))
				{
					printerror(2);
					return NULL;
				}

				// stop taking input if the character is not a digit
				if (!isdigit(input[buff]))
					break;

				number[i] = input[buff];
				i++;
				buff++;
			}

			// checking if the number exceeds the maximum length
			if (i == 5)
			{
				//if the input is a digit , which makes number more than 5 digits long, throw excessive number length error
				if (isdigit(input[buff]))
				{
					printerror(3);
					return NULL;
				}

				//if the input is not a digit, throw invalid identifier error
				if (isalpha(input[buff]))
				{
					printerror(2);
					return NULL;
				}
			}

			//assign numerical token type to the number
			list[lex_index].type = numbersym;
			list[lex_index].value = 0;

			//convert the array of characters that represents the number into an int type and store it in the value field
			for (int j = 0; j < i; ++j)
				list[lex_index].value = 10*list[lex_index].value + number[j] - 48;

			//move the lexeme list array index forward
			lex_index++;
			continue;
		}

		//check if character is a forward slash
		if (input[buff] == '/')
		{

			/*if the next character is not a *, that means it's a regular
			slash symbol, hence should not be ignored
			*/
			if (input[buff + 1] != '*')
			{
				list[lex_index++].type = slashsym;
				buff++;
				continue;
			}

			else
			{
				//create a flag to determine if the comment ends or not
				int endofcomment = 0;
				while (input[buff])
				{
					//if we encounter * followed by a /, program reached the end of the comment and should exit
					if (input[buff] == '*' && input[buff + 1] == '/')
					{
						endofcomment = 1;
						break;
					}

					buff++;
				}

				// if the comment was never ended, throw never ending comment error
				if (!endofcomment)
				{
					printerror(5);
					return NULL;
				}
				
				//move the buff to "point" at character after the / symbol
				buff += 2;
				continue;
			}
		}

		// if the character is % operator, add its type based on the enum from compiler.h
		if (input[buff] == '%')
		{
			list[lex_index++].type = modsym;
			buff++;
			continue;
		}

		// if the character is * operator, add its type based on the enum from compiler.h
		if (input[buff] == '*')
		{
			list[lex_index++].type = multsym;
			buff++;
			continue;
		}

		// if the character is +, add its type based on the enum from compiler.h
		if (input[buff] == '+')
		{
			list[lex_index++].type = plussym;
			buff++;
			continue;
		}

		// if the character is -, add its type based on the enum from compiler.h

		if (input[buff] == '-')
		{
			list[lex_index++].type = minussym;
			buff++;
			continue;
		}

		// if the character is (, add its type based on the enum from compiler.h
		if (input[buff] == '(')
		{
			list[lex_index++].type = lparentsym;
			buff++;
			continue;
		}

		// if the character is ), add its type based on the enum from compiler.h
		if (input[buff] == ')')
		{
			list[lex_index++].type = rparentsym;
			buff++;
			continue;
		}

		// if the character is "," , add its type based on the enum from compiler.h
		if (input[buff] == ',')
		{
			list[lex_index++].type = commasym;
			buff++;
			continue;
		}

		// if the character is "." , add its type based on the enum from compiler.h
		if (input[buff] == '.')
		{
			list[lex_index++].type = periodsym;
			buff++;
			continue;
		}

		// if the character is ";" , add its type based on the enum from compiler.h

		if (input[buff] == ';')
		{
			list[lex_index++].type = semicolonsym;
			buff++;
			continue;
		}

		if (input[buff] == '=' || input[buff] == ':')
		{	
			//if the character following "=" or ":" is an "=", then assign a token type based on the preceding character
			if (input[buff + 1] == '=')
			{
				if (input[buff] == '=')
					list[lex_index++].type = eqlsym;
				else
					list[lex_index++].type = becomessym;

				buff += 2;
				continue;
			}

			// if "=" or ":" are not followed by an "=", throw invalid symbol error
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
				//if "<" is followed by ">", then it's a negation operator
				if (input[buff + 1] == '>')
					list[lex_index++].type = neqsym;

				//if "<" is followed by "=", then it's a "less than or equal to" operator
				else
					list[lex_index++].type = leqsym;

				buff+=2;
				continue;
			}

			//if "<" is not followed by ">", "=", then assign its token type to be of "less than" operator
			else
			{
				list[lex_index++].type = lessym;
				buff++;
				continue;
			}
		}

		if (input[buff] == '>')
		{
			//if ">" is followed by "=", then assign its type to be of "greater than or equal to" operator
			if (input[buff + 1] == '=')
			{
				list[lex_index++].type = geqsym;
				buff += 2;
				continue;
			}

			//if ">" is not followed by, "=", then assign its token type to be of "greater than" operator
			else
			{
				list[lex_index++].type = gtrsym;
				buff++;
				continue;
			}
		}

		/*if the character being read does not fall under any of the possible categories above,
		throw invalid symbol error
		*/
		printerror(1);
		return NULL;
	}

	//print the obtained tokens and return the lexeme list
	printtokens();
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");

	//print the tokens and the token type for each of the lexemes. 
	// if the token is a reserved word, print the keyword, if it is a numerical value, 
	// print its value, and if it is an identifier, print the variable name
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

	//print the token list, along with values for numerical tokens, and identifier names for identifiers
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

//throw errors based on the lexical constraints of the PL/0 language
void printerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");

	//if the number is more than 5 digits long
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	
	//if identifier name is more than 12 characters long
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Neverending Comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
