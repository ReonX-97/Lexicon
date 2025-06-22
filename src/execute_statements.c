#include "execute.h"
#include <stdio.h>
#include <stdlib.h>

static token return_value_global;
static int has_returned = 0;

void interpret(parse_results* parsed_tokens) {
    if (!parsed_tokens || !parsed_tokens->statements) {
        fprintf(stderr, "Error: No statements to execute\n");
        return;
    }
    
    if (!init_universe()) {
        fprintf(stderr, "Error: Universe not initialized\n");
        fflush(stderr);
        exit(70);
    }

    stmt** statements = parsed_tokens->statements;
    for (int i = 0; statements[i] != NULL; i++) {
        execute_statement(statements[i]);
    }
}

void execute_statement(stmt* statement) {
    if (!statement || has_returned) return;
    
    switch (statement->type) {
        case STMT_EXPR: {
            token result = evaluate_expression(statement->as.expr_stmt_var.expression);
            // free_token(&result);
            break;
        }
        
        case STMT_PRINT: {
            token result = evaluate_expression(statement->as.expr_stmt_var.expression);
            print_token(result);
            // free_token(&result);
            break;
        }

        case STMT_VAR: {
            value val;
            if (statement->as.var_stmt_var.initializer != NULL) {
                token init_token = evaluate_expression(statement->as.var_stmt_var.initializer);
                val = token_to_value(init_token);
                // free_token(&init_token);
            } else {
                val = create_nil_value();
            }
            define_variable(statement->as.var_stmt_var.tkn.symbol, val);
            free_value(val);
            break;
        }

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

        case STMT_FUN_DEF:
            execute_function_def(statement);
            break;

        case STMT_RETURN:
            execute_return(statement);
            break;

        default:
            fprintf(stderr, "Warning: Unknown statement type\n");
            break;
    }
}

void print_token(token tkn) {
    switch (tkn.type) {
        case NUMBER:
        case STRING:
            if (tkn.value) {
                printf("%s\n", tkn.value);
            } else {
                printf("nil\n");
            }
            break;
        case TRUE: 
            printf("true\n"); 
            break;
        case FALSE: 
            printf("false\n"); 
            break;
        case NIL: 
            printf("nil\n"); 
            break;
        case IDENTIFIER:
            if (tkn.symbol) {
                value var_val = get_variable_value(tkn.symbol);
                if (var_val.type == VAL_FUNCTION) {
                    printf("<fn %s>\n", tkn.symbol);
                } else {
                    print_value(var_val);
                }
                free_value(var_val);
            } else {
                printf("Unable to print.\n");
            }
            break;
        default: 
            printf("Unable to print.\n"); 
            break;
    }
    fflush(stdout);
}

