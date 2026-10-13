// simulates the copy (cp) function

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/** 
    argv[0] = file
    argv[1] = source
    argv[2] = destination
    => argc = 3
*/ 

#define BUFF_SIZE 1024    // the buffer's size

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s source destination\n", argv[0]);
        return 1;
    }
    
    char buff[BUFF_SIZE];
    int fdSource;   // file descriptor 
    int fdDest;
    int readBytes;  // the number of read bytes

    fdSource = open(argv[1], O_RDONLY);     // open the sorce file, in read-only mode

    if (fdSource < 0) {
        printf("Can't open the source file: %s\n", argv[1]);
        return 2;
    }

    fdDest = open(argv[2], O_EXCL | O_CREAT | O_WRONLY, 0640);    // open the destination file (if it exists -> error) - write only mode

    if (fdDest < 0) {
        printf("Can't open the destination file: %s\n", argv[2]);
        close(fdSource);
        return 3;
    }

    while ((readBytes = read(fdSource, buff, BUFF_SIZE)) > 0) {   // reads from the file 
        int writeBytes = write(fdDest, buff, readBytes);    // write the buffer into the dest. file

        if (writeBytes != readBytes) {
            printf("Error while writing %d bytes instead of %d\n", writeBytes, readBytes);

            close(fdSource);
            close(fdDest);

            return 4;
        }
    }

    if (readBytes < 0) {
        printf("Error, can't read from the source file %s\n", argv[1]);

        close(fdSource);
        close(fdDest);

        return 5;
    }
    
    close(fdSource);
    close(fdDest);

    if (unlink(argv[1]) < 0) {      // the source file is being deleted 
        printf("Failed to delete source file %s\n", argv[1]);
        return 6;
    }

    return 0;
}