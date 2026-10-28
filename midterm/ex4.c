/**
 * Write a C program using open(), read(), write(), and close() that reads an entire input file (first argument) and splits its content into:
    - Uppercase letters → second argument file
    - Lowercase letters → third argument file
    - Other characters (digits, spaces, symbols) → fourth argument file
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int openWrite(char *fileName) {
    int fd;
    fd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd < 0) {
        printf("Error, couldn't open the file %s ", fileName);
    }

    return fd;
}

void tryClose(int fd) {
    if (fd > 0) {
        close(fd);
    }
}

int main(int argc, char **argv) {
    int fd, upperLettersFile, lowerLettersFile, othersFile;

    if (argc != 5) {
        printf("Error, usage %s input-file upper-letters-file lower-letters-file others-file\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Error, couldn't read the file %s", argv[1]);
    }

    upperLettersFile = openWrite(argv[2]);
    lowerLettersFile = openWrite(argv[3]);
    othersFile = openWrite(argv[4]);

    if ((fd < 0) || (upperLettersFile < 0) || (lowerLettersFile < 0) || (othersFile < 0)) {
        tryClose(fd);
        tryClose(upperLettersFile);
        tryClose(lowerLettersFile);
        tryClose(othersFile);
        return 1;
    }

    char c;
    int bytesRead;

    while ((bytesRead = read(fd, &c, 1)) > 0) {
        if (c >= 'A' && c <= 'Z') {
            write(upperLettersFile, &c, 1);

        } else if (c >= 'a' && c <= 'z') {
            write(lowerLettersFile, &c, 1);
            
        } else {
            write(othersFile, &c, 1);
        }
    }

    tryClose(fd);
    tryClose(upperLettersFile);
    tryClose(lowerLettersFile);
    tryClose(othersFile);

    return 0;
}