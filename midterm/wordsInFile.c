/**
 * Write a C program to print the number of lines of a file 
 * (received as a second cmd line argument) which contains a word given as 
 * the first cmd line execution argument 
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, usage: %s file word\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    ssize_t n;
    char buff[BUFF_SIZE];

    int line_count = 0;
    char line[BUFF_SIZE];
    int lineLength = 0;

    char *word = argv[2];

    while ((n = read(fd, buff, sizeof(buff))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            char c = buff[i];

            if (c == '\n') {
                /* End of one line: close string and check if word appears */
                line[lineLength] = '\0';

                if (strstr(line, word) != NULL) {
                    line_count++;
                }

                lineLength = 0;
                continue;
            }

            /* Keep building current line */
            if (lineLength < BUFF_SIZE - 1) {
                line[lineLength++] = c;
            }
        }
    }
    if (n == -1) {
        fprintf(stderr, "Error reading file\n");
        close(fd);
        return 1;
    }

    /* If file does not end with '\n', check the last partial line too */
    if (lineLength > 0) {
        line[lineLength] = '\0';
        
        if (strstr(line, word) != NULL) {
            line_count++;
        }
    }

    printf("Number of lines containing \"%s\": %d\n", word, line_count);
    close(fd);
    return 0;
}
