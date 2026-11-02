#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int openWrite(char *fileName) {
    int fd;
    fd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open the file %s\n", fileName);
    }
    return fd;
}

void tryClose(int fd) {
    if (fd >= 0) {
        close(fd);
    }
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Error, usage %s <input-file> <letters-file> <digits-file> <others-file>\n", argv[0]);
        return 1;
    }

    int fd;
    fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, can't open the file %s\n", argv[1]);
        return 1;
    }

    int lettersFile, digitsFile, othersFile;
    lettersFile = openWrite(argv[2]);
    digitsFile = openWrite(argv[3]);
    othersFile = openWrite(argv[4]);

    if ((lettersFile < 0) || (digitsFile < 0) || (othersFile < 0)) {
        tryClose(fd);
        tryClose(lettersFile);
        tryClose(digitsFile);
        tryClose(othersFile);

        return 1;
    }

    char c;
    int bytesRead;

    while ((bytesRead = read(fd, &c, 1)) > 0) {
        int destFd = othersFile;

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            destFd = lettersFile;

        } else if (c >= '0' && c <= '9') {
            destFd = digitsFile;
        }

        int bytesWrite;
        bytesWrite = write(destFd, &c, 1);

        if (bytesWrite < 0) {
            fprintf(stderr, "Error, couldn't write in the destination file\n");
            
            tryClose(fd);
            tryClose(lettersFile);
            tryClose(digitsFile);
            tryClose(othersFile);

            return 1;
        }
    }

    tryClose(fd);
    tryClose(lettersFile);
    tryClose(digitsFile);
    tryClose(othersFile);

    return 0;
}