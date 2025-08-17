#include "utils.h"

char *_strndup(const char *src, size_t n){
    if(src == NULL)     /* Check if the src is null */ 
        return NULL;    /* Exit and return null */

    size_t len = strnlen(src, n);  
    char *dest = malloc(len + 1);
    if(dest == NULL)                /* Check if the allocation failed */
        return NULL;                /* Exit and return null */
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;                    /* Return dest */
}
