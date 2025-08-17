// =============================== lexer.c ===============================
// Implementation for TreeMaker Base Lexer with error reporting
// ---------------------------------------------------------------------
// See lexer.h for the public API. This file focuses on robust lexing,
// indentation handling and precise error diagnostics.

#include "lexer.h"


// ---------------- Utilities ----------------
static const char* kind_name(LexErrorKind k){
    switch(k){
        case LEX_OK: return "OK";
        case LEX_ERR_UNEXPECTED_CHAR: return "UNEXPECTED_CHAR";
        case LEX_ERR_BAD_INDENT: return "BAD_INDENT";
        case LEX_ERR_CONTROL_IN_NAME: return "CONTROL_IN_NAME";
        case LEX_ERR_INTERNAL: return "INTERNAL";
        default: return "?";
    }
}

const char* token_type_name(Lx_TokenType t){
    switch(t){
        case T_INDENT: return "INDENT";
        case T_DEDENT: return "DEDENT";
        case T_NEWLINE: return "NEWLINE";
        case T_EOF: return "EOF";
        case T_NAME: return "NAME";
        case T_COMMENT: return "COMMENT";
        default: return "?";
    }
}

void token_free(Token* t){
    if(!t) 
        return;
    free(t->lexeme);
    t->lexeme = NULL; t->length = 0;
}

void lex_error_free(LexError* e){
    if(!e) 
        return;

    free(e->message); 
    e->message = NULL;
}

void lex_error_print(const LexError* e, const char* filename){
    if(!e) 
        return;
        
    const char* file = filename ? filename : "<stdin>";
    fprintf(stderr, "\n\n%s:%d:%d: error: %s%s%s(%s)\n\n",
            file, e->line, e->column,
            e->message ? e->message : "",
            e->message ? " " : "",
            e->message ? "" : "",
            kind_name(e->kind));
}

// ---------------- Int stack ----------------
static void stack_init(IntStack* s){ 
    s->data=NULL; 
    s->size=0; 
    s->cap=0; 
}

static void stack_free(IntStack* s){ 
    free(s->data); 
    s->data = NULL; 
    s->size = s->cap=0; 
}

static void stack_push(IntStack* s, int v){
    if(s->size == s->cap){ s->cap = s->cap ? s->cap*2 : 8; s->data =(int*)realloc(s->data, s->cap*sizeof(int)); }
    s->data[s->size++] = v;
}

static int stack_top(const IntStack* s){ 
    return s->size ? 
    s->data[s->size-1] : 0; 
}

static int stack_pop(IntStack* s){ 
    return s->size ? s->data[--s->size] : 0; 
}

// ---------------- Pending token queue ----------------
typedef struct QNode{ 
    Token t; 
    struct QNode* next; 
} QNode;

static void q_push(Pending** h, Pending** t, Token tok){
    Pending* n =(Pending*)malloc(sizeof(Pending));
    n->tok = tok; n->next = NULL;
    if(*t)
        (*t)->next = n; 
    else{
        *h = n; 
        *t = n;
    }
}

static bool q_pop(Pending** h, Pending** t, Token* out){
    if(!*h) 
        return false;

    Pending* n = *h; 
    *out = n->tok; 
    *h = n->next;

    if(!*h) 
        *t = NULL; 

    free(n); 
    return true;
}

static void q_clear(Pending** h, Pending** t){ 
    Token tmp; 
    while(q_pop(h, t, &tmp)) 
        token_free(&tmp); 
}

// ---------------- Core lexer helpers ----------------
static inline bool __eof(Lexer* L){ 
    return L->i >= L->len; 
}

static inline char peek(Lexer* L){ 
    return L->i < L->len ? L->src[L->i] : '\0'; 
}

static inline char peek2(Lexer* L){ 
    return(L->i+1 < L->len) ? L->src[L->i+1] : '\0';
}

static inline char getc_(Lexer* L){
    if(L->i >= L->len) return '\0';
    char c = L->src[L->i++];
    if(c == '\n'){ L->line++; L->col = 1; L->at_line_start = true; }
    else{ L->col++; }
    return c;
}

static void add_error(Lexer* L, LexErrorKind kind, int line, int col, const char* msg, size_t nmsg){
    if(L->err_count == L->err_cap){
        L->err_cap = L->err_cap ? L->err_cap*2 : 4;
        L->errors = (LexError*)realloc(L->errors, L->err_cap * sizeof(LexError));
    }

    LexError* e = &L->errors[L->err_count++];
    e->kind = kind; e->line = line; e->column = col;
    e->message = msg ? _strndup(msg, nmsg) : NULL;

    if(L->cfg.stop_on_first_error) 
        L->had_fatal = true;
}

