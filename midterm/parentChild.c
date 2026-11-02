/**
 * Parent-child with alternation
 * Parent prints 1..5, child prints A..E.
 * This trains process scheduling and synchronization intuition.
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1); 

    } else if (pid == 0) {
        // Child process prints A..E
        for (char c = 'A'; c <= 'E'; c++) {
            printf("%c\n", c);
            fflush(stdout);
            sleep(1);
        }
        exit(0);
        
    } else {
        // Parent process prints 1..5
        for (int i = 1; i <= 5; i++) {
            printf("%d\n", i);
            fflush(stdout);
            sleep(1);
        }
        wait(NULL);
    }
    return 0;
}