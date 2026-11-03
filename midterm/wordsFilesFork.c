/*
Create child processes to count words in separate files
Write a C program that receives:
    a number N of files as command-line arguments

The program must:
    create N child processes using fork()
    each child process opens one file and counts how many words it contains
    each child prints its result
    the parent waits for all children to finish 

Example:

command:
./program file1.txt file2.txt file3.txt

file1.txt contains 10 words
file2.txt contains 18 words
file3.txt contains 7 words

possible output:

Child 1: 10 words
Child 2: 18 words
Child 3: 7 words

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Error, usage: %s <file1> <file2> <file3>\n", argv[0]);
        return 1;
    }

     pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        for (int i = 1; i < argc; i++) {
            int fd = open(argv[i], O_RDONLY);

            if (fd < 0) {
                fprintf(stderr, "Error opening file %s\n", argv[i]);
                return 1;
            }

            char buff[1024];
            ssize_t n;
            int words = 0;
            int in_word = 0;

            while ((n = read(fd, buff, sizeof(buff))) > 0) {
                for (ssize_t j = 0; j < n; j++) {
                    if (isspace((unsigned char)buff[j])) {
                        in_word = 0;
                        
                    } else if (!in_word) {
                        words++;
                        in_word = 1;
                    }
                }
            }

            if (n < 0) {
                fprintf(stderr, "Error reading file %s\n", argv[i]);
                close(fd);
                return 1;
            }

            if (close(fd) < 0) {
                fprintf(stderr, "Error closing file %s\n", argv[i]);
                return 1;
            }

            printf("%d", words);
            if (i < argc - 1) {
                printf(" ");
            }
        }

        printf("\n");
        return 0;
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        printf("Error waiting for child process\n");
        exit(1);
    }

    return 0;
}