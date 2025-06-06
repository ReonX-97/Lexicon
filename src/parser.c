#define _POSIX_C_SOURCE 200809L

#include "parser.h"

parser psr;

expr* make_literal_expr(token value) {
    expr* e = malloc(sizeof(expr));
    e->type = EXPR_LITERAL;
    e->as.literal.value = value;
    return e;
}

expr* make_binary_expr(expr* left, token_type operator, expr* right) {
    expr* e = malloc(sizeof(expr));
    e->type = EXPR_BINARY;
    e->as.binary.left = left;
    e->as.binary.operator = operator;
    e->as.binary.right = right;
    return e;
}

expr* make_unary_expr(token_type operator, expr* operand) {
    expr* e = malloc(sizeof(expr));
    e->type = EXPR_UNARY;
    e->as.unary.operator = operator;
    e->as.unary.operand = operand;
    return e;
}

expr* make_grouping_expr(expr* expression) {
    expr* e = malloc(sizeof(expr));
    e->type = EXPR_GROUPING;
    e->as.grouping.expression = expression;
    return e;
}

void free_expr(expr* expression) {
    if (!expression) return;

    switch (expression->type) {
    case EXPR_BINARY:
        free_expr(expression->as.binary.left);
        free_expr(expression->as.binary.right);
        break;
    case EXPR_UNARY:
        free_expr(expression->as.unary.operand);
        break;
    case EXPR_GROUPING:
        free_expr(expression->as.grouping.expression);
        break;
    case EXPR_LITERAL:
        break;
    default:
        break;
    }
}

void print_expr(expr* expression) {
    if (!expression) {
        printf("NULL");
        return;
    }
    
    switch (expression->type) {
        case EXPR_LITERAL:
            if (expression->as.literal.value.value) {
                printf("%s", expression->as.literal.value.value);
            } else if (expression->as.literal.value.symbol) {
                printf("%s", expression->as.literal.value.symbol);
            }
            break;
        case EXPR_BINARY:
            printf("(");
            switch (expression->as.binary.operator) {
                case PLUS: printf("+ "); break;
                case MINUS: printf("- "); break;
                case STAR: printf("* "); break;
                case SLASH: printf("/ "); break;
                case EQUAL_EQUAL: printf("== "); break;
                case BANG_EQUAL: printf("!= "); break;
                case GREATER: printf("> "); break;
                case GREATER_EQUAL: printf(">= "); break;
                case LESS: printf("< "); break;
                case LESS_EQUAL: printf("<= "); break;
                case AND: printf("and "); break;
                case OR: printf("or "); break;
                default: printf("? "); break;
            }
            print_expr(expression->as.binary.left);
            printf(" ");
            print_expr(expression->as.binary.right);
            printf(")");
            break;
        case EXPR_UNARY:
            printf("(");
            printf("%s ", (expression->as.unary.operator == MINUS) ? "-" : "!");
            print_expr(expression->as.unary.operand);
            printf(")");
            break;
        case EXPR_GROUPING:
            printf("(group ");
            print_expr(expression->as.grouping.expression);
            printf(")");
            break;
    }
}

expr* parsed_token_expression(token_array *t_array) {
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

expr* parse_expression() {
    return parse_or();
}

expr* parse_or() {
    expr* left = parse_and();

    while (parser_match(OR)) {
        token_type operator = OR;
        expr* right = parse_and();

        left = make_binary_expr(left, operator, right);
    }

    return left;
    
}

expr* parse_and() {
    expr* left = parse_equality();

    while (parser_match(AND)) {
        token_type operator = AND;
        expr* right = parse_and();

        left = make_binary_expr(left, operator, right);
    }

    return left;
}

expr* parse_equality() {
    expr* left = parse_comparison();

    while (parser_match(EQUAL_EQUAL) || parser_match(BANG_EQUAL)) {
        token_type operator = parser_previous().type;
        expr* right = parse_comparison();
        
        left = make_binary_expr(left, operator, right);
    }

    return left;  
}

expr* parse_comparison() {
    expr* left = parse_term();

    while (parser_match(GREATER) || parser_match(GREATER_EQUAL) || 
        parser_match(LESS) || parser_match(LESS_EQUAL)) {
        
        token_type operator = parser_previous().type;
        expr* right = parse_term();
        
        left = make_binary_expr(left, operator, right);
    }

    return left; 
}

expr* parse_term() {
    expr* left = parse_factor();
    
    while (parser_match(MINUS) || parser_match(PLUS)) {
        token_type operator = parser_previous().type;
        expr* right = parse_factor();
        
        left = make_binary_expr(left, operator, right);
    }

    return left;
}

expr* parse_factor() {
    expr* left = parse_unary();
    
    while (parser_match(SLASH) || parser_match(STAR)) {
        token_type operator = parser_previous().type;
        expr* right = parse_unary();
        
        left = make_binary_expr(left, operator, right);
    }

    return left; 
}

expr* parse_unary() {
    if (parser_match(BANG) || parser_match(MINUS)) {
        token_type operator = parser_previous().type;
        expr* right = parse_unary();
        
        return make_unary_expr(operator, right);
    }

    return parse_primary(); 
}

expr* parse_primary() {
    if (parser_match(TRUE)) return make_literal_expr(parser_previous());
    if (parser_match(FALSE)) return make_literal_expr(parser_previous());
    if (parser_match(NIL)) return make_literal_expr(parser_previous());
    if (parser_match(NUMBER)) return make_literal_expr(parser_previous());
    if (parser_match(STRING)) return make_literal_expr(parser_previous());
    if (parser_match(IDENTIFIER)) return make_literal_expr(parser_previous());
    if (parser_match(LEFT_PAREN)) {
        expr* expression =  parse_expression();

        if (parser_match(RIGHT_PAREN)) return make_grouping_expr(expression);

        free_expr(expression);
        fprintf(stderr, "[line %d] Error at ')': Expect expression.", parser_previous().line);
        fflush(stderr);
        exit(65);
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