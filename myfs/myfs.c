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

#define ARGUMENTS "tacm"    //the list of arguments accepted by the program

int touchFile(char *fileName){
    int fd; // fd = file descriptor for the opened file

    fd = open(fileName, O_CREAT | O_RDONLY, 0640);

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
    for (int index = optind; index < argc; index++) {
        touchFile(argv[index]);
    }
}

int main(int argc, char **argv) {
    int opt;    //current option from getopt

    int exeTouch = 0;   // flag for the 'touch' option
    int exeCat = 0;     // flag for the 'cat' option
    int exeCopy = 0;    // flag for the 'copy' option
    int exeMove = 0;    // flag for the 'move' option
    
    //arguments for touch/cat/cp/mv with -
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 't':
                exeTouch = 1;
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

    //other argments after the last one parsed by getopt 
    // -> starting from optind
    
    
       

}