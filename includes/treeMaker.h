#ifndef __TREEMAKER_H__
    #define __TREEMAKER_H__

    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include "utils.h"
    #include "fs.h"

    typedef struct TreeNode {
        char *path;
        bool is_directory;
        size_t child_count;
        struct TreeNode *parent;
        struct TreeNode **children;
    } TreeNode, *Tree;
    
    Tree new_tree(const char *path);
    Tree attach_child(Tree parent, const char *name);
    bool is_empty_tree(Tree tree);
    void print_tree(Tree tree, int level);
    void clean_tree(Tree *tree);

#endif