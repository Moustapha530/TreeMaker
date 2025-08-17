# TreeMaker

TreeMaker is a small command-line tool, written in C (C11), that reads a simple "tree template" (.trm) file describing a filesystem layout and creates the corresponding directories and files on disk. The codebase contains a lexer, parser, in-memory tree structure, filesystem builder and command-line argument handling.

This README summarizes the components found in the supplied source files, how they work together, how to build/run and test, and issues/improvements observed from the code.

---

Table of contents
- Project overview
- Project components (files & responsibilities)
- Public APIs and data types
- How it works (workflow)
- Usage examples
- Build & test instructions
- Observed issues and recommendations
- Contribution ideas
- License

---

Project overview
----------------
TreeMaker parses a simple, indentation-based text format describing trees of files and directories and constructs that structure in the filesystem. It uses:

- A lexer to break input into tokens (indentation-aware).
- A parser which interprets indentation and names to build an in-memory tree.
- A builder which traverses the tree to create directories and files.
- Utility code for string duplication, filesystem operations, and command-line arguments.

This README reflects the code you provided: `lexer.h/.c`, `parser.c`, `treeMaker.c`, `builder.c`, `fs.c`, `args.c`, `utils.c`, `main.c` and supporting headers.

Project components
------------------
- main.c
  - CLI driver. Parses arguments, sets a lexer config, runs the lexer on each input file, parses to an in-memory tree, builds files/folders, and frees resources.

- args.c / args.h
  - Handles command-line argument parsing and storage (`Args`), defaulting destination path to the current working directory.
  - Functions: `init_args`, `add_input_file`, `free_args`, and `parse_args` (partial in your copy).

- lexer.h / lexer.c
  - Indentation-aware lexer that recognizes tokens like `T_INDENT`, `T_DEDENT`, `T_NEWLINE`, `T_EOF`, `T_NAME`, and `T_COMMENT`.
  - Tracks line/column numbers, supports configurable tab width, can optionally emit blank newlines, and can either stop on first error or accumulate errors.
  - Public functions: `lexer_init`, `lexer_next`, `lexer_free`, `lexer_run_file`, error accessors and token/error helper functions.
  - Internals include an indentation stack, a pending token queue for virtual INDENT/DEDENT tokens, and error collection.

- parser.c / parser.h
  - Reads an input file line-by-line and builds an in-memory `Tree` (nodes created by `new_tree`, attached via `attach_child`).
  - Uses indentation counting (groups of 4 spaces or tabs) and normalizes excessive jumps.
  - Strips comments and leading whitespace/hyphens and maintains an expanding stack of current nodes per level.

- treeMaker.c / treeMaker.h
  - Implementation of the in-memory tree: node creation (`new_tree`), attaching children (`attach_child`), printing (`print_tree`) and cleaning (`clean_tree`).
  - Nodes hold `path` (string without trailing slash for directories), `is_directory` flag, `children` array and `child_count`, plus parent pointer.

- builder.c / builder.h
  - Traverses the `Tree` and creates the directories and files on disk.
  - Functions include:
    - `build_full_path` — builds a PATH_MAX buffer combining base path and node path.
    - `build_directories_only` — creates only directories.
    - `build_directory_recursive` — recursive directory creation.
    - `build_file_recursive` — recursive file creation (partial in provided snippet).
  - Uses `create_folder` and `create_file` from `fs.c`.

- fs.c / fs.h
  - Platform-sensitive wrappers for filesystem operations:
    - `create_folder` — creates a directory (POSIX uses `mkdir`, Windows uses `_mkdir`).
    - `create_file` — creates a file (POSIX uses `open` with `O_CREAT|O_WRONLY`, Windows uses `fopen("w")`).

- utils.c / utils.h
  - Utility helpers. Provided: `_strndup` — portable version of strndup (duplicates up to n bytes, NUL-terminates).

- errors.c / error.h (partial)
  - Placeholder; in your copy `errors.c` includes `error.h` but contains no implementation.

Public APIs and data types
--------------------------
(Names inferred from code excerpts)

