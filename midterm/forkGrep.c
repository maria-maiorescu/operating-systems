/*
Parent-child counting with grep

Problem - Write a C program that:
    - creates a child process
    - in the child, runs grep on a file to count how many lines contain the word "error"
    - in the parent, waits for the child and prints:
        - the child exit status
        - the number of matching lines

Example:
    If file.txt contains:
        hello
        error here
        no error
        error again

Then output should be:
    3  - matching lines
    Parent: child exited with status 0

Requirements:
    use fork()
    use execlp() or execvp() with grep
    use wait() or waitpid()
    print both the matched count and the exit status
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, usage: <word> %s file\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        execlp("grep", "grep", "-F", "-c", argv[1], argv[2], NULL);
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

    return 0;
}