#include "parser.h"

expr* parse_expression() {
    return parse_assignment();
}

expr* parse_assignment() {
    expr* expr_node = parse_or();
    
    if (parser_match(EQUAL)) {
        token equals = parser_previous();
        expr* value = parse_assignment();
        
        if (expr_node->type == EXPR_LITERAL && expr_node->as.literal.value.type == IDENTIFIER) {
            expr* assign_expr = malloc(sizeof(expr));
            assign_expr->type = EXPR_ASSIGN;
            assign_expr->as.assign.name = expr_node->as.literal.value;
            assign_expr->as.assign.value = value;
            
            free(expr_node);
            return assign_expr;
        }
        
        fprintf(stderr, "[line %d] Invalid assignment target.\n", equals.line);
        fflush(stderr);
        exit(70);
    }
    
    return expr_node;
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
        expr* right = parse_equality();

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
    if (parser_match(IDENTIFIER)) {
        if (parser_peek().type == LEFT_PAREN) {
            expr* call = make_call_expr(parser_previous());

            parser_advance();

            int arg_count = 0;
            expr** arguments = NULL;

            if (!parser_check(RIGHT_PAREN)) {
                do {
                    expr* arg = parse_expression();
                    ++arg_count;
                    arguments = realloc(arguments, arg_count * sizeof(expr*));
                    arguments[arg_count - 1] = arg;
                } while (parser_match(COMMA));
            }

            if (!parser_match(RIGHT_PAREN)) {
                fprintf(stderr, "[line %d] Error at ')': Expect expression.", parser_previous().line);
                fflush(stderr);
                exit(65);                
            }

            call->as.call.arguments = arguments;
            call->as.call.arg_count = arg_count;

            return call;
        }
        else return make_literal_expr(parser_previous());
    }
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

