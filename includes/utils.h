#ifndef __UTILS_H__  // Include guard to prevent multiple inclusions of this header file
    #define __UTILS_H__

    #include <string.h>  // Include for string manipulation functions
    #include <stdlib.h>  // Include for memory allocation functions

    // Function to duplicate a string up to a specified number of characters
    // Allocates memory for the new string and returns a pointer to it
    char *_strndup(const char *src, size_t n);

#endif  // End of include guard
