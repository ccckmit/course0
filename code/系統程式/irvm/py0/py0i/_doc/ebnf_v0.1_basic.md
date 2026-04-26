

```
(* 頂層結構 *)
program        = { statement } ;

(* 語句結構 *)
statement      = print_stmt
               | assign_stmt
               | if_stmt ;

(* Print 語句: print(VAR) *)
print_stmt     = "print" "(" IDENTIFIER ")" ;

(* 賦值語句: VAR = VAL | VAR = VAR + VAR *)
assign_stmt    = IDENTIFIER "=" expression ;

expression     = term [ "+" term ] ;

term           = IDENTIFIER | NUMBER ;

(* 條件判斷: if VAR > NUMBER : ... *)
(* 注意：目前實作僅能識別此結構的開頭，細節需擴充 *)
if_stmt        = "if" IDENTIFIER ">" NUMBER ":" ;

(* 終端符號定義 *)
IDENTIFIER     = [a-zA-Z_][a-zA-Z0-9_]* ;
NUMBER         = [0-9]+ ;
```
