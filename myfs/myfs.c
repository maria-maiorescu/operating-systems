#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>

/**
 * The operation must be selected using one of the following options:
    -t : Create files or modify timestamps (touch)
    -a : Display file contents (cat)
    -c : Copy files (cp)
    -m : Move or rename files (mv)
    -p : Preserve permissions and timestamps for copy/move
 */

#define ARGUMENTS "tacmCiNvpn"    //the list of arguments accepted by the program

int exeTouch = 0;         // flag for the 'touch' option
int exeCat = 0;           // flag for the 'cat' option
int exeCopy = 0;          // flag for the 'copy' option
int exeMove = 0;          // flag for the 'move' option
int optNonExFile = 0;     // flag for the 'touch -C ' (don't create non-existing files)
int optConfOverwrite = 0; // flag to confirm before overwriting
int optNoOverwrite = 0;   // flag to not overwrite exisiting files
int optDisplayOp = 0;     // flag to display each copy/ move operation
int optPreserve = 0;      // flag to preserve permissions and timestamps
int optNrLines = 0;       // flag that numbers all lines

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

    // POSIX open(): create/open file and return a file descriptor.
    fd = open(fileName, openFlags, 0640);

    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", fileName, strerror(errno));
        return errno;
    }

    // POSIX close(): release the file descriptor.
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

