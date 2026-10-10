#include <dirent.h>   // opendir, readdir, closedir
#include <errno.h>    // errno
#include <limits.h>   // PATH_MAX
#include <stdio.h>    // printf, fprintf, perror
#include <stdlib.h>   // exit codes
#include <string.h>   // strcmp, snprintf
#include <sys/stat.h> // lstat, struct stat, S_ISDIR, S_ISREG

/* Walk through dir_path recursively and print size for each regular file. */
void traverse_directory(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Cannot open directory '%s': ", dir_path);
        perror("");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;
        char full_path[PATH_MAX];
        int written;

        /* Skip current and parent directory entries to avoid infinite loops. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        /* Build full path: "<dir_path>/<entry_name>" */
        written = snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        if (written < 0 || written >= (int)sizeof(full_path)) {
            fprintf(stderr, "Path too long, skipping: %s/%s\n", dir_path, entry->d_name);
            continue;
        }

        /* lstat reads metadata (type, size, etc.) for this path. */
        if (lstat(full_path, &st) == -1) {
            fprintf(stderr, "Cannot stat '%s': ", full_path);
            perror("");
            continue;
        }

        /* If this is a regular file, print its size in bytes. */
        if (S_ISREG(st.st_mode)) {
            printf("%s -> %lld bytes\n", full_path, (long long)st.st_size);
        }
        /* If this is a directory, recurse into it. */
        else if (S_ISDIR(st.st_mode)) {
            traverse_directory(full_path);
        }
    } // while

    if (closedir(dir) == -1) {
        fprintf(stderr, "Cannot close directory '%s': ", dir_path);
        perror("");
    }
}

int main(int argc, char *argv[])
{
    struct stat st;

    /* Expect exactly one argument: the directory to scan. */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    /* Validate that the provided path exists and can be inspected. */
    if (lstat(argv[1], &st) == -1) {
        fprintf(stderr, "Cannot access '%s': ", argv[1]);
        perror("");
        return 1;
    }

    /* Stop early if the argument is not a directory. */
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", argv[1]);
        return 1;
    }

    /* Start recursive traversal from the user-provided root directory. */
    traverse_directory(argv[1]);
    return 0;
}
