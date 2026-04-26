# C5 (v1.0) 語言 EBNF 語法規範

C5 專案實作了 C 語言的一個具備高度擴充套件的子集。它使用了 Recursive Descent (遞迴下降) 方法來解析語法。原版 C5 本來只有非常受限的變數與函式，但在我們這個 v1.0 擴展版中，已經加入了 `struct`、浮點數 `double/float`、`for` 迴圈以及多檔宣告用的 `extern` 等等。

以下是以 **EBNF (Extended Backus-Naur Form)** 描述 `c5.c` 所實質支援的語法規範。

---

## 1. 頂層結構 (Program Structure)
一個 C5 程式由一連串的宣告區塊、列舉 (Enum) 或是函式定義所組成。

```ebnf
program = { global_declaration | struct_declaration | enum_declaration | function_definition } ;

global_declaration = [ "extern" ] type_specifier identifier [ array_decl | "=" expression ] 
                     { "," identifier [ array_decl | "=" expression ] } ";" ;

array_decl = "[" integer_literal "]" ;

enum_declaration = "enum" [ identifier ] "{" enumerator { "," enumerator } "}" ";" ;
enumerator = identifier [ "=" integer_literal ] ;

struct_declaration = "struct" identifier "{" { type_specifier identifier ";" } "}" ";" ;
```
> **注意**：原版 c5 直接略過 `#include` 與 `#define`，它不具備任何巨集前置處理能力，遇到以 `#` 開頭的行會予以忽略。

## 2. 函式與區域變數 (Functions and Locals)
C5 的函式必須符合早期 C89 的規範，亦即所有的**區域變數宣告必須集中在 `{` 開頭的第一層**，不能與一般 Statement 混雜。

```ebnf
function_definition = type_specifier identifier "(" parameter_list ")"
                      "{" 
                          { local_declaration } 
                          { statement } 
                      "}" ;

parameter_list = [ type_specifier identifier { "," type_specifier identifier } ] ;

local_declaration = type_specifier identifier [ array_decl | "=" expression ] 
                    { "," identifier [ array_decl | "=" expression ] } ";" ;
```

## 3. 型態系統 (Type System)
C5 的型態極度仰賴宣告時的型態關鍵字以及其後的指標星號 `*`。

```ebnf
type_specifier = base_type { "*" } ;

base_type = "int" | "char" | "long" | "short"
          | "float" | "double"
          | "void"
          | "struct" identifier ;
```

## 4. 流程控制敘述 (Statements)
支援 C 語言中常見的控制流：`if`, `while`, `for`, `return`, `break`, `continue` 還有巢狀區塊 `{ ... }`。

```ebnf
statement = "if" "(" expression ")" statement [ "else" statement ]
          | "while" "(" expression ")" statement
          | "for" "(" [ expression ] ";" [ expression ] ";" [ expression ] ")" statement
          | "return" [ expression ] ";"
          | "break" ";"
          | "continue" ";"
          | "{" { statement } "}"
          | expression ";"
          | ";" ;
```

## 5. 運算式 (Expressions & Operators)
C5 具備非常完整的算符優先級 (Operator Precedence) 處理，從最低的賦值 (`=`) 到最高的陣列 (`[]`) / 結構呼叫 (`->`, `.`)。

```ebnf
expression = assignment_expr ;

assignment_expr = conditional_expr [ "=" assignment_expr ] ;

conditional_expr = logical_or_expr [ "?" expression ":" conditional_expr ] ;

logical_or_expr  = logical_and_expr { "||" logical_and_expr } ;
logical_and_expr = bitwise_or_expr { "&&" bitwise_or_expr } ;
bitwise_or_expr  = bitwise_xor_expr { "|" bitwise_xor_expr } ;
bitwise_xor_expr = bitwise_and_expr { "^" bitwise_and_expr } ;
bitwise_and_expr = equality_expr { "&" equality_expr } ;

equality_expr    = relational_expr { ("==" | "!=") relational_expr } ;
relational_expr  = shift_expr { ("<" | ">" | "<=" | ">=") shift_expr } ;
shift_expr       = additive_expr { ("<<" | ">>") additive_expr } ;
additive_expr    = multiplicative_expr { ("+" | "-") multiplicative_expr } ;
multiplicative_expr = prefix_expr { ("*" | "/" | "%") prefix_expr } ;

prefix_expr = postfix_expr
            | ( "++" | "--" ) prefix_expr
            | ( "+" | "-" | "!" | "~" | "*" | "&" ) prefix_expr
            | "sizeof" "(" type_specifier ")"
            | "sizeof" prefix_expr
            | "(" type_specifier ")" prefix_expr ;   (* 強制轉型 Type Casting *)

postfix_expr = primary_expr { postfix_op } ;

postfix_op = "[" expression "]"
           | "(" [ expression { "," expression } ] ")"
           | "." identifier
           | "->" identifier
           | "++"
           | "--" ;

```

## 6. 基本元素 (Primary Elements)

```ebnf
primary_expr = identifier
             | integer_literal
             | floating_point_literal
             | string_literal
             | character_literal
             | "(" expression ")" ;

integer_literal = [ "0x" | "0X" | "0b" | "0B" ] { digit_or_hex } ;
floating_point_literal = digit_sequence "." digit_sequence [ exponent_part ] ;
string_literal = '"' { any_char_except_quotes | escape_sequence } '"' ;
character_literal = "'" ( any_char | escape_sequence ) "'" ;
```

---

### 重要語法限制與特性解析
1. **沒有 `#include` 展開機制**：在多檔專案中，開發者必須使用 `extern type func_name();` 在頂部宣告要跨檔呼叫的外部程式！
2. **區域變數位置**：所有區域變數必須嚴格排在函式的 `{` 正下方第一行開始宣告。一但出現了 `if` 或是其他一般 `statement` 操作或賦值，函式體內就再也不能宣告新的變數。
3. **無巨集展開**：無法使用 `#define MAX 100`，如需常數請使用 `enum { MAX = 100 };` 作為替代。
4. **指標與陣列**：支援多級指標 `int **p` 以及靜態一維陣列 `int arr[10]`，這使得 C5 在本質上能相容多數計算演算法的要求。
