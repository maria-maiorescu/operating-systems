// Maria Maiorescu - IE - gr 3

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <limits.h>

/**
 * The operation must be selected using one of the following options:
    -t : Create files or modify timestamps (touch)
    -a : Display file contents (cat)
    -c : Copy files (cp)
    -m : Move or rename files (mv)
    -p : Preserve permissions and timestamps for copy/move
 */

#define ARGUMENTS "tacmCiNvpnbESs"    //the list of arguments accepted by the program

int exeTouch = 0;         // flag for the 'touch' option
int exeCat = 0;           // flag for the 'cat' option
int exeCopy = 0;          // flag for the 'copy' option
int exeMove = 0;          // flag for the 'move' option
int optNonExFile = 0;     // flag for the 'touch -C ' (don't create non-existing files)
int optConfOverwrite = 0; // flag to confirm before overwriting
int optNoOverwrite = 0;   // flag to not overwrite existing files
int optDisplayOp = 0;     // flag to display each copy/ move operation
int optPreserve = 0;      // flag to preserve permissions and timestamps
int optNrLines = 0;       // flag that numbers all lines
int optNonEmptyLines = 0; // flag that numbers non-empty lines only
int optEndLine = 0;       // flag that marks line endings with $
int optTabs = 0;          // flag that displays isplay tabs as ^I
int optSuppLines = 0;     // flag that suppresses consecutive empty lines into a single one

int invalidOpt() {
    // exactly one operation selector must be selected
    if ((exeTouch + exeCat + exeCopy + exeMove) != 1) {
        return 1;
    }

    // touch: only -C is valid in this program
    if (exeTouch) {
        return (optConfOverwrite || optNoOverwrite || optDisplayOp || optPreserve ||
                optNrLines || optNonEmptyLines || optEndLine || optTabs || optSuppLines);
    }

    // cat: -n and -b are mutually exclusive; other cat flags are allowed together
    if (exeCat) {
        if (optNrLines && optNonEmptyLines) {
            return 1;
        }
        if (optNonExFile || optConfOverwrite || optNoOverwrite || optDisplayOp || optPreserve) {
            return 1;
        }
        return 0;
    }

    // copy/move: -i and -N are mutually exclusive; other cp/mv flags are valid only here
    if (exeCopy || exeMove) {
        if (optConfOverwrite && optNoOverwrite) {
            return 1;
        }
        if (optNonExFile || optNrLines || optNonEmptyLines || optEndLine || optTabs || optSuppLines) {
            return 1;
        }
        return 0;
    }

    return 1;
}

void help() {
    printf("Usage: \n");
    printf("-t : Create files or modify timestamps (touch)\n");
    printf("\t -C : Do not create nonexistent files.\n");

    printf("-a : Display file contents (cat)\n");
    printf("\t -n : Number all lines.\n");
    printf("\t -b : Number non-empty lines only.\n");
    printf("\t -E : Mark line endings with $.\n");
    printf("\t -S : Display tabs as ^I.\n");
    printf("\t -s : Suppress consecutive empty lines into a single one.\n");

    printf("-c : Copy files (cp)\n");
    printf("\t -i : Confirm before overwriting.\n");
    printf("\t -N : Do not overwrite existing files.\n");
    printf("\t -v : Display each copy operation.\n");
    printf("\t -p : Preserve permissions and timestamps.\n");

    printf("-m : Move or rename files (mv)\n");
    printf("\t -i : Confirm before overwriting.\n");
    printf("\t -N : Do not overwrite existing files.\n");
    printf("\t -v : Display each move operation.\n");
}

// touch option implemented
int touchFile(char *fileName) {
    int fd;
    int openFlags;
    struct timespec times[2];

    if (optNonExFile) {
        fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            if (errno == ENOENT) {
                return 0;
            }
            fprintf(stderr, "Error opening %s: %s\n", fileName, strerror(errno));
            return errno;
        }
    } else {
        openFlags = O_CREAT | O_WRONLY;
        fd = open(fileName, openFlags, 0640);
        if (fd < 0) {
            fprintf(stderr, "Error opening %s: %s\n", fileName, strerror(errno));
            return errno;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &times[0]) != 0 || clock_gettime(CLOCK_REALTIME, &times[1]) != 0) {
        fprintf(stderr, "Error updating time for %s: %s\n", fileName, strerror(errno));
        close(fd);
        return errno;
    }

    if (utimensat(AT_FDCWD, fileName, times, 0) < 0) {
        fprintf(stderr, "Error updating timestamps for %s: %s\n", fileName, strerror(errno));
        close(fd);
        return errno;
    }

    close(fd);
    return 0;
}