// displays the content of a file
int catDisplayContents(char *fileName) {
    int fd;
    // POSIX open(): open existing file for reading.
    fd = open(fileName, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open %s: %s\n", fileName, strerror(errno));
        return 1;
    }
    printf("\nContents of file %s\n", fileName);

    int n;  // number of read characters
    char buff[10];  // temporary buffer (temporary space for processing)

    // POSIX read(): read bytes from fd into buffer; returns number of bytes read.
    while ((n = read(fd, buff, 10)) > 0) {      // reads 10 characters from file to buffer
        // POSIX write(): write exactly n bytes to stdout.
        int n_write = write(STDOUT_FILENO, buff, n);    // writes n charcters from buff to STDOUT (standard output = screen/ console)

        if (n_write < 0) {  // read from file, but couldn't write 
            fprintf(stderr, "Error writing to the screen: %s\n", strerror(errno));
            // POSIX close(): close input file descriptor.
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

int readLine(int fd, char *lineBuffer, int bufferSize) {
    int length = 0;

    // Reads one line (or part of it if it is longer than the buffer).
    // Return values:
    //  1  => at least one byte was read (line available in lineBuffer)
    //  0  => EOF (end of file) and no bytes were read
    // -1  => read error (errno is set by read)

    while (1) {
        char ch;    //one character that will be read
        int bytesRead = read(fd, &ch, 1); // read one charcter

        if (bytesRead < 0) {
            return -1;
        }

        if (bytesRead == 0) {
            if (length == 0) {
                return 0;
            }
            break;
        }

        if (length + 1 < bufferSize) {
            lineBuffer[length++] = ch;  // saves the read char. in buffer 
        }

        if (ch == '\n') {
            break;  //stops when a new line is found
        }
    }

    lineBuffer[length] = '\0';  // null terminates the buffer
    return 1;
}

int catCountLines(char *fileName) {
    int fd;
    int lineCount = 0;
    char lineBuffer[1024];  // buffer to hold the lines
    int readStatus;

    // POSIX open(): open existing file for reading.
    fd = open(fileName, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "Error, couldn't open %s: %s\n", fileName, strerror(errno));
        return 1;
    }

    // Repeatedly read lines with a fixed-size stack buffer and count them.
    while ((readStatus = readLine(fd, lineBuffer, sizeof(lineBuffer))) == 1) {
        lineCount++;
    }

    if (readStatus < 0) {
        fprintf(stderr, "Error reading lines from %s: %s\n", fileName, strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);
    printf("Number of lines in %s: %d\n", fileName, lineCount);
    return 0;
}

int catFile(char *fileName) {
    if (optNrLines) {
        return catCountLines(fileName);
    }
    return catDisplayContents(fileName);
}

void executeCat(int argc, char **argv) {
    for (int index = optind; index < argc; index++) {
        catFile(argv[index]);
    }
}

int doNotOverwrite(char *fileName) {
    printf("Do you want to overwrite the file %s? (y/n): ", fileName);
    fflush(stdout);

    char answer[32];

    while (fgets(answer, sizeof(answer), stdin) != NULL) {
        char option = answer[0];

        if (option == 'y') {
            return 0;   // overwrite accepted
        }
        if (option == 'n') {
            return 1;   // do not overwrite
        }

        printf("Invalid answer. Please type y or n: ");
        fflush(stdout);
    }

    // if input stream closes, default to "do not overwrite"
    return 1;
}

int executeCopy(char *sourceFile, char *destFile) {
    #define BUFF_SIZE 1024

    char buff[BUFF_SIZE];
    int fdSource;   // file descriptor 
    int fdDest;
    int readBytes;  // the number of read bytes
    int openFlags;  // flags used to open destination file
    struct stat sourceStat;

    // POSIX open(): open source file for reading.
    fdSource = open(sourceFile, O_RDONLY);     // open the sorce file, in read-only mode

    if (fdSource < 0) {
        fprintf(stderr, "Can't open the source file %s: %s\n", sourceFile, strerror(errno));
        return errno;
    }

    // POSIX fstat(): fetch source metadata (mode + timestamps) from the open descriptor.
    if (optPreserve && fstat(fdSource, &sourceStat) < 0) {
        fprintf(stderr, "Can't read metadata from source file %s: %s\n", sourceFile, strerror(errno));
        close(fdSource);
        return errno;
    }

    if (optConfOverwrite) {
        // checks if the file exists
        // POSIX access(F_OK): check destination path existence.
        if (access(destFile, F_OK) == 0) { 
            //file exists
            if (doNotOverwrite(destFile)) {
                // the user doesn't accept the overwrite
                close(fdSource);
                return 1;
            }
        }
    }

    openFlags = O_CREAT | O_WRONLY;

    if (optNoOverwrite) {
        openFlags |= O_EXCL;    // adds the exclusive mode -> throws an error if the file exists
    }

    // open the destination file (if it exists -> overwrite) 
    // POSIX open(): create/open destination file for writing.
    fdDest = open(destFile, openFlags, 0640);  

    if (fdDest < 0) {
        fprintf(stderr, "Can't open the destination file %s: %s\n", destFile, strerror(errno));
        close(fdSource);
        return errno;
    }

    // POSIX read()/write(): stream file contents from source fd to destination fd.
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

    if (optPreserve) {
        struct timeval times[2];

        // POSIX fchmod(): apply source permission bits to destination file.
        if (fchmod(fdDest, sourceStat.st_mode & 07777) < 0) {
            fprintf(stderr, "Can't preserve permissions on %s: %s\n", destFile, strerror(errno));
            close(fdSource);
            close(fdDest);
            return errno;
        }

        TIMESPEC_TO_TIMEVAL(&times[0], &sourceStat.st_atimespec);
        TIMESPEC_TO_TIMEVAL(&times[1], &sourceStat.st_mtimespec);

        // POSIX futimes(): copy access/modify timestamps to destination file.
        if (futimes(fdDest, times) < 0) {
            fprintf(stderr, "Can't preserve timestamps on %s: %s\n", destFile, strerror(errno));
            // POSIX close(): release both descriptors after copy completes.
            close(fdSource);
            close(fdDest);
            return errno;
        }
    }
    
    close(fdSource);
    close(fdDest);

    return 0;
}

int deleteFile(char *fileName) {
    // POSIX unlink(): remove directory entry for source file.
    if (unlink(fileName) < 0) {      // the source file is being deleted 
        fprintf(stderr, "Failed to delete source file %s: %s\n", fileName, strerror(errno)); 
        return errno;
    }
    return 0;
}

int main(int argc, char **argv) {
    int opt;    //current option from getopt

    //arguments for touch/cat/cp/mv with -
    // POSIX getopt(): parse short options from command-line arguments.
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

            case 'i':
                optConfOverwrite = 1;
                break;

            case 'N':
                optNoOverwrite = 1;
                break;

            case 'v':
                optDisplayOp = 1;
                break;

            case 'p':
                optPreserve = 1;
                break;

            case 'n':
                optNrLines = 1;
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
            if (executeCopy(argv[optind], argv[optind + 1]) == 0) {
                if (optDisplayOp) {
                    printf("Copy successful %s -> %s\n", argv[optind], argv[optind +1]);
                }
            }
        } else {
            fprintf(stderr, "Not enough arguments\n");
        }
        
    }
  
    if (exeMove) {
        if (argc == optind + 2) {
            if (executeCopy(argv[optind], argv[optind + 1]) == 0) {
                if (deleteFile(argv[optind]) == 0) {
                    if (optDisplayOp) {
                        printf("Move successful %s -> %s\n", argv[optind], argv[optind +1]);
                    }
                }                
            }  
        } else {
            fprintf(stderr, "Not enough arguments\n");
        }
    }
    
}