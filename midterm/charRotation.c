/*
Printable ASCII rotation

Problem:

Create a C program that takes several command-line arguments and for each character in each argument:
    - shifts it by 7 positions inside the printable ASCII range [32, 126]
    - wraps around when it reaches the end (wrapping around from '~' to '.')
    - prints a '-' between arguments
    - uses a subprocess for each argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void charProcess(char *text) {
    char *c = text;

    while (*c != 0) {
        if (*c >= 32 && *c <= 126) {
            unsigned char newChar;
            newChar = *c + 7;

            if (newChar >= 126) {
                *c = 32 + (newChar - 126);
            } else {
                *c = (char)newChar;
            }
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
            charProcess(argv[i]);

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

    return 0;
}