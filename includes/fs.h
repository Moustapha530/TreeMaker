#ifndef __FS_H__
#define __FS_H__

/*
 * Standard C library includes for common functionality:
 * - stdio.h: Input/output operations (e.g., FILE handling)
 * - stdlib.h: General utilities like memory allocation
 * - string.h: String manipulation functions
 * - stdbool.h: Boolean type support (bool, true, false)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * Platform-specific includes and definitions:
 * On Windows (_WIN32 defined):
 *  - direct.h: for directory creation (_mkdir)
 *  - windows.h: Windows API functions
 *  - io.h: low-level I/O operations
 *  - PATH_SEPARATOR defined as backslash '\\'
 * 
 * On POSIX systems (Linux, macOS, etc.):
 *  - sys/types.h and sys/stat.h: for file and directory status functions and mode constants
 *  - fcntl.h: file control options (open flags)
 *  - unistd.h: POSIX API (close, write, etc.)
 *  - PATH_SEPARATOR defined as forward slash '/'
 */
#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #include <io.h>
    #define PATH_SEPARATOR '\\'
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

/*
 * create_file
 *
 * Creates a new empty file at the specified path.
 *
 * Parameters:
 *  - path: C string representing the full path of the file to create.
 *
 * Returns:
 *  - 0 on success (file created)
 *  - non-zero on failure (e.g., permission denied, invalid path)
 *
 * Notes:
 *  - If the file already exists, the behavior depends on the implementation
 *    (may truncate or fail).
 *  - Implementation should handle platform-specific system calls.
 */
int create_file(const char *path);

/*
 * create_folder
 *
 * Creates a directory (folder) at the specified path.
 *
 * Parameters:
 *  - path: C string representing the directory path to create.
 *
 * Returns:
 *  - 0 on success (directory created or already exists)
 *  - non-zero on failure (e.g., permission denied, path invalid)
 *
 * Notes:
 *  - Should handle platform differences in mkdir calls.
 *  - May need to consider if intermediate directories should be created.
 */
int create_folder(const char *path);

#endif
