/*
Write a C program called fileStats that displays statistics about a text file. 
Use getopt() to handle command-line options for which statistics to display.

Requirements:

Accept a file path as the last argument (positional)
Support the following options:
    -l : Display number of lines
    -w : Display number of words
    -c : Display number of characters
    -h : Display help message and exit

If no options are provided, display all stats (default behavior)
Use getopt() to parse options
Handle errors gracefully (file not found, permission denied, invalid options)
Report invalid options with an appropriate error message
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv) {
    int opt;
    int linesNr = 0;
    int wordsNr = 0;
    int charNr = 0;

    while ((opt = getopt(argc, argv, "lwch")) != -1) {
        switch(opt) {
            case 'l':
                linesNr = 1;
                break;

            case 'w':
                wordsNr = 1;
                break;

            case 'c':
                charNr = 1;
                break;

            case 'h':
                fprintf(stderr, "Usage: %s [-l -w -c -h] file\n -l: <lines-number> -w: <words-number> -c: <characters-number>\n", argv[0]);
                return 0;

            case '?':
                fprintf(stderr, "Error: unknown option -%c\n", optopt);
                fprintf(stderr, "Usage: %s [-l -w -c -h] file\n -l: <lines-number> -w: <words-number> -c: <characters-number>\n", argv[0]);
                return 1;

        }
    }

    if (optind < argc - 1) {
        fprintf(stderr, "Error, not enough arguments\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf (stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        char *arg[10];
        int k = 0;

        arg[k++] = "wc";
        if (linesNr) {
            arg[k++] = "-l";
        }

        if (wordsNr) {
            arg[k++] = "-w";
        }

        if (charNr) {
            arg[k++] = "-c";
        }

        arg[k++] = argv[optind];
        arg[k] = NULL;

        execvp("wc", arg);
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        fprintf(stderr, "Error while waiting for child process\n");
        exit(1);
    }

    return 0;
}