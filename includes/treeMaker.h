#ifndef __TREEMAKER_H__
#define __TREEMAKER_H__

typedef struct TreeNode {
    char *name;
    _Bool is_directory;
    struct TreNode **children;
    int child_count;
} TreeNode;

#endif