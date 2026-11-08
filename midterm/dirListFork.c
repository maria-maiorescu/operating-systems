#include <dirent.h>   
#include <errno.h>    
#include <limits.h>   
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

int traverseDirectory (const char *dirPath) {
    DIR *dir;
    dir = opendir(dirPath);

    if (dir < 0) {
        fprintf(stderr, "Error, couldn't open the directory %s\n", dirPath);
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat st;
        if (lstat(fullPath, &st) < 0) {
            fprintf(stderr, "Error, couldn't get the status %s\n", fullPath);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            traverseDirectory(fullPath);
        }

        else if (S_ISREG(st.st_mode)) {
            printf("%s\t%lld\n", fullPath, st.st_size);
        }
    }

    if (closedir(dir) < 0) {
        fprintf(stderr, "Error, couldn't close the directory %s\n", dirPath);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error, usage: %s directory\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error while creating the process\n");
        exit(1);
    }

    if (pid == 0) {
        traverseDirectory(argv[1]);
        exit(0);
    }

    int status;
    pid_t w = waitpid(pid, &status, 0);

    if (w == 1) {
        fprintf(stderr, "Error while waiting for the child process\n");
        exit(1);
    }

    return 0;
}