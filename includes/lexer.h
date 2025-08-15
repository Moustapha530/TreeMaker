// =============================== lexer.h ===============================
// TreeMaker Base Lexer 
// ---------------------------------------------------------------------
// This header defines a minimal lexer for a TreeMaker-like "tree template"
// syntax (no attribute braces). It recognizes:
//   - INDENT / DEDENT (Python-style indentation blocks)
//   - NEWLINE, EOF
//   - NAME (file or directory name)
//   - DIR_MARK (implicit: NAME ending with '/')
//   - COMMENT (text after '#')
//
// Error handling:
//   - Inconsistent indentation (DEDENT to a non-existing level)
//   - Illegal control character in name token
//   - Unterminated line (should not happen with normal text, guarded anyway)
//   - Generic unexpected characters
//
// Two operation modes via config:
//   - stop_on_first_error = true  -> stop tokenization on first error (default)
//   - stop_on_first_error = false -> continue lexing and collect errors
//
// All code is written in clean, modern C (C11) with clear, English comments.
// Licensed under MIT.

#ifndef LEXER_H
    #define LEXER_H

    #include <stddef.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/stat.h>
    #include <ctype.h>
    #include "utils.h"


    #ifdef __cplusplus
        extern "C" {
    #endif

    // ---------------- Token types ----------------
    typedef enum Lx_TokenType {
        T_INDENT = 1,
        T_DEDENT,
        T_NEWLINE,
        T_EOF,
        T_NAME,
        T_COMMENT
    } Lx_TokenType;

    // Token structure
    typedef struct Token {
        Lx_TokenType type;     // token kind
        char*        lexeme;   // malloc'ed string (NULL for punctuation-like tokens)
        size_t       length;   // bytes in lexeme
        int          line;     // 1-based
        int          column;   // 1-based (begin of token)
    } Token;

    // Free resources held by a token (safe to call on zeroed tokens)
    void token_free(Token* t);

    // Human-readable token name
    const char* token_type_name(Lx_TokenType t);

    // ---------------- Error types ----------------
    typedef enum LexErrorKind {
        LEX_OK = 0,
        LEX_ERR_UNEXPECTED_CHAR,      // stray/invalid character
        LEX_ERR_BAD_INDENT,           // indentation level not matching any prior level
        LEX_ERR_CONTROL_IN_NAME,      // control character within a NAME
        LEX_ERR_INTERNAL              // unexpected internal state
    } LexErrorKind;

    // Error record
    typedef struct LexError {
        LexErrorKind kind; // error category
        int line;             // source line
        int column;           // source column
        char* message;        // malloc'ed explanatory message (may be NULL)
    } LexError;

    // Free error
    void lex_error_free(LexError* e);

    // Format and print: "error: <kind> at line X, column Y: <message>"
    void lex_error_print(const LexError* e, const char* filename);

    // ---------------- Configuration ----------------
    typedef struct LexerConfig {
        int  tab_width;            // expand '\t' to this many spaces (default 4)
        bool emit_blank_newlines;  // if true, blank lines produce NEWLINE (default true)
        bool stop_on_first_error;  // default true
    } LexerConfig;

    // ---------------- Lexer state ----------------
    typedef struct IntStack {
        int*   data;
        size_t size;
        size_t cap;
    } IntStack;

    typedef struct Pending {
        Token tok;
        struct Pending* next;
    } Pending;

    typedef struct Lexer {
        const char* src;   // input buffer
        size_t      len;   // size in bytes
        size_t      i;     // current index
        int         line;  // 1-based
        int         col;   // 1-based

        bool        at_line_start;
        IntStack indents;        // stack of indentation column counts

        Pending* qh;             // pending tokens (INDENT/DEDENT queue)
        Pending* qt;

        LexerConfig cfg;         // configuration

        // Error reporting
        LexError*  errors;       // dynamic array of collected errors
        size_t        err_count;
        size_t        err_cap;
        bool          had_fatal;    // set if we should stop yielding tokens
    } Lexer;

    // ---------------- API ----------------
    void lexer_init(Lexer* L, const char* src, size_t len, const LexerConfig* cfg);
    void lexer_free(Lexer* L);
    int lexer_run_file(const char* filename, const LexerConfig* cfg);

    // Return next token. If a fatal error occurred and stop_on_first_error is true,
    // returns T_EOF immediately.
    Token lexer_next(Lexer* L);

    // Access collected errors (valid until lexer_free)
    size_t lexer_error_count(const Lexer* L);
    const LexError* lexer_errors(const Lexer* L);

    #ifdef __cplusplus
    }
    #endif

#endif // LEXER_H
