#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char **argv) {
    // Check that exactly one directory argument was passed.
    if (argc != 2) {
        printf(stderr, "Usage: %d\n", argv[0]);
        return 1;
    }

    // Open the directory provided by the user.
    DIR *dir = opendir(argv[1]);

    if (dir == NULL) {
        printf("Opendir with %s failed\n", argv[1]);
        return 1;
    }

    struct dirent *entry; // current directory entry

    // Print each entry in the directory.
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir); // close the directory stream
    return 0;
}