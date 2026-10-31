#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

// Recursively scan a directory tree and print what kind of entry it is:
// directory, regular file, or symbolic link.
void list_dir(const char *path) {
    // 1. Open the current directory.
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return; // skip unreadable directories
    }

    // 2. while
    struct dirent *entry; // holds one directory entry at a time
    while ((entry = readdir(dir)) != NULL) {
        // 3. Ignore the current and parent directory entries.
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 4. Build the full path to the current entry.
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // 5. lstat() checks the entry itself without following a symlink.
        struct stat st; // metadata for the current entry

        if (lstat(full_path, &st) == 0) {
            // 6. procesare
            if (S_ISDIR(st.st_mode)) {
                // ......
                printf("D: %s [directory]\n", full_path); // print subfolder path
                
                // 7. go deeper into this subfolder
                list_dir(full_path);                  

            } else if (S_ISREG(st.st_mode)) {
                // ......
                printf("F: %s [file]\n", full_path); // regular file found

            } else if (S_ISLNK(st.st_mode)) {
                // .....
                printf("L: %s [symbolic link]\n", full_path); // link detected
            }
        }
    }

    // 8. close the directory after processing it
    closedir(dir); 
}

int main(int argc, char **argv) {
    // Expect exactly one directory argument.
    if (argc != 2) {
        printf("Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    // 0. Start recursion from the user-provided directory.
    list_dir(argv[1]);
    return 0;
}