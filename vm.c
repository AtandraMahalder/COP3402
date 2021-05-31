#include<stdio.h>
#include<stdlib.h>
#define MAX_PAS_SIZE 500

int sp;
int bp;
int ir[3];
int pc;
int halt = 0;

int pas[MAX_PAS_SIZE];

int base(int);

int main(int argc, char **argv)
{
    for (int i = 0; i < MAX_PAS_SIZE; ++i)
        pas[i] = 0;

    FILE *fp = fopen(argv[1], "r");

    int i = 0;
    while (!feof(fp))
    {
        fscanf(fp, "%d", pas + i);
        i++;
    }

    sp = i - 2;
    bp = i - 1;
    pc = 0;

    int arbdiv[MAX_PAS_SIZE];
    int arbnum = 0;

    int stackstart = i - 1;

    printf("                 PC   BP   SP    stack\n");
    printf("Initial values:   0  %3d  %3d\n", bp, sp);

    while (!halt)
    {
        int curr_pc = pc;

        for (int i = 0; i < 3; ++i)
            ir[i] = pas[pc + i];

        pc += 3;

        int aux1, aux2;

        switch (ir[0])
        {
            case 1: sp = sp + 1;
                    pas[sp] = ir[2];
                    printf("%3d LIT %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 2: switch(ir[2])
                    {
                        case 0: aux1 = bp;
                                aux2 = sp;

                                sp = bp - 1;
                                bp = pas[sp + 2];
                                pc = pas[sp + 3];

                                for (int i = aux1; i <= aux2; ++i)
                                    pas[i] = 0;

                                arbnum--;

                                printf("%3d RTN %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);

                                break;

                        case 1: pas[sp] = -pas[sp];
                                printf("%3d NEG %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 2: sp = sp - 1;
                                pas[sp] = pas[sp] + pas[sp + 1];
                                printf("%3d ADD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 3: sp = sp - 1;
                                pas[sp] = pas[sp] - pas[sp + 1];
                                printf("%3d SUB %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 4: sp = sp - 1;
                                pas[sp] = pas[sp] * pas[sp + 1];
                                printf("%3d MUL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 5: sp = sp - 1;
                                pas[sp] = pas[sp] / pas[sp + 1];
                                printf("%3d DIV %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 6: pas[sp] = pas[sp] & 1;
                                printf("%3d ODD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 7: sp = sp - 1;
                                pas[sp] = pas[sp] % pas[sp + 1];
                                printf("%3d MOD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 8: sp = sp - 1;
                                pas[sp] = (pas[sp] == pas[sp + 1]);
                                printf("%3d EQL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 9: sp = sp - 1;
                                pas[sp] = (pas[sp] != pas[sp + 1]);
                                printf("%3d NEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        case 10: sp = sp - 1;
                                 pas[sp] = (pas[sp] < pas[sp + 1]);
                                 printf("%3d LSS %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        case 11: sp = sp - 1;
                                 pas[sp] = (pas[sp] <= pas[sp + 1]);
                                 printf("%3d LEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        case 12: sp = sp - 1;
                                 pas[sp] = (pas[sp] > pas[sp + 1]);
                                 printf("%3d GTR %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        case 13: sp = sp - 1;
                                 pas[sp] = (pas[sp] >= pas[sp + 1]);
                                 printf("%3d GEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;
                    }
                    break;

            case 3: sp = sp + 1;
                    pas[sp] = pas[base(ir[1]) + ir[2]];
                    printf("%3d LOD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 4: pas[base(ir[1]) + ir[2]] = pas[sp];
                    sp = sp - 1;
                    printf("%3d STO %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 5: pas[sp + 1] = base(ir[1]);
                    pas[sp + 2] = bp;
                    pas[sp + 3] = pc;
                    bp = sp + 1;
                    pc = ir[2];

                    arbdiv[arbnum] = bp;
                    arbnum++;

                    printf("%3d CAL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 6: sp = sp + ir[2];
                    printf("%3d INC %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 7: pc = ir[2];
                    printf("%3d JMP %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 8: if (pas[sp] == 1)
                        pc = ir[2];

                    sp = sp - 1;

                    printf("%3d JPC %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            case 9: switch (ir[2])
                    {
                        case 1: printf("Output result is: %d\n", pas[sp]);
                                sp = sp - 1;
                                break;

                        case 2: sp = sp + 1;
                                printf("Please enter an integer: ");
                                scanf("%d", &pas[sp]);
                                break;

                        case 3: halt = 1;
                                break;
                    }

                    printf("%3d SYS %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;
        }

        int j = 0;
        for (int i = stackstart; i <= sp; ++i)
        {
            if (j < arbnum && i == arbdiv[j])
            {
                printf("|");
                j++;
            }
            
            printf("%3d ", pas[i]);
        }

        printf("\n");
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