- Token and lexer API (lexer.h/c)
  - Types: `Lx_TokenType`, `Token`
  - Lexer struct: `Lexer`, `LexerConfig`
  - Functions: `lexer_init(Lexer*, const char*, size_t, const LexerConfig*)`, `Token lexer_next(Lexer*)`, `lexer_free(Lexer*)`, `lexer_run_file(const char*, const LexerConfig*)`, error helpers.

- Tree API (treeMaker.h / treeMaker.c)
  - Type alias `Tree` (pointer to `TreeNode`)
  - Functions: `Tree new_tree(const char*)`, `Tree attach_child(Tree, const char*)`, `bool is_empty_tree(Tree)`, `void print_tree(Tree, int)`, `void clean_tree(Tree*)`.

- Builder API (builder.h / builder.c)
  - `int build_directories_only(const Tree, const char*)`, `int build_directory_recursive(const Tree, const char*)`, `int build_file_recursive(const Tree, const char*)`.

- FS API (fs.h / fs.c)
  - `int create_folder(const char*)`, `int create_file(const char*)`.

- Args API (args.h / args.c)
  - `int init_args(Args*)`, `int add_input_file(Args*, const char*)`, `void free_args(Args*)`, `int parse_args(int, char**, Args*)`.

How it works (workflow)
-----------------------
1. CLI parsing (main.c):
   - `parse_args()` builds `Args` which contains input filenames and destination path.
   - `main` sets a `LexerConfig` (tab width 4, emit blank newlines true, do not stop on first error).

2. For each input file:
   - `lexer_run_file()` (wrapper) runs the lexer across the file. The lexer collects tokens and errors; exact behavior depends on `lexer_run_file` implementation (not fully shown but declared).
   - `parse_file()` opens the file, reads it line-by-line, strips comments and leading whitespace, computes indentation levels, and builds a `Tree` with `new_tree` and `attach_child`.
   - `build_tree()` (used in `main.c`) traverses the returned tree and calls filesystem helpers to create directories and files at the destination path. In your code, builder functions such as `build_directory_recursive` and `build_file_recursive` provide that behavior (ensure `build_tree` calls them in the correct order).
   - `clean_tree()` frees the tree.

3. After all files processed, `free_args()` cleans up CLI memory.

Usage examples
--------------
Build and run examples below assume you supply a `Makefile` or compile manually and that all header files and missing implementations are present.

- Example .trm input (simple.trm):
  ```
  project/
      README.md
      src/
          main.c
          util.c
      docs/
          guide.md
  ```

- Example run:
  ```
  ./treemaker -t simple.trm -d /tmp/myproject
  ```

This should create the directory `/tmp/myproject/project` and subdirectories and files described (depending on builder implementation and which builder function `build_tree` invokes).

Build & test instructions
-------------------------
1. Ensure you have all header files and implementations:
   - lexer.h, lexer.c
   - parser.h, parser.c
   - treeMaker.h, treeMaker.c
   - builder.h, builder.c
   - fs.h, fs.c
   - args.h, args.c
   - utils.h, utils.c
   - main.c
   - any missing pieces (e.g., `error.h`, `errors.c` content, and the remainder of `parse_args` from args.c if cut)

2. Compile with a C11 compiler:
   - Example using gcc:
     ```
     gcc -std=c11 -O2 -Wall -Wextra -Wpedantic \
         main.c args.c lexer.c parser.c treeMaker.c builder.c fs.c utils.c \
         -o treemaker
     ```
   - On POSIX you may need to link with -D_POSIX_C_SOURCE or include relevant headers; on Windows adjust compile/link steps.

3. Run tests:
   - Create small `.trm` files and run the program.
   - Test edge cases (nonexistent destination, bad permissions, indentation errors).

Testing suggestions
-------------------
- Basic structure creation
  - A file with nested directories and files; verify directories are created before files.
- Indentation edge cases
  - Jumping indentation (e.g., from 1 to 3 levels) to see warning and normalization.
  - Misaligned spaces (leftover spaces after groups of 4) to trigger the warning in `count_indent`.
- Comment handling
  - Lines with `#` anywhere should have trailing comment removed.
- Error handling
  - Introduce bad input (control characters in names, inconsistent dedents) and inspect errors collected by the lexer and messages printed by the parser.