/**
 * executes the touch function
 * it will apply all arguments starting from 
 *  OPTIND = first not processed argument by getopt
 */
void executeTouch(int argc, char **argv) {
    //other arguments after the last one parsed by getopt 
    // -> starting from optind
    for (int index = optind; index < argc; index++) {
        touchFile(argv[index]);
    }
}

// displays the content of a file
int catDisplayContents(char *fileName) {
    int fd;

    if (fileName == NULL) {
        fd = STDIN_FILENO;
    } else {
        // POSIX open(): open existing file for reading.
        fd = open(fileName, O_RDONLY);

        if (fd < 0) {
            fprintf(stderr, "Error, couldn't open %s: %s\n", fileName, strerror(errno));
            return 1;
        }
    }

    int n;  // number of read characters
    char buff[10];  // temporary buffer (temporary space for processing)
    int previousLineEmpty = 0;   // tracks if the previously finished line was empty
    int currentLineHasContent = 0; // tracks whether current line has non-newline content

    // POSIX read(): read bytes from fd into buffer; returns number of bytes read.
    while ((n = read(fd, buff, 10)) > 0) {      // reads 10 characters from file to buffer
        int n_write;    // nr of characters written on a line

        if (optEndLine || optTabs || optSuppLines) {
            for (int i = 0; i < n; i++) {   // searches for ne line characters 
                if (buff[i] == '\n') {
                    int isEmptyLine = !currentLineHasContent;  

                    if (optSuppLines && isEmptyLine && previousLineEmpty) {
                        currentLineHasContent = 0;
                        continue;   // suppress consecutive empty line
                    }

                    if (optEndLine) {
                        n_write = write(STDOUT_FILENO, "$\n", 2);    // writes a new line with the $ ending
                    } else {
                        n_write = write(STDOUT_FILENO, &buff[i], 1);
                    }

                    // prepares the flags for the next line
                    previousLineEmpty = isEmptyLine;
                    currentLineHasContent = 0;

                } else if (buff[i] == '\t' && optTabs) {    // check if tabs exist
                    n_write = write(STDOUT_FILENO, "^I", 2);
                    currentLineHasContent = 1;

                } else {
                    n_write = write(STDOUT_FILENO, &buff[i], 1);  // writes normal character

                    currentLineHasContent = 1;
                }
            }

        } else {
            // POSIX write(): write exactly n bytes to stdout
            n_write = write(STDOUT_FILENO, buff, n);    // writes n characters from buff to STDOUT (standard output = screen/ console)
        }
        
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
        int bytesRead = read(fd, &ch, 1); // read one character

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

int catPrintNumbered(char *fileName) {
    int fd;
    char lineBuffer[1024];
    int readStatus;
    int lineNumber = 0;
    int shouldPrintLine;

    if (fileName == NULL) {
        fd = STDIN_FILENO;
    } else {
        fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Error, couldn't open %s: %s\n", fileName, strerror(errno));
            return 1;
        }
    }

    while ((readStatus = readLine(fd, lineBuffer, sizeof(lineBuffer))) == 1) {
        int hasContent = 0;

        for (int index = 0; lineBuffer[index] != '\0'; index++) {
            if (lineBuffer[index] != '\n' && lineBuffer[index] != '\r') {
                hasContent = 1;
                break;
            }
        }

        shouldPrintLine = 1;
        if (optNonEmptyLines && !hasContent) {
            shouldPrintLine = 0;
        }

        if (shouldPrintLine) {
            lineNumber++;
            printf("%3d  ", lineNumber);
            printf("%s", lineBuffer);
        }
    }

    if (readStatus < 0) {
        fprintf(stderr, "Error reading %s: %s\n", fileName ? fileName : "STDIN", strerror(errno));
        if (fileName != NULL) {
            close(fd);
        }
        return 1;
    }

    if (fileName != NULL) {
        close(fd);
    }

    return 0;
}

int catFile(char *fileName) {
    if (optNrLines || optNonEmptyLines) {
        return catPrintNumbered(fileName);
    }
    return catDisplayContents(fileName);
}

void executeCat(int argc, char **argv) {
    if (optind == argc) {
        catFile(NULL);
    }
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

int executeCopyFile(char *sourceFile, char *destFile) {
    #define BUFF_SIZE 1024

    char buff[BUFF_SIZE];
    int fdSource;   // file descriptor 
    int fdDest;
    int readBytes;  // the number of read bytes
    int openFlags;  // flags used to open destination file
    struct stat sourceStat;

    // POSIX open(): open source file for reading.
    fdSource = open(sourceFile, O_RDONLY);     // open the source file, in read-only mode

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
    while ((readBytes = read(fdSource, buff, BUFF_SIZE)) > 0) {
        size_t totalWritten = 0;

        while (totalWritten < (size_t)readBytes) {
            ssize_t writeBytes = write(fdDest, buff + totalWritten, (size_t)readBytes - totalWritten);

            if (writeBytes < 0) {
                fprintf(stderr, "Error while writing to %s: %s\n", destFile, strerror(errno));
                close(fdSource);
                close(fdDest);
                return errno;
            }

            totalWritten += (size_t)writeBytes;
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

int dirExists(char *path) {
    struct stat st; // metadata for the current entry

    if (lstat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

int copyDir(char *sourceName, char *destName) {
    DIR *dir = opendir(sourceName);  

    if (dir == NULL) {
        fprintf(stderr, "Can't open the %s directory\n", sourceName);
        return errno;
    }

    if (!dirExists(destName)) {
        int fdDest = mkdir(destName, 0775);

        if (fdDest < 0) {
            fprintf(stderr, "Can't create the directory %s\n", destName);
            return errno;
        }
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPathSrc[PATH_MAX];
        char fullPathDst[PATH_MAX];

        snprintf(fullPathSrc, sizeof(fullPathSrc), "%s/%s", sourceName, entry->d_name);
        snprintf(fullPathDst, sizeof(fullPathDst), "%s/%s", destName, entry->d_name);

        struct stat st;

        if (lstat(fullPathSrc, &st) == 0) {
            // 6. processing
            if (S_ISDIR(st.st_mode)) {
                // ......
                
            } else if (S_ISREG(st.st_mode)) {
               executeCopyFile(fullPathSrc, fullPathDst);

            } else if (S_ISLNK(st.st_mode)) {
                // .....
            }
        } 


    }
    closedir(dir);
    return 0;
}

int executeCopyDirOrFile(char *sourceName, char *destName) {
    struct stat st; // metadata for the current entry

    if (lstat(sourceName, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            copyDir(sourceName, destName);
        
        } else if (S_ISREG(st.st_mode)) {
            executeCopyFile(sourceName, destName);
        
        } else if (S_ISLNK(st.st_mode)) {
            // .....
        }
    }
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
            
            case 'b':
                optNonEmptyLines = 1;
                break;

            case 'E':
                optEndLine = 1;
                break;
            
            case 'S':
                optTabs = 1;
                break;

            case 's':
                optSuppLines = 1;
                break;

            case 'm':
                exeMove = 1;
                break;
            
            case '?':
                fprintf(stderr, "Unknown option character `%c'.\n", optopt);
                help();
                break;

            default:
                help();
                abort();

        }
    }

    if (invalidOpt()) {
        fprintf(stderr, "Invalid arguments!\n");
        help();
        return 1;
    }

    if (exeTouch) {
        executeTouch(argc, argv);
    }

    if (exeCat) {
        executeCat(argc, argv);
    }

    if (exeCopy) {
        if (argc == optind + 2) {
            if (executeCopyDirOrFile(argv[optind], argv[optind + 1]) == 0) {
                if (optDisplayOp) {
                    printf("%s -> %s\n", argv[optind], argv[optind + 1]);
                }
            }
        } else {
            fprintf(stderr, "Not enough arguments\n");
            help();
        }
    }

    if (exeMove) {
        if (argc == optind + 2) {
            if (executeCopyFile(argv[optind], argv[optind + 1]) == 0) {
                if (deleteFile(argv[optind]) == 0) {
                    if (optDisplayOp) {
                        printf("%s -> %s\n", argv[optind], argv[optind + 1]);
                    }
                }
            }
        } else {
            fprintf(stderr, "Not enough arguments\n");
            help();
        }
    }
    
}