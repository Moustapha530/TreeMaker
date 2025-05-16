#include "args.h"

int parse_args(Args *args){
    args->input_file = NULL;
    args->debug_mode = 0;
    for(int i = 1; i < __argc; i++){
        if(!strcmp(__argv[i], "--help")){
            print_help();
            return 0;
        }
        else if(!strcmp(__argv[i], "--debug") || !strcmp(__argv[i], "-d"))
            args->debug_mode = 1;
        else if((!strcmp(__argv[i], "--tree") || !strcmp(__argv[i], "-d")) && i+1 < __argc)
            args->input_file = __argv[i+1];
        else {
            fprint(stderr, "Unknown argument %s\n", __argv[i]);
            return 0;
        }
    }

    return args->input_file != NULL;
}

void print_help(){
    printf("Create tree templates from tree file.\n\ntreemaker [options] --tree, -t [input_file]\n\n--debug, -d\tActive the debug mode.\n\n--tree, -t\tThe input file to create the project tree.\n\n");
}