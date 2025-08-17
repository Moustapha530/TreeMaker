#include "treeMaker.h"

Tree new_tree(const char *path){
    // Allocate the exact memory for TreeNode size
    Tree tree = malloc(sizeof(TreeNode));
    if(tree == NULL){                   /* Check if allocation failed */
        // Print the error
        fprintf(stderr, "fatal (tree making): new_tree name %s allocation failed\n", path);
        return NULL;                    /* Exit and return null */
    }

    // Check if the path is given
    if(path == NULL || strlen(path) == 0){
        fprintf(stderr, "fatal (tree making): invalid name ");      /* Print the error */
        return NULL;                                                /* Exit and return null */
    }

    bool is_dir = false;                                            
    size_t len = strlen(path);                                      /* Get the path length */

    // Check if the path ends with a '/'
    if(path[len - 1] == '/'){
        is_dir = true;                                              /* Pass is dir to true */
        // Remove trailing slash for the stored path string
        tree->path = _strndup(path, len - 1);
    } else {
        tree->path = strdup(path);                                  /* Reallocate the tree path */
    }

    // Initialize other tree fields
    tree->is_directory = is_dir;                                     
    tree->child_count = 0;    
    tree->children = NULL;
    tree->parent = NULL;

    return tree;
}

Tree attach_child(Tree parent, const char *name){
    // Check if the parent is empty
    if(is_empty_tree(parent)){
        // Print the error
        fprintf(stderr, "warning (attach node) : parent is NULL, creating standalone root for \"%s\".\n", name);
        return new_tree(name);
    }

    // Create a new node
    Tree node = new_tree(name);

    /* ======================== Attach node to the parent ======================== */

    node->parent = parent;      
    // Reallocate path buffer to PATH_MAX and build full path including parent
    node->path = realloc(node->path, PATH_MAX);
    if(node->path == NULL){                         /* Check if allocation failed */
        // Print the error
        fprintf(stderr, "fatal (memory): memory allocation failed for \"%s\".\n", name);
        return NULL;                                /* Exit and return null */
    }
    // Attach the parent and child path to node path
    snprintf(node->path, PATH_MAX, "%s%c%s", node->parent->path, PATH_SEPARATOR, name);

    // Reallocate parent children
    Tree *new_children = realloc(parent->children, (parent->child_count + 1) * sizeof(Tree));
    if(new_children == NULL){                       /* Check if allocation failed */
        // Print the error
        fprintf(stderr, "fatal (memory) : memory allocation failed for \"%s\".\n", name);
        clean_tree(&node);                          /* Clean the tree */
        return NULL;                                /* Exit and return null */
    }

    // Reaffect the parent children
    parent->children = new_children;

    // Add the created node to parent children array
    parent->children[parent->child_count++] = node;
    return node;                                    /* return the node */
}

bool is_empty_tree(Tree tree){
    return tree == NULL;    /* Return the test result */
}

void print_tree(Tree tree, int level){
    // Return if tree is empty
    if(is_empty_tree(tree))
        return;

    // Print tab for each level 
    for(int i = 0; i < level; i++)
        printf("    "); 
    
    // Print the tree
    printf("%s%s\n", tree->path,(tree->is_directory) ? "/" : "");    

    // Recusivelly call the function for each child of the tree
    for(size_t i = 0; i < tree->child_count; i++) 
        print_tree(tree->children[i], level + 1);
        
}

void clean_tree(Tree *tree){
    // Check if tree is empty and return
    if(is_empty_tree(*tree))    
        return;

    // Check if the tree has no children
    if((*tree)->children != NULL){
        // Recursivelly call the fucntion for each tree child
        for(size_t i = 0; i <(*tree)->child_count; ++i){
            if((*tree)->children[i] != NULL){
                clean_tree(&(*tree)->children[i]);
                (*tree)->children[i] = NULL; 
            }
        }
        // Free the children array
        free((*tree)->children); 
        (*tree)->children = NULL;
    }

    // Free the tree path
    free((*tree)->path);
    (*tree)->path = NULL;

    // Finally free the tree 
    free((*tree));
    (*tree) = NULL;
}
