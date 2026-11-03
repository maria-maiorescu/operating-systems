/*
    Count lines ending with a given suffix.
    Write a C program that receives:
        a file path as the first command-line argument
        a suffix string as the second command-line argument

The program must:
    - open the file
    - read it line by line
    - count how many lines end with that suffix
    - print the result

Example:

file content:
    - hello.txt
    - world.txt
    - test.txt
    - file123

command:
    ./program data.txt .txt

output:
    Number of lines ending with ".txt": 3

*/

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main (int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, usage: %s file <suffix>\n", argv[0]);
        return 1;
    }

    int fd;
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open the file %s\n", argv[1]);
        return 1;
    }

    ssize_t n;
    char buff[BUFF_SIZE];

    int line_count = 0;
    char line[BUFF_SIZE];
    int lineLength = 0;

    while ((n = read(fd, buff, sizeof(buff))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (buff[i] == '\n') {
                line[lineLength] = '\0';

                // Check if the current line ends with the given suffix
                if (lineLength >= strlen(argv[2]) && strcmp(line + lineLength - strlen(argv[2]), argv[2]) == 0) {
                    line_count++;
                }
                lineLength = 0;

            } else {
                line[lineLength++] = buff[i];
            }
        }

    }

    if (lineLength > 0) {
        line[lineLength] = '\0';

        // Check if the last line (without a trailing newline) ends with the given suffix
        if (lineLength >= strlen(argv[2]) && strcmp(line + lineLength - strlen(argv[2]), argv[2]) == 0) {
            line_count++;
        }
    }

    printf("Number of lines ending with \"%s\": %d\n", argv[2], line_count);

    return 0;
}    