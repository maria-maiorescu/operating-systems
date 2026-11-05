/*
Write a C program named mycopy that copies one file to another and supports these options:

-s <source> : source file
-d <dest> : destination file
-v : verbose mode
-o <offset> : copy starting from a byte position
-h : display help

Requirements:

    Use getopt() to parse the arguments.
    If an unsupported option is entered, print:
    "Unknown option: -x" --> using optopt.

    If -s or -d is missing, print usage and exit.
    If -o is given, copy only the specified range of bytes from the source file.
    If -v is enabled, print progress information such as:
        "Copying source.txt to dest.txt"
        "Copied 1024 bytes"
    If the source file does not exist, print an error and exit.

Example:

./mycopy -s input.txt -d output.txt -v
./mycopy -s input.txt -d output.txt -o 100 -b 500
./mycopy -s input.txt -d output.txt -z
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv) {
    int opt;
    char *srcFile = NULL;
    char *destFile = NULL;
    int verbose = 0;
    int offset = 0;

    while((opt = getopt(argc, argv, "s:d:vo:b:h")) != -1) {
        switch(opt) {
            case 's':
                srcFile = optarg;
                break;

            case 'd':
                destFile = optarg;
                break;
            
            case 'v':
                verbose = 1;
                break;

            case 'o':
                offset = atoi(optarg);
                break;
                
            case 'h':
                fprintf(stderr, "Usage: %s -s <source> -d <dest> [-v] [-o <offset>] [-b <bytes>] [-h]\n", argv[0]);
                return 0;
            
            case '?':
                fprintf(stderr, "Error: unknown option -%c\n", optopt);
                fprintf(stderr, "Usage: %s -s <source> -d <dest> [-v] [-o <offset>] [-b <bytes>] [-h]\n", argv[0]);
                return 1;
        }
    }

    int srcFd, destFd;

    srcFd = open(srcFile, O_RDONLY);
    if (srcFd < 0) {
        fprintf(stderr, "Error, couldn't open the source file %s\n", srcFile);
        return 1;
    }
    
    destFd = open(destFile, O_WRONLY | O_CREAT| O_TRUNC, 0644);
    if (destFd < 0) {
        close(srcFd);
        fprintf(stderr, "Error, couldn't open the source file %s\n", destFile);
        return 1;
    }

    if (offset > 0) {
        if (verbose) {
            printf("Seeking to %d position\n", offset);
        }

        if ((lseek(srcFd, offset, SEEK_SET)) < 0) {
            printf("Couldn't seek\n");
        }
    }
    
    ssize_t n;
    char buff[BUFF_SIZE];

    while ((n = read(srcFd, buff, sizeof(buff))) > 0) {
        ssize_t written = 0;

        while (written < n) {
            ssize_t writeBytes = write(destFd, buff + written, n - written);

            if (writeBytes == -1) {
                fprintf(stderr, "Error, couldn't write in the dest-file %d\n", destFd);
                close(srcFd);    
                return 1;
            }
            written += writeBytes;
        }
    }

    if (n == -1) {
        fprintf(stderr, "Error, couldn't read from the source-file %d\n", srcFd);
        close(destFd);
        return 1;
    }

    close(srcFd);
    close(destFd);

    if (verbose) {
        printf("Copy successfully from %s to %s \n", srcFile, destFile);
    }

    return 0;
}