/**
 *  Create a C program that converts all lowercase letters from the command line arguments to
 *      uppercase letters and prints the result.
 *  Use a subprocess for each given argument.
 *  
 *  If there are no arguments, the program will print to STDOUT:
 *      "Error: no arguments" - exiting with exit code 0
 * 
 *  ./a.out abcd AbCd abtd3 q-t3
 *  ABCD ABCD ABTD3 Q-T3
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void uppercase(char *c) {
    if (*c >= 'a' && *c <= 'z') {
        putchar(*c - 32);

    } else {
        putchar(*c);
    }
}

int main(int argc, char ** argv) {
    if (argc < 2) {
        printf("Error: no arguments\n");
        return 0;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(0);
    }

    if (pid == 0) {
        for (int i = 1; i < argc; i++) {
            char *c = argv[i];

            while (*c != '\0') {
                uppercase(c);
                c++;
            }

            if (i < argc - 1) {
                printf(" ");
            }
        }
        printf("\n");
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        printf("Error waiting for child process\n");
        exit(1);
    }

    return 0;
}

    


