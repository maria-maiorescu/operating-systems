/**
 * Read an entire input file (first argument) and split content into:
 *  - letters  -> second argument file
 *  - digits   -> third argument file
 *  - other    -> fourth argument file
 *
 * Use lseek() to move through the input file.
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int openWrite(char *fileName) {
    int fd;
    fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        printf("Error, couldn't open the file %s", fileName);
    }

    return fd;
}

void tryClose(int fd) {
    if (fd > 0) {
        close(fd);
    }
}

int main(int argc, char **argv){
    int fd, lettersFile, digitsFile, othersFile;

    if (argc != 5) {
        printf("Error, usage %s input-file letter-file digit-file others-file\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Error, couldn't open the file %s", argv[1]);
    }
    
    lettersFile = openWrite(argv[2]);
    digitsFile = openWrite(argv[3]);
    othersFile = openWrite(argv[4]);

    if ((fd < 0) || (lettersFile < 0) || (digitsFile < 0) || (othersFile < 0)) {
        
        tryClose(fd);
        tryClose(lettersFile);
        tryClose(digitsFile);
        tryClose(othersFile);
        
        return 1;
    }

    char c;
    int bytesRead;

    // reads one character 
    while ((bytesRead = read(fd, &c, 1)) > 0) {
        int destinationFd = othersFile;

        if ((c >= 'a' && c<= 'z') || (c >= 'A' && c <= 'Z')) {
            destinationFd = lettersFile;
        }

        if (c >= '0' && c <= '9') {
            destinationFd = digitsFile;
        }

        int bytesWrite;
       bytesWrite = write(destinationFd, &c, 1);

       if (bytesWrite < 0) {
            printf("Couldn't write in the destination file %d", destinationFd);
       }

    }

    tryClose(fd);
    tryClose(lettersFile);
    tryClose(digitsFile);
    tryClose(othersFile);

    return 0;
}