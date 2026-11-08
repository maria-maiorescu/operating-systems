/** 
Shift letters and digits with different offsets

Problem - Create a C program that receives several command-line arguments and for each argument:
    shifts every letter by 5 positions in the alphabet
    shifts every digit by 2 positions in the decimal set
    leaves other characters unchanged
    prints a dash '-' between arguments
    uses a child process for each argument

Better to define precise rules:
    letters wrap around A-Z and a-z
    digits wrap around 0-9
    if a char is not a letter/digit, print it unchanged

Example:
    ./a.out ab12 Z9

Result:
    fg34 e?

But we need exact output:
    a -> f
    b -> g
    1 -> 3
    2 -> 4
    Z -> E
    9 -> 1

So final for args:
    fg34-E1

If multiple args, print '-' between them.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void process(char *text) {
    char *c = text;

    while (*c != 0) {
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')) {
            *c = (*c + 5) & 0x7F;
        }

        if (*c >= '0' && *c <= '9') {
            *c = (*c + 2) & 0x7F;
        }

        printf("%c", *c);
        c++;
    }
}

int main(int argc, char ** argv) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        for (int i = 1; i < argc; i++) {
            process(argv[i]);

            if (i < argc - 1) {
                printf("-");
            }
            
        }
        printf("\n");
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        fprintf(stderr, "Error waiting for child process\n");
        exit(1);
    }

    /*
        if (WIFEXITED(status)) {
            printf("Parent: child exited with status %d\n", WEXITSTATUS(status));
        }
    */
    
}