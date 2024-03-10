#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>  // Include for close function
#include <string.h>  // Include for strcmp function
#include <stdarg.h>  // Include for va_start and va_end functions

#define PERMS 0666
#define BUFSIZ 1024

void error(char *fmt, ...);

void copy_file(const char *from, const char *to);
void copy_directory(const char *from, const char *to);

int main(int argc, char *argv[]) {
    if (argc != 3)
        error("Usage: cp from to");

    struct dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(argv[1])) == NULL) {
        // Not a directory, treat it as a file
        copy_file(argv[1], argv[2]);
    } else {
        // It's a directory, copy its contents
        copy_directory(argv[1], argv[2]);
        closedir(dfd);
    }

    return 0;
}

void copy_file(const char *from, const char *to) {
    int f1, f2, n;
    char buf[BUFSIZ];

    if ((f1 = open(from, O_RDONLY)) == -1)
        error("cp: can't open %s", from);

    if ((f2 = creat(to, PERMS)) == -1)
        error("cp: can't create %s, mode %03o", to, PERMS);

    while ((n = read(f1, buf, BUFSIZ)) > 0) {
        if (write(f2, buf, n) != n)
            error("cp: write error on file %s", to);
    }

    close(f1);
    close(f2);
}

void copy_directory(const char *from, const char *to) {
    struct dirent *dp;
    DIR *dfd;
    char src[BUFSIZ], dest[BUFSIZ];

    if ((dfd = opendir(from)) == NULL)
        error("cp: can't open directory %s", from);

    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        snprintf(src, BUFSIZ, "%s/%s", from, dp->d_name);
        snprintf(dest, BUFSIZ, "%s/%s", to, dp->d_name);

        if (dp->d_type == DT_DIR) {
            // Recursively copy subdirectories
            copy_directory(src, dest);
        } else {
            // Copy regular files
            copy_file(src, dest);
        }
    }

    closedir(dfd);
}

void error(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);

}

