/**
 * Write a C program to print the number of lines of a file 
 * (received as a second cmd line argument) which contains a word given as 
 * the first cmd line execution argument 
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    //char command[50]; 
    //sprintf(command, "grep -F %s %s | wc -l", argv[1], argv[2]);
    //execlp("bash", "bash", "-c", command, NULL);

    execlp("grep", "grep", "-F", "-c", argv[1], argv[2], NULL);

    return 0;
}