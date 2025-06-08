#include "execute.h"

universe* univ;

environment* init_environment() {
    int initial_capacity = 8;
    
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
    int initial_capacity = 4;
    
    univ = malloc(sizeof(universe));
    if (!univ) return 0;
    
    univ->env = malloc(sizeof(environment*) * initial_capacity);
    if (!univ->env) {
        free(univ);
        return 0;
    }
    
    univ->count = 0;
    univ->capacity = initial_capacity;
    
    environment* global_env = init_environment();
    if (!global_env) {
        free(univ->env);
        free(univ);
        return 0;
    }
    
    univ->env[0] = global_env;
    univ->count = 1;
    
    return 1;
}

int push_environment(environment* env) {
    if (!env) return -1;
    
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
    if (univ->count <= 1) return NULL;
    
    univ->count--;
    return univ->env[univ->count];
}

int append_variable(char* name, token value) {    
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
    env->vars[env->count].value = value;  // Direct assignment, no copying
    env->count++;
    return 0;
}

void free_environment(environment* env) {
    if (!env) return;
    
    for (int i = 0; i < env->count; i++) {
        free(env->vars[i].name);
    }
    
    free(env->vars);
    free(env);
}

void free_universe() {    
    for (int i = 0; i < univ->count; i++) {
        free_environment(univ->env[i]);
    }
    
    free(univ->env);
    free(univ);
}

token get_variable(char* name) {    
    if (!name) {
        token error;
        error.type = NIL;
        error.value = "nil";
        error.symbol = "nil";
        error.line = 0;
        return error;
    }
    
    for (int i = univ->count - 1; i >= 0; i--) {
        environment* env = univ->env[i];
        
        for (int j = 0; j < env->count; j++) {
            if (strcmp(env->vars[j].name, name) == 0) {
                return env->vars[j].value;
            }
        }
    }
    
    fprintf(stderr, "Error: Undefined variable '%s'\n", name);
    fflush(stderr);
    exit(70);

    token error;
    error.type = NIL;
    error.value = "nil";
    error.symbol = "nil";
    error.line = 0;

    return error;
}