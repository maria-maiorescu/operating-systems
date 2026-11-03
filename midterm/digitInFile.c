/**
 *  Count lines containing a digit. Write a C program that receives:
 *      - a file path as the first command-line argument
 *      - a digit character (for example: 0, 1, 2, ..., 9) as the second command-line argument
 *  The program must read the file line by line and print how many lines contain that digit at least once.
 *
 * Example:
 *      ./program file.txt 9 ---> Number of lines containing digit 9: 1
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, usage: %s file <digit>\n", argv[0]);
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
    
    char *digit = argv[2];

    while ((n = read(fd, buff, sizeof(buff))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            char c = buff[i];

            if (c == '\n') {
                line[lineLength] = '\0';

                if (strchr(line, digit[0]) != NULL) {
                    line_count++;
                }

                lineLength = 0;
                continue;
            }

            if (lineLength < BUFF_SIZE - 1) {
                line[lineLength++] = c;
            }
        }
    }

    if (n == -1) {
        fprintf(stderr, "Error while reading the file\n");
        close(fd);
        return 1;
    }

    if (lineLength > 0) {
        line[lineLength] = '\0';

        if (strchr(line, digit[0]) != NULL) {
            line_count++;
        }
    }

    printf("Number of lines containing \"%s\": %d\n", digit, line_count);
    close(fd);

    return 0;
}
