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
    snprintf(raw, sizeof(raw), "%f", value);

    int len = strlen(raw);
    int i;

    for (i = len - 1; i > 0 && raw[i] == '0'; i--) raw[i] = '\0';

    if (raw[i] == '.') raw[i] = '\0';

    tkn.value = malloc(strlen(raw) + 1);
    if (tkn.value) strcpy(tkn.value, raw);

    return tkn;
}

token make_boolean_token(int is_true) {
    token t;
    t.type = is_true ? TRUE : FALSE;
    t.line = 0;
    t.value = is_true ? "true" : "false";
    t.symbol = is_true ? "true" : "false";
    return t;
}