#ifndef __BUILDER_H__
    #define __BUILDER_H__

    /* Platform-agnostic filesystem operations */
    #include "fs.h"
    /* Tree structure and node operations */
    #include "treeMaker.h"

    /* Generate absolute path for a tree node relative to base directory.
     *
     * Combines base_path and node's path using system-specific separators.
     * Caller must free returned string.
     */
    char *build_full_path(const Tree node, const char *base_path);

    /* Create directory structure for node and its children.
     *
     * Recursively creates directories only (skips files).
     * Returns 0 on success, non-zero on filesystem error.
     */
    int build_directories_only(const Tree node, const char *base_path);

    /* Recursively create directory hierarchy for node.
     *
     * Creates current node's directory then processes children.
     * Handles directory creation errors immediately.
     */
    int build_directory_recursive(const Tree node, const char *base_path);

    /* Create files for node and its children.
     *
     * Processes files only after directories exist.
     * Returns 0 if all files created successfully.
     */
    int build_file_recursive(const Tree node, const char *base_path);

    /* Materialize tree structure on filesystem.
     *
     * Orchestrates directory-first, file-second creation.
     * - First creates all directories recursively
     * - Then creates all files recursively
     * Returns 0 on full success, exits early on error.
     */
    int build_tree(const Tree root, const char *dest_dir);

#endif
