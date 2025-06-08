#include "evaluate.h"

typedef struct {
    char* name;
    token value;
} variable;

typedef struct {
    variable* vars;
    int count;
    int capacity;
} environment;

typedef struct {
    environment** env;
    int count;
    int capacity;
} universe;

extern universe* univ;

void interpret(parse_results* parsed_tokens);
void print_token(token tkn);
void define_variable(char* name, token value);
token copy_token(token original);
void execute_statement(stmt* statement);
void execute_block(stmt** statements, int count);
void execute_if(stmt* statement);
void execute_while(stmt* statement);
void execute_for(stmt* statement);
token get_variable(char* name);

environment* init_environment();
int init_universe();
int push_environment(environment* env);
environment* pop_environment();
int append_variable(char* name, token value);
void free_environment(environment* env);
void free_universe();