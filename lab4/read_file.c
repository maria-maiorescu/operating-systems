#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/**
 * Read an entire input file (first argument) and split content into:
 *  - letters  -> second argument file
 *  - digits   -> third argument file
 *  - other    -> fourth argument file
 *
 * Uses lseek() to move through the input file.
 */

int main(int argc, char **argv) {
    int opt;

    // Parse options (there are no flags for this task, only file arguments).
    while ((opt = getopt(argc, argv, "")) != -1) {
        if (opt == '?') {
            return 1;
        }
    }

    // Expected arguments after options:
    // input_file letters_file digits_file other_file
    if (argc - optind != 4) {
        return 1;
    }

    char *input_path = argv[optind];
    char *letters_path = argv[optind + 1];
    char *digits_path = argv[optind + 2];
    char *other_path = argv[optind + 3];

    int fd_input = open(input_path, O_RDONLY);
    int fd_letters = open(letters_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    int fd_digits = open(digits_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    int fd_other = open(other_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    // Minimal validation: stop if one file cannot be opened.
    if (fd_input < 0 || fd_letters < 0 || fd_digits < 0 || fd_other < 0) {
        return 1;
    }

    // Get file size, then return to the start for reading.
    off_t file_size = lseek(fd_input, 0, SEEK_END);
    lseek(fd_input, 0, SEEK_SET);

    // Read one character at a time and write it to the correct output file.
    for (off_t i = 0; i < file_size; i++) {
        char ch;
        if (read(fd_input, &ch, 1) != 1) {
            break;
        }

        if (isalpha((unsigned char)ch)) {
            write(fd_letters, &ch, 1);
        } else if (isdigit((unsigned char)ch)) {
            write(fd_digits, &ch, 1);
        } else {
            write(fd_other, &ch, 1);
        }
    }

    close(fd_input);
    close(fd_letters);
    close(fd_digits);
    close(fd_other);

    return 0;
}
