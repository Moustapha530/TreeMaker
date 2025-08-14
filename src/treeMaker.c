#include "treeMaker.h"

Tree new_tree(const char *path){
    Tree tree = malloc(sizeof(TreeNode));
    if(tree == NULL){
        fprintf(stderr, "fatal (tree making): new_tree name %s allocation failed\n", path);
        return NULL;
    }

    if(path == NULL || strlen(path) == 0){
        fprintf(stderr, "fatal (tree making): invalid name ");
        return NULL;
    }

    bool is_dir = false;
    size_t len = strlen(path);

    if(path[len - 1] == '/'){
        is_dir = true;
        tree->path = _strndup(path, len - 1);
    } else {
        tree->path = strdup(path);
    }

    tree->is_directory = is_dir; 
    tree->child_count = 0;    
    tree->children = NULL;
    tree->parent = NULL;

    return tree;
}

Tree attach_child(Tree parent, const char *name){
    if(is_empty_tree(parent)){
        fprintf(stderr, "fatal (attach node) : parent is NULL, creating standalone root for \"%s\"\n", name);
        return new_tree(name);
    }

    Tree node = new_tree(name);
    node->parent = parent;

    Tree *new_children = realloc(parent->children, (parent->child_count + 1) * sizeof(Tree));
    if(new_children == NULL){
        fprintf(stderr, "fatal (memory) : memory allocation failed for \"%s\"\n", name);
        clean_tree(&node);
        return NULL;
    }

    parent->children = new_children;
    parent->children[parent->child_count++] = node;
    return node;
}

bool is_empty_tree(Tree tree){
    return tree == NULL;
}

void print_tree(Tree tree, int level){
    if(is_empty_tree(tree))
        return;

    for(int i = 0; i < level; i++)
        printf("    "); 
    
    printf("%s%s\n", tree->path,(tree->is_directory) ? "/" : "");    

    for(size_t i = 0; i < tree->child_count; i++) 
        print_tree(tree->children[i], level + 1);
        
}

void clean_tree(Tree *tree){
    if(is_empty_tree(*tree)) 
        return;

    if((*tree)->children != NULL){
        for(size_t i = 0; i <(*tree)->child_count; ++i){
            if((*tree)->children[i] != NULL){
                clean_tree(&(*tree)->children[i]);
                (*tree)->children[i] = NULL; 
            }
        }
        free((*tree)->children);
        (*tree)->children = NULL;
    }

    free((*tree)->path);
    (*tree)->path = NULL;

    free((*tree));
    (*tree) = NULL;
}