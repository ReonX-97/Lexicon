#include "parser.h"

int is_truthy(token tkn);
int token_equals(token left, token right);
double token_to_double(token tkn);
token make_number_token(double value);
token make_boolean_token(int is_true);
token evaluate_expression(expr* expression);