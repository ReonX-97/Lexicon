#include "parser.h"

stmt* declaration() {
    if (parser_match(VAR)) return var_declaration();
    return statement();
}

stmt* statement() {
    if (parser_match(PRINT)) return print_statement();
    if (parser_match(LEFT_BRACE)) return block();
    if (parser_match(IF)) return if_statement();
    if (parser_match(WHILE)) return while_statement();
    if (parser_match(FOR)) return for_statement();
    if (parser_match(FUN)) return fun_def_statement();
    if (parser_match(RETURN)) return return_statement();

    return expression_statement();
}

stmt* expression_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_EXPR;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    output->as.expr_stmt_var.expression = parse_expression();

    if (!parser_match(SEMICOLON)) {
        fprintf(stderr, "[line %d] Expect ';' after expression.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    return output;
}

stmt* print_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_PRINT;
    
    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    output->as.print_stmt_var.expression = parse_expression();
    
    if (!parser_match(SEMICOLON)) {
        fprintf(stderr, "[line %d] Expect ';' after expression.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }
    
    return output;
}

stmt* var_declaration() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_VAR;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    if (!parser_match(IDENTIFIER)) {
        fprintf(stderr, "[line %d] No Identifier.", parser_peek().line);
        fflush(stderr); 
        exit(65);       
    }

    output->as.var_stmt_var.tkn = parser_previous();
    expr* initializer = NULL;

    if (parser_match(EQUAL)) initializer = parse_expression();

    if (!parser_match(SEMICOLON)) {
        fprintf(stderr, "[line %d] Expect ';' after expression.", parser_peek().line);
        fflush(stderr);
        exit(65); 
    }
    
    output->as.var_stmt_var.initializer = initializer;
    return output;
}


stmt* block() {
    int size = 0;
    int capacity = 1;
    stmt** statements = malloc(capacity * sizeof(stmt*));

    if (!statements) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    while (!parser_check(RIGHT_BRACE) && !parser_is_at_end()) {
        if (size >= capacity) {
            capacity = 2 * capacity;
            statements = realloc(statements, capacity * sizeof(stmt*));
        }

        statements[size++] = declaration();
    }

    if (parser_is_at_end()) {
        fprintf(stderr, "[line %d] Expected '}' after block.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }
    
    parser_advance();
    
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_BLOCK;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    output->as.blk_stmt_var.count = size;
    output->as.blk_stmt_var.statements = statements;

    return output;
}

stmt* if_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_IF;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }
    
    if (!parser_match(LEFT_PAREN)) {
        fprintf(stderr, "[line %d] Expected '(' after if.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    output->as.if_stmt_var.condition = parse_expression();

    if (!parser_match(RIGHT_PAREN)) {
        fprintf(stderr, "[line %d] Expected ')' after if condition.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    output->as.if_stmt_var.then_branch = declaration();

    if (parser_match(ELSE)) output->as.if_stmt_var.else_branch = declaration();
    else output->as.if_stmt_var.else_branch = NULL;

    return output;
}

stmt* while_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_WHILE;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }
    
    if (!parser_match(LEFT_PAREN)) {
        fprintf(stderr, "[line %d] Expected '(' after while.", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    output->as.while_stmt_var.condition = parse_expression();

    if (!parser_match(RIGHT_PAREN)) {
        fprintf(stderr, "[line %d] Expected ')' after while condition.", parser_peek().line);
        fflush(stderr);exit(70);
        exit(65);
    }

    output->as.while_stmt_var.body = declaration();

    return output;
}

stmt* for_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_FOR;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }
    
    if (!parser_match(LEFT_PAREN)) {
        fprintf(stderr, "[line %d] Expected '(' after for.", parser_peek().line);
        fflush(stderr);
        free_statement(output);
        exit(65);
    }

    if (parser_match(SEMICOLON)) {
        output->as.for_stmt_var.init_condition = NULL;
    } else if (parser_match(VAR)) {
        stmt* var_stmt = malloc(sizeof(stmt));
        if (!var_stmt) {
            fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
            fflush(stderr);
            free_statement(output);
            exit(70);
        }
        var_stmt->type = STMT_VAR;

        if (!parser_match(IDENTIFIER)) {
            fprintf(stderr, "[line %d] No Identifier.", parser_peek().line);
            fflush(stderr);
            free_statement(output); 
            exit(65);       
        }

        var_stmt->as.var_stmt_var.tkn = parser_previous();
        expr* initializer = NULL;

        if (parser_match(EQUAL)) initializer = parse_expression();

        if (!parser_match(SEMICOLON)) {
            fprintf(stderr, "[line %d] Expect ';' after variable declaration.", parser_peek().line);
            fflush(stderr);
            free_statement(output);
            exit(65); 
        }
        
        var_stmt->as.var_stmt_var.initializer = initializer;
        output->as.for_stmt_var.init_condition = var_stmt;
    } else {
        output->as.for_stmt_var.init_condition = expression_statement();
    }

    if (parser_check(SEMICOLON)) {
        output->as.for_stmt_var.loop_condition = NULL;
    } else {
        stmt* condition_stmt = malloc(sizeof(stmt));
        if (!condition_stmt) {
            fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
            fflush(stderr);
            free_statement(output);
            exit(70);
        }
        condition_stmt->type = STMT_EXPR;
        condition_stmt->as.expr_stmt_var.expression = parse_expression();
        output->as.for_stmt_var.loop_condition = condition_stmt;
    }
    
    if (!parser_match(SEMICOLON)) {
        fprintf(stderr, "[line %d] Expected ';' after for condition.", parser_peek().line);
        fflush(stderr);
        free_statement(output);
        exit(65);
    }
    
    if (parser_check(RIGHT_PAREN)) output->as.for_stmt_var.change = NULL;
    else {
        stmt* change_stmt = malloc(sizeof(stmt));
        if (!change_stmt) {
            fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
            fflush(stderr);
            free_statement(output);
            exit(70);
        }
        change_stmt->type = STMT_EXPR;
        change_stmt->as.expr_stmt_var.expression = parse_expression();
        output->as.for_stmt_var.change = change_stmt;
    }
    
    if (!parser_match(RIGHT_PAREN)) {
        fprintf(stderr, "[line %d] Expected ')' after for clauses.", parser_peek().line);
        fflush(stderr);
        free_statement(output);
        exit(65);
    }

    output->as.for_stmt_var.body = statement();

    return output;    
}

stmt* fun_def_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_FUN_DEF;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    if (!parser_match(IDENTIFIER)) {
        fprintf(stderr, "[line %d] Identifier Required", parser_peek().line);
        fflush(stderr);
        exit(65);        
    } else output->as.fun_def_stmt_var.name = parser_previous();

    if (parser_match(LEFT_PAREN)) {
        output->as.fun_def_stmt_var.parameters = NULL;
        output->as.fun_def_stmt_var.param_count = 0;

        while(!parser_check(RIGHT_PAREN) && !parser_is_at_end()) {
            if (!parser_match(IDENTIFIER)) {
                fprintf(stderr, "[line %d] Expected parameter name", parser_peek().line);
                fflush(stderr);
                exit(65);
            }

            output->as.fun_def_stmt_var.param_count++;
            output->as.fun_def_stmt_var.parameters = realloc(
                    output->as.fun_def_stmt_var.parameters,
                    output->as.fun_def_stmt_var.param_count * sizeof(token)
                );

            if (!output->as.fun_def_stmt_var.parameters) {
                fprintf(stderr, "[line %d] Memory allocation failed for parameters", parser_peek().line);
                fflush(stderr);
                exit(70);
            }

            output->as.fun_def_stmt_var.parameters[output->as.fun_def_stmt_var.param_count - 1] = parser_previous();

            if (!parser_match(COMMA)) break;
        }

        if (!parser_match(RIGHT_PAREN)) {
            fprintf(stderr, "[line %d] Expected ')' after parameters", parser_peek().line);
            fflush(stderr);
            exit(65);
        }
    } else {
        fprintf(stderr, "[line %d] Expected '(' after function name", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    if (parser_match(LEFT_BRACE)) output->as.fun_def_stmt_var.body = block();
    else if (parser_match(SEMICOLON)) output->as.fun_def_stmt_var.body = NULL;
    else {
        fprintf(stderr, "[line %d] Expected ';' before function declaration", parser_peek().line);
        fflush(stderr);
        exit(65);
    }

    return output;
}

stmt* return_statement() {
    stmt* output = malloc(sizeof(stmt));
    output->type = STMT_RETURN;

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    if (parser_match(SEMICOLON)) {
        output->as.return_stmt_var.return_value = NULL;
        return output;
    }

    output->as.return_stmt_var.return_value = parse_expression();

    if (!parser_match(SEMICOLON)) {
        fprintf(stderr, "[line %d] Expect ';' after expression.", parser_peek().line);
        fflush(stderr);
        exit(65); 
    }

    return output;
}