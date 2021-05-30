#include<stdio.h>
#include<stdlib.h>
#define MAX_PAS_SIZE 500

int sp;
int bp;
int ir[3];
int pc;
int halt = 0;

int pas[MAX_PAS_SIZE];
char *instructions[24];

int base(int);

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "r");

    int i = 0;
    while (fp != EOF)
    {
        fscanf(fp, "%d", pas + i);
        i++;
    }

    sp = i - 1;
    bp = i;
    pc = 0;


    while (!halt)
    {
        for (int i = 0; i < 3; ++i)
            ir[i] = pas[pc + i];

        pc += 3;

        switch (ir[0])
        {
            case 1: sp + 1;
                    pas[sp] = M;
                    break;

            case 2: switch(ir[2])
                    {
                        case 0: sp = bp - 1;
                                bp = pas[sp + 2];
                                pc = pas[sp + 3];
                                break;

                        case 1: pas[sp] = -pas[sp];
                                break;

                        case 2: sp = sp - 1;
                                pas[sp] = pas[sp] + pas[sp + 1];
                                break;

                        case 3: sp = sp - 1;
                                pas[sp] = pas[sp] - pas[sp + 1];
                                break;

                        case 4: sp = sp - 1;
                                pas[sp] = pas[sp] * pas[sp + 1];
                                break;

                        case 5: sp = sp - 1;
                                pas[sp] = pas[sp] / pas[sp + 1];
                                break;

                        case 6: pas[sp] = pas[sp] & 1;
                                break;

                        case 7: sp = sp - 1;
                                pas[sp] = pas[sp] % pas[sp + 1];
                                break;

                        case 8: sp = sp - 1;
                                pas[sp] = (pas[sp] == pas[sp + 1]);
                                break;

                        case 9: sp = sp - 1;
                                pas[sp] = (pas[sp] != pas[sp + 1]);
                                break;

                        case 10: sp = sp - 1;
                                 pas[sp] = (pas[sp] < pas[sp + 1]);
                                 break;

                        case 11: sp = sp - 1;
                                 pas[sp] = (pas[sp] <= pas[sp + 1]);
                                 break;

                        case 12: sp = sp - 1;
                                 pas[sp] = (pas[sp] > pas[sp + 1]);
                                 break;

                        case 13: sp = sp - 1;
                                 pas[sp] = (pas[sp] >= pas[sp + 1]);
                                 break;
                    }
                    break;

            case 3: sp = sp + 1;
                    pas[sp] = pas[base(ir[1]) + ir[2]];
                    break;

            case 4: pas[base(ir[1]) + ir[2]] = pas[sp];
                    sp = sp - 1;
                    break;

        }

    }

    return 0;
}

int base(int l)
{
    int arb = bp;

    while (l > 0)
    {
        arb = pas[arb];
        l--;
    }

    return arb;
}
