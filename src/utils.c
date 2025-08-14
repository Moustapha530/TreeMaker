#include "utils.h"

char *_strndup(const char *src, size_t n){
    if(src == NULL)
        return NULL;

    size_t len = strnlen(src, n);
    char *dest = malloc(len + 1);
    if(dest == NULL)
        return NULL;
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}