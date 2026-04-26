#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_SUITE,
    NODE_ASSIGN_STMT,
    NODE_RETURN_STMT,
    NODE_PASS_STMT,
    NODE_EXPR_STMT,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_NUMBER,
    NODE_IDENTIFIER,
    NODE_CALL
} NodeType;

typedef struct Program Program;
typedef struct Function Function;
typedef struct Suite Suite;
typedef struct Statement Statement;
typedef struct Expr Expr;

struct Program {
    Function *functions;
};

struct Function {
    char *name;
    char **params;
    int param_count;
    Suite *body;
    Function *next;
};

struct Suite {
    Statement *statements;
};

struct Statement {
    NodeType type;
    Statement *next;
    union {
        struct { char *target; Expr *value; } assign;
        struct { Expr *value; } ret;
        struct { Expr *expr; } expr;
        struct { Expr *condition; Suite *then_branch; Suite *else_branch; } if_stmt;
        struct { Expr *condition; Suite *body; } while_stmt;
    } data;
};

struct Expr {
    NodeType type;
    Expr *next;
    union {
        struct { char *op; Expr *left; Expr *right; } binary;
        struct { char *op; Expr *operand; } unary;
        struct { int value; } number;
        struct { char *name; } ident;
        struct { char *name; Expr **args; int arg_count; } call;
    } data;
};

Program *create_program(void);
Function *create_function(char *name);
Suite *create_suite(void);
Statement *create_assign_stmt(char *target, Expr *value);
Statement *create_return_stmt(Expr *value);
Statement *create_pass_stmt(void);
Statement *create_expr_stmt(Expr *expr);
Statement *create_if_stmt(Expr *cond, Suite *then_b, Suite *else_b);
Statement *create_while_stmt(Expr *cond, Suite *body);
Expr *create_binary_op(char *op, Expr *left, Expr *right);
Expr *create_unary_op(char *op, Expr *operand);
Expr *create_number(int value);
Expr *create_identifier(char *name);
Expr *create_call(char *name, Expr **args, int arg_count);
void add_function(Program *prog, Function *func);
void add_statement(Suite *suite, Statement *stmt);
char **add_param(Function *func, char *param);

void free_program(Program *prog);
void free_function(Function *func);
void free_suite(Suite *suite);
void free_statement(Statement *stmt);
void free_expr(Expr *expr);

#endif
