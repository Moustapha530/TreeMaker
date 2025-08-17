#ifndef __ARGS_H__
    #define __ARGS_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>

    /* Platform-specific includes for directory operations:
     * - On Windows, <direct.h> provides _mkdir etc.
     * - On Unix-like systems, <unistd.h> provides access to POSIX functions.
     */
    #ifdef _WIN32
        #include <direct.h>
    #else
        #include <unistd.h>
    #endif

    /* Define PATH_MAX if not already defined by the system.
     * This represents the maximum path length allowed.
     * 4096 is a common default on Linux systems.
     */
    #ifndef PATH_MAX
        #define PATH_MAX 4096
    #endif

    /* Args structure to hold command-line arguments and options parsed from argv.
     *
     * Fields:
     *  - input_files: dynamically allocated array of strings holding input filenames.
     *  - file_count: number of input files stored in input_files.
     *  - dest_path: string holding the destination directory path where output is created.
     *  - debug_mode: boolean flag indicating if debug mode is enabled.
     */
    typedef struct {
        char **input_files;       // Array of input file paths
        unsigned int file_count;  // Number of input files
        char *dest_path;          // Destination directory path
        bool debug_mode;          // Debug mode flag
    } Args;

    /* Initialize an Args structure.
     *
     * Allocates or sets initial values as needed.
     *
     * Returns:
     *  - 0 on success
     *  - non-zero on failure 
     */
    int init_args(Args *args);

    /* Add an input file path to the Args structure.
     *
     * Parameters:
     *  - args: pointer to Args structure to modify
     *  - filename: string containing the path to add
     *
     * Behavior:
     *  - Dynamically resizes input_files array as needed.
     *  - Copies the filename string.
     *
     * Returns:
     *  - 0 on success
     *  - non-zero on failure 
     */
    int add_input_file(Args *args, const char *filename);

    /* Free all memory allocated inside the Args structure.
     *
     * Parameters:
     *  - args: pointer to Args structure to free
     *
     * Behavior:
     *  - Frees all input file strings and the input_files array.
     *  - Frees the dest_path string.
     *  - Resets fields to safe default values.
     */
    void free_args(Args *args);

    /* Parse command-line arguments into an Args structure.
     *
     * Parameters:
     *  - argc: argument count
     *  - argv: argument vector
     *  - args: pointer to Args structure to populate
     *
     * Behavior:
     *  - Parses known options and input file names.
     *  - Typically processes options like:
     *      * input files list
     *      * destination directory
     *      * debug mode flag
     *  - May print errors or help if parsing fails.
     *
     * Returns:
     *  - 0 on successful parsing
     *  - non-zero if parsing failed or invalid arguments provided
     */
    int parse_args(int argc, char **argv, Args *args);

    /* Print usage/help message to standard output.
     *
     * Behavior:
     *  - Outputs information about program usage, command-line options,
     *    and arguments expected.
     */
    void print_help();

#endif
