#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/**
 * The operation must be selected using one of the following options:
    -t : Create files or modify timestamps (touch)
    -a : Display file contents (cat)
    -c : Copy files (cp)
    -m : Move or rename files (mv)
 */

#define ARGUMENTS "tacmC"    //the list of arguments accepted by the program

int exeTouch = 0;       // flag for the 'touch' option
int exeCat = 0;         // flag for the 'cat' option
int exeCopy = 0;        // flag for the 'copy' option
int exeMove = 0;        // flag for the 'move' option
int optNonExFile = 0;   // flaf for the 'touch -C ' (don't create non-existing files)

// touch option implemented
int touchFile(char *fileName){
    int fd; // fd = file descriptor for the opened file
    int openFlags;  // flags used to open the file

    /**
     * checked the global flag -C
     * if flag -C is set -> opens the existing files (read only)
     * else -> opens the existing and nonexisting files
     */
    if (optNonExFile) {
        openFlags = O_RDONLY;
    } else {
        openFlags = O_CREAT | O_RDONLY;
    }

    fd = open(fileName, openFlags, 0640);

    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", fileName, strerror(errno));
        return errno;
    }

    close(fd);
    return 0; 
}

/**
 * executes the touch function
 * it will apply all arguments strating from 
 *  OPTIND = first not processed argument by getopt
 */
void executeTouch(int argc, char **argv) {
    //other argments after the last one parsed by getopt 
    // -> starting from optind
    for (int index = optind; index < argc; index++) {
        touchFile(argv[index]);
    }
}

int catFile(char *fileName) {
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

void executeCat(int argc, char **argv) {
    for (int index = optind; index < argc; index++) {
        catFile(argv[index]);
    }
}

int executeCopy(char *sourceFile, char *destFile) {
    #define BUFF_SIZE 1024

    char buff[BUFF_SIZE];
    int fdSource;   // file descriptor 
    int fdDest;
    int readBytes;  // the number of read bytes

    fdSource = open(sourceFile, O_RDONLY);     // open the sorce file, in read-only mode

    if (fdSource < 0) {
        fprintf(stderr, "Can't open the source file %s: %s\n", sourceFile, strerror(errno));
        return errno;
    }

    fdDest = open(destFile, O_EXCL | O_CREAT | O_WRONLY, 0640);    // open the destination file (if it exists -> error) - write only mode

    if (fdDest < 0) {
        fprintf(stderr, "Can't open the destination file %s: %s\n", destFile, strerror(errno));
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
        fprintf(stderr, "Error, can't read from the source file %s: %s\n", sourceFile, strerror(errno));

        close(fdSource);
        close(fdDest);

        return errno;
    }
    
    close(fdSource);
    close(fdDest);

    return 0;
}

int deleteFile(char *fileName) {
    if (unlink(fileName) < 0) {      // the source file is being deleted 
        fprintf(stderr, "Failed to delete source file %s: %s\n", fileName, strerror(errno)); 
        return errno;
    }
    return 0;
}

int main(int argc, char **argv) {
    int opt;    //current option from getopt

    //arguments for touch/cat/cp/mv with -
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 't':
                exeTouch = 1;
                break;

            case 'C':
                optNonExFile = 1;
                break;

            case 'a':
                exeCat = 1;
                break;

            case 'c':
                exeCopy = 1;
                break;

            case 'm':
                exeMove = 1;
                break;

            case '?':
                fprintf(stderr, "Unknown option character `%c'.\n", optopt);
                break;

            default:
                abort();

        }
    }

    if (exeTouch) {
        executeTouch(argc, argv);
    }

    if (exeCat) {
        executeCat(argc, argv);
    }

    if (exeCopy) {
        if (argc == optind + 2) {
            executeCopy(argv[optind], argv[optind + 1]);
        } else {
            fprintf(stderr, "Not enough arguments\n");
        }
        
    }
  
    if (exeMove) {
        if (argc == optind + 2) {
            executeCopy(argv[optind], argv[optind + 1]);
            deleteFile(argv[optind]);
        } else {
            fprintf(stderr, "Not enough arguments\n");
        }
    }
    
       

}