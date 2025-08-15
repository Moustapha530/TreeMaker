/*
    TreeMaker console application to create tree structure from a trm file. 
*/ 
#include "args.h"
#include "parser.h"
#include "builder.h"

int main(int argc, char **argv){
    Args args;
    if(parse_args(argc, argv, &args) != 0)
        return EXIT_FAILURE;

    for(size_t i = 0; i < args.file_count; i++){
        Tree tr = parse_file(args.input_files[i]);
        if(!tr){
            fprintf(stderr, "fatal : parsing error please check the input file \"%s\".\n", args.input_files[i]);
            return EXIT_FAILURE;
        } else {
            if(build_tree(tr, args.dest_path) != 0)
                return EXIT_FAILURE;
            clean_tree(&tr);
        }
    }
    free_args(&args);
    return 0;
}