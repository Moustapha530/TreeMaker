#ifndef __ARGS_H__
    #define __ARGS_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    #ifdef _WIN32
        #include <direct.h>
    #else
        #include <unistd.h>
    #endif

    #ifndef PATH_MAX
        #define PATH_MAX 4096
    #endif

    typedef struct {
        char **input_files;
        unsigned int file_count;
        char *dest_path;
        bool debug_mode;
    } Args;

    int init_args(Args *args);
    int add_input_file(Args *args, const char *filename);
    void free_args(Args *args);
    int parse_args(int argc, char **argv, Args *args);
    void print_help();

#endif