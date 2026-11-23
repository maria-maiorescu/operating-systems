#include <stddef.h>
#include <unistd.h>

int main(void) {
    /* Search recursively for "jale" in midterm, case-insensitive. */
    execlp("grep", "grep", "-i", "jale", "-R", "~/midterm", (char *)NULL);

    /* Show lines from a.txt that contain "draga". */
    execlp("grep", "grep", "draga", "a.txt", (char *)NULL);

    /* Show lines containing "draga" and include line numbers. */
    execlp("grep", "grep", "-n", "draga", "a.txt", (char *)NULL);

    /* Count how many lines in a.txt contain "draga". */
    execlp("grep", "grep", "-c", "draga", "a.txt", (char *)NULL);

    /* Show lines from a.txt containing "jale", case-insensitive. */
    execlp("grep", "grep", "-i", "jale", "a.txt", (char *)NULL);

    /* Repeat the previous case-insensitive "jale" search in a.txt. */
    execlp("grep", "grep", "-i", "jale", "a.txt", (char *)NULL);

    /* Print lines, words, and bytes count for a.txt. */
    execlp("wc", "wc", "a.txt", (char *)NULL);

    /* Print only the number of lines in a.txt. */
    execlp("wc", "wc", "-l", "a.txt", (char *)NULL);

    /* Print only the number of words in a.txt. */
    execlp("wc", "wc", "-w", "a.txt", (char *)NULL);

    /* Print only the number of bytes in a.txt. */
    execlp("wc", "wc", "-c", "a.txt", (char *)NULL);

    return 0;
}
