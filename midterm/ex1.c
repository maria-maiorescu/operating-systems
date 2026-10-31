#include <stdio.h>   // printf(), perror()
#include <fcntl.h>   // open(), O_RDONLY
#include <unistd.h>  // lseek(), read(), close()

int main() {
    // 1. Open the file for reading.
    int fd = open("notes.txt", O_RDONLY);

    if (fd == -1) {
        perror("Cannot open file");
        return 1;
    }

    // 2. Reserve space for 2 characters and a string-ending '\0'.
    char buffer[3];

    // 3. Move to position 3: the fourth character.
    if (lseek(fd, 3, SEEK_SET) == -1) {
        perror("Cannot change position");
        close(fd);
        return 1;
    }

    // 4. Read up to 2 bytes into the buffer.
    ssize_t n = read(fd, buffer, 2);

    if (n == -1) {
        perror("Cannot read file");
        close(fd);
        return 1;
    }

    // Turn the bytes read into a C string.
    buffer[n] = '\0';

    printf("Bytes read: %zd\n", n);
    printf("Characters: %s\n", buffer);

    // 5. Release the file descriptor.
    if (close(fd) == -1) {
        perror("Cannot close file");
        return 1;
    }

    return 0;  // Program completed successfully.
}