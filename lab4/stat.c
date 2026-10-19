// stat option - displays information about a file 

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void showType(struct stat *fileStat) {
    // first letter: 'd' = directory; 'l' = link (shortcut); '-' = file
    char c = '-';

    // checks if it's a directory
    if (S_ISDIR(fileStat->st_mode)) {
        c = 'd';
    }

    // checks if it's a link
    if (S_ISLNK(fileStat->st_mode)) {
        c = 'l';
    }

    printf("%c", c);
}

// prints the user rights (r = read; w = write; x = execute)
void showUser(struct stat *fileStat) {
    char r = '-';
    char w = '-';
    char x = '-';

    // checks if the bit is set for the read right for the user
    if (S_IRUSR & fileStat->st_mode) {
        r = 'r';
    }

    if (S_IWUSR & fileStat->st_mode) {
        w = 'w';
    }

    if (S_IXUSR & fileStat->st_mode) {
        x = 'x';
    }

    printf("%c%c%c",r, w, x);

}

void showGroup(struct stat *fileStat) {
    char r = '-';
    char w = '-';
    char x = '-';

    // checks if the bit is set for the read right for the user
    if (S_IRGRP & fileStat->st_mode) {
        r = 'r';
    }

    if (S_IWGRP & fileStat->st_mode) {
        w = 'w';
    }

    if (S_IXGRP & fileStat->st_mode) {
        x = 'x';
    }

    printf("%c%c%c",r, w, x);
}

void showOthers(struct stat *fileStat) {
    char r = '-';
    char w = '-';
    char x = '-';

    // checks if the bit is set for the read right for the user
    if (S_IROTH & fileStat->st_mode) {
        r = 'r';
    }

    if (S_IWOTH & fileStat->st_mode) {
        w = 'w';
    }

    if (S_IXOTH & fileStat->st_mode) {
        x = 'x';
    }

    printf("%c%c%c",r, w, x);
}

void showPermisions(struct stat *fileStat) {

    showType(fileStat);
    showUser(fileStat);
    showGroup(fileStat);
    showOthers(fileStat);
}

void showOwner(struct stat *fileStat) {
    // takes the info about a user id
    struct passwd *userInfo = getpwuid(fileStat->st_uid); 

    // takes the info about a group
    struct group *groupInfo = getgrgid(fileStat->st_gid);

    printf(" %s %s", userInfo->pw_name, groupInfo->gr_name);
    
}

void showSize(struct stat *fileStat) {
    printf("\t%d\t%lld\t", fileStat->st_nlink, fileStat->st_size);
}

void showDataModif(struct stat *fileStat) {
    struct tm *timeInfo = localtime(&fileStat->st_mtime);   // UNIX time converted to local time
    char dateBuffer[32];    // buffer to save the date

    // convert the structure to a format
    strftime(dateBuffer, sizeof(dateBuffer), "%b %d %Y %H:%M", timeInfo);
    printf(" %s", dateBuffer);
}

int showStats(char *fileName) {
    struct stat fileStat;   // structure that hold the information about a file

    // lstat = loads the file's properties in a structure
    // checks the function's result (if it could be executed)
    if (lstat(fileName, &fileStat) < 0) {
        fprintf(stderr, "Can't get the status of file %s, error %s", fileName, strerror(errno));

        return errno;
    }

    showPermisions(&fileStat);
    showOwner(&fileStat);
    showSize(&fileStat);
    showDataModif(&fileStat);

    printf("\t%s\n", fileName);

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Error! Usage: %s fileName\n", argv[0]);

        return 1;
    }

    for (int i = 1; i < argc; i++) {
        showStats(argv[i]);
    }

    return 0;
}