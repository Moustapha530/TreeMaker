#include "args.h"

int init_args(Args *args){
    args->input_files = NULL;
    args->file_count = 0;
    args->debug_mode = false;

    args->dest_path = malloc(PATH_MAX);
    if(!args->dest_path){
        fprintf(stderr, "fatal : attocation failed\n");
        free_args(args);
        return EXIT_FAILURE;
    }

    if(getcwd(args->dest_path, PATH_MAX) == NULL){
        fprintf(stderr, "fatal : failed to get the courant path\n");
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
}

int add_input_file(Args *args, const char *filename){
    args->input_files = realloc(args->input_files, sizeof(char*) *(args->file_count + 1));
    if(!args->input_files){
        return EXIT_FAILURE;
    }
    args->input_files[args->file_count] = strdup(filename);
    args->file_count++;

    return EXIT_SUCCESS;
}

void free_args(Args *args){
    for(unsigned int i = 0; i < args->file_count; i++){
        free(args->input_files[i]);
    }
    free(args->input_files);
    free(args->dest_path);
}

int parse_args(int argc, char **argv, Args *args){
    if(init_args(args) != 0)
        return EXIT_FAILURE;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0){
            while(i + 1 < argc && argv[i + 1][0] != '-')
                if(add_input_file(args, argv[++i]) != 0)
                    return EXIT_FAILURE;            
        }
        else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dest") == 0){
            if(i + 1 < argc){
                free(args->dest_path);
                args->dest_path = strdup(argv[++i]);
            } else {
                fprintf(stderr, "fatal : --dest/-d need to specify a argument\n");
                return EXIT_FAILURE;
            }
        }
        else if(strcmp(argv[i], "--debug") == 0)
            args->debug_mode = true;
        else if(argv[i][0] != '-'){
            if(add_input_file(args, argv[i]) != 0)
                return EXIT_FAILURE;
        }
        else {
            fprintf(stderr, "fatal : unkwown option: %s\n\n", argv[i]);
            print_help();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void print_help(){
    printf("Create tree templates from tree file.\n\ntreemaker [options] [input_file]\n\n--debug, -d\tActivate the debug mode.\n\n--tree, -t\tThe input file to create the project tree.\n--path, -p\tThe destination path to create the project tree.\n\n");
}
