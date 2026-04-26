# C Language EBNF Grammar (c0c subset)

This EBNF covers the C subset targeted by c0c, sufficient for self-hosting.
Notation: `{ }` = zero or more, `[ ]` = optional, `|` = alternation, `( )` = grouping.

---

## 1. Top-Level

```
translation_unit ::= { external_decl }

external_decl ::= function_def
                | decl ";"

function_def ::= decl_specifiers declarator compound_stmt
```

---

## 2. Declarations

```
decl ::= decl_specifiers init_declarator_list
       | decl_specifiers                        (* struct/typedef with no var *)

decl_specifiers ::= { storage_class_spec | type_qualifier } type_spec { type_qualifier }

storage_class_spec ::= "typedef" | "extern" | "static" | "auto" | "register"

type_qualifier ::= "const" | "volatile"

type_spec ::= "void"
            | "char"
            | "short"
            | "int"
            | "long"
            | "float"
            | "double"
            | "signed"
            | "unsigned"
            | struct_spec
            | enum_spec
            | typedef_name

struct_spec ::= ( "struct" | "union" ) [ IDENT ] "{" { struct_decl } "}"
              | ( "struct" | "union" ) IDENT

struct_decl ::= decl_specifiers struct_declarator_list ";"

struct_declarator_list ::= struct_declarator { "," struct_declarator }

struct_declarator ::= declarator
                    | [ declarator ] ":" const_expr   (* bit-field *)

enum_spec ::= "enum" [ IDENT ] "{" enumerator_list [ "," ] "}"
            | "enum" IDENT

enumerator_list ::= enumerator { "," enumerator }

enumerator ::= IDENT [ "=" const_expr ]

typedef_name ::= IDENT    (* previously declared via typedef *)

init_declarator_list ::= init_declarator { "," init_declarator }

init_declarator ::= declarator [ "=" initializer ]

declarator ::= { pointer } direct_declarator

pointer ::= "*" { type_qualifier }

direct_declarator ::= IDENT
                    | "(" declarator ")"
                    | direct_declarator "[" [ const_expr ] "]"
                    | direct_declarator "(" param_type_list ")"
                    | direct_declarator "(" [ ident_list ] ")"

param_type_list ::= param_list [ "," "..." ]

param_list ::= param_decl { "," param_decl }

param_decl ::= decl_specifiers declarator
             | decl_specifiers [ abstract_declarator ]

ident_list ::= IDENT { "," IDENT }

abstract_declarator ::= pointer [ direct_abstract_declarator ]
                       | direct_abstract_declarator

direct_abstract_declarator ::= "(" abstract_declarator ")"
                              | [ direct_abstract_declarator ] "[" [ const_expr ] "]"
                              | [ direct_abstract_declarator ] "(" [ param_type_list ] ")"

initializer ::= assign_expr
              | "{" initializer_list [ "," ] "}"

initializer_list ::= [ designator ] initializer { "," [ designator ] initializer }

designator ::= "[" const_expr "]" "="
             | "." IDENT "="
```

---

## 3. Statements

```
stmt ::= labeled_stmt
       | compound_stmt
       | expr_stmt
       | selection_stmt
       | iteration_stmt
       | jump_stmt

labeled_stmt ::= IDENT ":" stmt
               | "case" const_expr ":" stmt
               | "default" ":" stmt

compound_stmt ::= "{" { decl ";" | stmt } "}"

expr_stmt ::= [ expr ] ";"

selection_stmt ::= "if" "(" expr ")" stmt [ "else" stmt ]
                 | "switch" "(" expr ")" stmt

iteration_stmt ::= "while" "(" expr ")" stmt
                 | "do" stmt "while" "(" expr ")" ";"
                 | "for" "(" [ expr | decl ] ";" [ expr ] ";" [ expr ] ")" stmt

jump_stmt ::= "goto" IDENT ";"
            | "continue" ";"
            | "break" ";"
            | "return" [ expr ] ";"
```

---

## 4. Expressions

```
expr ::= assign_expr { "," assign_expr }

assign_expr ::= unary_expr assign_op assign_expr
              | cond_expr

assign_op ::= "=" | "+=" | "-=" | "*=" | "/=" | "%="
            | "&=" | "|=" | "^=" | "<<=" | ">>="

cond_expr ::= log_or_expr [ "?" expr ":" cond_expr ]

log_or_expr  ::= log_and_expr  { "||" log_and_expr }
log_and_expr ::= bit_or_expr   { "&&" bit_or_expr }
bit_or_expr  ::= bit_xor_expr  { "|"  bit_xor_expr }
bit_xor_expr ::= bit_and_expr  { "^"  bit_and_expr }
bit_and_expr ::= eq_expr       { "&"  eq_expr }

eq_expr ::= rel_expr { ( "==" | "!=" ) rel_expr }
rel_expr ::= shift_expr { ( "<" | ">" | "<=" | ">=" ) shift_expr }

shift_expr ::= add_expr { ( "<<" | ">>" ) add_expr }

add_expr ::= mul_expr { ( "+" | "-" ) mul_expr }
mul_expr ::= cast_expr { ( "*" | "/" | "%" ) cast_expr }

cast_expr ::= "(" type_name ")" cast_expr
            | unary_expr

unary_expr ::= postfix_expr
             | "++" unary_expr
             | "--" unary_expr
             | unary_op cast_expr
             | "sizeof" unary_expr
             | "sizeof" "(" type_name ")"

unary_op ::= "&" | "*" | "+" | "-" | "~" | "!"

postfix_expr ::= primary_expr
               | postfix_expr "[" expr "]"
               | postfix_expr "(" [ arg_expr_list ] ")"
               | postfix_expr "." IDENT
               | postfix_expr "->" IDENT
               | postfix_expr "++"
               | postfix_expr "--"

arg_expr_list ::= assign_expr { "," assign_expr }

primary_expr ::= IDENT
               | INT_LIT
               | FLOAT_LIT
               | CHAR_LIT
               | STRING_LIT { STRING_LIT }   (* adjacent string concat *)
               | "(" expr ")"

const_expr ::= cond_expr    (* value must be compile-time constant *)

type_name ::= decl_specifiers [ abstract_declarator ]
```

