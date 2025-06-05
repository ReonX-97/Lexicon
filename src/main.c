#include "parser.h"

scanner scn;

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
        case IDENTIFIER:    return "IDENTIFIER";
        case TOKEN_EOF:     return "EOF";
        case BANG:          return "BANG";
        case BANG_EQUAL:    return "BANG_EQUAL";
        case LESS:          return "LESS";
        case LESS_EQUAL:    return "LESS_EQUAL";
        case GREATER:       return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";
        case AND:           return "AND";
        case CLASS:         return "CLASS";
        case ELSE:          return "ELSE";
        case FALSE:         return "FALSE";
        case FOR:           return "FOR";
        case FUN:           return "FUN";
        case IF:            return "IF";
        case NIL:           return "NIL";
        case OR:            return "OR";
        case PRINT:         return "PRINT";
        case RETURN:        return "RETURN";
        case SUPER:         return "SUPER";
        case THIS:          return "THIS";
        case TRUE:          return "TRUE";
        case VAR:           return "VAR";
        case WHILE:         return "WHILE";
        case ERROR:         return "ERROR";
        default:            return "UNKNOWN";
    }
}

char *read_file_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fflush(stderr);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        fflush(stderr);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 3) {
        fprintf(stderr, "Usage: ./your_program <command> <filename>\n");
        fflush(stderr);
        return 1;
    }

    const char *command = argv[1];

    if (strcmp(command, "tokenize") == 0) {
        char *file_contents = read_file_contents(argv[2]);
        if (!file_contents) return 1;

        scanner_init(file_contents);

        while (1) {
            token t = scan_token();
            if (t.type == TOKEN_EOF) {
                printf("EOF  null\n");
                break;
            } else if (t.type == ERROR) {
                fprintf(stderr, "[line %d] Error: %s\n", t.line, t.symbol);
                fflush(stderr);
                exit(65);
            }
            else {
                printf("%s %s %s\n", token_type_to_string(t.type), t.symbol ? t.symbol : "null", t.value ? t.value : "null");
            }

            fflush(stdout);
            free_token(&t);
        }

        free(file_contents);
        return 0;
    } else if (strcmp(command, "parse") == 0) {
        char *file_contents = read_file_contents(argv[2]);
        token_array t_array;
        if (!file_contents) return 1;

        scanner_init(file_contents);
        init_token_array(&t_array);

        while (1) {
            token t = scan_token();
            add_to_token_array(&t_array, t);
            
            if (t.type == TOKEN_EOF) {
                break;
            } else if (t.type == ERROR) {
                fprintf(stderr, "[line %d] Error: %s\n", t.line, t.symbol);
                fflush(stderr);
                free(file_contents);
                free_token_array(&t_array);
                exit(65);
            }
        }

        char *parsed_expression = parsed_token_expression(&t_array);
        if (parsed_expression) {
            printf("%s\n", parsed_expression);
            free(parsed_expression);
        }

        free(file_contents);
        free_token_array(&t_array);
        return 0;
        
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        fflush(stderr);
        return 1;
    }
}