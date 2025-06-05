#define _POSIX_C_SOURCE 200809L

#include "parser.h"

parser psr;

char* parsed_token_expression(token_array *t_array) {
    init_parser(t_array);
    return parse_expression();
}

void init_parser(token_array *t_array) {
    psr.t_array = t_array;
    psr.current = 0;
}

int parser_is_at_end() {
    return parser_peek().type == TOKEN_EOF;
}

token parser_peek() {
    return psr.t_array->t[psr.current];
}

token parser_previous() {
    return psr.t_array->t[psr.current - 1];
}

token parser_advance() {
    if (parser_peek().type != TOKEN_EOF) psr.current++;
    return parser_previous();
}

int parser_check(token_type type) {
    if (parser_is_at_end()) return 0;
    else return parser_peek().type == type;
}

int parser_match(token_type type) {
    if (parser_check(type)) {
        parser_advance();
        return 1;
    } else return 0;
}

char* parse_expression() {
    return parse_or();
}

char* parse_or() {
    char* expr = parse_and();

    while (parser_match(OR)) {
        char* operator = strdup("or");
        char* right = parse_and();

        int len = strlen(expr) + strlen(operator) + strlen(right) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", operator, expr, right);

        free(expr);
        free(operator);
        free(right);

        expr = result;
    }

    return expr;
    
}

char* parse_and() {
    char* expr = parse_equality();

    while (parser_match(AND)) {
        char* operator = strdup("and");
        char* right = parse_and();

        int len = strlen(expr) + strlen(operator) + strlen(right) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", operator, expr, right);

        free(expr);
        free(operator);
        free(right);

        expr = result;
    }

    return expr;
}

char* parse_equality() {
    char* expr = parse_comparison();

    while (parser_match(EQUAL_EQUAL) || parser_match(BANG_EQUAL)) {
        token operator = parser_previous();
        char* op_str = (operator.type == BANG_EQUAL) ? "!=" : "==";
        char* right = parse_comparison();

        int len = strlen(expr) + strlen(op_str) + strlen(right) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", op_str, expr, right);

        free(expr);
        free(right);

        expr = result;
    }

    return expr;   
}

char* parse_comparison() {
    char* expr = parse_term();

    while (parser_match(GREATER) || parser_match(GREATER_EQUAL) || 
        parser_match(LESS) || parser_match(LESS_EQUAL)) {

        token operator = parser_previous();
        char* op_str;
        switch (operator.type) {
            case GREATER: op_str = ">"; break;
            case GREATER_EQUAL: op_str = ">="; break;
            case LESS: op_str = "<"; break;
            case LESS_EQUAL: op_str = "<="; break;
            default: op_str = "?"; break;
        }
        char* right = parse_term();

        int len = strlen(expr) + strlen(op_str) + strlen(right) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", op_str, expr, right);

        free(expr);
        free(right);

        expr = result;
    }
    
    return expr;
}

char* parse_term() {
    char* expr = parse_factor();
    
    while (parser_match(MINUS) || parser_match(PLUS)) {
        token operator = parser_previous();
        char* op_str = (operator.type == MINUS) ? "-" : "+";
        char* right = parse_factor();
        
        int len = strlen(expr) + strlen(right) + strlen(op_str) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", op_str, expr, right);
        
        free(expr);
        free(right);
        expr = result;
    }
    
    return expr;
}

char* parse_factor() {
    char* expr = parse_unary();
    
    while (parser_match(SLASH) || parser_match(STAR)) {
        token operator = parser_previous();
        char* op_str = (operator.type == SLASH) ? "/" : "*";
        char* right = parse_unary();
        
        int len = strlen(expr) + strlen(right) + strlen(op_str) + 10;
        char* result = malloc(len);
        snprintf(result, len, "(%s %s %s)", op_str, expr, right);
        
        free(expr);
        free(right);
        expr = result;
    }
    
    return expr;
}

char* parse_unary() {
    if (parser_match(BANG) || parser_match(MINUS)) {
        token operator = parser_previous();
        char* op_str = (operator.type == BANG) ? "!" : "-";
        char* right = parse_unary();

        int len = strlen(op_str) + strlen(right) + 10;
        char* result = malloc(len); 
        snprintf(result, len, "(%s %s)", op_str, right);

        free(right);
        return result;
    }

    return parse_primary();
}

char* parse_primary() {
    if (parser_match(TRUE)) return strdup("true");
    if (parser_match(FALSE)) return strdup("false");
    if (parser_match(NIL)) return strdup("nil");
    if (parser_match(NUMBER)) {
        token t = parser_previous();
        return strdup(t.value ? t.value : "0.0");
    }
    if (parser_match(STRING)) {
        token t = parser_previous();
        return strdup(t.value ? t.value : "");
    }
    if (parser_match(IDENTIFIER)) {
        token t = parser_previous();
        return strdup(t.symbol ? t.symbol : "identifier");
    }
    if (parser_match(LEFT_PAREN)) {
        char* expr =  parse_expression();

        if (parser_match(RIGHT_PAREN)) {
            int len = strlen(expr) + 10;
            char* result = malloc(len);

            snprintf(result, len, "(group %s)", expr);
            
            free(expr);
            return result;
        }

        free(expr);
        fprintf(stderr, "[line %d] Error at ')': Expect expression.", parser_previous().line);
        fflush(stderr);
        return NULL;
    }

    token current = parser_peek();
    if (current.type == TOKEN_EOF) {
        fprintf(stderr, "Error: Unexpected end of input\n");
    } else {
        fprintf(stderr, "[line %d] Error: Unexpected token '%s'\n", 
                current.line, current.symbol ? current.symbol : "unknown");
    }
    fflush(stderr);
    exit(65);
}