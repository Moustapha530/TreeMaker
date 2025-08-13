#include "args.h"

void init_args(Args *args){
    args->input_files = NULL;
    args->file_count = 0;
    args->debug_mode = false;

    args->dest_path = malloc(PATH_MAX);
    if(!args->dest_path){
        fprintf(stderr, "fatal : attocation failed\n");
        free_args(args);
        exit(EXIT_FAILURE);
    }

    if(getcwd(args->dest_path, PATH_MAX) == NULL){
        fprintf(stderr, "fatal : failed to get the courant path\n");
        exit(EXIT_FAILURE);
    }
    
}

void add_input_file(Args *args, const char *filename){
    args->input_files = realloc(args->input_files, sizeof(char*) *(args->file_count + 1));
    if(!args->input_files){
        perror("realloc");
        free_args(args);
        exit(EXIT_FAILURE);
    }
    args->input_files[args->file_count] = strdup(filename);
    args->file_count++;
}

void free_args(Args *args){
    for(unsigned int i = 0; i < args->file_count; i++){
        free(args->input_files[i]);
    }
    free(args->input_files);
    free(args->dest_path);
}

void parse_args(int argc, char **argv, Args *args){
    init_args(args);

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0){
            // Loop to get input files
            while(i + 1 < argc && argv[i + 1][0] != '-'){
                add_input_file(args, argv[++i]);
            }
        }
        else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dest") == 0){
            if(i + 1 < argc){
                free(args->dest_path);
                args->dest_path = strdup(argv[++i]);
            } else {
                fprintf(stderr, "fatal : --dest/-d need to specify a argument\n");
                free_args(args);
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i], "--debug") == 0){
            args->debug_mode = true;
        }
        else if(argv[i][0] != '-'){
            // Detect input files without option --tree/-t
            add_input_file(args, argv[i]);
        }
        else {
            fprintf(stderr, "Unkwown option: %s\n\n", argv[i]);
            print_help();
            free_args(args);
            exit(EXIT_FAILURE);
        }
    }
}

void print_help(){
    printf("Create tree templates from tree file.\n\ntreemaker [options] [input_file]\n\n--debug, -d\tActivate the debug mode.\n\n--tree, -t\tThe input file to create the project tree.\n--path, -p\tThe destination path to create the project tree.\n\n");
}