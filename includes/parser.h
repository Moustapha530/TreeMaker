#ifndef __PARSER_H__  // Include guard to prevent multiple inclusions of this header file
    #define __PARSER_H__

    #include "treeMaker.h"  // Include the treeMaker header for tree data structures and functions~
    #include "lexer.h"      // Include the lexer header for tokenize the input file


    // Function to parse tokens and return a Tree structure based on its contents
    Tree parse_tokens(const char *path);

#endif  // End of include guard
