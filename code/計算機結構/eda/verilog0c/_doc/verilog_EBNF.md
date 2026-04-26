# Verilog Subset EBNF

This EBNF describes the subset of Verilog used in MCU0 and our examples, covering structural declarations, combinational / sequential logic, and testbench features.

```ebnf
program = { directive | module_decl }

directive = "`define" ID expression

module_decl = "module" ID [ "(" port_list ")" ] ";" { module_item } "endmodule"

port_list = ID { "," ID }

module_item = port_decl
            | var_decl
            | parameter_decl
            | assign_decl
            | always_block
            | initial_block
            | module_instantiation

port_decl = ("input" | "output") [ "signed" ] [ range ] port_list ";"

var_decl = ("wire" | "reg" | "integer") [ "signed" ] [ range ] ID [ array_range ] ";"

range = "[" expression ":" expression "]"
array_range = "[" expression ":" expression "]"

parameter_decl = "parameter" [ range ] param_assignment { "," param_assignment } ";"
param_assignment = ID "=" expression

module_instantiation = ID ID "(" arg_list ")" ";"

assign_decl = "assign" lvalue "=" expression ";"

always_block = "always" [ "@" "(" event_expression ")" | delay_control ] statement

initial_block = "initial" [ delay_control ] statement

event_expression = "posedge" ID | "negedge" ID | ID { "or" event_expression }

delay_control = "#" number

statement = block_statement
          | assign_statement
          | if_statement
          | case_statement
          | for_statement
          | system_task
          | delay_statement
          | empty_statement

empty_statement = ";"

block_statement = "begin" { statement } "end"

assign_statement = lvalue "=" expression ";"

if_statement = "if" "(" expression ")" statement

case_statement = "case" "(" expression ")" { case_item } "endcase"
case_item = expression { "," expression } ":" statement

for_statement = "for" "(" assign_statement expression ";" assign_statement_no_semi ")" statement

delay_statement = delay_control statement

system_task = ("$display" | "$readmemh" | "$finish") [ "(" arg_list ")" ] ";"

arg_list = expression { "," expression }

lvalue = ID [ "[" expression ":" expression "]" ] [ "[" expression "]" ]
       | "{" lvalue { "," lvalue } "}"
       | "`" ID

expression = term { bin_op term } | unary_op expression
term = ID 
     | number 
     | "`" ID 
     | string 
     | "{" expression { "," expression } "}" 
     | "(" expression ")"
     | system_function
     | ID "[" expression "]"
     | ID "[" expression ":" expression "]"

system_function = "$stime"

number = decimal_number | hex_number | binary_number

bin_op = "+" | "-" | "*" | "/" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "&" | "|" | "^" | "&&" | "||" | "<<" | ">>"
unary_op = "~" | "!" | "-"
```
