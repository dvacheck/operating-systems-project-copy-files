#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h> 
#define PERMS 0777
#define BUFSIZ 1024

void error(char *fmt, ...);

void copy_file(const char *from, const char *to);
void copy_directory(const char *from, const char *to);

int main(int argc, char *argv[]) {
    if (argc != 3)
        error("How to use: cp from to");
    struct dirent *dp;
    DIR *dfd;
    if ((dfd = opendir(argv[1])) == NULL) {
        copy_file(argv[1], argv[2]);
    } else {
        copy_directory(argv[1], argv[2]);
        closedir(dfd);
    }
    return 0;
}

void copy_file(const char *from, const char *to) {
    int f1, f2, n;
    char buf[BUFSIZ];
    struct stat st;
    
    if ((f1 = open(from, O_RDONLY)) == -1)
        error("cp: unable to open %s", from);
  //for saving permissions this line down  
    if ((fstat(f1, &st)) == -1)
        error("cp: unable to get file status for %s", from);
//stating permissions here (which were saved)
    if ((f2 = creat(to, st.st_mode)) == -1)
        error("cp: unable to create %s, mode %03o", to, PERMS);

    while ((n = read(f1, buf, BUFSIZ)) > 0) {
        if (write(f2, buf, n) != n)
            error("cp: error write on file %s", to);
    }

    close(f1);
    close(f2);
}
void copy_directory(const char *from, const char *to) {
    struct dirent *dp;
    DIR *dfd;
    char src[BUFSIZ], dest[BUFSIZ];
    if ((dfd = opendir(from)) == NULL)
        error("cp: unable to open directory %s", from);
	mkdir(to,PERMS);
    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        snprintf(src, BUFSIZ, "%s/%s", from, dp->d_name);
        snprintf(dest, BUFSIZ, "%s/%s", to, dp->d_name);
        if (dp->d_type == DT_DIR) {
            copy_directory(src, dest);
        } else {
            copy_file(src, dest);
        }
    }
    closedir(dfd);
}

void error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: "); //as a stream
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

