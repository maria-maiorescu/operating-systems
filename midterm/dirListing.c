/*
Write a C program that takes a directory path as input 
    and prints all entries inside that directory (files and subdirectories), one per line.

Requirements:

    - The program should accept the directory path as a command-line argument.
    - It should open the directory and read all its entries.
    - Ignore the special entries "." and "..".
    - For each entry, print:
        - the name 
        - whether it is a file or a directory
    - If the given path is not a valid directory, print an error message.

Example:
    Input:
    ./data

Output:
    file1.txt FILE
    subdir DIRECTORY
    notes.txt FILE

*/

#include <dirent.h>   
#include <errno.h>    
#include <limits.h>   
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   
#include <sys/stat.h>

int traverse_directory(const char *dir_path) {
    DIR *dir;
    dir = opendir(dir_path);

    if (dir < 0) {
        fprintf(stderr, "Error, couldn't open the directory %s\n", dir_path);
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (lstat(fullPath, &st) == -1) {
            fprintf(stderr, "Can't stat '%s': ", fullPath);
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            printf("%s -> %lld bytes\n", fullPath, (long long)st.st_size);
        } else if (S_ISDIR(st.st_mode)) {
            traverse_directory(fullPath);
        }
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "Cannot close directory '%s': ", dir_path);
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error, usage: %s directory\n", argv[0]);
        return 1;
    }

   return traverse_directory(argv[1]);
}