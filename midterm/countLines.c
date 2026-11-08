/*
Write a C program called countLines that takes multiple file paths 
    as command-line arguments and counts the total number of lines in each file. 

For efficiency, create a child process for each file (using fork), 
    have each child count its file's lines and write the result to a temporary output file, 
    then have the parent process wait for all children and summarize the results.

Requirements:

- Read file paths from command-line arguments
- Use fork() to create a child process for each file

Each child process:
    - Opens its assigned file
    - Counts lines (by counting \n characters)
    - Writes results to a unique temp file (e.g., temp_PID.txt)
    - Exits with status 0 on success, 1 on error

Parent process:
    - Waits for all children using wait() or waitpid()
    - Reads all temp files and displays a summary (total lines per file, grand total)
    - Cleans up temp files
    - Handle errors gracefully (file not found, permission denied, etc.)

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error, usage: %s input file <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        char tempFile[PATH_MAX];
        snprintf(tempFile, sizeof(tempFile), "temp_%d.txt", getpid());
        

        int fdDest = open(tempFile, O_CREAT | O_WRONLY | O_CREAT, 0644);
        if (fdDest < 0) {
            fprintf(stderr, "Error, couldn't open the dest-file %s\n", tempFile);
            return 1;
        }

        for (int i = 1; i < argc; i++) {
            int fdSrc = open(argv[i], O_RDONLY);
            if (fdSrc < 0) {
                fprintf(stderr, "Error, couldn't open the source-file %s\n", argv[i]);
                return 1;
            }

            char buff[BUFF_SIZE];
            ssize_t bytesRead;
            int linesNr = 0;

            while ((bytesRead = read(fdSrc, buff, sizeof(buff))) > 0) {
                for (int i = 0; i < bytesRead; i++) {
                    if (buff[i] == '\n') {
                        linesNr++;
                    }
                }
            }
            int len;
            char msg[PATH_MAX + 10];
            len = snprintf(msg, sizeof(msg), "%s\t%d lines\n", argv[i], linesNr);
        
            write(fdDest, msg, len);
            close(fdSrc);
        }
        close(fdDest);
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        fprintf(stderr, "Error, waiting the child status\n");
        exit(1);
    }
    return 0;
}