---

## 5. Lexical Elements

```
IDENT ::= ( letter | "_" ) { letter | digit | "_" }
        (* not a keyword *)

letter ::= "a".."z" | "A".."Z"
digit  ::= "0".."9"

INT_LIT ::= decimal_lit | octal_lit | hex_lit

decimal_lit ::= nonzero_digit { digit } [ int_suffix ]
octal_lit   ::= "0" { octal_digit } [ int_suffix ]
hex_lit     ::= ( "0x" | "0X" ) hex_digit { hex_digit } [ int_suffix ]

int_suffix ::= ( "u" | "U" ) [ "l" | "L" | "ll" | "LL" ]
             | ( "l" | "L" | "ll" | "LL" ) [ "u" | "U" ]

nonzero_digit ::= "1".."9"
octal_digit   ::= "0".."7"
hex_digit     ::= digit | "a".."f" | "A".."F"

FLOAT_LIT ::= decimal_float | hex_float

decimal_float ::= { digit } "." { digit } [ exp_part ] [ float_suffix ]
                | digit { digit } exp_part [ float_suffix ]
hex_float     ::= ( "0x" | "0X" ) hex_significand bin_exp [ float_suffix ]

exp_part      ::= ( "e" | "E" ) [ "+" | "-" ] digit { digit }
bin_exp       ::= ( "p" | "P" ) [ "+" | "-" ] digit { digit }
hex_significand ::= hex_digit { hex_digit } [ "." { hex_digit } ]
                  | [ hex_digit { hex_digit } ] "." hex_digit { hex_digit }
float_suffix  ::= "f" | "F" | "l" | "L"

CHAR_LIT ::= "'" c_char "'"
c_char ::= any character except "'" and "\" | escape_seq

STRING_LIT ::= '"' { s_char } '"'
s_char ::= any character except '"' and "\" | escape_seq

escape_seq ::= "\" ( "'" | '"' | "?" | "\" | "a" | "b" | "f"
                   | "n" | "r" | "t" | "v" | "0" | octal_digit
                   | "x" hex_digit { hex_digit } )
```

---

## 6. Keywords

```
auto      break     case      char      const
continue  default   do        double    else
enum      extern    float     for       goto
if        int       long      register  return
short     signed    sizeof    static    struct
switch    typedef   union     unsigned  void
volatile  while
```

---

## 7. Operators & Punctuation

```
Arithmetic  : +  -  *  /  %
Bitwise     : &  |  ^  ~  <<  >>
Logical     : &&  ||  !
Comparison  : ==  !=  <  >  <=  >=
Assignment  : =  +=  -=  *=  /=  %=  &=  |=  ^=  <<=  >>=
Increment   : ++  --
Pointer     : *  &  ->
Member      : .
Ternary     : ?  :
Sizeof      : sizeof
Punctuation : (  )  [  ]  {  }  ;  ,  ...  #
```

---

## 8. Comments & Whitespace

```
line_comment  ::= "//" { any char except newline } newline
block_comment ::= "/*" { any char } "*/"
whitespace    ::= " " | "\t" | "\n" | "\r"
(* comments and whitespace are discarded by the lexer *)
```

---

## 9. Preprocessor (handled externally)

c0c assumes input has already been passed through `cpp` (the C preprocessor).
Directives such as `#include`, `#define`, `#ifdef` are **not** parsed by c0c itself.

---

## 10. Scope and Semantic Notes

- **Scoping**: block-scoped (`{}`) with file-level globals. Identifiers visible from declaration point to end of enclosing block.
- **Type system**: arithmetic conversions follow C99 usual arithmetic conversions; implicit int→pointer cast is an error.
- **Struct layout**: members laid out sequentially; alignment follows platform ABI (macOS ARM64 / x86-64).
- **Calling convention**: follows platform C ABI (System V AMD64 on x86-64; AAPCS64 on ARM64).
- **String literals**: `char *` read-only, stored in `.rodata` / `@.str` in LLVM IR.
- **`sizeof`**: evaluated at compile time; result type is `size_t` (i64 on 64-bit targets).
