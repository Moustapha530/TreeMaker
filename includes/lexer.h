#ifndef __LEXER_H__
#define __LEXER_H__

typedef struct {
    int line;
    int column;
    char *name;
    int depth;
    _Bool is_directory;
} Token;

#endif