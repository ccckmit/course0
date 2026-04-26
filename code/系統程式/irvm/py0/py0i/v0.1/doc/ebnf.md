# py0i (fact.py) EBNF Grammar

以下 EBNF 語法描述了 `fact.py` 中所使用到的 Python 語法子集（包含函數定義、條件判斷、回傳、變數賦值、函數呼叫及基本的運算）：

```ebnf
program      = statement*

statement    = def_stmt 
             | if_stmt 
             | return_stmt 
             | assign_stmt 
             | expr_stmt

def_stmt     = 'def' id '(' [ id_list ] ')' ':' block
if_stmt      = 'if' expr ':' block [ 'else' ':' block ]
return_stmt  = 'return' expr
assign_stmt  = id '=' expr
expr_stmt    = expr

id_list      = id ( ',' id )*

block        = NEWLINE INDENT statement+ DEDENT

expr         = logic_expr
logic_expr   = rel_expr ( ( 'and' | 'or' ) rel_expr )*
rel_expr     = add_expr ( ( '==' | '!=' | '<' | '<=' | '>' | '>=' ) add_expr )*
add_expr     = mul_expr ( ( '+' | '-' ) mul_expr )*
mul_expr     = primary ( ( '*' | '/' | '%' ) primary )*

primary      = id [ '(' [ expr_list ] ')' ]
             | num
             | str
             | '(' expr ')'

expr_list    = expr ( ',' expr )*

id           = [a-zA-Z_] [a-zA-Z0-9_]*
num          = [0-9]+
str          = '"' [^"]* '"' | "'" [^']* "'"
```

## 說明
- `program`: 一個程式由零個或多個陳述句 (`statement`) 組成。
- `statement`: 包含函數定義 (`def`)、條件控制 (`if/else`)、回傳 (`return`)、變數賦值 (`=`)、一般運算式陳述 (`expr_stmt`) 等。
- `block`: Python 的區塊透過換行 (`NEWLINE`) 加上縮排 (`INDENT`) 定義，並在區塊結束時終止縮排 (`DEDENT`)。
- 運算式的優先順序由低到高分別為：邏輯運算 (`logic_expr`) < 關係運算 (`rel_expr`) < 加減法 (`add_expr`) < 乘除法 (`mul_expr`) < 基本元素 (`primary`)。
- `primary`: 最基本的求值單元，支援變數、函數呼叫 (`id(...)`)、整數數字、字串以及用括號括起來的子運算式。
