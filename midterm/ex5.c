#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd;                   // file descriptor for the opened file
    char a[100];              // buffer used to read file contents
    char message[] = "Hello World!"; // data to write to the file

    // Open the file for reading and writing.
    fd = open("test5.txt", O_RDWR);
    printf("File descriptor value: %d\n", fd);

    if (fd != -1) {
        printf("write starts here: ...\n");
        write(fd, message, sizeof(message)); // write the string to the file
        printf("write ends here: ...\n");
        close(fd); // close the file after writing

        // Reopen the file in read-only mode and read it back.
        fd = open("test5.txt", O_RDONLY);
        read(fd, a, sizeof(a));
        printf("Data read: %s\n", a);
        close(fd); // close the file after reading
    }

    return 0;
}