- Memory & leak checks
  - Run under Valgrind or AddressSanitizer to detect leaks or invalid reads/frees.

Observed issues, missing pieces and recommendations
--------------------------------------------------
The code generally describes a workable pipeline. Below are points observed in your supplied files that you may want to fix or improve:

1. Partial / missing files:
   - `errors.c` contains only `#include "error.h"` and no implementation.
   - `args.c` was cut in the middle of `parse_args` in your pasted content; ensure the rest is present.
   - `lexer.c` ended mid-function in the paste; ensure the remainder of `next_core` and `lexer_next`, `lexer_run_file` are implemented and compiled.
   - `builder.c`'s `build_file_recursive` function is truncated in the snippet; ensure full implementation exists.

2. Message wording and consistency:
   - Some `fprintf` messages have typos and inconsistent phrasing (e.g., "attocation", "courant", "failed to create file" while creating a directory on POSIX). Polish messages for clarity.
   - `create_folder` on POSIX prints "failed to create file" — change to "directory".

3. Memory handling and ownership:
   - `attach_child` reallocs a child node's `path` to PATH_MAX and then formats it with parent + separator + name; this will work but overwrites the previously allocated pointer. Ensure `clean_tree` will free the final pointer (it does).
   - Many functions allocate PATH_MAX-sized buffers; consider using dynamic sizing or checking buffer overflows.
   - `treeMaker.c`'s `new_tree` prints an error and returns NULL on invalid input but may leak partially allocated memory in some branches — be careful.

4. Error handling approach:
   - `lexer` supports collecting multiple errors but `main` sets `stop_on_first_error=false` (so lexer will continue). That is OK, but ensure `lexer_run_file` returns meaningful non-zero status on fatal conditions (implementation-dependent).
   - Where the parser uses `exit(EXIT_FAILURE)` on unrecoverable errors, prefer bubbling up errors to the caller and allowing `main` to decide. This makes testing and library usage easier.

5. Portability:
   - `fs.c` uses platform-specific APIs. Already handled with `#ifdef _WIN32`, but be careful to include the correct headers on each platform (`direct.h` for `_mkdir` on Windows, `sys/stat.h` and `unistd.h` for POSIX, etc.).

6. Input handling:
   - The parser treats leading '-' as whitespace to skip; that is likely intended to accept list-style lines but should be documented in README and validated.

7. Lexer/parser integration:
   - The codebase contains both a full lexer implementation and a parser that currently reads files line-by-line directly (using `fgets` and indentation counting) rather than consuming tokens from `lexer_next`. Decide whether to use the lexer-driven parsing or the simple line-based parser — both exist in the repository. If you plan to use the lexer API, implement a parser that consumes `lexer_next()` tokens instead of manual file scanning.

8. Thread-safety / Reentrancy:
   - Lexer and Tree objects are not thread-safe; document this if you plan concurrent usage.

Recommended next steps
----------------------
- Finish/restore the missing/truncated implementations:
  - Complete `lexer.c` (finish `next_core` and expose `lexer_next`, `lexer_run_file`).
  - Complete `args.c::parse_args`.
  - Complete builder functions and `errors.c`.
- Add header files and ensure consistent signatures and includes.
- Fix message typos and unify logging format.
- Add unit tests:
  - Small lexer tests, parser tests (line-by-line), and end-to-end tests creating and verifying filesystem changes in a temporary directory.
- Add a Makefile or CMake project to make building and running easier.
- Consider adding a dry-run mode so users can inspect what would be created without touching the filesystem.
- Add more robust path handling (use `realpath`, ensure path joining handles trailing separators, etc.)

License
-------
Include your chosen license (the lexer comments suggested MIT). Add a top-level LICENSE file if you want to publish this project.

Contributing
------------
If you want, I can:
- Finish the missing implementations (`lexer_next`, `lexer_run_file`, `parse_args`, `build_file_recursive`, `errors.c`).
- Provide a Makefile and example tests with expected outputs.
- Implement a unit-test harness and CI (GitHub Actions) for builds and memory checks.

Tell me which of those you want next and I will prepare the code or additional documentation.

