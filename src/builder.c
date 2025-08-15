#include "builder.h"

char *build_full_path(const Tree node, const char *base_path){
    char *full_path = malloc(PATH_MAX);
    if(!full_path){
        fprintf(stderr, "fatal (memory): memory allocation failed for make full path.\n");
        return NULL;
    }
    snprintf(full_path, PATH_MAX, "%s%c%s", base_path, PATH_SEPARATOR, node->path);
    
    return full_path;
}

int build_directories_only(const Tree node, const char *base_path){
    if(node == NULL){
        fprintf(stderr, "fatal (build directory): can not create the directory because tree is empty.\n");
        return EXIT_FAILURE;
    }

    char *full_path = build_full_path(node, base_path);
    if(!full_path)
        return EXIT_FAILURE;

    if(node->is_directory){
        if(create_folder(full_path) != 0){
            free(full_path);
            return EXIT_FAILURE;
        }
        for(size_t i = 0; i < node->child_count; i++)
            if(node->children[i] && node->children[i]->is_directory)
                build_directories_only(node->children[i], base_path);
        
    }

    free(full_path);
    return EXIT_SUCCESS;
}

int build_directory_recursive(const Tree node, const char *base_path){
    if(is_empty_tree(node)){
        fprintf(stderr, "fatal (build directory) : can not create the directory because tree is empty.\n");
        return EXIT_FAILURE;
    }

    char *full_path = build_full_path(node, base_path);
    if(!full_path)
        return EXIT_FAILURE;

    if(node->is_directory){
        if(create_folder(full_path) != 0){
            free(full_path);
            return EXIT_FAILURE;
        }
        free(full_path);

        for(size_t i = 0; i < node->child_count; i++)
            if(node->children[i] && node->children[i]->is_directory)
                build_directory_recursive(node->children[i], base_path);
        
    } 
    else
        free(full_path);
    
    return EXIT_SUCCESS;
}

int build_file_recursive(const Tree node, const char *base_path){
    if(is_empty_tree(node)){
        fprintf(stderr, "fatal (build file) : can not create the file because the tree is empty.\n");
        return EXIT_FAILURE;
    }

    if(!node->is_directory){
        char *full_path = build_full_path(node, base_path);
        if(!full_path)
            return EXIT_FAILURE;

        if(create_file(full_path) != 0){
            free(full_path);
            return EXIT_FAILURE;
        }
        free(full_path);
    }

    for(size_t i = 0; i < node->child_count; i++)
        if(node->children[i])
            build_file_recursive(node->children[i], base_path);
        
    return EXIT_SUCCESS;
}

int build_tree(const Tree root, const char *dest_dir){
    if(is_empty_tree(root)){
        fprintf(stderr, "fatal (build tree): tree is empty, nothing to create.\n");
        return EXIT_FAILURE;
    }

    char *full_root_path = build_full_path(root, dest_dir);
    if(!full_root_path)
        return EXIT_FAILURE;

    if(create_folder(full_root_path) != 0){
        free(full_root_path);
        return EXIT_FAILURE;
    }
    free(full_root_path);

    for(size_t i = 0; i < root->child_count; i++)
        if(root->children[i] && root->children[i]->is_directory)
            build_directory_recursive(root->children[i], dest_dir);

    for(size_t i = 0; i < root->child_count; i++)
        if(root->children[i])
            build_file_recursive(root->children[i], dest_dir);
        
    return EXIT_SUCCESS;
}
