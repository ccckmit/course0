# py0c EBNF (v0.1)

This EBNF defines the initial static subset of `py0` for v0.1.
The subset is intentionally small to enable a clean parser/AST pipeline.

```
program      = { function } ;

function     = "def" identifier "(" [ param_list ] ")" ":" suite ;
param_list   = identifier { "," identifier } ;

suite        = NEWLINE INDENT { statement } DEDENT ;

statement    = simple_stmt | if_stmt | while_stmt ;

simple_stmt  = assign_stmt
             | return_stmt
             | pass_stmt
             | expr_stmt ;

assign_stmt  = identifier "=" expr NEWLINE ;
return_stmt  = "return" expr NEWLINE ;
pass_stmt    = "pass" NEWLINE ;
expr_stmt    = expr NEWLINE ;

if_stmt      = "if" expr ":" suite [ "else" ":" suite ] ;
while_stmt   = "while" expr ":" suite ;

expr         = equality ;
equality     = relational { ( "==" | "!=" ) relational } ;
relational   = additive { ( "<" | ">" | "<=" | ">=" ) additive } ;
additive     = multiplicative { ( "+" | "-" ) multiplicative } ;
multiplicative = unary { ( "*" | "/" | "%" ) unary } ;

unary        = ( "+" | "-" ) unary | primary ;
primary      = number | identifier | call | "(" expr ")" ;

call         = identifier "(" [ arg_list ] ")" ;
arg_list     = expr { "," expr } ;

identifier   = letter { letter | digit | "_" } ;
number       = digit { digit } ;
```

Notes:
- Indentation is significant and produces `INDENT`/`DEDENT` tokens.
- Only `int` is supported in v0.1; all expressions are `int`.
- A file may contain multiple function definitions; no top-level statements.
- Function bodies are non-empty suites; `pass` is allowed.
