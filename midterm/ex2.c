/* Study demo for Unix / Linux / macOS. See study_guide.md.
 * Run inside a NEW directory: this program creates lab_demo and removes
 * only the files it created. On failure it leaves lab_demo for inspection.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>       /* FILE, printf, fopen, fprintf, fscanf, fread, fwrite */
#include <stdlib.h>      /* exit, system */
#include <unistd.h>      /* read, write, close, lseek, fork, exec, links, IDs */
#include <fcntl.h>       /* open, creat, O_* */
#include <sys/stat.h>    /* mkdir, stat, permissions, S_IS* */
#include <sys/wait.h>    /* wait, waitpid, WIFEXITED, WEXITSTATUS */
#include <dirent.h>      /* DIR, struct dirent, directory functions */
#include <errno.h>       /* errno, EINTR */
#include <stdint.h>      /* intmax_t, uintmax_t */

static void fail(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

/* System calls usually return -1 on error; pointers use NULL instead. */
static void check(int result, const char *message)
{
    if (result == -1)
        fail(message);
}

/* A write may transfer fewer bytes than requested, so finish in a loop. */
static void write_all(int fd, const char *data, size_t length)
{
    size_t done = 0;
    while (done < length) {
        ssize_t n = write(fd, data + done, length - done);
        if (n == -1 && errno == EINTR)
            continue;
        if (n == -1)
            fail("write");
        if (n == 0) {
            errno = EIO;
            fail("write made no progress");
        }
        done += (size_t)n;
    }
}

static ssize_t read_some(int fd, char *buffer, size_t length)
{
    ssize_t n;
    do {
        n = read(fd, buffer, length);
    } while (n == -1 && errno == EINTR);
    if (n == -1)
        fail("read");
    return n;
}

/* 1. File descriptors, flags, permissions, reads, writes and seeking. */
static void file_demo(void)
{
    puts("\n1. FILE DESCRIPTORS AND SYSTEM CALLS");
    int fd = open("notes.txt", O_RDWR | O_CREAT | O_EXCL,
                  S_IRUSR | S_IWUSR); /* Owner read/write, subject to umask. */
    check(fd, "open notes.txt");
    printf("open returned descriptor %d\n", fd);
    write_all(fd, "ABCDEFGH", 8);

    /* write advanced the offset to 8. Move back to byte 3. */
    if (lseek(fd, 3, SEEK_SET) == (off_t)-1)
        fail("lseek SEEK_SET");
    char buffer[32];
    ssize_t n = read_some(fd, buffer, 2);
    buffer[n] = '\0'; /* read does NOT add this string terminator. */
    printf("Read %zd bytes from position 3: %s\n", n, buffer);

    off_t position = lseek(fd, 0, SEEK_CUR); /* Query current position. */
    if (position == (off_t)-1)
        fail("lseek SEEK_CUR");
    printf("Current position: %jd\n", (intmax_t)position);

    if (lseek(fd, -2, SEEK_END) == (off_t)-1)
        fail("lseek SEEK_END");
    n = read_some(fd, buffer, 2);
    buffer[n] = '\0';
    printf("Last bytes: %s\n", buffer);
    n = read_some(fd, buffer, sizeof buffer);
    printf("Read at end of file: %zd (0 means EOF)\n", n);

    struct stat info;
    check(fstat(fd, &info), "fstat");
    printf("Size from fstat: %jd bytes\n", (intmax_t)info.st_size);
    check(close(fd), "close notes.txt");

    fd = open("notes.txt", O_WRONLY | O_APPEND);
    check(fd, "open append");
    write_all(fd, "\n", 1);
    check(close(fd), "close append");

    /* creat is equivalent to open with O_WRONLY | O_CREAT | O_TRUNC. */
    fd = creat("scratch.txt", S_IRUSR | S_IWUSR);
    check(fd, "creat");
    write_all(fd, "old contents", 12);
    check(close(fd), "close scratch");
    fd = open("scratch.txt", O_WRONLY | O_TRUNC);
    check(fd, "open truncate");
    check(close(fd), "close truncated file");
    check(stat("scratch.txt", &info), "stat scratch");
    printf("Size after O_TRUNC: %jd\n", (intmax_t)info.st_size);
}

/* 2. FILE* library streams: formatted text and arrays of objects. */
static void library_demo(void)
{
    puts("\n2. LIBRARY FUNCTIONS: FILE *");
    FILE *stream = fopen("student.txt", "w");
    if (stream == NULL)
        fail("fopen write");
    if (fprintf(stream, "%s %d\n", "Maria", 10) < 0)
        fail("fprintf");
    check(fclose(stream), "fclose write");

    stream = fopen("student.txt", "r");
    if (stream == NULL)
        fail("fopen read");
    char name[32];
    int grade;
    if (fscanf(stream, "%31s %d", name, &grade) != 2) {
        fputs("Expected a name and grade\n", stderr);
        exit(EXIT_FAILURE);
    }
    printf("Formatted read: %s, grade %d\n", name, grade);
    check(fclose(stream), "fclose read");

    int original[] = {10, 20, 30};
    int restored[3];
    stream = fopen("numbers.bin", "wb");
    if (stream == NULL)
        fail("fopen binary write");
    if (fwrite(original, sizeof original[0], 3, stream) != 3) {
        fputs("Could not write all 3 integers\n", stderr);
        exit(EXIT_FAILURE);
    }
    check(fclose(stream), "fclose binary write");
    stream = fopen("numbers.bin", "rb");
    if (stream == NULL)
        fail("fopen binary read");
    if (fread(restored, sizeof restored[0], 3, stream) != 3) {
        fputs("Could not read all 3 integers\n", stderr);
        exit(EXIT_FAILURE);
    }
    check(fclose(stream), "fclose binary read");
    printf("Binary read: %d %d %d\n", restored[0], restored[1], restored[2]);
}

/* 3. Hard/symbolic links and metadata. */
static void links_demo(void)
{
    puts("\n3. LINKS AND METADATA");
    check(link("notes.txt", "notes_hard.txt"), "link");
    check(symlink("notes.txt", "notes_symbolic.txt"), "symlink");
    struct stat info;
    check(stat("notes_symbolic.txt", &info), "stat target");
    printf("stat follows symlink: target size = %jd\n", (intmax_t)info.st_size);
    printf("Target hard-link count = %ju\n", (uintmax_t)info.st_nlink);
    printf("Target permissions = %03o\n", (unsigned)(info.st_mode & 0777));
    check(lstat("notes_symbolic.txt", &info), "lstat link");
    printf("lstat inspects link itself: is symlink = %s\n",
           S_ISLNK(info.st_mode) ? "yes" : "no");
    check(rename("scratch.txt", "renamed.txt"), "rename");
}

static void directory_demo(void)
{
    puts("\n4. DIRECTORIES");
    check(mkdir("empty_directory", S_IRWXU), "mkdir empty");
    DIR *directory = opendir(".");
    if (directory == NULL)
        fail("opendir");
    struct dirent *entry;
    for (;;) {
        errno = 0; /* Distinguish end of directory from readdir failure. */
        entry = readdir(directory);
        if (entry == NULL) {
            if (errno != 0)
                fail("readdir");
            break;
        }
        printf("Entry: %s\n", entry->d_name); /* Includes . and .. */
    }
    rewinddir(directory); /* Next readdir starts again from the beginning. */
    errno = 0;
    entry = readdir(directory);
    if (entry != NULL)
        printf("After rewinddir, first entry: %s\n", entry->d_name);
    else if (errno != 0)
        fail("readdir after rewind");
    check(closedir(directory), "closedir");
}

static void show_status(pid_t child, int status)
{
    /* status is encoded: do not print it as though it were the exit code. */
    if (WIFEXITED(status))
        printf("Child %jd exited with code %d\n",
               (intmax_t)child, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("Child %jd ended by signal %d\n",
               (intmax_t)child, WTERMSIG(status));
}

/* 5. Separate variables after fork; inherited descriptors share offsets. */
static void process_demo(void)
{
    puts("\n5. PROCESSES");
    printf("Parent PID=%jd PPID=%jd UID=%ju GID=%ju\n",
           (intmax_t)getpid(), (intmax_t)getppid(),
           (uintmax_t)getuid(), (uintmax_t)getgid());
    int value = 10;
    int fd = open("notes.txt", O_RDONLY);
    check(fd, "open before fork");
    if (fflush(NULL) == EOF) /* Avoid duplicating buffered output at fork. */
        fail("fflush");
    pid_t child = fork();
    if (child == -1)
        fail("fork");
    if (child == 0) {
        value = 99; /* Changes only this child's copy. */
        printf("Child PID=%jd PPID=%jd: value=%d\n",
               (intmax_t)getpid(), (intmax_t)getppid(), value);
        char buffer[3];
        ssize_t n = read_some(fd, buffer, 2);
        buffer[n] = '\0';
        printf("Child read: %s\n", buffer);
        check(close(fd), "child close");
        exit(7);
    }
    int status;
    pid_t finished;
    do {
        finished = waitpid(child, &status, 0); /* Wait for this child. */
    } while (finished == -1 && errno == EINTR);
    if (finished == -1)
        fail("waitpid");
    show_status(finished, status);
    printf("Parent still has value=%d\n", value);
    char buffer[3];
    ssize_t n = read_some(fd, buffer, 2);
    buffer[n] = '\0';
    printf("Parent reads next bytes through inherited offset: %s\n", buffer);
    check(close(fd), "parent close");

    if (fflush(NULL) == EOF)
        fail("fflush before exec child");
    child = fork();
    if (child == -1)
        fail("fork exec child");
    if (child == 0) {
        /* execlp replaces this child's program; PID stays the same.
         * p searches PATH; l supplies the arguments as a list.
         * argv[0] is the program name; a NULL pointer ends the list. */
        execlp("ls", "ls", "-l", ".", (char *)NULL);
        perror("execlp"); /* Reached ONLY when exec fails. */
        _exit(127);
    }
    do {
        finished = wait(&status); /* Any child; only one remains here. */
    } while (finished == -1 && errno == EINTR);
    if (finished == -1)
        fail("wait");
    show_status(finished, status);

    if (fflush(NULL) == EOF)
        fail("fflush before system");
    status = system("printf 'Hello from the shell via system()\\n'");
    if (status == -1)
        fail("system");
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fputs("Shell command failed\n", stderr);
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    char original_directory[4096];
    char current_directory[4096];
    if (getcwd(original_directory, sizeof original_directory) == NULL)
        fail("getcwd original");
    /* Refuse to reuse an existing directory, preventing accidental overwrite. */
    check(mkdir("lab_demo", S_IRWXU), "mkdir lab_demo (must not already exist)");
    check(chdir("lab_demo"), "chdir lab_demo");
    if (getcwd(current_directory, sizeof current_directory) == NULL)
        fail("getcwd current");
    printf("Working directory: %s\n", current_directory);

    file_demo();
    library_demo();
    links_demo();
    directory_demo();
    process_demo();

    puts("\n6. CLEANUP");
    const char *files[] = {"notes_symbolic.txt", "notes_hard.txt", "notes.txt",
                          "renamed.txt", "student.txt", "numbers.bin"};
    for (size_t i = 0; i < sizeof files / sizeof files[0]; ++i)
        check(unlink(files[i]), "unlink");
    check(rmdir("empty_directory"), "rmdir empty");
    check(chdir(original_directory), "chdir original");
    check(rmdir("lab_demo"), "rmdir lab_demo");
    puts("Finished. Demo files and directories removed.");
    return EXIT_SUCCESS;
}
