/**
 *  Read bytes from source and write them to destination.
 *  Use a fixed-size buffer, like 256 or 1024 bytes.
 *  This is the core of cp and is simpler than directory copying.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error, not enough arguments!\n Usage: %s <src> <dest>\n", argv[0]);
        return 1;
    }

    int fdSrc, fdDest;
    fdSrc = open(argv[1], O_RDONLY);
    
    if (fdSrc < 0) {
        fprintf(stderr, "Can't open the source file %s\n", argv[1]);
        return 1;
    }

    fdDest = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);

    if (fdDest < 0) {
        close(fdSrc);

        fprintf(stderr, "Can't open the destination file %s\n", argv[2]);
        return 1;
    }

    ssize_t n;
    char buf[BUFF_SIZE];

    while ((n = read(fdSrc, buf, sizeof(buf))) > 0) {
        ssize_t written = 0;

        while (written < n) {
            ssize_t writeBytes = write(fdDest, buf + written, n - written);

            if (writeBytes < 0) {
                fprintf(stderr, "Error, couldn't write %d\n", errno);
                return -1;
            }
            written += writeBytes;
        }
    }

    if (n < 0) {
        close(fdDest);

        fprintf(stderr, "Couldn't read the file %s, error %d\n", argv[1], errno);
        return 1;
    }

    close(fdSrc);
    close(fdDest);

    return 0;
}