# Development documentation for BasicPlusPlus

## Overall structure

- Tokenization
- Parsing
- Interpreting
- Main entry point (`main.cpp`), stitching all together.

### Tokenization
- Files: `Tokenization.hpp`, `Tokenization.cpp`
- Responsible for converting input source code to vector of tokens.
- Defines `TokenType` enum with all tokens that exists.
- Defines `Literal` variant for all possible Literal types.
- Defines `Tokenizer` class for scanning tokens from istream to a vector of Tokens.
  - Can throw `TokenizationError`

### Parsing
- Files: `Parser.hpp`, `Parser.cpp`, `ExpressionsStatements.hpp`
- Responsible for converting vector of tokens to Abstract Syntax Tree (AST).
- `ExpressionsStatements`:
  - Defines `Expr` and `Stmt` with its subclasses for all possible expressions and statements.
  - Defines `AbstractExprVisitor`, `AbstractStmtVisitor` for implementation by the interpreter.
- `Parser`:
  - Defines `Parser` class for parsing tokens to AST.
  - Uses recursive descent parsing for parsing expressions and statements.
  - Uses `Expr` and `Stmt` subclasses for representing expressions and statements.
  - Can throw `ParsingError`

### Interpreting
- Files: `Interpreter.hpp`, `Interpreter.cpp`
- Responsible for interpreting AST.
- Defines `Interpreter` class implementing `AbstractExprVisitor` and `AbstractStmtVisitor` for interpreting AST using `interpret(statement)`.

### Main entry point
- Files: `main.cpp`
- Responsible for stitching all together.
- Gives help to user, opens input file, prints errors, sets random seed.
