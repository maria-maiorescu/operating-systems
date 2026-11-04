/*
Parent and Child search different words

Write a C program that - Receives a filename as argument:
    - Calls fork().
    - In the child process, run:
    - grep -c error <file>
    - In the parent process, run:
    - grep -c warning <file>
    - Parent must wait() for child and print child exit status.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error, usage: %s file", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("grep", "grep", "-c", "-i", "error", argv[1], NULL);
        exit(0);
    }
    
    pid_t pid2 = fork();

    if (pid2 < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid2 == 0) {
        execlp("grep", "grep", "-c", "-i", "warning", argv[1], NULL);
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        fprintf(stderr, "Error waiting for child process\n");
        exit(1);
    }

    if (WIFEXITED(status)) {
        printf("Parent: child exited with status %d\n", WEXITSTATUS(status));
    }

    int status1;
    pid_t w1 = waitpid(pid2, &status1, 0);

    if (w1 == -1) {
        fprintf(stderr, "Error waiting for child process\n");
        exit(1);
    }

    if (WIFEXITED(status1)) {
        printf("Parent: child exited with status %d\n", WEXITSTATUS(status1));
    }

    return 0;
}