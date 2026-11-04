/*
Fork and count words in multiple files

Problem:
    Write a C program that:
        - takes 3 filenames as arguments
        - for each file, creates a child process
        - each child runs wc -w on its file
        - the parent waits for all children
        - the parent prints the total number of words across all files

Example:
    If files are:
        - a.txt → 10 words
        - b.txt → 25 words
        - c.txt → 15 words

Then output should be:
    Total words: 50

Requirements:
    - use fork()
    - use execlp() or execvp() with wc
    - each child processes one file
    - parent collects and sums the results
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char**argv) {
    if (argc < 2) {
        fprintf(stderr, "Error, usage: %s <file1> <fie2>...\n", argv[0]);
        return 1;
    }

    pid_t pid[10];
    for (int i = 1; i < argc; i++) {
        pid[i] = fork();

        if (pid[i] < 0) {
            fprintf(stderr, "Error while creating the process\n");
            exit(1);
        }

        if (pid[i] == 0) {
            execlp("wc", "wc", "-w", argv[i], NULL);
            exit(0);
        }
    }
    
    for (int i = 1; i < argc; i++) {
        int status;
        pid_t w = waitpid(pid[i], &status, 0);

        if (w == -1) {
            fprintf(stderr, "Error waiting for child process\n");
            exit(1);
        }

        if (WIFEXITED(status)) {
            printf("Parent: child exited with status %d\n", WEXITSTATUS(status));
        }
    }
   
    return 0;
}