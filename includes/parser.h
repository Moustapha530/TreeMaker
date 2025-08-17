#ifndef __PARSER_H__  // Include guard to prevent multiple inclusions of this header file
    #define __PARSER_H__

    #include <limits.h>      // Include limits.h for constant definitions (e.g., INT_MAX)
    #include "treeMaker.h"  // Include the treeMaker header for tree data structures and functions

    // Function to count the number of leading spaces (indentation) in a line
    int count_indent(const char *line);

    // Function to remove leading and trailing whitespace from a line
    char *strip_whitespace(char *line);

    // Function to remove comments from a line
    void strip_comment(char *line);

    // Function to parse a file and return a Tree structure based on its contents
    Tree parse_file(const char *filename);

#endif  // End of include guard
