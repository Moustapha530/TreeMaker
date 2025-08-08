#ifndef __QUEUE_H__
    #define __QUEUE_H__

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>

    typedef struct TreeNode {
        char *path;
        struct TreeNode *next;
    } *TreeNode;

    typedef struct Tree {
        TreeNode front;
        TreeNode rear;
    } *Tree;
    
    void init_tree(Tree tree);
    bool is_empty_tree(Tree tree);
    Tree new_tree(void);
    Tree push_tree(Tree tree, char *path);
    Tree pop_tree(Tree tree);
    Tree clear_tree(Tree tree);

#endif