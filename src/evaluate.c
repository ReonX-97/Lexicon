#include "evaluate.h"

int is_truthy(token tkn) {
    if (tkn.type == FALSE || tkn.type == NIL) return 0;
    else return 1;
}

int token_equals(token left, token right) {
    if (left.type != right.type) return 0;
    
    switch (left.type) {
    case NIL:
    case TRUE:
    case FALSE:
        return 1;
        break;
    case NUMBER:
    case STRING:
        if (left.value && right.value) return strcmp(left.value, right.value) == 0;
        return 0;
        break;
    case IDENTIFIER:
        if (left.symbol && right.symbol) return strcmp(left.symbol, right.symbol) == 0;
        return 0;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}

double token_to_double(token tkn) {
    if (tkn.type == NUMBER && tkn.value) return atof(tkn.value);
    return 0.0;
}

token make_number_token(double value) {
    token tkn;
    tkn.type = NUMBER;
    tkn.line = 0;
    tkn.symbol = NULL;

    char raw[64];
    
    // Always format with decimal places first
    snprintf(raw, sizeof(raw), "%.15g", value);
    
    tkn.value = malloc(strlen(raw) + 1);
    if (tkn.value) {
        strcpy(tkn.value, raw);
    }

    return tkn;
}

token make_boolean_token(int is_true) {
    token t;
    t.type = is_true ? TRUE : FALSE;
    t.line = 0;
    t.value = NULL;
    t.symbol = is_true ? "true" : "false";
    return t;
}

token evaluate_expression(expr* expression) {
    if (!expression) {
        token error_token;
        error_token.type = NIL;
        error_token.value = NULL;
        error_token.symbol = "nil";
        error_token.line = 0;
        return error_token;
    }

    switch (expression->type) {
    case EXPR_BINARY:
        token left = evaluate_expression(expression->as.binary.left);
        token right = evaluate_expression(expression->as.binary.right);
        switch (expression->as.binary.operator) {
            case PLUS:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);
                    return make_number_token(left_val + right_val);
                } else if (left.type == STRING && right.type == STRING) {
                    if (left.value && right.value) {
                        token result;
                        result.type = STRING;
                        result.line = 0;
                        result.symbol = NULL;
                        
                        size_t len = strlen(left.value) + strlen(right.value) + 1;
                        result.value = malloc(len);
                        strcpy(result.value, left.value);
                        strcat(result.value, right.value);

                        return result;
                    }
                } else {
                    fprintf(stderr, "Error must be two Numbers or two Strings for +\n");
                    exit(70);
                }
                break;
            case MINUS:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);
                    return make_number_token(left_val - right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for -\n");
                    exit(70);
                }
                break;
            case STAR:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);
                    return make_number_token(left_val * right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for *\n");
                    exit(70);
                }
                break;
            case SLASH:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);
                    if (right_val == 0) {
                        fprintf(stderr, "Error: Division by zero\n");
                        exit(65);
                    }
                    return make_number_token(left_val / right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for /\n");
                    exit(70);
                }
                break;
            case GREATER:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);

                    return make_boolean_token(left_val > right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for >\n");
                    exit(70);
                }
                break;
            case GREATER_EQUAL:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);

                    return make_boolean_token(left_val >= right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for >=\n");
                    exit(70);
                }
                break;
            case LESS:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);

                    return make_boolean_token(left_val < right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for <\n");
                    exit(70);
                }
                break;
            case LESS_EQUAL:
                if (left.type == NUMBER && right.type == NUMBER) {
                    double left_val = token_to_double(left);
                    double right_val = token_to_double(right);

                    return make_boolean_token(left_val <= right_val);
                } else {
                    fprintf(stderr, "Error: Operands must be numbers for <=\n");
                    exit(70);
                }
                break;
            case EQUAL_EQUAL: return make_boolean_token(token_equals(left, right));break;
            case BANG_EQUAL: return make_boolean_token(!token_equals(left, right));break;
            case AND: 
                if (!is_truthy(left)) return left;
                return right;
                break;
            case OR:
                if (is_truthy(left)) return left;
                return right;
                break;
            default:
                fprintf(stderr, "Error: Unknown binary operator\n");
                exit(70);
                break;
        }
        break;
    case EXPR_UNARY:
        token operand = evaluate_expression(expression->as.unary.operand);
        switch (expression->as.unary.operator) {
            case BANG:
                return make_boolean_token(!is_truthy(operand));
                break;
            case MINUS:
                if (operand.type == NUMBER) {
                    double value = token_to_double(operand);
                    return make_number_token(-value);
                } else {
                    fprintf(stderr, "Error: Operand must be a number for unary minus\n");
                    exit(70);
                }
                break;
            default:
                break;
        }
        break;
    case EXPR_GROUPING:
        return evaluate_expression(expression->as.grouping.expression);
        break;
    case EXPR_LITERAL:
        if (expression->as.literal.value.type == NUMBER) return make_number_token(token_to_double(expression->as.literal.value));
        return expression->as.literal.value;
        break;
    default:
        return error_token("Type Error MisMatch (evaluate.c line 235)");
        break;
    }

    return error_token("Type Error MisMatch (evaluate.c line 239)");
}