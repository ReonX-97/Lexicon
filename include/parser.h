#include "tokenizer_scanner.h"

typedef struct {
    token_array *t_array;
    int current;
} parser;

extern parser psr;

char* parsed_token_expression(token_array *t_array);
void init_parser(token_array *t_array);
int parser_is_at_end();
token parser_peek();
token parser_previous();
token parser_advance();
int parser_check(token_type type);
int parser_match(token_type type);
char* parse_expression(); // first call or if (){}[] then called
char* parse_or(); // or
char* parse_and(); // and
char* parse_equality(); // equal / not equal
char* parse_comparison(); // smaller/equal / greater/equal
char* parse_term(); // addition / substraction
char* parse_factor(); // multiiplication / division
char* parse_unary(); // negations
char* parse_primary(); //identifiers / booleans / nil






