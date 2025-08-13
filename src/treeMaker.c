#include "treeMaker.h"

Tree new_tree(const char *path, const bool is_directory){
    Tree tree = malloc(sizeof(TreeNode));
    if(tree == NULL){
        fprintf(stderr, "fatal : new_tree name %s allocation failed\n", path);
        exit(EXIT_FAILURE);
    }

    tree->is_directory = is_directory;
    tree->path = strdup(path);
    tree->child_count = 0;    
    tree->children = NULL;

    return tree;
}

Tree add_child(Tree parent, const char *path){
    Tree tree = new_tree(path, true);

    parent->children = realloc(parent->children,(parent->child_count + 1) * sizeof(Tree));
    if(parent->children == NULL){
        fprintf(stderr, "fatal : allocation for tree %s failed\n", path);
        exit(EXIT_FAILURE);
    }
    
    parent->children[parent->child_count++] = tree;    
    return parent;
}

Tree add_file_child(Tree parent, const char *path){
    Tree tree = new_tree(path, false);
    
    parent->children = realloc(parent->children,(parent->child_count + 1) * sizeof(Tree));
    if(parent->children == NULL){
        fprintf(stderr, "fatal : allocation for tree %s failed\n", path);
        exit(EXIT_FAILURE);
    }
        
    parent->children[parent->child_count++] = tree;
    return parent;
}

bool is_empty_tree(Tree tree){
    return tree == NULL;
}

void print_tree(Tree tree, int level) {
    if(is_empty_tree(tree))
        return;

    for(int i = 0; i < level; i++)
        printf("\t"); 
    
    printf("%s%s\n", tree->path,(tree->is_directory) ? "/" : "");    

    for(size_t i = 0; i < tree->child_count; i++) 
        print_tree(tree->children[i], level + 1);
        
}

void clean_tree(Tree *tree) {
    if(is_empty_tree(*tree)) 
        return;

    if((*tree)->children != NULL) {
        for(size_t i = 0; i < (*tree)->child_count; ++i) {
            if((*tree)->children[i] != NULL) {
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