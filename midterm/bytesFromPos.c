/*
    Read bytes from a position and print only the last part
    Write a C program that receives:
       - a file path as the first command-line argument
       - a starting offset as the second argument
       - a length as the third argument

    The program must:
       - open the file
       - move to the given offset using lseek()
       - read exactly the requested number of bytes
       - print them to standard output

Example:

Command:
./program file.txt 10 25

Output:
prints the 25 bytes starting from byte 10
*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Error, usage: %s file <starting-offset> <bytes-read>\n", argv[0]);
        return 1;
    }

    int fd;
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open the file %s\n", argv[1]);
        return 1;
    }

    char buff[26];

    if (lseek(fd, 10, SEEK_SET) == -1) {
        printf("Error, couldn't change the position\n");
        return 1;
    }

    ssize_t n = read(fd, buff, 25);
    if (n == -1) {
        fprintf(stderr, "Error, couldn't read from the file %s", argv[1]);
        close(fd);
        return 1;
    }

    buff[n] = '\0';
    printf("Characters: %s\n", buff);

    return 0;
}