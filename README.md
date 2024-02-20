### Example code
```basic
REM Personalised welcome script

INPUT "Whats your name: ", name
INPUT "How old are you: ", age
TONUM age

IF age < 12 THEN
  PRINT "Hi kid!"
ELSE
  PRINT "Welcome " + name
END
```


### Variable names
- Can contain only english alphabet and underscore `a-zA-Z_`
- Must not collide with builtin keywords
- Case sensitive


### Variable types
- String: variable length UTF-8 string
- Number: IEEE double-precision floating point number (64 bit)
- Boolean: true / false


### Variable assigment
Variable are declared and assigned using `LET` keyword
- eg. `LET num = 14`
- eg. `LET VarOne = "Age: " + 14`


- You must use let even when assigning to already existing variable.
- There is no such value as null. Every variable must have a value.

### Operations with types

- Number
  - Comparing `<`,`>`,`<=`,`>=`,`==`,`<>`
    - `<number> OP <number> -> <boolean>`
    - eg. `7 > 5 -> true`

  - Arithmetic `+`,`-`,`*`,`/`
    - `<number> OP <number> -> <number>`
    - eg. `7 / 2 -> 3.5`
    - `DivisionByZero` runtime error may occur.
  
  - Negation `-`
    - `- <number> -> <number>`
    - eg. `-7 -> -7`

- String
  - Joining `+`
    - `<string> + <string> -> <string>`
    - `<string> + <number> -> <string>`
    - `<string> + <boolean> -> <string>`
    - `<number> + <string> -> <string>`
    - `<boolean> + <string> -> <string>`
    - eg. `"I am from" + "Pilsen" -> "I am from Pilsen"`
    - eg. `"I am " + 7 + "years old." -> "I am 7 years old."`
    - eg. `"This statement is " + True -> "This statement is TRUE"`

  - Comparing `==`, `<>`
    - `<string> OP <string> -> <boolean>`
    - eg. `"Hello" == "hello" -> false`
    - Comparing is case sensitive

- Boolean
  - Negation `NOT `
    - `NOT <boolean> -> <boolean>`
    - eg. `NOT true -> false`

  - Logic operators `AND`,`OR`
    - `<boolean> OP <boolean> -> <boolean>`
    - eg. `false OR 1 > 0 AND true -> true`


### Language functionality

- Keywords are case-insensitive

- There is no such value as null. Every variable must have a value.

#### Input / Output
- `PRINT <expr>`
  - Evaluates and prints expr to stdout with new line at the end
  - \<expr\> can be `string`, `number`, `boolean`
  - `number` is printed to two decimal places if decimal, otherwise it gets printed without decimal places if whole.
  - eg. `PRINT "Age: " + 7`
- `INPUT <expr>, var` 
  - Evaluates and prints expr to stdout and reads user input to variable var
  - \<expr\> can be `string`, `number`, `boolean`
  - eg. `INPUT "Age: ", age`

#### Control flow

- `IF`
  - Executes `expr`, if result is true, execute expressions in `then_branch`, else execute expressions in `else_branch` if exists.
  - Else branch is optional
  - Branches can contain multiple expressions
  - Usage: 
  ```basic
  IF expr THEN
    ...then_branch...
  (ELSE
    ...else_branch...)
  END
  ```
  - eg.
  ```basic
  IF 5 < 7 THEN
    PRINT "Hello"
    PRINT "Low number"
  ELSE
    PRINT "Else never occures"
  END
  ```
  
- `WHILE`
  - Execute `code` until `expr` evaluates to true
  - Usage:
  ```basic
  WHILE expr DO
    code
  END 
  ```
  - eg.
  ```basic
  LET i = 0
  WHILE i < 10 DO
    PRINT "Hello for " + i + " time."
  END
  ```

- `BREAK`
  - Stops execution of `code` and exits the closest `WHILE` loop.
  - `LoopControlOutsideLoop` error may occur if not inside a loop.

- `CONTINUE`
  - Stops execution of `code` and continues at the beginning of the closest `WHILE` loop.
  - `LoopControlOutsideLoop` error may occur if not inside a loop.

#### Other
- `REM comment`
  - Comment. Anything after `REM` on that line is ignored.

- `TONUM var(, out_var)`
  - Converts `var` to number and saves the result to `out_var`
  - If only `var` is defined, output is saved back to `var`
  - `InvalidNumberFormat` error may occur if conversion failed 

- `TOSTR var(, out_var)`
  - Converts `var` to string and saves the result to `out_var`
  - If only `var` is defined, output is saved back to `var`


### Runtime errors
If error occur, execution of code stops and error message is printed to stderr.

- `DivisionByZero`
- `LoopControlOutsideLoop`
- `InvalidNumberFormat`
- `VariableNotDeclared`
