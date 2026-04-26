# cpy0i EBNF

這份文件描述目前 `cpy0/py/cpy0i.py` 與 `cpy0/c/` 實際共用的 Python 子集語法。

它不是完整 Python 語法，而是目前專案中可執行、可自我解譯路徑所依賴的最小子集。

## Lexical Notes

- 縮排採 Python 風格的 `INDENT` / `DEDENT`
- 一個 statement 以 `NEWLINE` 結束
- 字串目前以單引號或雙引號字面值為主
- 支援整數與簡單浮點數

## EBNF

```ebnf
module          = { NEWLINE } , { statement , { NEWLINE } } , EOF ;

statement       = function_def
                | if_stmt
                | while_stmt
                | return_stmt
                | pass_stmt
                | assign_stmt
                | expr_stmt
                ;

function_def    = "def" , NAME , "(" , [ param_list ] , ")" , ":" , block ;

param_list      = NAME , { "," , NAME } ;

if_stmt         = "if" , expression , ":" , block , [ "else" , ":" , block ] ;

while_stmt      = "while" , expression , ":" , block ;

return_stmt     = "return" , [ expression ] , NEWLINE ;

pass_stmt       = "pass" , NEWLINE ;

assign_stmt     = NAME , "=" , expression , NEWLINE ;

expr_stmt       = expression , NEWLINE ;

block           = NEWLINE , INDENT , { statement , { NEWLINE } } , DEDENT ;

expression      = comparison ;

comparison      = term , { compare_op , term } ;

compare_op      = "==" | "!=" | "<" | "<=" | ">" | ">=" ;

term            = factor , { add_op , factor } ;

add_op          = "+" | "-" ;

factor          = unary , { mul_op , unary } ;

mul_op          = "*" | "/" | "%" ;

unary           = [ unary_op ] , postfix ;

unary_op        = "-" ;

postfix         = primary , { call_suffix | attr_suffix | subscript_suffix } ;

call_suffix     = "(" , [ arg_list ] , ")" ;

arg_list        = expression , { "," , expression } ;

attr_suffix     = "." , NAME ;

subscript_suffix = "[" , expression , "]" ;

primary         = NAME
                | literal
                | "(" , expression , ")"
                ;

literal         = INT
                | FLOAT
                | STRING
                ;
```

## Supported Semantic Subset

目前實作語意大致如下：

- statement
  - expression statement
  - assignment
  - `if ... else`
  - `while`
  - `def`
  - `return`
  - `pass`

- expression
  - 名稱查找
  - 整數、浮點數、字串常值
  - `+ - * / %`
  - `== != < <= > >=`
  - 一元負號 `-x`
  - 函式呼叫
  - 屬性存取 `obj.attr`
  - 索引存取 `obj[idx]`

## Current Runtime Assumptions

這個子集目前特別依賴下列執行期能力：

- `__import__("sys")`
- `sys.argv`
- `run_path(path)`
- `print`
- `len`
- `int`
- `float`
- `str`
- `bool`

## Not Yet Supported

下列語法目前不應視為 `cpy0i` 語言的一部分：

- `import x` statement
- `from x import y`
- `for`
- `class`
- `try/except/finally`
- `with`
- list / dict / tuple literals
- slicing
- keyword arguments
- `and` / `or` / `not` boolean operators
- assignment targets other than simple `NAME`
- comprehensions
- f-string
- decorators
- `*args` / `**kwargs`

## Minimal Example

```python
sys = __import__("sys")

def main():
    if len(sys.argv) < 2:
        print("Usage")
        return
    run_path(sys.argv[1])

if __name__ == "__main__":
    main()
```

這段程式正是目前 `cpy0i.py` 採用的風格：避免複雜語法，只使用這份 EBNF 中已定義的子集。
