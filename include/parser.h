#include "tokenizer_scanner.h"

typedef enum {
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_GROUPING
} expr_type;

typedef struct expr expr;

typedef struct {
    expr* left;
    token_type operator;
    expr* right;
} binary_expr;

typedef struct {
    token_type operator;
    expr* operand;
} unary_expr;

typedef struct {
    token value;
} literal_expr;

typedef struct {
    expr* expression;
} grouping_expr;

struct expr {
    expr_type type;
    union {
        literal_expr literal;
        binary_expr binary;
        unary_expr unary;
        grouping_expr grouping;
    } as;
};

typedef struct {
    token_array *t_array;
    int current;
} parser;

extern parser psr;

expr* parsed_token_expression(token_array *t_array);
void init_parser(token_array *t_array);
int parser_is_at_end();
token parser_peek();
token parser_previous();
token parser_advance();
int parser_check(token_type type);
int parser_match(token_type type);

expr* parse_expression(); // first call or if (){}[] then called
expr* parse_or(); // or
expr* parse_and(); // and
expr* parse_equality(); // equal / not equal
expr* parse_comparison(); // smaller/equal / greater/equal
expr* parse_term(); // addition / substraction
expr* parse_factor(); // multiiplication / division
expr* parse_unary(); // negations
expr* parse_primary(); //identifiers / booleans / nil

expr* make_literal_expr(token value);
expr* make_binary_expr(expr* left, token_type operator, expr* right);
expr* make_unary_expr(token_type operator, expr* operand);
expr* make_grouping_expr(expr* expression);
void free_expr(expr* expression);
void print_expr(expr* expression);