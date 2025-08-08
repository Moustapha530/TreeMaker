#include "treeMaker.h"

void init_tree(Tree tree){
    if(tree == NULL)
        return;
    tree->front = NULL;
    tree->rear = NULL;
}

bool is_empty_tree(Tree tree){
    if(tree != NULL)
        return tree->front == NULL && tree->rear == NULL;
    else
        return true;
}

Tree push_tree(Tree tree, char *path){
    TreeNode element = malloc(sizeof(*element));
    if(element == NULL){
        fprintf(stderr, "fatal : allocation failed\n");
        exit(EXIT_FAILURE);
    }

    element->path = path;
    element->next = NULL;
    if(is_empty_tree(tree)){
        tree->front = element;
        tree->rear = element;
    } else {
        tree->rear->next = element;
        tree->rear = element;
    }
    return tree;
}

Tree pop_tree(Tree tree){   
    if(is_empty_tree(tree))
        return tree;

    TreeNode temp = tree->front;
    if(tree->front == tree->rear){
        tree->front = NULL;
        tree->rear = NULL;
    } else
        tree->front = tree->front->next;

    free(temp);
    return tree;
} 

Tree new_tree(void){
    Tree tree = malloc(sizeof(*tree));
    if(tree == NULL){
        fprintf(stderr, "fatal : allocation failed\n");
        exit(EXIT_FAILURE);
    }
    init_tree(tree);
    return tree;
}

Tree clear_tree(Tree tree){
    while(!is_empty_tree(tree))
        tree = pop_tree(tree);
    free(tree);
    tree = NULL;
    return tree;
}