/**
 *  Write a program that reads one file and prints its content to stdout.
 *  Use open, read, write, close.
 */


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error, usage: %s\n", argv[0]);
        return 1;
    }

    int fd;
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Can't open the file %s\n", argv[1]);
        return 1;
    }

    ssize_t n;
    char buffer[1024];
    
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        ssize_t written = 0;

        while (written < n) {
            ssize_t writeBytes = write(STDOUT_FILENO, buffer + written, n - written);

            if (writeBytes < 0) {
                fprintf(stderr, "Can't write %d\n", errno);
                close (fd);
                return -1;
            }
            written += writeBytes;
        }
    }

    if (n < 0) {
        fprintf(stderr, "Couldn't read the file %s, error %d\n", argv[1], errno);
        return 1;
    }

    close(fd);

    return 0;
}