# py0i 支援的 Python 語法 BNF

## 程式結構

```
program          ::= stmt*

stmt             ::= expr_stmt
                  | assignment_stmt
                  | aug_assignment_stmt
                  | annotated_assignment_stmt
                  | if_stmt
                  | while_stmt
                  | for_stmt
                  | function_def
                  | class_def
                  | return_stmt
                  | break_stmt
                  | continue_stmt
                  | pass_stmt
                  | global_stmt
                  | nonlocal_stmt
                  | delete_stmt
                  | import_stmt
                  | import_from_stmt
                  | try_stmt
                  | with_stmt
                  | raise_stmt
                  | assert_stmt

expr_stmt        ::= expression

assignment_stmt  ::= target "=" expression
target           ::= name | attribute | subscript | star_expr

aug_assignment_stmt
                  ::= target ( "+=" | "-=" | "*=" | "/=" | "//=" | "%=" | "**="
                                  | "&=" | "|=" | "^=" | "<<=" | ">>=" | "@=" ) expression

annotated_assignment_stmt
                  ::= target ":" type_expr ["=" expression]
```

## 控制流

```
if_stmt          ::= "if" expression ":" suite
                    ("elif" expression ":" suite)*
                    ["else" ":" suite]

while_stmt       ::= "while" expression ":" suite
                    ["else" ":" suite]

for_stmt         ::= "for" target "in" expression ":" suite
                    ["else" ":" suite]

return_stmt      ::= "return" [expression]
break_stmt       ::= "break"
continue_stmt    ::= "continue"
pass_stmt        ::= "pass"
```

## 函式與類別

```
function_def     ::= ["async"] "def" name "(" [parameters] ")" ["->" type_expr] ":" suite
parameters       ::= param ["," param]* ["," "*" name] ["," "**" name]
                  | "*" name ["," "**" name]
param            ::= name ["=" expression]

class_def        ::= "class" name ["(" expression ["," expression]* ")"] ":" suite
```

## 例外處理

```
try_stmt         ::= "try" ":" suite
                    ("except" [expression ["as" name]] ":" suite)+
                    ["else" ":" suite]
                    ["finally" ":" suite]

with_stmt        ::= "with" expression ["as" target] ":" suite

raise_stmt       ::= "raise" [expression]
assert_stmt      ::= "assert" expression ["," expression]
```

## 模組與作用域

```
global_stmt      ::= "global" name ["," name]*
nonlocal_stmt    ::= "nonlocal" name ["," name]*
delete_stmt      ::= "del" target ["," target]*
import_stmt      ::= "import" name ["as" name] ["," name ["as" name]]*
import_from_stmt ::= "from" name "import" ( name ["as" name]
                                          | "*" )
```

## 表達式

```
expression       ::= or_expr [":=" or_expr]
or_expr          ::= and_expr ("or" and_expr)*
and_expr         ::= not_expr ("and" not_expr)*
not_expr         ::= "not" not_expr | comparison
comparison       ::= bitwise_or (( "<" | ">" | "==" | ">=" | "<=" | "!="
                                  | "is" ["not"]
                                  | "in" ["not"]
                                  | "<>" ) bitwise_or)*
bitwise_or       ::= bitwise_xor ("|" bitwise_xor)*
bitwise_xor      ::= bitwise_and ("^" bitwise_and)*
bitwise_and      ::= shift ("&" shift)*
shift            ::= arith (("<<" | ">>") arith)*
arith            ::= term (("+" | "-") term)*
term             ::= factor (("*" | "/" | "//" | "%" | "@") factor)*
factor           ::= ("+" | "-" | "~") factor | power
power            ::= call_expr ["**" factor]
call_expr        ::= primary ["(" [argument ["," argument]*] ")"]
primary          ::= atom | attribute | subscript | call
                  | "(" [expression ["," expression]*] ")"
                  | lambda

atom             ::= name | literal | "None" | "True" | "False" | "..."
literal          ::= integer | float | string | bytes
                  | list_literal | tuple_literal | set_literal | dict_literal
                  | list_comp | set_comp | dict_comp | gen_expr

list_literal     ::= "[" [expression ["," expression]*] "]"
tuple_literal    ::= "(" [expression ["," expression]* [","] ] ")"
set_literal      ::= "{" expression ["," expression]* "}"
dict_literal     ::= "{" [key_value ["," key_value]*] "}"
key_value        ::= expression ":" expression

list_comp        ::= "[" expression comp_for "]"
set_comp         ::= "{" expression comp_for "}"
dict_comp        ::= "{" key_expr ":" value_expr comp_for "}"
gen_expr         ::= "(" expression comp_for ")"
comp_for         ::= "for" target "in" expression [comp_if]
comp_if          ::= "if" expression [comp_if]

attribute        ::= primary "." name
subscript        ::= primary "[" expression "]"
lambda           ::= "lambda" [parameters] ":" expression
```

## 內建類型 (部分支援)

```
type_expr        ::= name  (完整類型標註僅語法支援，無實際類型檢查)
```

## 運算子優先序 (從低到高)

| 層級 | 運算子 |
|------|--------|
| 1 | `lambda` |
| 2 | `if-else` |
| 3 | `or` |
| 4 | `and` |
| 5 | `not` |
| 6 | `in`, `not in`, `is`, `is not`, `<`, `>`, `<=`, `>=`, `==`, `!=` |
| 7 | `|` |
| 8 | `^` |
| 9 | `&` |
| 10 | `<<`, `>>` |
| 11 | `+`, `-` |
| 12 | `*`, `/`, `//`, `%`, `@` |
| 13 | `+`, `-`, `~` (一元) |
| 14 | `**` |
| 15 | `await` |

## 備註

- py0i/py0c 使用自寫的 `ast.py`（非 Python 標準庫），是 Python 語法的子集
- AsyncFunctionDef 會被當作一般 FunctionDef 處理（無真正的非同步執行）
- 類型標註 (type hints) 只會被解析，不會執行類型檢查
- f-string 支援完整的格式化规格 (`:.2f` 等)