static int count_indent(const char* s, size_t n, size_t* adv, int tabw){
    int spaces = 0; size_t i = 0;
    while(i < n){
        char c = s[i];
        if(c == ' '){
            spaces++; 
            i++;
        }
        else if(c == '\t'){
            spaces += tabw; 
            i++; 
        }
        else if(c == '\r')
            i++;
        else 
            break;
    }

    *adv = i; 
    return spaces;
}

static Token make_tok(Lx_TokenType ty, const char* start, size_t n, int line, int col){
    Token t; 
    t.type = ty; 
    t.length = n;
    t.line = line; 
    t.column = col; 
    t.lexeme = start ? _strndup(start, n) : NULL; 
    return t;
}

static void emit_indent_dedent(Lexer* L, int new_indent){
    int curr = stack_top(&L->indents);
    if(new_indent == curr) 
        return;

    if(new_indent > curr){
        stack_push(&L->indents, new_indent);
        Token t = make_tok(T_INDENT, NULL, 0, L->line, 1);
        q_push(&L->qh, &L->qt, t);
        return;
    }

    // new_indent < curr: must match a previous indent level
    while(stack_top(&L->indents) > new_indent){
        stack_pop(&L->indents);
        Token t = make_tok(T_DEDENT, NULL, 0, L->line, 1);
        q_push(&L->qh, &L->qt, t);
    }

    if(stack_top(&L->indents) != new_indent){
        // inconsistent dedent, report error
        const char* msg = "inconsistent indentation level";
        add_error(L, LEX_ERR_BAD_INDENT, L->line, 1, msg, strlen(msg));
    }
}

static bool is_name_char(unsigned char c){
    return isalnum(c) || c == '_' || c == '-' || c == '.' || c == '+' || c == '@' || c == '@';
}

static Token lex_name_or_dir(Lexer* L){
    int line = L->line, col = L->col;
    const char* start = &L->src[L->i];
    size_t n = 0;

    while(!__eof(L)){
        unsigned char c = (unsigned char)peek(L);
        if(is_name_char(c)){
            getc_(L); 
            n++;
        }
        else 
            break;
    }

    // Optional direct '/' marking a directory
    if(!__eof(L) && peek(L) == '/'){
        getc_(L);
        n++;
        return make_tok(T_NAME, start, n, line, col);
    }
    return make_tok(T_NAME, start, n, line, col);
}

// ---------------- Public API ----------------

void lexer_init(Lexer* L, const char* src, size_t len, const LexerConfig* cfg){
    memset(L, 0, sizeof(*L));

    L->src = src; 
    L->len = len; 
    L->i = 0; 
    L->line = 1; 
    L->col = 1; 
    L->at_line_start = true;
    stack_init(&L->indents); 
    stack_push(&L->indents, 0);
    L->qh = L->qt = NULL;
    L->errors = NULL; 
    L->err_count = L->err_cap = 0; 
    L->had_fatal = false;
    if(cfg) 
        L->cfg = *cfg; 
    else{ 
        L->cfg.tab_width = 4; 
        L->cfg.emit_blank_newlines = true; 
        L->cfg.stop_on_first_error = true; 
    }
}

void lexer_free(Lexer* L){
    if(!L) return;
    stack_free(&L->indents);
    q_clear(&L->qh, &L->qt);

    for(size_t i = 0; i < L->err_count; ++i) 
        lex_error_free(&L->errors[i]);
    free(L->errors); L->errors = NULL; 
    L->err_count = L->err_cap = 0;
}

