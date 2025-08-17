#include "args.h"

int init_args(Args *args){
    args->input_files = NULL;
    args->file_count = 0;
    args->debug_mode = false;

    /* Allocate a buffer for destination path */
    args->dest_path = malloc(PATH_MAX);
    if(!args->dest_path){
        /* Allocation failed: print an error and free any allocated resources. */
        fprintf(stderr, "fatal : attocation failed\n");
        free_args(args);
        return EXIT_FAILURE;
    }

    /* Get the current working directory and store it in dest_path.
     * If getcwd fails, print an error and return failure.
     */
    if(getcwd(args->dest_path, PATH_MAX) == NULL){
        fprintf(stderr, "fatal : failed to get the courant path\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int add_input_file(Args *args, const char *filename){
    args->input_files = realloc(args->input_files, sizeof(char*) *(args->file_count + 1));
    if(!args->input_files){
        /* Reallocation failed */
        return EXIT_FAILURE;
    }
    /* Duplicate the filename string so the Args structure owns its copy. */
    args->input_files[args->file_count] = strdup(filename);
    args->file_count++;

    return EXIT_SUCCESS;
}

void free_args(Args *args){
    /* Free each input file */
    for(unsigned int i = 0; i < args->file_count; i++){
        free(args->input_files[i]);
    }
    free(args->input_files);    /* Free the input files array */
    free(args->dest_path);      /* Free the dest */
}

int parse_args(int argc, char **argv, Args *args){
    if(init_args(args) != 0)    /* Init arguments and check returned value */
        return EXIT_FAILURE;    /* Exit on failure */

    // Loop to check given arguments
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0){   /* Check the --tree or -t option */
            while(i + 1 < argc && argv[i + 1][0] != '-')
                if(add_input_file(args, argv[++i]) != 0)                    /* Add given input files and check return value */
                    return EXIT_FAILURE;                                    /* Exit if the function failed */
        }

        else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dest") == 0){  /* Check the --dest or -d option */
            if(i + 1 < argc){                                                   /* Check the specified dest path */
                free(args->dest_path);                                          /* Free the default dest path */
                args->dest_path = strdup(argv[++i]);                            /* Reaffect the dest path */
            } else{
                // Print the error
                fprintf(stderr, "fatal : --dest/-d need to specify a argument\n");
                return EXIT_FAILURE; /* Exit with a failure code */
            }
        }

        else if(strcmp(argv[i], "--debug") == 0)    /* Check the debug option */
            args->debug_mode = true;                /* Pass debug mode to true */

        // Others arguments who are not option
        else if(argv[i][0] != '-'){
            if(add_input_file(args, argv[i]) != 0) /* Consider arg as input file */
                return EXIT_FAILURE;
        } else{                                    /* If the arg begin with '-' consider that as unkwown option */
            // Print the error
            fprintf(stderr, "fatal : unkwown option: %s\n\n", argv[i]);
            // Print the help message
            print_help();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;    /* Exit with success */
}

void print_help(){
    // Print a short help message
    printf("Create tree templates from tree file.\n\n",
    "treemaker [options] [input_file]\n\n",
    "--debug, -d\tActivate the debug mode.\n"
    "--tree, -t\tThe input file to create the project tree.\n",
    "--path, -p\tThe destination path to create the project tree.\n\n");
}
