/*
Write a C program named searchfile that searches a text file for a given word and supports the following options:

-f <file> : file to search
-w <word> : word to look for
-i : case-insensitive search
-n : show line numbers
-h : display help

Requirements:
    Use getopt() to parse command-line arguments.

    If the user gives an unknown option, print:
        "Error: unknown option -x"
            where x is the invalid option character.

    Use optopt to print the invalid option in the error message.
    If -f or -w is missing, print a usage message and exit.
    If the file cannot be opened, print an error and exit.
    If -i is used, ignore letter case while comparing.
    If -n is used, print the line number before each matching line.

Example:
    ./searchfile -f notes.txt -w hello -n
    ./searchfile -f notes.txt -w hello -x
    ./searchfile -f notes.txt -i -n

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int opt;
    char *file = NULL;
    char *word = NULL;
    int case_insensitive = 0;
    int line_numbers = 0;

    // f: and w: mean these options require an argument
    while((opt = getopt(argc, argv, "f:w:inh")) != -1) {
        switch(opt) {
            case 'f':
                file = optarg;
                break;

            case 'w':
                word = optarg;
                break;

            case 'i':
                case_insensitive = 1;
                break;

            case 'n':
                line_numbers = 1;
                break;

            case 'h':
                fprintf(stderr, "Usage: %s -f <file> -w <word> [-i] [-n]\n", argv[0]);
                return 0;

            case '?':
                fprintf(stderr, "Error: unknown option -%c\n", optopt);
                fprintf(stderr, "Usage: %s -f <file> -w <word> [-i] [-n]\n", argv[0]);
                return 1;
        }
    }

    if (file == NULL || word == NULL) {
        fprintf(stderr, "Usage: %s -f <file> -w <word> [-i] [-n]\n", argv[0]);
        return 1;
    }

    // Explicit file check required by the assignment (before launching grep)
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        perror(file);
        return 1;
    }
    close(fd);

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        // Build grep argv dynamically so -n and -i are applied only when requested
        char *args[8];
        int k = 0;

        args[k++] = "grep";
        if (line_numbers) {
            args[k++] = "-n";
        }

        if (case_insensitive) {
            args[k++] = "-i";
        }
        
        args[k++] = "-F"; // fixed-string match (no regex interpretation)
        args[k++] = word;
        args[k++] = file;
        args[k] = NULL; // execvp requires a NULL-terminated argv array
        
        execvp("grep", args);
        perror("grep");
        exit(1);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == -1) {
        fprintf(stderr, "Error while waiting for child process\n");
        exit(1);
    }

    return 0;
}