#include "execute.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

universe* univ;

// Utility functions for creating values
value create_nil_value() {
    value val;
    val.type = VAL_NIL;
    return val;
}

value create_number_value(double num) {
    value val;
    val.type = VAL_NUMBER;
    val.as.number = num;
    return val;
}

value create_string_value(const char* str) {
    value val;
    val.type = VAL_STRING;
    if (str) {
        val.as.string = malloc(strlen(str) + 1);
        if (val.as.string) {
            strcpy(val.as.string, str);
        } else {
            val.type = VAL_NIL;  // Fallback on allocation failure
        }
    } else {
        val.as.string = NULL;
        val.type = VAL_NIL;
    }
    return val;
}

value create_boolean_value(int boolean) {
    value val;
    val.type = VAL_BOOLEAN;
    val.as.boolean = boolean ? 1 : 0;
    return val;
}

value create_function_value(stmt* function) {
    value val;
    val.type = VAL_FUNCTION;
    val.as.function = function;  // No ownership, just reference
    return val;
}

value copy_value(value original) {
    value copy;
    copy.type = original.type;
    
    switch (original.type) {
        case VAL_STRING:
            if (original.as.string) {
                copy.as.string = malloc(strlen(original.as.string) + 1);
                if (copy.as.string) {
                    strcpy(copy.as.string, original.as.string);
                } else {
                    copy.type = VAL_NIL;
                }
            } else {
                copy.as.string = NULL;
            }
            break;
        case VAL_NUMBER:
            copy.as.number = original.as.number;
            break;
        case VAL_BOOLEAN:
            copy.as.boolean = original.as.boolean;
            break;
        case VAL_FUNCTION:
            copy.as.function = original.as.function;
            break;
        case VAL_NIL:
        default:
            break;
    }
    return copy;
}

value token_to_value(token tkn) {
    switch (tkn.type) {
        case NUMBER:
            return create_number_value(atof(tkn.value));
        case STRING:
            return create_string_value(tkn.value);
        case TRUE:
            return create_boolean_value(1);
        case FALSE:
            return create_boolean_value(0);
        case NIL:
        default:
            return create_nil_value();
    }
}

token value_to_token(value val) {
    token tkn;
    tkn.line = 0;
    tkn.value = NULL;
    tkn.symbol = NULL;
    
    switch (val.type) {
        case VAL_NUMBER:
            tkn.type = NUMBER;
            char raw[32];
            if (val.as.number == (int)val.as.number) {
                snprintf(raw, sizeof(raw), "%.0f", val.as.number);
            } else {
                snprintf(raw, sizeof(raw), "%f", val.as.number);
                int len = strlen(raw);
                while (len > 0 && raw[len - 1] == '0') {
                    len--;
                }
                if (len > 0 && raw[len - 1] == '.') {
                    raw[len++] = '0';
                }
                
                raw[len] = '\0';
            }
            tkn.value = malloc(strlen(raw) + 1);
            if (tkn.value) {
                strcpy(tkn.value, raw);
                tkn.symbol = tkn.value;
            }
            break;
        case VAL_STRING:
            tkn.type = STRING;
            if (val.as.string) {
                tkn.value = malloc(strlen(val.as.string) + 1);
                if (tkn.value) {
                    strcpy(tkn.value, val.as.string);
                    tkn.symbol = tkn.value;
                }
            }
            break;
        case VAL_BOOLEAN:
            if (val.as.boolean) {
                tkn.type = TRUE;
                tkn.value = malloc(5);
                if (tkn.value) {
                    strcpy(tkn.value, "true");
                    tkn.symbol = tkn.value;
                }
            } else {
                tkn.type = FALSE;
                tkn.value = malloc(6);
                if (tkn.value) {
                    strcpy(tkn.value, "false");
                    tkn.symbol = tkn.value;
                }
            }
            break;
        case VAL_FUNCTION:
            tkn.type = IDENTIFIER;
            if (val.as.function && val.as.function->as.fun_def_stmt_var.name.symbol) {
                char* name = val.as.function->as.fun_def_stmt_var.name.symbol;
                tkn.value = malloc(strlen(name) + 1);
                if (tkn.value) {
                    strcpy(tkn.value, name);
                    tkn.symbol = tkn.value;
                }
            }
            break;
        case VAL_NIL:
        default:
            tkn.type = NIL;
            tkn.value = malloc(4);
            if (tkn.value) {
                strcpy(tkn.value, "nil");
                tkn.symbol = tkn.value;
            }
            break;
    }
    return tkn;
}

token copy_token(token original) {
    token copy;
    copy.type = original.type;
    copy.line = original.line;
    
    if (original.value) {
        copy.value = malloc(strlen(original.value) + 1);
        if (copy.value) {
            strcpy(copy.value, original.value);
            copy.symbol = copy.value;
        } else {
            copy.value = NULL;
            copy.symbol = NULL;
        }
    } else {
        copy.value = NULL;
        copy.symbol = NULL;
    }
    
    return copy;
}

