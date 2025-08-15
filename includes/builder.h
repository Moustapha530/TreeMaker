#ifndef __BUILDER_H__
    #define __BUILDER_H__

    #include "fs.h"
    #include "treeMaker.h"

    char *build_full_path(const Tree node, const char *base_path);
    int build_directories_only(const Tree node, const char *base_path);
    int build_directory_recursive(const Tree node, const char *base_path);
    int build_file_recursive(const Tree node, const char *base_path);
    int build_tree(const Tree root, const char *dest_dir);

#endif