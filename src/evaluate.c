#include "evaluate.h"

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
        case EXPR_LITERAL:
            if (expression->as.literal.value.type == IDENTIFIER) {
                char* name = expression->as.literal.value.symbol;
                if (!name) {
                    fprintf(stderr, "Error: Identifier has no name\n");
                    fflush(stderr);
                    exit(70);
                }               
                if (strcmp(name, "clock") == 0 || strcmp(name, "len") == 0 || 
                    strcmp(name, "input") == 0 || strcmp(name, "str") == 0) {
                    return expression->as.literal.value;
                }                
                value var_val = get_variable_value(name);                
                return value_to_token(var_val);
            }            
            if (expression->as.literal.value.type == NUMBER) {
                return make_number_token(token_to_double(expression->as.literal.value));
            }
            return expression->as.literal.value;
            break;
        case EXPR_ASSIGN: 
            token value_t = evaluate_expression(expression->as.assign.value);
            value value_v = token_to_value(value_t);
            assign_variable(expression->as.assign.name.symbol, value_v);
            free_value(value_v);
            return value_t;
            break;
        case EXPR_CALL:
            token value = evaluate_expression(expression->as.call.callee);
            
            int arg_count = expression->as.call.arg_count;
            token* arguments = malloc(arg_count * sizeof(token));
            for (int i = 0; i < arg_count; i++) 
                arguments[i] = evaluate_expression(expression->as.call.arguments[i]);
            
            token result = call_function(value, arguments, arg_count);
            free(arguments);
            return result;
            break;
        case EXPR_BINARY:
            switch (expression->as.binary.operator) {
                case AND: {
                    token left_lA = evaluate_expression(expression->as.binary.left);
                    if (!is_truthy(left_lA)) return left_lA;
                    return evaluate_expression(expression->as.binary.right);
                }
                case OR: {
                    token left_lO = evaluate_expression(expression->as.binary.left);
                    if (is_truthy(left_lO)) return left_lO;
                    return evaluate_expression(expression->as.binary.right);
                }
                default:
                    break;
            }
            
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

                            result.symbol = result.value;

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
        default:
            return error_token("Type Error MisMatch (evaluate.c line 235)");
            break;
    }

    return error_token("Type Error MisMatch (evaluate.c line 239)");
}