void free_value(value val) {
    switch (val.type) {
        case VAL_STRING:
            if (val.as.string) {
                free(val.as.string);
            }
            break;
        case VAL_FUNCTION:
            // Don't free the statement, we don't own it
            break;
        case VAL_NUMBER:
        case VAL_BOOLEAN:
        case VAL_NIL:
        default:
            break;
    }
}

environment* init_environment() {
    const int initial_capacity = 8;
    
    environment* env = malloc(sizeof(environment));
    if (!env) return NULL;
    
    env->vars = malloc(sizeof(variable) * initial_capacity);
    if (!env->vars) {
        free(env);
        return NULL;
    }
    
    env->count = 0;
    env->capacity = initial_capacity;
    return env;
}

int init_universe() {
    const int initial_capacity = 4;
    
    univ = malloc(sizeof(universe));
    if (!univ) return 0;
    
    univ->env = malloc(sizeof(environment*) * initial_capacity);
    if (!univ->env) {
        free(univ);
        univ = NULL;
        return 0;
    }
    
    univ->count = 0;
    univ->capacity = initial_capacity;
    
    environment* global_env = init_environment();
    if (!global_env) {
        free(univ->env);
        free(univ);
        univ = NULL;
        return 0;
    }
    
    univ->env[0] = global_env;
    univ->count = 1;
    
    return 1;
}

int push_environment(environment* env) {
    if (!env || !univ) return -1;
    
    if (univ->count >= univ->capacity) {
        int new_capacity = univ->capacity * 2;
        environment** new_env = realloc(univ->env, sizeof(environment*) * new_capacity);
        if (!new_env) return -1;
        
        univ->env = new_env;
        univ->capacity = new_capacity;
    }
    
    univ->env[univ->count] = env;
    univ->count++;
    return 0;
}

environment* pop_environment() {
    if (!univ || univ->count <= 1) return NULL;  // Keep global environment
    
    univ->count--;
    return univ->env[univ->count];
}

int append_variable(char* name, value val) {
    if (!name || !univ || univ->count == 0) return -1;
    
    environment* env = univ->env[univ->count - 1];
    
    if (env->count >= env->capacity) {
        int new_capacity = env->capacity * 2;
        variable* new_vars = realloc(env->vars, sizeof(variable) * new_capacity);
        if (!new_vars) return -1;
        
        env->vars = new_vars;
        env->capacity = new_capacity;
    }
    
    char* name_copy = malloc(strlen(name) + 1);
    if (!name_copy) return -1;
    strcpy(name_copy, name);
    
    env->vars[env->count].name = name_copy;
    env->vars[env->count].val = copy_value(val);  // Always copy to avoid ownership issues
    env->count++;
    return 0;
}

void free_environment(environment* env) {
    if (!env) return;
    
    for (int i = 0; i < env->count; i++) {
        if (env->vars[i].name) {
            free(env->vars[i].name);
        }
        free_value(env->vars[i].val);
    }
    
    if (env->vars) {
        free(env->vars);
    }
    free(env);
}

void free_universe() {
    if (!univ) return;
    
    for (int i = 0; i < univ->count; i++) {
        free_environment(univ->env[i]);
    }
    
    if (univ->env) {
        free(univ->env);
    }
    free(univ);
    univ = NULL;
}

value get_variable_value(char* name) {
    if (!name || !univ) {
        return create_nil_value();
    }
    
    // Search from most recent environment to global
    for (int i = univ->count - 1; i >= 0; i--) {
        environment* env = univ->env[i];
        
        for (int j = 0; j < env->count; j++) {
            if (strcmp(env->vars[j].name, name) == 0) {
                return copy_value(env->vars[j].val);
            }
        }
    }
    
    // Variable not found
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    fflush(stderr);
    exit(70);
}

value get_variable_value_debug(char* name) {
    printf("DEBUG: Looking for variable '%s'\n", name);
    printf("DEBUG: Universe count: %d\n", univ ? univ->count : -1);
    
    if (!name || !univ) {
        printf("DEBUG: Null name or universe\n");
        return create_nil_value();
    }
    
    // Search from most recent environment to global
    for (int i = univ->count - 1; i >= 0; i--) {
        environment* env = univ->env[i];
        printf("DEBUG: Checking environment %d with %d variables\n", i, env->count);
        
        for (int j = 0; j < env->count; j++) {
            printf("DEBUG: Variable %d: '%s'\n", j, env->vars[j].name);
            if (strcmp(env->vars[j].name, name) == 0) {
                printf("DEBUG: Found variable '%s'!\n", name);
                return copy_value(env->vars[j].val);
            }
        }
    }
    
    // Variable not found
    printf("DEBUG: Variable '%s' not found in any environment\n", name);
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    fflush(stderr);
    exit(70);
}

token get_variable(char* name) {
    value val = get_variable_value(name);
    return value_to_token(val);
}