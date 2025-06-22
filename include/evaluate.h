#include "parser.h"

typedef enum {
    VAL_NUMBER,
    VAL_STRING, 
    VAL_BOOLEAN,
    VAL_NIL,
    VAL_FUNCTION
} value_type;

typedef struct {
    value_type type;
    union {
        double number;
        char* string;
        int boolean;
        stmt* function;
    } as;
} value;

int is_truthy(token tkn);
int token_equals(token left, token right);
double token_to_double(token tkn);
token make_number_token(double value);
token make_boolean_token(int is_true);
token evaluate_expression(expr* expression);
void assign_variable(char* name, value value);
token get_variable(char* name);

// Variable management
void define_variable(char* name, value val);
void assign_variable(char* name, value val);
value get_variable_value(char* name);
value get_variable_value_debug(char* name);
token get_variable(char* name);

// Helper functions for value conversion
value token_to_value(token tkn);
token value_to_token(value val);
void free_value(value val);
token copy_token(token original);

token call_function(token callee, token* arguments, int arg_count);
int is_native_function(char* name);