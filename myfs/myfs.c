#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * The operation must be selected using one of the following options:
    -t : Create files or modify timestamps (touch)
    -a : Display file contents (cat)
    -c : Copy files (cp)
    -m : Move or rename files (mv)
 */

#define ARGUMENTS "tacm"

int main(int argc, char **argv) {
    int opt;    //current option from getopt
    
    //arguments for touch/cat/cp/mv with -
    while ((opt = getopt(argc, argv, ARGUMENTS)) != -1) {
        switch (opt) {
            case 't':
                printf("touch\n");
                break;

            case 'a':
                printf("cat\n");
                break;

            case 'c':
                printf("copy\n");
                break;

            case 'm':
                printf("move\n");
                break;

            case '?':
                fprintf(stderr, "Unknown option character `%c'.\n", optopt);
                break;

            default:
                abort();

        }
    }

    //other argments after the last one parsed by getopt 
    // -> starting from optind
    
    for (int index = optind; index < argc; index++) {
        printf("Non-option argument %s\n", argv[index]);
    }
       

}