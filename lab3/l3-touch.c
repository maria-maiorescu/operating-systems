// simulates the touch command

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd; // fd = file descriptor for the opened file

    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]); //not enough arguments
        return 1;
    }

    fd = open(argv[1], O_CREAT | O_RDONLY, 0640);

    if (fd < 0) {
        printf("Error opening the %s", argv[0]);
        return 2;
    }

    close(fd);

    return 0;
}

