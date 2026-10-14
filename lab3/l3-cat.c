// simulates the cat (concatenate) function

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int showFile(char* fileName) {
    int fd;
    fd = open(fileName, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open %s: %s\n", fileName, strerror(errno));
        return 1;
    }
    printf("\nContents of file %s\n", fileName);

    int n;  // number of read characters
    char buff[10];  // temporary buffer (temporary space for processing)

    while ((n = read(fd, buff, 10)) > 0) {      // reads 10 characters from file to buffer
        int n_write = write(STDOUT_FILENO, buff, n);    // writes n charcters from buff to STDOUT (standard output = screen/ console)

        if (n_write < 0) {  // read from file, but couldn't write 
            fprintf(stderr, "Error writing to the screen: %s\n", strerror(errno));
            close(fd);
            return 4;
        }
    }
    if (n < 0) {    // couldn't read the file 
        fprintf(stderr, "Error reading %s: %s\n", fileName, strerror(errno));
        close(fd);
        return 3;
    }

   close(fd);

    return 0;
}

int main(int argc, char** argv) {    // argc = arguments count
    if (argc < 2) {     // 1 argument is by default = file's name
        printf("Usage: %s file1 file2...\n", argv[0]);  // argv[0] = file's name
        return 1;
    }

    int ret = 0; // errors counter
    for (int i = 1; i < argc; i++) {
        ret+=showFile(argv[i]);
    }

    return ret;
}
