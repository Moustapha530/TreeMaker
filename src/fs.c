#include "fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <direct.h>  // mkdir on Windows
    #include <windows.h>
    #include <io.h>
    #define PATH_SEPARATOR '\\'
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

int create_folder(const char *path) {
    #ifdef _WIN32
        if (_mkdir(path) == 0 || errno == EEXIST) {
            return 0;
        } else {
            perror("mkdir (Windows)");
            return -1;
        }
    #else
        if (mkdir(path, 0755) == 0 || errno == EEXIST) {
            return 0;
        } else {
            perror("mkdir (Unix)");
            return -1;
        }
    #endif
}

int create_file(const char *path) {
    #ifdef _WIN32
        FILE *f = fopen(path, "w");
        if (f == NULL) {
            perror("fopen (Windows)");
            return -1;
        }
        fclose(f);
        return 0;
    #else
        int fd = open(path, O_CREAT | O_WRONLY, 0644);
        if (fd < 0) {
            perror("open (Unix)");
            return -1;
        }
        close(fd);
        return 0;
    #endif
}