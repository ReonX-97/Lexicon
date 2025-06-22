# Lox Language Interpreter

A complete C implementation of the Lox programming language interpreter, built as part of the "Crafting Interpreters" book project.

## Overview

This project implements a tree-walking interpreter for the Lox programming language. It includes lexical analysis (tokenization), parsing, and execution of Lox programs with support for expressions, statements, variables, and control flow.

## Features

- **Tokenization**: Converts Lox source code into tokens
- **Expression Parsing**: Parses expressions with proper operator precedence
- **Statement Parsing**: Parses declarations, assignments, control flow, and function definitions
- **Expression Evaluation**: Evaluates expressions with proper operator precedence
- **Statement Execution**: Executes statements including variable declarations, assignments, and control flow
- **Variable Management**: Support for variable declaration, assignment, and scoping
- **Control Flow**: Implementation of `if/else`, `while`, and `for` loops

- **Error Handling**: Comprehensive error reporting with line numbers
- **Memory Management**: Proper allocation and deallocation of dynamic memory

## Supported Language Features

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
- Functions: `fun`, `return` (reserved for future implementation)
- Classes: `class`, `super`, `this` (reserved for future implementation)
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
src/
├── evaluate.c              # Expression evaluation
├── evaluation_helper.c     # Helper functions for evaluation
├── execute_helper.c        # Helper functions for statement execution
├── execute_statements.c    # Statement execution engine
├── expression_parser.c     # Expression parsing
├── main.c                 # Main program entry point
├── parser_helper.c        # Parser utility functions
├── scanner.c              # Token scanner
├── statement_parser.c     # Statement parsing
└── tokenizer.c           # Token creation and management
```

## Building

The project uses a Makefile for compilation. To build:

```bash
make
```

This will create the executable in the appropriate directory.

## Usage

The program supports four main commands:

### 1. Tokenization

```bash
./lexicon tokenize <filename>
```

Outputs each token in the format:
```
TOKEN_TYPE symbol value
```

Example:
```
VAR var null
IDENTIFIER x null
EQUAL = null
NUMBER 42 42.0
SEMICOLON ; null
EOF null
```

### 2. Parsing

```bash
./lexicon parse <filename>
```

Outputs the parsed expression in prefix notation (AST format).

Example input: `1 + 2 * 3`
Example output: `(+ 1.0 (* 2.0 3.0))`

### 3. Evaluation

```bash
./lexicon evaluate <filename>
```

Evaluates a single expression and outputs the result.

Example input: `1 + 2 * 3`
Example output: `7.0`

### 4. Program Execution

```bash
./lexicon run <filename>
```

Executes a complete Lox program with statements, variables, and control flow.

## Language Grammar

### Expressions
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

### Statements
The interpreter supports the following statement types:

```
program        → declaration* EOF
declaration    → varDecl | statement
varDecl        → "var" IDENTIFIER ( "=" expression )? ";"
statement      → exprStmt | printStmt | block | ifStmt | whileStmt | forStmt
exprStmt       → expression ";"
printStmt      → "print" expression ";"
block          → "{" declaration* "}"
ifStmt         → "if" "(" expression ")" statement ( "else" statement )?
whileStmt      → "while" "(" expression ")" statement
forStmt        → "for" "(" ( varDecl | exprStmt | ";" )
                 expression? ";"
                 expression? ")" statement
```

*Note: Function and class declarations are not yet implemented.*

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
- Undefined variables
- Runtime errors during execution

Errors are reported to stderr and the program exits with appropriate error codes:
- Exit code 65: Syntax/parsing errors
- Exit code 70: Runtime errors

## Memory Management

The implementation carefully manages memory allocation and deallocation:
- Dynamic token arrays that grow as needed
- Proper cleanup of token symbol and value strings
- Memory-safe string operations
- AST node cleanup after execution

## Example Programs

### Variable Declaration and Assignment
```lox
var x = 10;
var y = 20;
print x + y; // Output: 30
```

### Control Flow
```lox
var x = 5;
if (x > 3) {
    print "x is greater than 3";
} else {
    print "x is not greater than 3";
}
```

### Loops
```lox
// While loop
var i = 0;
while (i < 5) {
    print i;
    i = i + 1;
}

// For loop
for (var j = 0; j < 3; j = j + 1) {
    print "Iteration: " + j;
}
```

### Complex Expressions
```lox
var result = (5 + 3) * 2 == 16;
print result; // Output: true
```

## Development

This implementation follows the architecture described in "Crafting Interpreters" by Robert Nystrom, with a focus on:
- Clean separation between lexical analysis, parsing, and execution
- Recursive descent parsing for both expressions and statements
- Tree-walking interpretation
- Proper error recovery and reporting
- Efficient memory usage
- Modular code organization across multiple source files

## Architecture

The interpreter consists of several key components:

- **Scanner/Tokenizer**: Breaks source code into tokens
- **Expression Parser**: Builds AST for expressions using recursive descent
- **Statement Parser**: Handles declarations and control structures
- **Evaluator**: Executes expressions and returns values
- **Statement Executor**: Interprets statements and manages program flow
- **Helper Modules**: Utility functions for parsing and execution

Each component is designed to be modular and maintainable, following the principles outlined in the "Crafting Interpreters" book.