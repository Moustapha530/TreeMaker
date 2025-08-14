#include "parser.h"

int count_indent(const char *line){
    int level = 0;
    int spaces = 0;
    const char *p = line;

    while(*p == ' ' || *p == '\t'){
        if(*p == '\t'){
            level += spaces / 4;
            spaces = 0;
            level += 1;
        } else {
            spaces += 1;
            if(spaces == 4){
                level += 1;
                spaces = 0;
            }
        }
        p++;
    }

    if(spaces > 0){
        fprintf(stderr, "warning (count_indent) : leftover spaces (%d) ignored; use multiples of 4\n", spaces);
    }

    return level;
}

char *strip_whitespace(char *line){
    while(*line == ' ' || *line == '\t' || *line == '-') 
        line++;

    char *end = line + strlen(line) - 1;
    while(end > line &&(*end == '\n' || *end == '\r')) 
        *end-- = '\0';
    return line;
}

void strip_comment(char *line){
    char *comment_pos = strchr(line, '#');
    if(comment_pos){
        *comment_pos = '\0';
    }
}

Tree parse_file(const char *filename){
    FILE *fp = fopen(filename, "r");
    if(!fp){
        fprintf(stderr, "fatal : failed to open file \"%s\", please check path\n", filename);
        exit(EXIT_FAILURE);
    }

    Tree tree = NULL;
    char buffer[PATH_MAX];

    size_t stack_cap = 16;
    Tree *stack = calloc(stack_cap, sizeof(Tree));
    if(stack == NULL){
        fprintf(stderr, "fatal (parse_file) : failed to allocate level stack\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    int prev_level = -1;

    while(fgets(buffer, PATH_MAX, fp) != NULL){
        strip_comment(buffer);
        int level = count_indent(buffer);
        char *clean_line = strip_whitespace(buffer);

        if(clean_line[0] == '\0')
            continue;

        if((size_t)(level + 1) > stack_cap){
            size_t new_cap = stack_cap;
            while((size_t)(level + 1) > new_cap) new_cap *= 2;
            Tree *tmp = realloc(stack, new_cap * sizeof(Tree));
            if(tmp == NULL){
                fprintf(stderr, "fatal (parse_file) : failed to grow level stack\n");
                free(stack);
                fclose(fp);
                exit(EXIT_FAILURE);
            }
            memset(tmp + stack_cap, 0, (new_cap - stack_cap) * sizeof(Tree));
            stack = tmp;
            stack_cap = new_cap;
        }

        if(level > prev_level + 1 && prev_level >= 0){
            fprintf(stderr, "warning (parse_file) : indentation jumped from %d to %d, normalizing to %d\n",
                    prev_level, level, prev_level + 1);
            level = prev_level + 1;
        }

        if(tree == NULL){
            if(level != 0){
                fprintf(stderr, "warning (parse_file) : first line has indent %d, treating as level 0\n", level);
            }
            tree = new_tree(clean_line);
            stack[0] = tree;
            prev_level = 0;
            continue;
        }

        if(level == 0){
            fprintf(stderr, "warning (parse_file) : extra level-0 entry \"%s\"; attaching as child of root\n", clean_line);
            Tree node = attach_child(stack[0], clean_line);
            if(node == NULL){
                continue;
            }
            stack[1] = node;
            for(int l = 2; l <= prev_level; ++l) stack[l] = NULL;
            prev_level = 1;
            continue;
        }

        Tree parent = stack[level - 1];
        if(is_empty_tree(parent)){
            int seek = level - 1;
            while(seek > 0 && is_empty_tree(stack[seek])) seek--;
            if(is_empty_tree(stack[seek])){
                fprintf(stderr, "warning (parse_file) : missing ancestor for level %d; using root as parent\n", level);
                parent = stack[0];
                level = 1;
            } else {
                fprintf(stderr, "warning (parse_file) : missing ancestor at level %d; using level %d instead\n",
                        level - 1, seek);
                parent = stack[seek];
                level = seek + 1;
            }
        }

        Tree node = NULL;
        if(level > prev_level){
            node = attach_child(stack[prev_level], clean_line);
            if(node == NULL) continue;
            stack[level] = node;
        } else if(level == prev_level){
            node = attach_child(parent, clean_line);
            if(node == NULL) continue;
            stack[level] = node;
        } else { 
            node = attach_child(parent, clean_line);
            if(node == NULL) continue;
            stack[level] = node;
            for(int l = level + 1; l <= prev_level; ++l) stack[l] = NULL;
        }

        prev_level = level;
    }

    free(stack);
    fclose(fp);
    return tree;
}