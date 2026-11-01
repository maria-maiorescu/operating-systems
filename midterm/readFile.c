/**
*   Open a file, call lseek(fd, n, SEEK_SET), then read a few bytes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, usage: %s file offset\n", argv[0]);
        return 1;
    }

    char *end = NULL;
    errno = 0;
    long offsetValue = strtol(argv[2], &end, 10);

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Can't open the file %s\n", argv[1]);
        return 1;
    }

    off_t off = lseek(fd, (off_t)offsetValue, SEEK_SET);
    if (off < 0) {
        fprintf(stderr, "Error, can't do seek\n");
        close(fd);
        return 1;
    }

    char buf[50];
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        fprintf(stderr, "Error, can't read the file\n");
        close(fd);
        return 1;
    }

    if (n > 0) {
        write(STDOUT_FILENO, buf, (size_t)n);
    }

    close(fd);
    return 0;
}