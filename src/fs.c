#include "fs.h"

int create_folder(const char *path){
    #ifdef _WIN32   /* Create the directory depending on the os */
        // Create a directory for windows operaring system and manage errors
        if(_mkdir(path) == 0 || errno == EEXIST)
            return EXIT_SUCCESS;
        else{
            fprintf(stderr, "error : failed to create directory \"%s\".\n", path);
            return EXIT_FAILURE;
        }
    #else
        // Create a directory for unix operaring systems and manage errors
        if(mkdir(path, 0755) == 0 || errno == EEXIST)
            return EXIT_SUCCESS;
        else{
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
    #endif
}

int create_file(const char *path){
    #ifdef _WIN32   /* Create the file depending on the os */
        // Create a file for windows operaring system and manage errors
        FILE *f = fopen(path, "w");
        if(f == NULL){
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
        // Close the created file and exit successfully
        fclose(f);
        return EXIT_SUCCESS;
    #else
        // Create a file for unix operaring system and manage errors
        int fd = open(path, O_CREAT | O_WRONLY, 0644);
        if(fd < 0){
            fprintf(stderr, "error : failed to create file \"%s\".\n", path);
            return EXIT_FAILURE;
        }
        // Close the created file and exit successfully
        close(fd);
        return EXIT_SUCCESS;
    #endif
}