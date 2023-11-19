# Language Evaluator

## Overview

The Language Evaluator project is a command-line tool 
implemented in C that allows users to evaluate arithmetic 
expressions and obtain results in different number bases. 
The program supports expressions in base 10, base 2 (binary), 
and base 16 (hexadecimal). Additionally, users can manipulate the 
width of the expression by specifying the number of bits, and the 
default width is 32 bits.

## Features

- Evaluate arithmetic expressions.
- Specify the input expression and the desired output base using command-line flags.
- Support for base 10 (decimal), base 2 (binary), and base 16 (hexadecimal).
- Default base is 10 if no base flag is provided.
- Optionally manipulate the width of the expression using the `-w` flag.
    - Supported widths: 8, 16, and 32 bits (default).

## Usage

### Basic Usage

To evaluate a simple arithmetic expression in base 10:

```bash
./calc -e "10 * 2 / 5" 
```

### Specify Output Base

To evaluate a simple arithmetic expression in base 2, 10, or 16:

```bash
./calc -e "0b110 + 0b110" -b 16
```

### Manipulate Width

To evaluate an expression and display the result in binary with a width of 8, 16, or 32 bits:

```bash
./calc -e "0xA + 1" -b 2 -w 8
```

### Default Base and Width
If no base or width flag is provided, the default output base is 10, and the default width is
32 bits.

## Implementation Details
1. ***Tokenization:*** The input expression is tokenized to break it down into individual elements,
such as numbers, operators, and other operands.
2. ***Parsing:*** The tokens are then parsed to construct an abstract syntax tree (AST) based on
the EBNF grammar rules.
3. ***Evaluation:*** The AST is traversed to evaluate the expression, and the result id displayed
in the specified or default base and width

## Build
To build the Language Evaluator, use the provided Makefile:
```bash
make
```