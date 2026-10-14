// simulates the touch command

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fd; // fd = file descriptor for the opened file

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]); //not enough arguments
        return 1;
    }

    fd = open(argv[1], O_CREAT | O_RDONLY, 0640);

    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", argv[1], strerror(errno));
        return errno;
    }

    close(fd);

    return 0;
}

