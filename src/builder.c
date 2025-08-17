#include "builder.h"

char *build_full_path(const Tree node, const char *base_path){
    // Allocate memory for a path max size
    char *full_path = malloc(PATH_MAX);
    // Check if allocation failed
    if(!full_path){                     
        // Print the error
        fprintf(stderr, "fatal (memory): memory allocation failed for make full path.\n");
        return NULL;        /* Exit and return null */
    }
    // Build the full path
    snprintf(full_path, PATH_MAX, "%s%c%s", base_path, PATH_SEPARATOR, node->path);
    
    return full_path;       /* Return the built path */
}

int build_directories_only(const Tree node, const char *base_path){
    if(node == NULL){
        fprintf(stderr, "fatal (build directory): can not create the directory because tree is empty.\n");
        return EXIT_FAILURE;
    }

    /* Construct the full path for this node */
    char *full_path = build_full_path(node, base_path);
    if(!full_path)
        return EXIT_FAILURE;

    if(node->is_directory){
        /* Create the directory represented by this node */
        if(create_folder(full_path) != 0){
            free(full_path);
            return EXIT_FAILURE;
        }
        /* Recursively create directories for children that are directories */
        for(size_t i = 0; i < node->child_count; i++)
            if(node->children[i] && node->children[i]->is_directory)
                build_directories_only(node->children[i], base_path);
        
    }

    free(full_path);
    return EXIT_SUCCESS;
}

int build_directory_recursive(const Tree node, const char *base_path){
    // Check if the node is empty
    if(is_empty_tree(node)){
        // Print the error
        fprintf(stderr, "fatal (build directory) : can not create the directory because tree is empty.\n");
        return EXIT_FAILURE;    /* Exit and return a failure code */
    }

    // Build the path
    char *full_path = build_full_path(node, base_path);
    // Check if the path is not built and exit with a failure code 
    if(!full_path)
        return EXIT_FAILURE;

    // Check if the node is a directory
    if(node->is_directory){
        if(create_folder(full_path) != 0){      /* Check if the function failed */
            free(full_path);                    /* Free the full path */
            return EXIT_FAILURE;                /* Exit with a failure code */
        }
        free(full_path);                        /* Free the full path if the function did not fail */

        // Recursivelly call the function for each node child
        for(size_t i = 0; i < node->child_count; i++)
            if(node->children[i] && node->children[i]->is_directory)
                build_directory_recursive(node->children[i], base_path);
        
    } 
    else
        free(full_path);                        /* Free the full path if the node is not a directory */
    
    return EXIT_SUCCESS;                        /* Exit successfully */
}

int build_file_recursive(const Tree node, const char *base_path){
    // Check if the node is empty print the error and exit with a failure code
    if(is_empty_tree(node)){
        fprintf(stderr, "fatal (build file) : can not create the file because the tree is empty.\n");
        return EXIT_FAILURE;
    }

    if(!node->is_directory){                                    /* Check if the node is not a directory */
        char *full_path = build_full_path(node, base_path);     /* Build the full path */
        if(!full_path)                                          /* Check if the full path is built */
            return EXIT_FAILURE;                                /* Exit and return a failure code */    

        // Create the file path of the node and manage erros
        if(create_file(full_path) != 0){        
            free(full_path);
            return EXIT_FAILURE;
        }

        // Finally free the full path 
        free(full_path);
    }

    // Repeat the process for each node child 
    for(size_t i = 0; i < node->child_count; i++)
        if(node->children[i])
            build_file_recursive(node->children[i], base_path);
        
    return EXIT_SUCCESS;        // Exit successfully
}

int build_tree(const Tree root, const char *dest_dir){
    // Check if the root is empty print the error and exit with a failure code
    if(is_empty_tree(root)){
        fprintf(stderr, "fatal (build tree): tree is empty, nothing to create.\n");
        return EXIT_FAILURE;
    }

    // Build and check the full path
    char *full_root_path = build_full_path(root, dest_dir);
    if(!full_root_path)
        return EXIT_FAILURE;

    // Create the root and manage errors
    if(create_folder(full_root_path) != 0){
        free(full_root_path);
        return EXIT_FAILURE;
    }
    free(full_root_path);

    // Build recursivelly all directories
    for(size_t i = 0; i < root->child_count; i++)
        if(root->children[i] && root->children[i]->is_directory)
            build_directory_recursive(root->children[i], dest_dir);

    // Build recursivelly all files
    for(size_t i = 0; i < root->child_count; i++)
        if(root->children[i])
            build_file_recursive(root->children[i], dest_dir);
        
    return EXIT_SUCCESS;    /* Exit successfully */
}
