#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

/* Lexing = String -> Tokens */
enum token_type {
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

// increment pointer by one byte
#define EAT_C(string) string++
#define PEEK_C(string) string[0]

struct token **lex_string(char* string) {
    int capacity = 4;
    int len = 0;
    struct token **list = malloc(sizeof(struct token*) * capacity);
    
    while (*string != '\0') {
        assert(len <= capacity);
        
        struct token *new_token = malloc(sizeof(struct token));
        
        // Parse a paren
        switch (PEEK_C(string)) {
        case '(':
            // Eat the string
            EAT_C(string);
            // Create new token
            new_token->type = LPAREN_TOK;
            // Add token to list
            if (len == capacity) {
                capacity = capacity + capacity; // :P
                list = realloc(list, sizeof(struct token*) * capacity);
            }
            list[len++] = new_token;
            continue;
        case ')':
            // Eat the string
            EAT_C(string);
            // Create new token
            new_token->type = RPAREN_TOK;
            // Add token to list
            if (len == capacity) {
                capacity = capacity + capacity; // :P
                list = realloc(list, sizeof(struct token*) * capacity);
            }
            list[len++] = new_token;
            continue;
        }
        
        // Parse a integer
        if ('0' <= PEEK_C(string) && PEEK_C(string) <= '9') {
            long long int tmp_int = 0;
            while ('0' <= PEEK_C(string) && PEEK_C(string) <= '9') {
                tmp_int *= 10;
                tmp_int += PEEK_C(string) - '0';
                EAT_C(string);
            }
            // Create new token
            new_token->type = INT_TOK;
            new_token->integer = tmp_int;
            // Add token to list
            if (len == capacity) {
                capacity = capacity + capacity; // :P
                list = realloc(list, sizeof(struct token*) * capacity);
            }
            list[len++] = new_token;
            continue;
        }

        // Parse a symbol
        if (PEEK_C(string) != ' ' && PEEK_C(string) != '\n') {
            char* symbol_start = string;
            while (PEEK_C(string) != ' ' && PEEK_C(string) != '\n' &&
                   PEEK_C(string) != '(' && PEEK_C(string) != ')' && PEEK_C(string) != '\0') {
                EAT_C(string);
            }
            int length = (string - symbol_start) + 1;
            char* symbol = malloc(sizeof(char) * length);
            
            // Create new token
            new_token->type = SYMBOL_TOK;
            new_token->symbol = symbol;
            // Add token to list
            if (len == capacity) {
                capacity = capacity + capacity; // :P
                list = realloc(list, sizeof(struct token*) * capacity);
            }
            list[len++] = new_token;
            continue;
        }
    }
    
    if (len == capacity) {
        capacity = capacity + capacity; // :P
        list = realloc(list, sizeof(struct token*) * capacity);
    }

    list[len] = NULL;

    return list;
}

/* Parsing = Tokens -> AST (Abstract Syntax Tree) */

// "Discriminator"
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

struct sexp parse_sexp(struct token ***tokens) {
    struct sexp new_sexp;
    enum token_type first_token_type = (*tokens)[0]->type;
    
    switch (first_token_type) {
    case LPAREN_TOK:
        new_sexp.type = LIST;
        
        (*tokens)++;
        
        int len = 0;
        int capacity = 4;
        struct sexp *list = malloc(sizeof(struct sexp) * capacity);
        
        while ((*tokens)[0]->type != RPAREN_TOK) {
            if (len == capacity) {
                capacity *= 2;
                list = realloc(list, sizeof(struct sexp) * capacity);
            }
            list[len++] = parse_sexp(tokens);
        }
        
        if (len == capacity) {
            capacity *= 2;
            list = realloc(list, sizeof(struct sexp) * capacity);
        }
        list[len].type = END_OF_LIST;
        
        new_sexp.list = list;
        return new_sexp;
    case INT_TOK:
        new_sexp.type = INTEGER;
        new_sexp.integer = (*tokens)[0]->integer;
        (*tokens)++; // Everywhere, drop the first token
        return new_sexp;
    case SYMBOL_TOK:
        new_sexp.type = SYMBOL;
        new_sexp.symbol = (*tokens)[0]->symbol;
        (*tokens)++;
        return new_sexp;
    default:
        // WHAT THE FUCK
        assert(0);
    }
}

/*
A program is a list of s-expressions. s-expressions are (lists of elements) or symbols or numbers

parse(tokens) parses an entire program by parsing sexp until it reaches a NULL
*/
struct sexp *parse(struct token ***tokens) {
    int len = 0;
    int capacity = 4;
    struct sexp *list = malloc(sizeof(struct sexp) * capacity);
    
    while ((*tokens)[0] != NULL) {
        if (len == capacity) {
            capacity *= 2;
            list = realloc(list, sizeof(struct sexp) * capacity);
        }
        list[len++] = parse_sexp(tokens);
    }
    
    if (len == capacity) {
        capacity *= 2;
        list = realloc(list, sizeof(struct sexp) * capacity);
    }
    list[len].type = END_OF_LIST;
    
    return list;
}

void print_ast_node(struct sexp ast_node) {
    switch (ast_node.type) {
        case INTEGER:
            printf("INTEGER(%lld)", ast_node.integer);
            break;
        case SYMBOL:
            printf("SYMBOL(%s)", ast_node.symbol);
            break;
        case LIST:
            printf("LIST(\n");
            struct sexp *list = ast_node.list;
            while (list->type != END_OF_LIST) {
                print_ast_node(*list);
                printf(",");
                list++;
            }
            printf("\n)");
            break;
    }
}

int main() {
    struct token **lexed_string = lex_string("(+ 1 2)");
    struct sexp *parsed = parse(&lexed_string);
    
    print_ast_node(parsed[0]);
}

/*

() <= empty list

(a b c) <= symbols a, b, and c
(+ 1 (+ 1 3))

(1 3 5)
(a 1 3 5)

================
| Jake's Notes |
================

Recursion vs Looping?
First time implementing a lisp?
Compare lisp parsing to other language parsing.
Learn about the other stages in compiling.
How might lisp deal with functions and the such for parsing.

================
Usually you lex a token and then parse a token all at the same time





*/
