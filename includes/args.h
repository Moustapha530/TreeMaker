#ifndef __ARGS_H__
#define __ARGS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *input_file;
    _Bool debug_mode;
} Args;

int parse_args(Args *args);
int print_help();

#endif