// Simple POSIX C example: write a file, then read and print it.
// Compile on Linux/macOS: gcc OS_System_Calls_Example.c -o os_example
// Run: ./os_example

#include <fcntl.h>   // open(), O_WRONLY, O_CREAT, O_TRUNC, O_RDONLY
#include <unistd.h>  // read(), write(), close()
#include <stdio.h>   // perror()

int main(void) {
    // 1. Open/create a file for writing and erase the old contents.
    // This is like creating a new text file for our example.
    int fd = open("demo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("open for writing");
        return 1;
    }

    // Message to write into the file.
    const char message[] = "Hello from Operating Systems!\n";

    if (write(fd, message, sizeof(message) - 1) == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    // Close the file after writing.
    close(fd);

    // 2. Reopen the same file for reading.
    fd = open("demo.txt", O_RDONLY);
    if (fd == -1) {
        perror("open for reading");
        return 1;
    }

    // Buffer where we store the text read from the file.
    char buffer[100];
    ssize_t n = read(fd, buffer, sizeof(buffer));

    if (n == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    // 3. Print only the bytes that were actually read.
    if (n > 0 && write(STDOUT_FILENO, buffer, (size_t)n) == -1) {
        perror("write to terminal");
        close(fd);
        return 1;
    }

    // Done: close the file descriptor before exiting.
    close(fd);
    return 0;
}
