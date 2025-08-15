#include "fs.h"

int create_folder(const char *path){
    #ifdef _WIN32
        if(_mkdir(path) == 0 || errno == EEXIST)
            return EXIT_SUCCESS;
        else{
            fprintf(stderr, "error : failed to create directory \"%s\".\n", path);
            return EXIT_FAILURE;
        }
    #else
        if(mkdir(path, 0755) == 0 || errno == EEXIST)
            return EXIT_SUCCESS;
        else{
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
    #endif
}

int create_file(const char *path){
    #ifdef _WIN32
        FILE *f = fopen(path, "w");
        if(f == NULL){
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
        fclose(f);
        return EXIT_SUCCESS;
    #else
        int fd = open(path, O_CREAT | O_WRONLY, 0644);
        if(fd < 0){
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
        close(fd);
        return EXIT_SUCCESS;
    #endif
}