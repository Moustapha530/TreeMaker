#ifndef __ERROS_H__
#define __ERROS_H__

typedef struct {
    int line;
    int column;
    char *message;
    char *type;
} Error;

#endif