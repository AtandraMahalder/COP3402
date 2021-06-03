// Atandra Mahalder and Emin Mammadzada
// Project - 1: Virtual Machine implementing a PM/0 stack machine
// COP 3402 Summer CV01
// 05/31/2021

// included libraries
#include<stdio.h>
#include<stdlib.h>

// defining constants to be used throughout the program
#define MAX_PAS_SIZE 500

// defining global variables
int sp;
int bp;
int ir[3];
int pc;
int halt = 0;
int pas[MAX_PAS_SIZE];

// function prototypes
int base(int);

// main function
int main(int argc, char **argv)
{
    // initializing the program address space pas
    for (int index = 0; index < MAX_PAS_SIZE; ++index)
        pas[index] = 0;

    // fetching the input file from command line and opening it with
    // file pointer fp
    FILE *fp = fopen(argv[1], "r");

    // loading the program from the file into the pas
    int index = 0;
    while (!feof(fp))
    {
        fscanf(fp, "%d", pas + index);
        index++;
    }

    // initializing the stack pointer, base pointer and program counter
    sp = index - 2;
    bp = index - 1;
    pc = 0;

    // creating an array to store the beginning of each activation record
    int arbdiv[MAX_PAS_SIZE];
    int arbnum = 0;

    // creating a variable to store the base of the stack
    int stackstart = bp;

    // initial output
    printf("                 PC   BP   SP    stack\n");
    printf("Initial values:   0  %3d  %3d\n", bp, sp);

    // fetching and executing the program line by line
    while (!halt)
    {
        // storing the current address of the program counter
        int curr_pc = pc;

        // executing the fetch cycle
        for (int i = 0; i < 3; ++i)
            ir[i] = pas[pc + i];

        // updating the program counter
        pc += 3;

        // declaring variables for auxilliary purposes
        int aux1, aux2;

        // using operation code to select the instruction to be executed
        switch (ir[0])
        {
            // Pushes a constant value (literal) ir[2] onto the stack
            case 1: sp = sp + 1;
                    pas[sp] = ir[2];

                    // print relevant values associated with the stack machine
                    printf("%3d LIT %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Operation to be performed on the data at the top of the stack
            case 2: switch(ir[2])
                    {
                        // if operation is return
                        case 0: // storing base and stack pointer of current activation record
                                // using auxilliary variables
                                aux1 = bp;
                                aux2 = sp;

                                // updating stack variables to return from current activation record
                                // to actiavtion record from where the function was called
                                sp = bp - 1;
                                bp = pas[sp + 2];
                                pc = pas[sp + 3];

                                // setting all the values in the function activation record to 0
                                pas[sp + 1] = 0;
                                pas[sp + 2] = 0;
                                pas[sp + 3] = 0;
                                for (int i = aux1; i <= aux2; ++i)
                                    pas[i] = 0;

                                // updating size of arbdiv
                                arbnum--;

                                // print relevant values associated with the stack machine
                                printf("%3d RTN %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);

                                break;

                        // if operation is negate
                        case 1: pas[sp] = -pas[sp];

                                // print relevant values associated with the stack machine
                                printf("%3d NEG %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is add
                        case 2: sp = sp - 1;
                                pas[sp] = pas[sp] + pas[sp + 1];
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d ADD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is subtract
                        case 3: sp = sp - 1;
                                pas[sp] = pas[sp] - pas[sp + 1];
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d SUB %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is multiply
                        case 4: sp = sp - 1;
                                pas[sp] = pas[sp] * pas[sp + 1];
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d MUL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is divide
                        case 5: sp = sp - 1;
                                pas[sp] = pas[sp] / pas[sp + 1];
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d DIV %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is to check whether the integer on top of the stack is odd
                        case 6: pas[sp] = pas[sp] & 1;

                                // print relevant values associated with the stack machine
                                printf("%3d ODD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is modulus
                        case 7: sp = sp - 1;
                                pas[sp] = pas[sp] % pas[sp + 1];
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d MOD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is to check for equality between the value on top of stack and the
                        // one beneath it
                        case 8: sp = sp - 1;
                                pas[sp] = (pas[sp] == pas[sp + 1]);
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d EQL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is to check for inequality between the value on top of stack and the
                        // one beneath it
                        case 9: sp = sp - 1;
                                pas[sp] = (pas[sp] != pas[sp + 1]);
                                pas[sp + 1] = 0;

                                // print relevant values associated with the stack machine
                                printf("%3d NEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                break;

                        // if the operation is to check whether the value on top of stack is greater than the one
                        // beneath it
                        case 10: sp = sp - 1;
                                 pas[sp] = (pas[sp] < pas[sp + 1]);
                                 pas[sp + 1] = 0;

                                 // print relevant values associated with the stack machine
                                 printf("%3d LSS %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        // if the operation is to check whether the value on top of stack is greater than or equal
                        // to the one beneath it
                        case 11: sp = sp - 1;
                                 pas[sp] = (pas[sp] <= pas[sp + 1]);
                                 pas[sp + 1] = 0;

                                 // print relevant values associated with the stack machine
                                 printf("%3d LEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        // if the operation is to check whether the value on top of stack is less than the one
                        // beneath it
                        case 12: sp = sp - 1;
                                 pas[sp] = (pas[sp] > pas[sp + 1]);
                                 pas[sp + 1] = 0;

                                 // print relevant values associated with the stack machine
                                 printf("%3d GTR %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;

                        // if the operation is to check whether the value on top of stack is less than or equal
                        // to the one beneath it
                        case 13: sp = sp - 1;
                                 pas[sp] = (pas[sp] >= pas[sp + 1]);
                                 pas[sp + 1] = 0;

                                 // print relevant values associated with the stack machine
                                 printf("%3d GEQ %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                                 break;
                    }
                    break;

            // Load value to top of stack from the stack location at offset ir[2] from ir[1] lexicographical levels down
            case 3: sp = sp + 1;
                    pas[sp] = pas[base(ir[1]) + ir[2]];

                    // print relevant values associated with the stack machine
                    printf("%3d LOD %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Store value at top of stack in the stack location at offset ir[2] from ir[1] lexicographical levels down
            case 4: pas[base(ir[1]) + ir[2]] = pas[sp];
                    pas[sp] = 0;
                    sp = sp - 1;

                    // print relevant values associated with the stack machine
                    printf("%3d STO %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Call procedure at code index ir[2] (generates new Activation Record and PC = ir[2])
            case 5: // initialize new actiavtion record
                    pas[sp + 1] = base(ir[1]);
                    pas[sp + 2] = bp;
                    pas[sp + 3] = pc;
                    bp = sp + 1;
                    pc = ir[2];

                    // store the base pointer of new activation record for printing purposes
                    arbdiv[arbnum] = bp;
                    arbnum++;

                    // print relevant values associated with the stack machine
                    printf("%3d CAL %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Allocate M memory words (increment SP by M). First four are reserved to Static Link (SL),
            // Dynamic Link (DL), Return Address (RA), and Parameter (P)
            case 6: sp = sp + ir[2];

                    // print relevant values associated with the stack machine
                    printf("%3d INC %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Jump to instruction ir[2] (PC = ir[2])
            case 7: pc = ir[2];

                    // print relevant values associated with the stack machine
                    printf("%3d JMP %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // Jump to instruction M if top stack element is 1
            case 8: if (pas[sp] == 1)
                        pc = ir[2];

                    pas[sp] = 0;
                    sp = sp - 1;

                    // print relevant values associated with the stack machine
                    printf("%3d JPC %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;

            // if the instruction is a system call
            case 9: switch (ir[2])
                    {
                        // Write the top stack element to the screen
                        case 1: printf("Output result is: %d\n", pas[sp]);
                                pas[sp] = 0;
                                sp = sp - 1;
                                break;

                        // Read in input from the user and store it on top of the stack
                        case 2: sp = sp + 1;
                                printf("Please Enter an Integer:\n");
                                scanf("%d", pas + sp);
                                break;

                        // End of program
                        case 3: halt = 1;
                                break;
                    }

                    // print relevant values associated with the stack machine
                    printf("%3d SYS %3d %3d %3d  %3d  %3d  ", curr_pc, ir[1], ir[2], pc, bp, sp);
                    break;
        }

        // printing the program stack
        int j = 0;
        for (int i = stackstart; i <= sp; ++i)
        {
            // if we are at the beginning of an activation record
            // print out a | to denote demarcation
            if (j < arbnum && i == arbdiv[j])
            {
                printf("|");
                j++;
            }

            // print out the stack element at the current index
            printf("%3d ", pas[i]);
        }

        // set cursor to newline to print information about execution of the next instruction
        printf("\n");
    }

    // close the file pointer
    fclose(fp);

    // end of main
    return 0;
}

// function to locate bp of actiavtion record l lexicographical levels down
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
