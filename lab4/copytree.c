#include <limits.h> 
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h> 
#include <fcntl.h> 
#include <errno.h>

/**
 *  Write a C program for Unix, callable from the command line like this:
 *       copytree path1 path2
 *      which copies the directory path1, 
 *      with all its subtrees of directories, 
 *      to the destination path2. 
 * 
 *  If path2 already exists, 
 *      an error message will be generated. 
 * 
 *  If symbolic links are found in the source, 
 *      symbolic links will be created at the destination that 
 *      refer to the same file as the original 
 *      (the indicated file is not copied!).
 */

#define BUFF_SIZE 1024

int copyFile(char *srcFileName, char *destFileName) {
    char buff[BUFF_SIZE];
    int fdSource;   // file descriptor 
    int fdDest;
    int readBytes;  // the number of read bytes

    fdSource = open(srcFileName, O_RDONLY);     // open the sorce file, in read-only mode

    if (fdSource < 0) {
        fprintf(stderr, "Can't open the source file %s: %s\n", srcFileName, strerror(errno));
        return errno;
    }

    fdDest = open(destFileName, O_CREAT | O_WRONLY, 0640);  

    if (fdDest < 0) {
        fprintf(stderr, "Can't open the destination file %s: %s\n", destFileName, strerror(errno));
        close(fdSource);
        return errno;
    }

    while ((readBytes = read(fdSource, buff, BUFF_SIZE)) > 0) {   // reads from the file 
        int writeBytes = write(fdDest, buff, readBytes);    // write the buffer into the dest. file

        if (writeBytes != readBytes) {
            fprintf(stderr, "Error while writing %d bytes instead of %d: %s\n", writeBytes, readBytes, strerror(errno));

            close(fdSource);
            close(fdDest);

            return errno;
        }
    }

    if (readBytes < 0) {
        fprintf(stderr, "Error, can't read from the source file %s: %s\n", srcFileName, strerror(errno));

        close(fdSource);
        close(fdDest);

        return errno;
    }
    
    close(fdSource);
    close(fdDest);

    return 0;
}

void processFile(char *srcFileName, char *destFileName) {
    printf("Processing file: %s -> %s\n", srcFileName, destFileName);
    copyFile(srcFileName, destFileName);
}

void createDir(char *dirName) {
    /*
     * S_IRWXU | S_IRWXG | S_IRWXO gives rwx for user, group, and others,
     * matching the usual full-access directory permissions.
     */

    if (mkdir(dirName, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        fprintf(stderr, "Error, couldn't create the directory %s: ", dirName);
        perror("");
        return;
    }

    printf("Directory %s was created\n", dirName);
}

void processDir(char *srcDir, char *destDir) {
    printf("Processing dir: %s -> %s\n", srcDir, destDir);
    createDir(destDir);
}

void traverseDir(char *srcPath, char *destPath) {
    DIR *dir;
    struct dirent *entry;   // file or directory from the source directory 

    dir = opendir(srcPath);

    if (dir == NULL) {
        fprintf(stderr, "Cannot open directory '%s': ", srcPath);
        perror("");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;
        char fullSrcPath[PATH_MAX];
        char fullDestPath[PATH_MAX];
            
        /* Skip current and parent directory entries to avoid infinite loops. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        /* Build full path: "<dir_path>/<entry_name>" */
        snprintf(fullSrcPath, sizeof(fullSrcPath), "%s/%s", srcPath, entry->d_name);
        snprintf(fullDestPath, sizeof(fullDestPath), "%s/%s", destPath, entry->d_name);

        /* lstat reads metadata (type, size, etc.) for this path. */
        if (lstat(fullSrcPath, &st) == -1) {
            fprintf(stderr, "Cannot stat '%s': ", fullSrcPath);
            perror("");
            continue;
        }

        /* If this is a regular file, it processes it */
        if (S_ISREG(st.st_mode)) {
            processFile(fullSrcPath, fullDestPath);
        }
        /* If this is a directory, it processes it */
        else if (S_ISDIR(st.st_mode)) {
            processDir(fullSrcPath, fullDestPath);
            traverseDir(fullSrcPath, fullDestPath);
        }
    }

    if (closedir(dir) == -1) {
        fprintf(stderr, "Cannot close directory '%s': ", srcPath);
        perror("");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error, usage: %s src-path dest-path", argv[0]);
        return 1;
    }
    
    createDir(argv[2]);
    traverseDir(argv[1], argv[2]);

    return 0;
}