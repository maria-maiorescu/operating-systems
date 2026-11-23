#include <stdio.h>     // printf()
#include <stdlib.h>    // NULL
#include <unistd.h>    // fork(), execlp()
#include <sys/wait.h>  // wait()

int handler(char **argv)
{
    printf("%s", argv[1]);
    int pid = fork();

    if (pid == 0) {
        execlp("head", "head", argv[1], "-n", "4", NULL);
        printf("Error at exec\n");
        exit(3);
    }
    else {
        wait(NULL); //wait(&status) -> WEXITSTATUS(status);
        printf("Child Complete");
    }
}
