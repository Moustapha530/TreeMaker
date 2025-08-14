#ifndef __PARSER_H__
    #define __PARSER_H__

    #include <limits.h>
    #include "treeMaker.h"

    int count_indent(const char *line);
    char *strip_whitespace(char *line);
    void strip_comment(char *line);
    Tree parse_file(const char *filename);

#endif