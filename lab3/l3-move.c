// simulates the copy (cp) function

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

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
        // it outputs on stadard error (conole like, but separate from the main console)
        fprintf(stderr, "Can't open the source file %s: %s\n", argv[1], strerror(errno));
        return errno;
    }

    fdDest = open(argv[2], O_EXCL | O_CREAT | O_WRONLY, 0640);    // open the destination file (if it exists -> error) - write only mode

    if (fdDest < 0) {
        fprintf(stderr, "Can't open the destination file %s: %s\n", argv[2], strerror(errno));
        close(fdSource);
        return errno;
    }

    while ((readBytes = read(fdSource, buff, BUFF_SIZE)) > 0) {   // reads from the file 
        int writeBytes = write(fdDest, buff, readBytes);    // write the buffer into the dest. file

        if (writeBytes != readBytes) {
            fprintf(stderr, "Error while writing %d bytes instead of %d: %s\n", writeBytes, readBytes, strerror(errno));

            close(fdSource);
            close(fdDest);

            return errno;
        }
    }

    if (readBytes < 0) {
        fprintf(stderr, "Error, can't read from the source file %s: %s\n", argv[1], strerror(errno));

        close(fdSource);
        close(fdDest);

        return errno;
    }
    
    close(fdSource);
    close(fdDest);

    if (unlink(argv[1]) < 0) {      // the source file is being deleted 
        fprintf(stderr, "Failed to delete source file %s: %s\n", argv[1], strerror(errno)); 
        return errno;
    }

    return 0;
}