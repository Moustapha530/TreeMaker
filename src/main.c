/*
    TreeMaker console application to create directory and file structures from a .trm file.

    Overview:
    This program takes a .trm file as input, which describes a directory and file structure.
    It parses the .trm file, creates an in-memory tree representation of the described structure,
    and then builds that structure on the filesystem at a specified destination path.

    Key Components:
    - args.h/args.c:  Handles command-line argument parsing (input files, destination path, debug mode).
    - lexer.h/lexer.c:  Lexical analyzer; converts the input file into a stream of tokens.
    - parser.h/parser.c:  Parses the token stream and builds the tree structure in memory.
    - builder.h/builder.c:  Traverses the in-memory tree and creates the corresponding directories and files on disk.
    - fs.h/fs.c: Provides basic file system operations such as create_folder and create_file.

    Workflow:
    1. Parse command-line arguments to get input .trm files and the destination directory.
    2. For each input file:
        a. Lex the file to generate tokens.
        b. Parse the tokens to create a tree representation of the file system structure.
        c. Build the file system structure on disk using the created tree and the destination directory.
        d. Clean up the in-memory tree.
    3. Free resources used by command-line arguments.
*/
#include "args.h"
#include "parser.h"
#include "builder.h"

int main(int argc, char **argv){
    Args args;

    if(parse_args(argc, argv, &args) != 0)              // Parse command-line arguments. If parsing fails, exit.
        return EXIT_FAILURE;

    for(size_t i = 0; i < args.file_count; i++){       // Iterate through each input file provided.
        Tree tr = parse_tokens(args.input_files[i]);      // Parse the input file to create the tree structure.
        if(!tr){                                        // If parsing fails (returns NULL), print an error and exit.
            fprintf(stderr, "fatal : parsing error please check the input file \"%s\".\n", args.input_files[i]);
            return EXIT_FAILURE;
        } else {
            if(build_tree(tr, args.dest_path) != 0)     // Build the directory/file structure based on the tree. If building fails, exit.
                return EXIT_FAILURE;
            clean_tree(&tr);                            // Clean up the allocated memory for the tree.
        }
    }
    free_args(&args);                                   // Free the memory allocated for the command-line arguments.
    return 0;                                           // Exit successfully.
}