static Token next_core(Lexer* L){
    Token out;
    if(q_pop(&L->qh, &L->qt, &out))
        return out;

    if(__eof(L)){
        if(stack_top(&L->indents) > 0){
            stack_pop(&L->indents);
            return make_tok(T_DEDENT, NULL, 0, L->line, L->col);
        }
        return make_tok(T_EOF, NULL, 0, L->line, L->col);
    }

    // ----- Start-of-line indentation management -----
    if(L->at_line_start){
        size_t adv = 0;
        int spaces = count_indent(L->src + L->i, L->len - L->i, &adv, L->cfg.tab_width);

        const char *p = L->src + L->i + adv;
        char c = (p < (L->src + L->len)) ? *p : '\0';

        if(c == '\n' || c == '\0'){
            L->i += adv;
            if(c == '\n') (void)getc_(L);
            return next_core(L);
        }

        if(c == '#'){
            L->i += adv;
            while(!__eof(L) && getc_(L) != '\n') {}
            return next_core(L);
        }

        L->i += adv;
        L->col += (int)adv;
        L->at_line_start = false;

        emit_indent_dedent(L, spaces);
        if(q_pop(&L->qh, &L->qt, &out))
            return out;
    }

    while(!__eof(L)){
        char c = peek(L);
        if(c == ' ' || c == '\t' || c == '\r'){
            (void)getc_(L);
            continue;
        }
        if(c == '#'){
            while(!__eof(L) && getc_(L) != '\n') {}
            return next_core(L);
        }
        break;
    }

    if(__eof(L)){
        if(stack_top(&L->indents) > 0){
            stack_pop(&L->indents);
            return make_tok(T_DEDENT, NULL, 0, L->line, L->col);
        }
        return make_tok(T_EOF, NULL, 0, L->line, L->col);
    }

    if(peek(L) == '\n'){
        (void)getc_(L);
        if(L->cfg.emit_blank_newlines){
            return make_tok(T_NEWLINE, NULL, 0, L->line - 1, 1);
        }else{
            return next_core(L);
        }
    }

    // NAME
    if(!__eof(L)){
        unsigned char c =(unsigned char)peek(L);
        if(is_name_char(c)){
            return lex_name_or_dir(L);
        }
        // Any other visible non-space character is unexpected here
        if(!isspace(c) && c != '\0'){
            int line = L->line, col = L->col;
            char msgbuf[64];
            int m = snprintf(msgbuf, sizeof(msgbuf), "unexpected character '%c'", (char)c);
            if(m < 0) 
                m = 0;

            (void)m;

            add_error(L, LEX_ERR_UNEXPECTED_CHAR, line, col, msgbuf, strlen(msgbuf));
            // consume to avoid infinite loop
            getc_(L);
            // recover by returning a NAME-like token of length 0
            return make_tok(T_NAME, "", 0, line, col);
        }
    }

    // Skip and recurse(covers stray CR etc.)
    if(!__eof(L)) 
        getc_(L);

    return next_core(L);
}

Token lexer_next(Lexer* L){
    if(L->had_fatal)
        return make_tok(T_EOF, NULL, 0, L->line, L->col);
    
    Token t = next_core(L);
    if(L->had_fatal && t.type != T_EOF){
        // Force EOF if a fatal error occurred mid-stream
        token_free(&t);
        return make_tok(T_EOF, NULL, 0, L->line, L->col);
    }
    return t;
}

size_t lexer_error_count(const Lexer* L){ 
    return L ? L->err_count : 0; 
}

const LexError* lexer_errors(const Lexer* L){ 
    return L ? L->errors : NULL; 
}

size_t lexer_tokenize_file(const char *filename, Token **out_tokens, const LexerConfig *cfg){
    *out_tokens = NULL;

    FILE *fp = fopen(filename, "rb");
    if(!fp){
        fprintf(stderr, "fatal (tokenizing): cannot open '%s'.\n", filename);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    long n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(n < 0){ 
        fclose(fp); 
        return 0; 
    }

    char *buf = (char*)malloc((size_t)n + 1);
    if(!buf){ 
        fclose(fp); 
        fprintf(stderr, "fatal (tokenizing): alocation failed for the file bufeer.\n"); 
        return 0; 
    }

    size_t rd = fread(buf, 1, (size_t)n, fp);
    fclose(fp);
    buf[rd] = '\0';

    LexerConfig local = cfg ? *cfg : (LexerConfig){ .tab_width = 4, .emit_blank_newlines = false, .stop_on_first_error = true };
    Lexer L; 
    lexer_init(&L, buf, rd, &local);

    size_t cap = 64, count = 0;
    Token *arr = (Token*)malloc(cap * sizeof(Token));
    if(!arr){ 
        free(buf); 
        fprintf(stderr, "fatal (tokenizing): allocation failed for tokens array.\n"); 
        return 0; 
    }

    for(;;){
        Token t = next_core(&L);
        if(t.type == T_NAME || t.type == T_INDENT || t.type == T_DEDENT || t.type == T_EOF){
            if(count == cap){
                cap *= 2;
                Token *tmp = (Token*)realloc(arr, cap * sizeof(Token));
                if(!tmp){
                    fprintf(stderr, "fatal: OOM growing token array\n");
                    for(size_t i=0; i < count; ++i) 
                        token_free(&arr[i]);

                    free(arr); 
                    free(buf); 
                    lexer_free(&L);
                    return 0;
                }
                arr = tmp;
            }
            arr[count++] = t;
        }else{
            token_free(&t);
        }

        if(t.type == T_EOF) 
            break;
    }

    lexer_free(&L);
    free(buf);
    *out_tokens = arr;
    return count;
}
