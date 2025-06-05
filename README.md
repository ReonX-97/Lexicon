# Lox Language Tokenizer and Parser

A C implementation of a tokenizer and expression parser for the Lox programming language, built as part of the "Crafting Interpreters" book project.

## Overview

This project implements the lexical analysis (tokenization) and parsing phases of a Lox interpreter. It can tokenize Lox source code into a stream of tokens and parse expressions into an Abstract Syntax Tree (AST) representation.

## Features

- **Tokenization**: Converts Lox source code into tokens
- **Expression Parsing**: Parses expressions with proper operator precedence
- **Error Handling**: Comprehensive error reporting with line numbers
- **Memory Management**: Proper allocation and deallocation of dynamic memory

## Supported Tokens

### Operators
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `<`, `<=`, `>`, `>=`, `==`, `!=`
- Logical: `and`, `or`, `!`
- Assignment: `=`

### Literals
- Numbers (integers and floats)
- Strings (double-quoted)
- Booleans (`true`, `false`)
- `nil`

### Keywords
- Control flow: `if`, `else`, `for`, `while`
- Functions: `fun`, `return`
- Classes: `class`, `super`, `this`
- Variables: `var`
- I/O: `print`

### Punctuation
- Parentheses: `(`, `)`
- Braces: `{`, `}`
- Comma: `,`
- Dot: `.`
- Semicolon: `;`

## Project Structure

```
.
├── bin/                    # Compiled executables
│   └── tokenizer
├── include/               # Header files
│   ├── parser.h
│   └── tokenizer_scanner.h
├── obj/                   # Object files
├── src/                   # Source files
│   ├── main.c            # Main program entry point
│   ├── parser.c          # Expression parser implementation
│   ├── scanner.c         # Token scanner
│   └── tokenizer.c       # Token creation and management
├── test/                  # Test files and outputs
└── Makefile              # Build configuration
```

## Building

The project uses a Makefile for compilation. To build:

```bash
make
```

This will create the executable in the `bin/` directory.

## Usage

The program supports two main commands:

### Tokenization

```bash
./bin/tokenizer tokenize <filename>
```

Outputs each token in the format:
```
TOKEN_TYPE symbol value
```

Example:
```
NUMBER 123 123.0
PLUS + null
IDENTIFIER name null
EOF null
```

### Parsing

```bash
./bin/tokenizer parse <filename>
```

Outputs the parsed expression in prefix notation (AST format):

Example input: `1 + 2 * 3`
Example output: `(+ 1.0 (* 2.0 3.0))`

## Expression Grammar

The parser implements the following expression grammar with proper precedence:

```
expression     → or
or             → and ( "or" and )*
and            → equality ( "and" equality )*
equality       → comparison ( ( "!=" | "==" ) comparison )*
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )*
term           → factor ( ( "-" | "+" ) factor )*
factor         → unary ( ( "/" | "*" ) unary )*
unary          → ( "!" | "-" ) unary | primary
primary        → "true" | "false" | "nil" | NUMBER | STRING | IDENTIFIER
               | "(" expression ")"
```

## Operator Precedence (highest to lowest)

1. Unary (`!`, `-`)
2. Multiplicative (`*`, `/`)
3. Additive (`+`, `-`)
4. Comparison (`<`, `<=`, `>`, `>=`)
5. Equality (`==`, `!=`)
6. Logical AND (`and`)
7. Logical OR (`or`)

## Error Handling

The program provides detailed error messages with line numbers for:
- Unexpected characters during tokenization
- Unterminated strings
- Invalid numbers
- Unexpected tokens during parsing
- Missing closing parentheses

Errors are reported to stderr and the program exits with appropriate error codes.

## Memory Management

The implementation carefully manages memory allocation and deallocation:
- Dynamic token arrays that grow as needed
- Proper cleanup of token symbol and value strings
- Memory-safe string operations

## Example

Given the input file `test.lox`:
```
1 + 2 * 3 == 7
```

**Tokenization output:**
```
NUMBER 1 1.0
PLUS + null
NUMBER 2 2.0
STAR * null
NUMBER 3 3.0
EQUAL_EQUAL == null
NUMBER 7 7.0
EOF null
```

**Parsing output:**
```
(== (+ 1.0 (* 2.0 3.0)) 7.0)
```

## Development

This implementation follows the architecture described in "Crafting Interpreters" by Robert Nystrom, with a focus on:
- Clean separation between lexical analysis and parsing
- Recursive descent parsing
- Proper error recovery and reporting
- Efficient memory usage