#include "../include/ast.h"

Program *create_program(void) {
    Program *prog = (Program *)malloc(sizeof(Program));
    prog->functions = NULL;
    return prog;
}

Function *create_function(char *name) {
    Function *func = (Function *)malloc(sizeof(Function));
    func->name = strdup(name);
    func->params = NULL;
    func->param_count = 0;
    func->body = NULL;
    func->next = NULL;
    return func;
}

Suite *create_suite(void) {
    Suite *suite = (Suite *)malloc(sizeof(Suite));
    suite->statements = NULL;
    return suite;
}

Statement *create_assign_stmt(char *target, Expr *value) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_ASSIGN_STMT;
    stmt->next = NULL;
    stmt->data.assign.target = strdup(target);
    stmt->data.assign.value = value;
    return stmt;
}

Statement *create_return_stmt(Expr *value) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_RETURN_STMT;
    stmt->next = NULL;
    stmt->data.ret.value = value;
    return stmt;
}

Statement *create_pass_stmt(void) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_PASS_STMT;
    stmt->next = NULL;
    return stmt;
}

Statement *create_expr_stmt(Expr *expr) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_EXPR_STMT;
    stmt->next = NULL;
    stmt->data.expr.expr = expr;
    return stmt;
}

Statement *create_if_stmt(Expr *cond, Suite *then_b, Suite *else_b) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_IF_STMT;
    stmt->next = NULL;
    stmt->data.if_stmt.condition = cond;
    stmt->data.if_stmt.then_branch = then_b;
    stmt->data.if_stmt.else_branch = else_b;
    return stmt;
}

Statement *create_while_stmt(Expr *cond, Suite *body) {
    Statement *stmt = (Statement *)malloc(sizeof(Statement));
    stmt->type = NODE_WHILE_STMT;
    stmt->next = NULL;
    stmt->data.while_stmt.condition = cond;
    stmt->data.while_stmt.body = body;
    return stmt;
}

Expr *create_binary_op(char *op, Expr *left, Expr *right) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = NODE_BINARY_OP;
    expr->next = NULL;
    expr->data.binary.op = strdup(op);
    expr->data.binary.left = left;
    expr->data.binary.right = right;
    return expr;
}

Expr *create_unary_op(char *op, Expr *operand) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = NODE_UNARY_OP;
    expr->next = NULL;
    expr->data.unary.op = strdup(op);
    expr->data.unary.operand = operand;
    return expr;
}

Expr *create_number(int value) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = NODE_NUMBER;
    expr->next = NULL;
    expr->data.number.value = value;
    return expr;
}

Expr *create_identifier(char *name) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = NODE_IDENTIFIER;
    expr->next = NULL;
    expr->data.ident.name = strdup(name);
    return expr;
}

Expr *create_call(char *name, Expr **args, int arg_count) {
    Expr *expr = (Expr *)malloc(sizeof(Expr));
    expr->type = NODE_CALL;
    expr->next = NULL;
    expr->data.call.name = strdup(name);
    expr->data.call.args = args;
    expr->data.call.arg_count = arg_count;
    return expr;
}

void add_function(Program *prog, Function *func) {
    if (prog->functions == NULL) {
        prog->functions = func;
    } else {
        Function *p = prog->functions;
        while (p->next) p = p->next;
        p->next = func;
    }
}

void add_statement(Suite *suite, Statement *stmt) {
    if (suite->statements == NULL) {
        suite->statements = stmt;
    } else {
        Statement *p = suite->statements;
        while (p->next) p = p->next;
        p->next = stmt;
    }
}

char **add_param(Function *func, char *param) {
    func->params = (char **)realloc(func->params, sizeof(char *) * (func->param_count + 1));
    func->params[func->param_count++] = strdup(param);
    return func->params;
}

void free_expr(Expr *expr) {
    if (!expr) return;
    switch (expr->type) {
        case NODE_BINARY_OP:
            free(expr->data.binary.op);
            free_expr(expr->data.binary.left);
            free_expr(expr->data.binary.right);
            break;
        case NODE_UNARY_OP:
            free(expr->data.unary.op);
            free_expr(expr->data.unary.operand);
            break;
        case NODE_IDENTIFIER:
            free(expr->data.ident.name);
            break;
        case NODE_CALL:
            free(expr->data.call.name);
            for (int i = 0; i < expr->data.call.arg_count; i++) {
                free_expr(expr->data.call.args[i]);
            }
            free(expr->data.call.args);
            break;
        default:
            break;
    }
    free(expr);
}

void free_statement(Statement *stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case NODE_ASSIGN_STMT:
            free(stmt->data.assign.target);
            free_expr(stmt->data.assign.value);
            break;
        case NODE_RETURN_STMT:
            free_expr(stmt->data.ret.value);
            break;
        case NODE_EXPR_STMT:
            free_expr(stmt->data.expr.expr);
            break;
        case NODE_IF_STMT:
            free_expr(stmt->data.if_stmt.condition);
            free_suite(stmt->data.if_stmt.then_branch);
            if (stmt->data.if_stmt.else_branch)
                free_suite(stmt->data.if_stmt.else_branch);
            break;
        case NODE_WHILE_STMT:
            free_expr(stmt->data.while_stmt.condition);
            free_suite(stmt->data.while_stmt.body);
            break;
        default:
            break;
    }
    free(stmt);
}

void free_suite(Suite *suite) {
    if (!suite) return;
    Statement *p = suite->statements;
    while (p) {
        Statement *next = p->next;
        free_statement(p);
        p = next;
    }
    free(suite);
}

void free_function(Function *func) {
    if (!func) return;
    free(func->name);
    for (int i = 0; i < func->param_count; i++) {
        free(func->params[i]);
    }
    free(func->params);
    if (func->body) free_suite(func->body);
    free(func);
}

void free_program(Program *prog) {
    if (!prog) return;
    Function *p = prog->functions;
    while (p) {
        Function *next = p->next;
        free_function(p);
        p = next;
    }
    free(prog);
}
