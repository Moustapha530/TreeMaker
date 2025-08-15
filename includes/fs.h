#ifndef __FS_H__
    #define __FS_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    #ifdef _WIN32
        #include <direct.h> 
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

    int create_file(const char *path);
    int create_folder(const char *path);

#endif