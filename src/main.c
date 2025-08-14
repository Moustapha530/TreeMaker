/*
    TreeMaker console application to create tree structure from a trm file. 
*/ 
#include "args.h"
#include "parser.h"

int main(int argc, char **argv){
    Args args;
    parse_args(argc, argv, &args);

    for(size_t i = 0; i < args.file_count; i++){
        Tree tr = parse_file(args.input_files[i]);
        if(!tr){
            fprintf(stderr, "fatal : parsing error please check the input file \"%s\".\n", args.input_files[i]);
            exit(EXIT_FAILURE);
        } else {
            print_tree(tr, 0);
            clean_tree(&tr);
        }
    }
    free_args(&args);
    return 0;
}