void print_value(value val) {
    switch (val.type) {
        case VAL_NUMBER:
            printf("%.6g\n", val.as.number);
            break;
        case VAL_STRING:
            if (val.as.string) {
                printf("%s\n", val.as.string);
            } else {
                printf("nil\n");
            }
            break;
        case VAL_BOOLEAN:
            printf("%s\n", val.as.boolean ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil\n");
            break;
        case VAL_FUNCTION:
            printf("[function]\n");
            break;
        default:
            printf("Unable to print.\n");
            break;
    }
    fflush(stdout);
}

void define_variable(char* name, value val) {
    if (!name) {
        fprintf(stderr, "Error: Cannot define variable with null name\n");
        return;
    }
    
    if (!univ || univ->count == 0) {
        fprintf(stderr, "Error: No environment available for variable definition\n");
        return;
    }
    
    environment* current_env = univ->env[univ->count - 1];
    
    for (int j = 0; j < current_env->count; j++) {
        if (strcmp(current_env->vars[j].name, name) == 0) {
            free_value(current_env->vars[j].val);
            current_env->vars[j].val = copy_value(val);
            return;
        }
    }
    
    if (append_variable(name, val) != 0) {
        fprintf(stderr, "Error: Failed to define variable '%s'\n", name);
        exit(70);
    }
}

void assign_variable(char* name, value val) {
    if (!name) {
        fprintf(stderr, "Error: Cannot assign to variable with null name\n");
        return;
    }
    
    if (!univ) {
        fprintf(stderr, "Error: No universe available for variable assignment\n");
        return;
    }
    
    for (int i = univ->count - 1; i >= 0; i--) {
        environment* env = univ->env[i];
        
        for (int j = 0; j < env->count; j++) {
            if (strcmp(env->vars[j].name, name) == 0) {
                free_value(env->vars[j].val);
                env->vars[j].val = copy_value(val);
                return;
            }
        }
    }
    
    fprintf(stderr, "Error: Undefined variable '%s' in assignment\n", name);
    fflush(stderr);
    exit(70);
}

void execute_block(stmt** statements, int count) {
    if (!statements || count <= 0) return;
    
    environment* env = init_environment();
    if (!env) {
        fprintf(stderr, "Error: Failed to create block environment\n");
        fflush(stderr);
        exit(70);
    }

    if (push_environment(env) != 0) {
        fprintf(stderr, "Error: Failed to push environment to universe\n");
        free_environment(env);
        fflush(stderr);
        exit(70);
    }

    for (int i = 0; i < count && !has_returned; i++) {  // Check for return
        if (statements[i]) {
            execute_statement(statements[i]);
        }
    }
    
    environment* popped = pop_environment();
    free_environment(popped);
}

void execute_if(stmt* statement) {
    if (!statement) return;
    
    token condition = evaluate_expression(statement->as.if_stmt_var.condition);
    int is_true = is_truthy(condition);
    // free_token(&condition);
    
    if (is_true) {
        execute_statement(statement->as.if_stmt_var.then_branch);
    } else if (statement->as.if_stmt_var.else_branch != NULL) {
        execute_statement(statement->as.if_stmt_var.else_branch);
    }
}

void execute_while(stmt* statement) {
    if (!statement) return;
    
    while (1) {    
        token condition = evaluate_expression(statement->as.while_stmt_var.condition);
        int is_true = is_truthy(condition);
        // free_token(&condition);
        
        if (!is_true) break;

        execute_statement(statement->as.while_stmt_var.body);

        if (has_returned) break;
    }
}

void execute_for(stmt* statement) {
    if (!statement) return;
    
    if (statement->as.for_stmt_var.init_condition != NULL) {
        execute_statement(statement->as.for_stmt_var.init_condition);
    }

    while (1) {
        if (statement->as.for_stmt_var.loop_condition != NULL) {
            token condition = evaluate_expression(
                statement->as.for_stmt_var.loop_condition->as.expr_stmt_var.expression
            );
            int is_true = is_truthy(condition);
            // free_token(&condition);
            
            if (!is_true) break;
        }

        execute_statement(statement->as.for_stmt_var.body);

        if (has_returned) break;
        
        if (statement->as.for_stmt_var.change != NULL) {
            execute_statement(statement->as.for_stmt_var.change);
        }
    }
}

void execute_function_def(stmt* statement) {
    if (!statement) return;
    
    token name_token = statement->as.fun_def_stmt_var.name;
    if (!name_token.symbol) {
        fprintf(stderr, "Error: Function definition missing name\n");
        return;
    }
    
    value func_val = create_function_value(statement);
    define_variable(name_token.symbol, func_val);
    free_value(func_val);
}

token call_function(token callee, token* arguments, int arg_count) {
    if (!callee.symbol) {
        fprintf(stderr, "Error: Invalid function token\n");
        fflush(stderr);
        exit(70);
    }
    
    if (strcmp(callee.symbol, "clock") == 0) return native_clock(arg_count);
    // else if (strcmp(callee.symbol, "len")) return native_len(arguments, arg_count);
    // else if (strcmp(callee.symbol, "input")) return native_input(arguments, arg_count);
    // else if (strcmp(callee.symbol, "str")) return native_str(arguments, arg_count);
    
    value func_val = get_variable_value(callee.symbol);

    stmt* function_stmt = func_val.as.function;
    int param_count = function_stmt->as.fun_def_stmt_var.param_count;

    if (function_stmt->type != STMT_FUN_DEF) {
        fprintf(stderr, "Error: Invalid function statement type\n");
        fflush(stderr);
        exit(70);
    }
    
    token* parameters = function_stmt->as.fun_def_stmt_var.parameters;

    if (arg_count != param_count) {
        fprintf(stderr, "Error: Invalid Number of Parameters\n");
        fflush(stderr);
        exit(65);
    }

    environment* env = init_environment();
    if (!env) {
        fprintf(stderr, "Error: Failed to create function environment\n");
        fflush(stderr);
        exit(70);
    }
    
    if (push_environment(env) != 0) {
        fprintf(stderr, "Error: Failed to push function environment\n");
        free_environment(env);
        fflush(stderr);
        exit(70);
    }

    for (int i = 0; i < param_count; i++) {
        if (parameters[i].symbol && i < arg_count) {
            value arg_value = token_to_value(arguments[i]);
            define_variable(parameters[i].symbol, arg_value);
            free_value(arg_value);
        }
    }
    
    reset_return_state();
    
    if (function_stmt->as.fun_def_stmt_var.body) 
        execute_statement(function_stmt->as.fun_def_stmt_var.body);
    
    token return_value;
    if (has_function_returned()) {
        return_value = get_return_value();
    } else {
        return_value.type = NIL;
        return_value.value = NULL;
        return_value.symbol = "nil";  // Use string literal, not allocated memory
        return_value.line = 0;
    }

    environment* popped = pop_environment();
    free_environment(popped);

    free_value(func_val);

    return return_value;
}

void execute_return(stmt* statement) {
    if (!statement) return;
    
    if (statement->as.return_stmt_var.return_value) {
        return_value_global = evaluate_expression(statement->as.return_stmt_var.return_value);
    } else {
        return_value_global.type = NIL;
        return_value_global.value = NULL;
        return_value_global.symbol = "nil";
        return_value_global.line = 0;
    }
    
    has_returned = 1;
}

int has_function_returned() {
    return has_returned;
}

token get_return_value() {
    token ret = return_value_global;
    has_returned = 0;
    return ret;
}

void reset_return_state() {
    has_returned = 0;
}

