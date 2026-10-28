#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(void) {
    int fd;                 // file descriptor returned by open()
    char buffer[80];        // buffer used to read data back
    char msg[] = "Hello world!";  // message to write to the file

    fd = open("check.txt", O_RDWR | O_CREAT, 0666); // open/create file for read/write
    printf("File descriptor = %d\n", fd);

    if (fd != -1) {
        printf("\ncheck.txt opened with read-write access\n");

        write(fd, msg, strlen(msg));  // write the message to the file
        lseek(fd, 0, SEEK_SET);       // move file pointer back to start
        read(fd, buffer, sizeof(buffer) - 1); // read the file content back
        buffer[sizeof(buffer) - 1] = '\0';

        printf("\n%s was written to my file\n", buffer);
        close(fd); // close the file
    }

    return 0;
}