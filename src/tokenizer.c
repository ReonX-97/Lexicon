#include "tokenizer_scanner.h"

token make_token(token_type type) {
    token t;
    t.type = type;
    t.start = scn.start;
    t.length = (int)(scn.current - scn.start);
    t.line = scn.line;
    
    t.symbol = malloc(t.length + 1);
    if (t.symbol) {
        memcpy(t.symbol, scn.start, t.length);
        t.symbol[t.length] = '\0';
    }
    
    t.value = NULL;
    return t;
}

token make_token_with_value(token_type type, const char *value) {
    token t = make_token(type);
    if (value) {
        t.value = malloc(strlen(value) + 1);
        if (t.value) {
            strcpy(t.value, value);
        }
    }
    return t;
}

token error_token(const char *message) {
    token t;
    t.type = ERROR;
    t.start = NULL;
    t.length = 0;
    t.line = scn.line;
    t.symbol = malloc(strlen(message) + 1);
    if (t.symbol) {
        strcpy(t.symbol, message);
    }
    t.value = NULL;
    return t;
}

void free_token(token *t) {
    if (t->symbol) {
        free(t->symbol);
        t->symbol = NULL;
    }
    if (t->value) {
        free(t->value);
        t->value = NULL;
    }
}

token string_token() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') scn.line++;
        advance();
    }
    
    if (is_at_end()) return error_token("Unterminated string.");

    advance();

    token t = make_token(STRING);
    
    if (t.length >= 2) {
        int value_length = t.length - 2;
        t.value = malloc(value_length + 1);
        if (t.value) {
            memcpy(t.value, scn.start + 1, value_length);
            t.value[value_length] = '\0';
        }
    }

    return t;
}

token number_token() {
    int dot_count = 0;
    while ((is_digit(peek()) || peek() == '.') && dot_count <= 1) {
        if (peek() == '.') dot_count++;
        advance();
    }

    if (dot_count > 1) return error_token("Not a Valid Number");

    token t = make_token(NUMBER);

    char raw[t.length + 1];
    memcpy(raw, scn.start, t.length);
    raw[t.length] = '\0';

    if (strchr(raw, '.')) {
        int len = strlen(raw);
        while (len > 0 && raw[len - 1] == '0') len--;
        if (len > 0 && raw[len - 1] == '.') {
            raw[len++] = '0';
        }
        raw[len] = '\0';
    } else {
        strcat(raw, ".0");
    }

    t.value = malloc(strlen(raw) + 1);
    if (t.value) {
        strcpy(t.value, raw);
    }

    return t;
}

token identifier_keyword_token() {
    while (is_letter(peek()) || is_digit(peek()) || peek() == '_') advance();
    size_t length = scn.current - scn.start;

    switch (scn.start[0]) {
        case 'a': if (length == 3 && memcmp(scn.start, "and", 3) == 0) return make_token(AND);
        case 'c': if (length == 5 && memcmp(scn.start, "class", 5) == 0) return make_token(CLASS);
        case 'e': if (length == 4 && memcmp(scn.start, "else", 4) == 0) return make_token(ELSE);
        case 'f':
            if (length == 5 && memcmp(scn.start, "false", 5) == 0) return make_token(FALSE);
            if (length == 3 && memcmp(scn.start, "for", 3) == 0) return make_token(FOR);
            if (length == 3 && memcmp(scn.start, "fun", 3) == 0) return make_token(FUN);
        case 'i': if (length == 2 && memcmp(scn.start, "if", 2) == 0) return make_token(IF);
        case 'n': if (length == 3 && memcmp(scn.start, "nil", 3) == 0) return make_token(NIL);
        case 'o': if (length == 2 && memcmp(scn.start, "or", 2) == 0) return make_token(OR);
        case 'p': if (length == 5 && memcmp(scn.start, "print", 5) == 0) return make_token(PRINT);
        case 'r': if (length == 6 && memcmp(scn.start, "return", 6) == 0) return make_token(RETURN);
        case 's': if (length == 5 && memcmp(scn.start, "super", 5) == 0) return make_token(SUPER);
        case 't':
            if (length == 4 && memcmp(scn.start, "this", 4) == 0) return make_token(THIS);
            if (length == 4 && memcmp(scn.start, "true", 4) == 0) return make_token(TRUE);
        case 'v': if (length == 3 && memcmp(scn.start, "var", 3) == 0) return make_token(VAR);
        case 'w': if (length == 5 && memcmp(scn.start, "while", 5) == 0) return make_token(WHILE);
    }

    return make_token(IDENTIFIER);
}