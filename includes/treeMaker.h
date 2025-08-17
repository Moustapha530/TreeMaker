#ifndef __TREEMAKER_H__  // Include guard to prevent multiple inclusions of this header file
    #define __TREEMAKER_H__

    #include <stdbool.h>    // Include for boolean type support (true, false)
    #include <stdlib.h>     // Include standard library for memory allocation and exit functions
    #include <stdio.h>      // Include standard I/O for input and output functions
    #include <string.h>     // Include string manipulation functions
    #include "utils.h"      // Include custom utility functions (not defined here)
    #include "fs.h"        // Include file system related functions (not defined here)

    // Structure representing a node in the tree
    typedef struct TreeNode {
        char *path;                // Path associated with this node (file or directory)
        bool is_directory;         // Flag indicating if this node is a directory
        size_t child_count;        // Number of child nodes
        struct TreeNode *parent;   // Pointer to the parent node
        struct TreeNode **children; // Array of pointers to child nodes
    } TreeNode, *Tree;            // Type definition for TreeNode and Tree (pointer to TreeNode)

    // Function to create a new tree with a specified root path
    Tree new_tree(const char *path);

    // Function to attach a child node to a parent node with a specified name
    Tree attach_child(Tree parent, const char *name);

    // Function to check if a tree is empty (i.e., has no nodes)
    bool is_empty_tree(Tree tree);

    // Function to print the tree structure, indented by level
    void print_tree(Tree tree, int level);

    // Function to clean up and free resources associated with a tree
    void clean_tree(Tree *tree);

#endif  // End of include guard
