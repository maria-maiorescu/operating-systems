/**
 * Write a C program for UNIX that creates 20 processes (including the parent). 
 * Each process prints 10 lines on the screen containing its type (parent, child1, child2, ... , child19) 
 *  and its PID. 
 * After that, the child processes will terminate with different values, 
 *  and the parent will print the values ​​returned by the children.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define NR 19

int main() {
    pid_t id;
    int status;
    int childPid[NR];

    for (int i = 0; i < NR; i++) {
        id = fork();

        if (id < 0) {
            fprintf(stderr, "Error creating the child!\n");
            exit(1);
        }

        if (id == 0) {
            // child process
            int pidChild = getpid();
            int pidParent = getppid();

            for (int j = 0; j < 10; j++) {
                printf("I'm the child nr %d with PID %d (parent PID is: %d) - %d\n", i+1, pidChild, pidParent, j+1);
                usleep(100000 * j);
            }

            exit(100 + i);
        } 

        childPid[i] = id;
    }

    //parent process
    for (int j = 0; j < 10; j++) {
        printf("I'm the parent with PID %d - %d\n", getpid(), j+1);
        usleep(300000);
    }
    
    // waits for children
    for (int i = 0; i < NR; i++) {
        waitpid(childPid[i], &status, 0);

        if (WIFEXITED(status)) {
            printf("The child nr %d EXITSTATUS is %d\n", i+1, WEXITSTATUS(status));
        } else {
            printf("The child nr %d did not end normally\n", i+1);
        }
    }

    printf("All processes are finished\n");
    return 0;
} 