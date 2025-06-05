#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    // Syntactic Tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    SEMICOLON,
    TOKEN_EOF,
    ERROR,

    // Mathematical Operators
    MINUS,
    PLUS,
    STAR,
    SLASH,
    EQUAL,

    // Relational Operators
    EQUAL_EQUAL,
    BANG,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    // Data Types
    STRING,
    NUMBER,
    IDENTIFIER,

    //KEYWORDS
    AND, 
    CLASS, 
    ELSE, 
    FALSE, 
    FOR, 
    FUN, 
    IF, 
    NIL, 
    OR, 
    PRINT, 
    RETURN, 
    SUPER, 
    THIS, 
    TRUE, 
    VAR, 
    WHILE

} token_type;

typedef struct {
    token_type type;
    const char *start;
    int length;
    int line;
    char *symbol;
    char *value;
} token;

typedef struct {
    const char *start;
    const char *current;
    int line;
} scanner;

extern scanner scn;

char *read_file_contents(const char *filename);
void scanner_init(const char *source);
char advance();
char peek();
char peek_next();
int is_at_end();
int match(char expected);
token make_token(token_type type);
token make_token_with_value(token_type type, const char *value);
token error_token(const char *message);
token scan_token();
token string_token();
token number_token();
token identifier_keyword_token();
const char* token_type_to_string(token_type type);
int is_digit(char c);
int is_letter(char c);
void free_token(token *t);
