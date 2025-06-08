#include "execute.h"

void interpret(parse_results* parsed_tokens) {
    stmt** statements = parsed_tokens->statements;
    if (!init_universe()) {
        fprintf(stderr, "Universe Not Initialized");
        fflush(stderr);
        exit(70);
    }

    for (int i = 0; statements[i] != NULL; i++)  
        execute_statement(statements[i]);
}

void execute_statement(stmt* statement) {
    switch (statement->type) {
    case STMT_EXPR:
        evaluate_expression(statement->as.expr_stmt_var.expression);
        break;
    
    case STMT_PRINT:
        print_token(
            evaluate_expression(statement->as.print_stmt_var.expression)
        );
        break;

    case STMT_VAR:
        token value;
        if (statement->as.var_stmt_var.initializer != NULL) {
            value = evaluate_expression(statement->as.var_stmt_var.initializer);
        } else {
            value.type = NIL;
            value.value = "nil";
            value.symbol = "nil";
        }
        define_variable(statement->as.var_stmt_var.tkn.symbol, value);
        break;

    case STMT_BLOCK:
        execute_block(statement->as.blk_stmt_var.statements,
                    statement->as.blk_stmt_var.count);
        break;

    case STMT_IF:
        execute_if(statement);  
        break;

    case STMT_WHILE:
        execute_while(statement);
        break;

    case STMT_FOR:
        execute_for(statement);
        break;

    default:
        break;
    }
}

void print_token(token tkn) {
    switch (tkn.type) {
        case NUMBER:
        case STRING:
            printf("%s\n", tkn.value);
            break;
        case TRUE: printf("true\n"); break;
        case FALSE: printf("false\n"); break;
        case NIL: printf("nil\n"); break;
        default: printf("Unable to print.\n"); break;
    }
    fflush(stdout);
}

void define_variable(char* name, token value) {
    if (!name) return;
    
    // Only check the current environment (top of stack) for redefinition
    environment* current_env = univ->env[univ->count - 1];
    
    // Check if variable already exists in current scope
    for (int j = 0; j < current_env->count; j++) {
        if (strcmp(current_env->vars[j].name, name) == 0) {
            // Variable exists in current scope, update it
            // Don't free the old token since tokens are managed elsewhere
            current_env->vars[j].value = value;
            return;
        }
    }
    
    // Variable doesn't exist in current scope, create new one
    append_variable(name, value);
    return;
}

// New function: Assign to an existing variable
void assign_variable(char* name, token value) {
    if (!name) return;
    
    // Search through environments from innermost to outermost
    for (int i = univ->count - 1; i >= 0; i--) {
        environment* env = univ->env[i];
        
        for (int j = 0; j < env->count; j++) {
            if (strcmp(env->vars[j].name, name) == 0) {
                env->vars[j].value = value;
                return;
            }
        }
    }
    
    // Variable not found - this is an error
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    fflush(stderr);
    exit(70);
}

void execute_block(stmt** statements, int count) {
    environment* env = init_environment();
    if (!env) {
        fprintf(stderr, "Error: Failed to create block environment\n");
        fflush(stderr);
        exit(70);
    }

    if (push_environment(env)) {
        fprintf(stderr, "Error: Failed to push environment to Universe\n");
        fflush(stderr);
        exit(70);
    }

    for (int i = 0; i < count; i++) 
        if (statements[i]) execute_statement(statements[i]);
    
    environment* popped = pop_environment();
    free_environment(popped);
   
    return;
}

void execute_if(stmt* statement) {
    token condition = evaluate_expression(statement->as.if_stmt_var.condition);
    if (is_truthy(condition)) execute_statement(statement->as.if_stmt_var.then_branch);
    else if (statement->as.if_stmt_var.else_branch != NULL) execute_statement(statement->as.if_stmt_var.else_branch);

    return;
}

void execute_while(stmt* statement) {
    while(1) {    
        token condition = evaluate_expression(statement->as.while_stmt_var.condition);
        if (!is_truthy(condition)) break;

        execute_statement(statement->as.while_stmt_var.body);
    }

    return;
}

void execute_for(stmt* statement) {
    if (statement->as.for_stmt_var.init_condition != NULL) execute_statement(statement->as.for_stmt_var.init_condition);

    while (1) {
        if (statement->as.for_stmt_var.loop_condition != NULL) {
            token condition = evaluate_expression(statement->as.for_stmt_var.loop_condition->as.expr_stmt_var.expression);
            if (!is_truthy(condition)) break;
        }

        execute_statement(statement->as.for_stmt_var.body);
        
        if (statement->as.for_stmt_var.change != NULL) execute_statement(statement->as.for_stmt_var.change);
    }

    return;
}