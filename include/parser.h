#define _POSIX_C_SOURCE 200809L

#include "tokenizer_scanner.h"

typedef enum {
    EXPR_ASSIGN,
    EXPR_LITERAL,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_GROUPING
} expr_type;

typedef enum {
    STMT_EXPR,
    STMT_PRINT,
    STMT_VAR,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_RETURN
} stmt_type;

typedef struct expr expr;
typedef struct stmt stmt;

typedef struct {
    token name;
    expr* value;
} assign_expr;

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
        assign_expr assign;
        literal_expr literal;
        binary_expr binary;
        unary_expr unary;
        grouping_expr grouping;
    } as;
};

typedef struct {
    expr* expression;
} expr_stmt;

typedef struct {
    expr* expression;
} print_stmt;

typedef struct {
    token tkn;
    expr* initializer;
} var_stmt;

typedef struct {
    stmt** statements;
    int count;
} blk_stmt;

typedef struct {
    expr* condition;
    stmt* then_branch;
    stmt* else_branch;
} if_stmt;

typedef struct {
    expr* condition;
    stmt* body;
} while_stmt;

typedef struct {
    stmt* init_condition;
    stmt* loop_condition;
    stmt* change;
    stmt* body;
} for_stmt;

struct stmt {
    stmt_type type;
    union {
        expr_stmt expr_stmt_var;
        print_stmt print_stmt_var;
        var_stmt var_stmt_var;
        blk_stmt blk_stmt_var;
        if_stmt if_stmt_var;
        while_stmt while_stmt_var;
        for_stmt for_stmt_var;
    } as;
};

typedef struct {
    token_array *t_array;
    int current;
} parser;

typedef struct {
    stmt** statements;
    int count;
} parse_results;

extern parser psr;

expr* parsed_token_expression(token_array *t_array);
void init_parser(token_array *t_array);
int parser_is_at_end();
token parser_peek();
token parser_previous();
token parser_advance();
int parser_check(token_type type);
int parser_match(token_type type);

parse_results* parse(token_array *t_array);

expr* parse_expression(); // first call or if (){}[] then called
expr* parse_or(); // or
expr* parse_and(); // and
expr* parse_equality(); // equal / not equal
expr* parse_comparison(); // smaller/equal / greater/equal
expr* parse_term(); // addition / substraction
expr* parse_factor(); // multiiplication / division
expr* parse_unary(); // negations
expr* parse_primary(); // identifiers / booleans / nil

expr* make_literal_expr(token value);
expr* make_binary_expr(expr* left, token_type operator, expr* right);
expr* make_unary_expr(token_type operator, expr* operand);
expr* make_grouping_expr(expr* expression);
expr* parse_assignment();
expr* make_assign_expr(token name, expr* value);
void free_expr(expr* expression);
void print_expr(expr* expression);

stmt* declaration();
stmt* statement();
stmt* var_declaration();
stmt* print_statement();
stmt* expression_statement();
stmt* block();
stmt* if_statement();
stmt* while_statement();
stmt* for_statement();
void free_statement(stmt* statement);
void free_statements(stmt** statements);