#include "tokenizer.h"

void scanner_init(const char *source) {
    scn.start = source;
    scn.current = source;
    scn.line = 1;
}

char advance() {
    return *scn.current++;
}

char peek() {
    return *scn.current;
}

char peek_next() {
    return is_at_end() || *(scn.current + 1) == '\0' ? '\0' : *(scn.current + 1);
}

int is_at_end() {
    return *scn.current == '\0';
}

int is_digit(char c) {
    return (c >= '0' && c <= '9');
}

int is_letter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int match(char expected) {
    if (is_at_end()) return 0;
    if (*scn.current != expected) return 0;
    scn.current++;
    return 1;
}

token scan_token() {
    while (1) {
        if (is_at_end()) return make_token(TOKEN_EOF);

        scn.start = scn.current;
        char c = advance();

        switch (c) {
            case '(': return make_token(LEFT_PAREN);
            case ')': return make_token(RIGHT_PAREN);
            case '{': return make_token(LEFT_BRACE);
            case '}': return make_token(RIGHT_BRACE);
            case ',': return make_token(COMMA);
            case '.': return make_token(DOT);
            case '-': return make_token(MINUS);
            case '+': return make_token(PLUS);
            case ';': return make_token(SEMICOLON);
            case '*': return make_token(STAR);
            case '/':
                if (match('/')) {
                    while (peek() != '\n' && !is_at_end()) advance();
                    break;
                }
                return make_token(SLASH);
            case '=':
                if (match('=')) return make_token(EQUAL_EQUAL);
                return make_token(EQUAL);
            case '!':
                if (match('=')) return make_token(BANG_EQUAL);
                return make_token(BANG);
            case '<':
                if (match('=')) return make_token(LESS_EQUAL);
                return make_token(LESS);
            case '>':
                if (match('=')) return make_token(GREATER_EQUAL);
                return make_token(GREATER);
            case '"': return string_token();           
            case '\n': scn.line++; break;
            case ' ':
            case '\r':
            case '\t':
                break;
            case '#':
            case '@':
            case '$':
            case '%': {
                char message[25];
                snprintf(message, sizeof(message), "Unexpected character: %c", c);
                return error_token(message);
            }
            default:
                if (is_digit(c)) return number_token();
                break;
        }
    }
}
