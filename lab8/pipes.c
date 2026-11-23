
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int fd[2];   // Child -> Parent
    int fd2[2];  // Parent -> Child
    pid_t id;
    size_t length;
    const char reply[] = "alabalaportocala";

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    if (pipe(fd) < 0) {
        perror("Error creating first pipe");
        return 2;
    }

    if (pipe(fd2) < 0) {
        perror("Error creating second pipe");
        close(fd[0]);
        close(fd[1]);
        return 2;
    }

    length = strlen(argv[1]);

    id = fork();

    if (id < 0) {
        perror("Error creating fork");
        close(fd[0]);
        close(fd[1]);
        close(fd2[0]);
        close(fd2[1]);
        return 3;
    }

    if (id == 0) {
        // CHILD PROCESS

        close(fd[0]);
        close(fd2[1]);

        // Write the argument into the first pipe
        size_t written = 0;
        while (written < length) {
            ssize_t n = write(fd[1],
                              argv[1] + written,
                              length - written);
            if (n <= 0) {
                perror("Error writing to pipe from child");
                _exit(4);
            }
            written += (size_t)n;
        }

        printf("Child wrote %s to first pipe\n", argv[1]);
        fflush(stdout);
        close(fd[1]);

        // Read the reply from the second pipe
        char buffer2[sizeof(reply)];
        size_t received = 0;
        size_t reply_length = strlen(reply);

        while (received < reply_length) {
            ssize_t n = read(fd2[0],
                             buffer2 + received,
                             reply_length - received);
            if (n < 0) {
                perror("Error reading second pipe");
                _exit(5);
            }
            if (n == 0)
                break;

            received += (size_t)n;
        }

        buffer2[received] = '\0';

        printf("Child read %s from second pipe\n", buffer2);

        close(fd2[0]);
        exit(0);
    }
    else {
        // PARENT PROCESS

        close(fd[1]);
        close(fd2[0]);

        char *buffer = malloc(length + 1);

        if (buffer == NULL) {
            perror("malloc");
            close(fd[0]);
            close(fd2[1]);
            waitpid(id, NULL, 0);
            return 6;
        }

        // Read the argument from the first pipe
        size_t received = 0;

        while (received < length) {
            ssize_t n = read(fd[0],
                             buffer + received,
                             length - received);
            if (n < 0) {
                perror("Error reading first pipe");
                free(buffer);
                close(fd[0]);
                close(fd2[1]);
                waitpid(id, NULL, 0);
                return 5;
            }
            if (n == 0)
                break;

            received += (size_t)n;
        }

        buffer[received] = '\0';

        printf("Parent read %s from first pipe\n", buffer);
        fflush(stdout);

        close(fd[0]);
        free(buffer);

        // Write a reply into the second pipe
        size_t written = 0;
        size_t reply_length = strlen(reply);

        while (written < reply_length) {
            ssize_t n = write(fd2[1],
                              reply + written,
                              reply_length - written);
            if (n <= 0) {
                perror("Error writing second pipe");
                close(fd2[1]);
                waitpid(id, NULL, 0);
                return 4;
            }

            written += (size_t)n;
        }

        printf("Parent wrote %s to second pipe\n", reply);
        fflush(stdout);

        close(fd2[1]);
        waitpid(id, NULL, 0);
    }

    return 0;
}
