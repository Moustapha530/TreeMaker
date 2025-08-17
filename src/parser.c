#include "parser.h"

Tree parse_tokens(const char *path){
    Token *toks = NULL;
    // Configure the lexer
    LexerConfig cfg = { .tab_width = 4, .emit_blank_newlines = false, .stop_on_first_error = false };
    size_t ntok = lexer_tokenize_file(path, &toks, &cfg);       /* Tokenize the file */

    Tree tree = NULL;

    size_t stack_cap = 16;
    Tree *stack = (Tree*)calloc(stack_cap, sizeof(Tree));
    if(!stack){
        fprintf(stderr, "fatal (parsing file): failed to allocate level stack\n\n");
        for(size_t i=0; i<ntok; ++i) 
            token_free(&toks[i]);
            
        free(toks);
        exit(EXIT_FAILURE);
    }

    int level = 0;
    stack[0] = NULL;

    for(size_t i=0; i<ntok; ++i){
        Token *t = &toks[i];

        if(t->type == T_INDENT){
            level++;
            if((size_t)(level) >= stack_cap){
                size_t new_cap = stack_cap;
                while((size_t)level >= new_cap) new_cap *= 2;
                Tree *tmp = (Tree*)realloc(stack, new_cap * sizeof(Tree));
                if(!tmp){
                    fprintf(stderr, "fatal (parsing): failed to grow level stack\n\n");
                    for(size_t k=0; k<ntok; ++k) 
                        token_free(&toks[k]);
                    free(toks); 
                    free(stack);
                    exit(EXIT_FAILURE);
                }
                for(size_t z = stack_cap; z < new_cap; ++z) tmp[z] = NULL;
                stack = tmp; stack_cap = new_cap;
            }
            continue;
        }

        if(t->type == T_DEDENT){
            if(level > 0) 
                level--;

            continue;
        }

        if(t->type == T_NAME){
            const char *name = (t->lexeme)? t->lexeme : "";
            Tree parent = (level > 0)? stack[level-1] : NULL;
            Tree node = attach_child(parent, name);
            if(is_empty_tree(node)) 
                continue;

            if(is_empty_tree(tree)) 
                tree = node;

            stack[level] = node;

            for(size_t l = (size_t)level + 1; l < stack_cap; ++l) 
                stack[l] = NULL;

            continue;
        }

        if(t->type == T_EOF){
            break;
        }
    }

    for(size_t i=0;i<ntok;++i) token_free(&toks[i]);
    free(toks);
    free(stack);

    return tree;

}