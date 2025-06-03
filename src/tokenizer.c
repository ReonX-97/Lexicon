#include "tokenizer.h"

const char* token_type_to_string(token_type type) {
    switch (type) {
        case LEFT_PAREN:    return "LEFT_PAREN";
        case RIGHT_PAREN:   return "RIGHT_PAREN";
        case LEFT_BRACE:    return "LEFT_BRACE";
        case RIGHT_BRACE:   return "RIGHT_BRACE";
        case COMMA:         return "COMMA";
        case DOT:           return "DOT";
        case MINUS:         return "MINUS";
        case PLUS:          return "PLUS";
        case SEMICOLON:     return "SEMICOLON";
        case STAR:          return "STAR";
        case SLASH:         return "SLASH";
        case EQUAL:         return "EQUAL";
        case EQUAL_EQUAL:   return "EQUAL_EQUAL";
        case STRING:        return "STRING";
        case NUMBER:        return "NUMBER";
        case TOKEN_EOF:     return "EOF";
        case BANG:          return "BANG";
        case BANG_EQUAL:    return "BANG_EQUAL";
        case LESS:          return "LESS";
        case LESS_EQUAL:    return "LESS_EQUAL";
        case GREATER:       return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";                
        case ERROR:         return "ERROR";
        default:            return "UNKNOWN";
    }
}

char *read_file_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

token make_token(token_type type) {
    token t;
    t.type = type;
    t.start = scn.start;
    t.length = (int)(scn.current - scn.start);
    t.line = scn.line;
    return t;
}

token error_token(const char *message) {
    token t;
    t.type = ERROR;
    t.start = strdup(message);
    t.length = (int)strlen(message);
    t.line = scn.line;
    return t;
}

token string_token() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n') scn.line++;
        advance();
    }
    
    if (is_at_end()) return error_token("Unterminated string.");

    advance();

    return make_token(STRING);
}

token number_token() {
    int dot_count = 0;
    while ((is_digit(peek()) || peek() == '.') && dot_count <= 1) {
        if (peek() == '.') dot_count++;
        advance();
    }
    
    if (dot_count > 1) error_token("Not a Valid Number");

    return make_token(NUMBER);
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 3) {
        fprintf(stderr, "Usage: ./your_program tokenize <filename>\n");
        return 1;
    }

    const char *command = argv[1];
    if (strcmp(command, "tokenize") != 0) {
        fprintf(stderr, "Unknown command: %s\n", command);
        return 1;
    }

    char *file_contents = read_file_contents(argv[2]);
    if (!file_contents) return 1;

    scanner_init(file_contents);
    int exit_code = 0;

    while (1) {
        token t = scan_token();
        if (t.type == TOKEN_EOF) {
            printf("EOF  null\n");
            break;
        } else if (t.type == ERROR) {
            fprintf(stderr, "[line %d] Error: %.*s\n", t.line, t.length, t.start);
            exit_code = 65;
        } else if (t.type == STRING) {
            printf("%s %.*s %.*s\n", token_type_to_string(t.type), t.length, t.start, t.length - 2, t.start + 1);
        } else if (t.type == NUMBER) {
            
            char temp[t.length + 1];
            memcpy(temp, t.start, t.length);
            temp[t.length] = '\0';
            double double_val = strtod(temp, NULL);

            if (double_val == (int)double_val) printf("%s %.*s %.0f.0\n", token_type_to_string(t.type), t.length, t.start, double_val);
            else {
                int i = strlen(temp) - 1;
                while (i >= 0 && temp[i] == '0') {
                    temp[i--] = '\0';
                }
                printf("%s %.*s %s\n", token_type_to_string(t.type), t.length, t.start, temp);
            }
        }
        else {
            printf("%s %.*s null\n", token_type_to_string(t.type), t.length, t.start);
        }

        fflush(stdout);
    }

    free(file_contents);
    return exit_code;
}





