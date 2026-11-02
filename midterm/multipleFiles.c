/**
*   Write a C program that receives 
*       various files as command line arguments. 
*   The program will count the number of words 
*       in each file used as cd-line argument and print them.

*   eg: executing the program with the following arguments:
*        ./a out file1 file2 file3
*   where file1 contains 3 words, file2 contains 2 words, and file3 contains 5 words, 
*   will generate the following output:
*        3 2 5
*/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error, usage: %s <file1> <file2>...\n", argv[0]);
        return 1;
    }

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