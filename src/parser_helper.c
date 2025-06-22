#include "parser.h"

parser psr;

expr* parsed_token_expression(token_array *t_array) {
    init_parser(t_array);
    return parse_expression();
}

parse_results* parse(token_array *t_array) {
    init_parser(t_array);
    
    int size = 0;
    int capacity = 64;
    stmt** statements = malloc(capacity * sizeof(stmt*));

    if (!statements) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    while (!parser_is_at_end()) {
        if (size >= capacity) {
            capacity = 2 * capacity;
            stmt** originals = statements;
            statements = realloc(statements, capacity * sizeof(stmt*));

            if (!statements) {
                fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
                fflush(stderr);
                free_statements(originals);
                exit(70);
            }
        }

        statements[size++] = declaration(); 
    }

    if (size >= capacity) statements = realloc(statements, (capacity + 1) * sizeof(stmt*));
    statements[size++] = NULL;

    parse_results* output = malloc(sizeof(parse_results));

    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", parser_peek().line);
        fflush(stderr);
        exit(70);
    }

    output->count = size;
    output->statements = statements;

    return output;
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

token parser_peek_next() {
    if (!parser_is_at_end()) return psr.t_array->t[psr.current + 1];
    else return error_token("At the End");
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

expr* make_assign_expr(token name, expr* value) {
    expr* e = malloc(sizeof(expr));
    e->type = EXPR_ASSIGN;
    e->as.assign.name = name;
    e->as.assign.value = value;
    return e;
}

expr* make_call_expr(token value) {
    expr* output = malloc(sizeof(expr));
    if (!output) {
        fprintf(stderr, "[line %d] Memory allocation failed", value.line);
        fflush(stderr);
        exit(70);
    }
    
    output->type = EXPR_CALL;
    
    output->as.call.callee = malloc(sizeof(expr));
    if (!output->as.call.callee) {
        fprintf(stderr, "[line %d] Memory allocation failed for callee", value.line);
        fflush(stderr);
        free(output);
        exit(70);
    }
    
    output->as.call.callee->type = EXPR_LITERAL;
    output->as.call.callee->as.literal.value = value;
    
    output->as.call.arguments = NULL;
    
    return output;
}

void free_expr(expr* expression) {
    if (!expression) return;

    switch (expression->type) {
        case EXPR_ASSIGN:
            free_expr(expression->as.assign.value);
            break;
        case EXPR_CALL:
            free_expr(expression->as.call.callee);
            for (int i = 0; i < expression->as.call.arg_count; i++) 
                free_expr(expression->as.call.arguments[i]);
            break;
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

    free(expression);
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
        case EXPR_ASSIGN:
            printf("(");
            printf(expression->as.assign.name.symbol);
            printf(" = ");
            print_expr(expression->as.assign.value);
            printf(")");
            break;
        case EXPR_CALL:
            break;

    }
    fflush(stdout);
}

void free_statement(stmt* statement) {
    if (!statement) return;

    switch (statement->type) {
        case STMT_EXPR:
            free_expr(statement->as.expr_stmt_var.expression);
            break;

        case STMT_PRINT:
            free_expr(statement->as.print_stmt_var.expression);
            break;

        case STMT_VAR:
            free_expr(statement->as.var_stmt_var.initializer);
            break;

        case STMT_BLOCK:
            for (int i = 0; i < statement->as.blk_stmt_var.count; i++) {
                free_statement(statement->as.blk_stmt_var.statements[i]);
            }
            free(statement->as.blk_stmt_var.statements);
            break;

        case STMT_IF:
            free_expr(statement->as.if_stmt_var.condition);
            free_statement(statement->as.if_stmt_var.then_branch);
            free_statement(statement->as.if_stmt_var.else_branch);
            break;

        case STMT_WHILE:
            free_expr(statement->as.while_stmt_var.condition);
            free_statement(statement->as.while_stmt_var.body);
            break;

        case STMT_FOR:
            free_statement(statement->as.for_stmt_var.init_condition);
            free_statement(statement->as.for_stmt_var.loop_condition);
            free_statement(statement->as.for_stmt_var.change);
            free_statement(statement->as.for_stmt_var.body);
            break;

        case STMT_FUN_DEF:
            free_token(&(statement->as.fun_def_stmt_var.name));
            for (int i = 0; i < statement->as.fun_def_stmt_var.param_count; i++) {
                free_token(&(statement->as.fun_def_stmt_var.parameters[i]));
            }
            free_statement(statement->as.fun_def_stmt_var.body);
            break;

        case STMT_RETURN:
            free_expr(statement->as.return_stmt_var.return_value);
            break;
    }
    free(statement);
}

void free_statements(stmt** statements) {
    int i = 0;
    while (statements[i] != NULL) free_statement(statements[i++]);

    free(statements);
    
    return;
}
