#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#define PERMS 0666

void error(char *fmt, ...);

void copy_file(char *source, char *dest) {
    int f1, f2, n;
    char buf[BUFSIZ];

    // Open source file for reading
    if ((f1 = open(source, O_RDONLY, 0)) == -1) {
        error("cp: can't open %s", source);
    }

    // Check if destination file already exists
    if (access(dest, F_OK) != -1) {
        // Destination file already exists
        printf("Destination file '%s' already exists. Do you want to overwrite? (y/n): ", dest);
        char response;
        scanf(" %c", &response);
        if (response != 'y') {
            printf("Copy operation aborted.\n");
            close(f1);
            return;
        }
    }

    // Create or truncate destination file for writing
    if ((f2 = creat(dest, PERMS)) == -1) {
        error("cp: can't create %s, mode %03o", dest, PERMS);
    }

    // Copy data from source to destination
    while ((n = read(f1, buf, BUFSIZ)) > 0) {
        if (write(f2, buf, n) != n) {
            error("cp: write error on file %s", dest);
        }
    }

    // Close file descriptors
    close(f1);
    close(f2);
}

void copy_directory(char *source, char *dest) {
    DIR *dfd;
    struct dirent *dp;
    char src[BUFSIZ], dst[BUFSIZ];

    // Open source directory
    if ((dfd = opendir(source)) == NULL) {
        error("cp: can't open directory %s", source);
    }

    // Check if destination directory already exists
    if (access(dest, F_OK) != -1) {
        // Destination directory already exists
        printf("Destination directory '%s' already exists. Do you want to overwrite? (y/n): ", dest);
        char response;
        scanf(" %c", &response);
        if (response != 'y') {
            printf("Copy operation aborted.\n");
            closedir(dfd);
            return;
        }
    }

    // Create destination directory
    if (mkdir(dest, PERMS) == -1) {
        error("cp: can't create directory %s", dest);
    }

    // Read and copy files/directories from source to destination
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        snprintf(src, BUFSIZ, "%s/%s", source, dp->d_name);
        snprintf(dst, BUFSIZ, "%s/%s", dest, dp->d_name);

        if (dp->d_type == DT_DIR) {
            // Recursively copy subdirectories
            copy_directory(src, dst);
        } else {
            // Copy regular files
            copy_file(src, dst);
        }
    }

    // Close directory
    closedir(dfd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        error("Usage: cp source destination");
    }

    char *source = argv[1];
    char *dest = argv[2];

    // Check if source file/directory exists
    if (access(source, F_OK) == -1) {
        error("Source '%s' does not exist", source);
    }

    // Check if source and destination are directories
    int isSourceDir = (opendir(source) != NULL);
    int isDestDir = (opendir(dest) != NULL);

    if (isSourceDir) {
        // Source is a directory
        if (!isDestDir) {
            // Destination does not exist, create it
            if (mkdir(dest, PERMS) == -1) {
                error("cp: can't create directory %s", dest);
            }
        }
        copy_directory(source, dest);
    } else {
        // Source is a file
        copy_file(source, dest);
    }

    return 0;
}

void error(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(1);
}
