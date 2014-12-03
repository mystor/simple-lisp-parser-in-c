#include<stdio.h>

enum token_type {
    EOF_TOK = -1,

    LPAREN_TOK = 0,
    RPAREN_TOK = 1,
    INT_TOK = 2,
    SYMBOL_TOK = 3
};

struct token {
    enum token_type type;

    union {
        long long int integer;
        char *symbol;
    };
};

struct p_state {
    // The file to read chars from
    FILE *file;
    // The current token buffer
    struct token tok;
};

struct p_state new_p_state(FILE *file);

enum sexp_type {
    LIST = 0,
    SYMBOL = 1,
    INTEGER = 2,

    END_OF_LIST = -1
};

struct sexp {
    enum sexp_type type;

    union {
        struct sexp *list; // NULL-terminated
        char *symbol;
        long long int integer;
    };
};

struct sexp *parse(struct p_state *state);

void print_ast_node(struct sexp ast_node);
