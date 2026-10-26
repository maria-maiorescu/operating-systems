#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    /* fork() creates a new process:
       - returns -1 on error
       - returns 0 in the child
       - returns child's PID in the parent */
    int id = fork();
    int status;

    if ( id < 0) {
        printf("Error creating the process\n");
        exit(1);
    }
    
    if (id == 0) {
        /* Child path: id is always 0 here. */
        printf("This is the child\n");
        printf("The PID of the child is %d\n", getpid());
        printf("The value of the fork return in the child is %d\n", id);
        printf("The PID of the parent (from child) is %d\n", getppid());
        exit(0);

    } 
    
    /* Parent path: id contains the PID of the created child. */
    printf("This is the parent\n");
    printf("The PID of the parent is %d\n", getpid());
    printf("The value of the fork return in the parent is %d\n", id);
    printf("Hello world!\n");

    /* waitpid() collects the child's termination status in "status". */
    if (waitpid(id, &status, 0) == -1) {
        printf("Error waiting for child process\n");
        exit(1);
    }

    /* WIFEXITED checks if child ended with exit()/return from main(). */
    if (WIFEXITED(status)) {
        /* WEXITSTATUS extracts the exit code given by exit(code). */
        printf("The child EXITSTATUS is %d\n", WEXITSTATUS(status));
    } else {
        /* Example: terminated by a signal, not by normal exit. */
        printf("The child did not end normally\n");
    }


    return 0;
}