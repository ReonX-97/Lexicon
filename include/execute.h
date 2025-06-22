#include "evaluate.h"

typedef struct {
    char* name;             // Always malloc'd, need to free
    value val;              // Always use value, not token
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

// Core execution functions
void interpret(parse_results* parsed_tokens);
void print_token(token tkn);
void print_value(value val);
void execute_statement(stmt* statement);
void execute_block(stmt** statements, int count);
void execute_if(stmt* statement);
void execute_while(stmt* statement);
void execute_for(stmt* statement);
void execute_function_def(stmt* statement);
void execute_return(stmt* statement);

// Environment management
environment* init_environment();
int init_universe();
int push_environment(environment* env);
environment* pop_environment();
int append_variable(char* name, value val);
void free_environment(environment* env);
void free_universe();

// Utility functions
value create_nil_value();
value create_number_value(double num);
value create_string_value(const char* str);
value create_boolean_value(int boolean);
value create_function_value(stmt* function);
value copy_value(value original);

// Function declarations for return handling
void execute_return(stmt* statement);
int has_function_returned();
token get_return_value();
void reset_return_state();

// native functions
token native_clock(int arg_count);
token native_len(token* args, int arg_count);
token native_input(token* args, int arg_count);
token native_str(token* args, int arg_count);
