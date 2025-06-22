// #include "execute.h"
// #include <time.h>

// // Native function implementations
// token native_clock(int arg_count) {
//     if (arg_count != 0) {
//         fprintf(stderr, "Error: clock() takes no arguments\n");
//         exit(70);
//     }
    
//     double time_val = (double)clock() / CLOCKS_PER_SEC;
//     return make_number_token(time_val);
// }

// token native_len(token* args, int arg_count) {
//     if (arg_count != 1) {
//         fprintf(stderr, "Error: len() takes exactly 1 argument\n");
//         exit(70);
//     }
    
//     if (args[0].type != STRING) {
//         fprintf(stderr, "Error: len() argument must be a string\n");
//         exit(70);
//     }
    
//     int length = args[0].value ? strlen(args[0].value) : 0;
//     return make_number_token((double)length);
// }

// token native_input(token* args, int arg_count) {
//     if (arg_count > 1) {
//         fprintf(stderr, "Error: input() takes 0 or 1 arguments\n");
//         exit(70);
//     }
    
//     // Print prompt if provided
//     if (arg_count == 1 && args[0].type == STRING && args[0].value) {
//         printf("%s", args[0].value);
//         fflush(stdout);
//     }
    
//     char buffer[1024];
//     if (fgets(buffer, sizeof(buffer), stdin)) {
//         // Remove newline if present
//         size_t len = strlen(buffer);
//         if (len > 0 && buffer[len-1] == '\n') {
//             buffer[len-1] = '\0';
//         }
        
//         token result;
//         result.type = STRING;
//         result.value = malloc(strlen(buffer) + 1);
//         strcpy(result.value, buffer);
//         result.symbol = NULL;
//         result.line = 0;
//         return result;
//     }
    
//     // Return empty string on EOF
//     token result;
//     result.type = STRING;
//     result.value = malloc(1);
//     result.value[0] = '\0';
//     result.symbol = NULL;
//     result.line = 0;
//     return result;
// }

// token native_str(token* args, int arg_count) {
//     if (arg_count != 1) {
//         fprintf(stderr, "Error: str() takes exactly 1 argument\n");
//         exit(70);
//     }
    
//     token result;
//     result.type = STRING;
//     result.symbol = NULL;
//     result.line = 0;
    
//     switch (args[0].type) {
//         case NUMBER:
//             result.value = malloc(32);
//             snprintf(result.value, 32, "%.6g", token_to_double(args[0]));
//             break;
//         case STRING:
//             result.value = malloc(strlen(args[0].value) + 1);
//             strcpy(result.value, args[0].value);
//             break;
//         case TRUE:
//             result.value = malloc(5);
//             strcpy(result.value, "true");
//             break;
//         case FALSE:
//             result.value = malloc(6);
//             strcpy(result.value, "false");
//             break;
//         case NIL:
//             result.value = malloc(4);
//             strcpy(result.value, "nil");
//             break;
//         default:
//             result.value = malloc(8);
//             strcpy(result.value, "unknown");
//             break;
//     }
    
//     return result;
// }


#include "execute.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>

// // Fixed native_clock implementation
// token native_clock(int arg_count) {
//     // Check argument count
//     if (arg_count != 0) {
//         fprintf(stderr, "Error: clock() takes no arguments\n");
//         fflush(stderr);
//         exit(70);
//     }
    
//     // Get current time with microsecond precision
//     struct timeval tv;
    
//     if (gettimeofday(&tv, NULL) == 0) {
//         // Convert to seconds with decimal precision
//         // This gives us seconds.microseconds
//         double time_val = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
        
//         // Create token manually with better precision
//         token result;
//         result.type = NUMBER;
//         result.value = malloc(32);
//         // Use %.6f to maintain precision and avoid scientific notation for reasonable values
//         snprintf(result.value, 32, "%.6f", time_val);
//         result.symbol = NULL;
//         result.line = 0;
        
//         return result;
//     } else {
//         // Fallback to time() if gettimeofday fails
//         time_t current_time = time(NULL);
        
//         token result;
//         result.type = NUMBER;
//         result.value = malloc(32);
//         snprintf(result.value, 32, "%ld.000000", (long)current_time);
//         result.symbol = NULL;
//         result.line = 0;
        
//         return result;
//     }
// }

token native_clock(int arg_count) {
    if (arg_count != 0) {
        fprintf(stderr, "Error: clock() takes no arguments\n");
        fflush(stderr);
        exit(70);
    }
    
    struct timeval tv;
    time_t current_time;
    
    if (gettimeofday(&tv, NULL) == 0) {
        current_time = tv.tv_sec;
    } else {
        current_time = time(NULL);
    }
    
    token result;
    result.type = NUMBER;
    
    char raw[32];

    snprintf(raw, sizeof(raw), "%ld", (long)current_time);
    
    result.value = malloc(strlen(raw) + 1);
    if (result.value) {
        strcpy(result.value, raw);
    }
    
    result.symbol = NULL;
    result.line = 0;
    return result;
}

token native_len(token* args, int arg_count) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: len() takes exactly 1 argument\n");
        exit(70);
    }
    
    if (args[0].type != STRING) {
        fprintf(stderr, "Error: len() argument must be a string\n");
        exit(70);
    }
    
    int length = args[0].value ? strlen(args[0].value) : 0;
    
    // Create token manually
    token result;
    result.type = NUMBER;
    result.value = malloc(32);
    snprintf(result.value, 32, "%d", length);
    result.symbol = NULL;
    result.line = 0;
    
    return result;
}

token native_input(token* args, int arg_count) {
    if (arg_count > 1) {
        fprintf(stderr, "Error: input() takes 0 or 1 arguments\n");
        exit(70);
    }
    
    // Print prompt if provided
    if (arg_count == 1 && args[0].type == STRING && args[0].value) {
        printf("%s", args[0].value);
        fflush(stdout);
    }
    
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove newline if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        
        token result;
        result.type = STRING;
        result.value = malloc(strlen(buffer) + 1);
        strcpy(result.value, buffer);
        result.symbol = NULL;
        result.line = 0;
        return result;
    }
    
    // Return empty string on EOF
    token result;
    result.type = STRING;
    result.value = malloc(1);
    result.value[0] = '\0';
    result.symbol = NULL;
    result.line = 0;
    return result;
}

token native_str(token* args, int arg_count) {
    if (arg_count != 1) {
        fprintf(stderr, "Error: str() takes exactly 1 argument\n");
        exit(70);
    }
    
    token result;
    result.type = STRING;
    result.symbol = NULL;
    result.line = 0;
    
    switch (args[0].type) {
        case NUMBER:
            result.value = malloc(32);
            if (args[0].value) {
                strcpy(result.value, args[0].value);
            } else {
                strcpy(result.value, "0");
            }
            break;
        case STRING:
            if (args[0].value) {
                result.value = malloc(strlen(args[0].value) + 1);
                strcpy(result.value, args[0].value);
            } else {
                result.value = malloc(4);
                strcpy(result.value, "nil");
            }
            break;
        case TRUE:
            result.value = malloc(5);
            strcpy(result.value, "true");
            break;
        case FALSE:
            result.value = malloc(6);
            strcpy(result.value, "false");
            break;
        case NIL:
            result.value = malloc(4);
            strcpy(result.value, "nil");
            break;
        default:
            result.value = malloc(8);
            strcpy(result.value, "unknown");
            break;
    }
    
    return result;
}