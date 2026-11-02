#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int id = fork();
    int status;

    if (id < 0) {
        printf("Error creating the process\n");
        exit(1);
    }

    if (id == 0) {
        printf("This is the child\n");
        printf("The child's PID is %d\n", getpid());
        printf("The value of the fork return in the child is %d\n", id);
        printf("The value of the child's parent is %d\n", getppid());

        exit(0);
    }

    printf("This is the parent\n");
    printf("The parent's PID is %d\n", getpid());
    printf("The value of the fork return in the parent is %d\n", id);

    if (waitpid(id, &status, 0) == -1) {
        printf("Error waiting for child process\n");
        exit(1);
    }

    if (WIFEXITED(status)) {
        printf("The child's exit status is %d\n", WEXITSTATUS(status));
    } else {
        printf("The child didn't end normally\n");
    }

    return 0;
}