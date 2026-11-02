/**
*   Create a C program that applies Caesar's cypher on each runtime argument character
*       and prints the result. Use a subprocess for each given argument. 
*    
*   The Caesar cypher is computed by shifting each character by 13 positions 
*       within the 7-bit ASCli code. After every non-last runtime argument there will be a dash symbol printed.
*
*   Eg:
*   ./a.out abcd AbCd abtd3 q-t3
*   --> will generate the output:
*   nopq-NoPq-no\x01q@-~:@
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <strings>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        /* Process one argument character by character. */
        for (int j = 0; arg[j] != '\0'; j++) {
            char c = arg[j];

            /* Shift by 13 in 7-bit ASCII (values 0..127), then wrap with & 0x7F. */
            c = (c + 13) & 0x7F;

            /* Use write to print one encrypted character. */
            if (write(STDOUT_FILENO, &c, 1) < 0) {
                fprintf(stderr, "Error writing to stdout\n");
                return 1;
            }
        }

        /* Print '-' only between arguments, not after the last one. */
        if (i < argc - 1) {
            char dash = '-';

            if (write(STDOUT_FILENO, &dash, 1) < 0) {
                fprintf(stderr, "Error writing to stdout\n");
                return 1;
            }
        }
    }
    return 0;
}