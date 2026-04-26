#include "../include/quad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *new_temp(Quadruple *q) {
    char buf[32];
    sprintf(buf, "t%d", q->temp_count++);
    return strdup(buf);
}

static char *new_label(Quadruple *q) {
    char buf[32];
    sprintf(buf, "L%d", q->label_count++);
    return strdup(buf);
}

static void emit(Quadruple *q, QuadOp op, char *a1, char *a2, char *res) {
    Quad *quad = (Quad *)malloc(sizeof(Quad));
    quad->op = op;
    quad->arg1 = a1;
    quad->arg2 = a2;
    quad->result = res;
    quad->next = NULL;
    
    if (q->tail) {
        q->tail->next = quad;
        q->tail = quad;
    } else {
        q->head = q->tail = quad;
    }
}

static char *gen_expr(Quadruple *q, Expr *expr);

static char *gen_binary_op(Quadruple *q, Expr *expr) {
    char *left = gen_expr(q, expr->data.binary.left);
    char *right = gen_expr(q, expr->data.binary.right);
    char *res = new_temp(q);
    
    QuadOp op;
    if (strcmp(expr->data.binary.op, "+") == 0) op = QUAD_ADD;
    else if (strcmp(expr->data.binary.op, "-") == 0) op = QUAD_SUB;
    else if (strcmp(expr->data.binary.op, "*") == 0) op = QUAD_MUL;
    else if (strcmp(expr->data.binary.op, "/") == 0) op = QUAD_DIV;
    else if (strcmp(expr->data.binary.op, "%") == 0) op = QUAD_MOD;
    else if (strcmp(expr->data.binary.op, "==") == 0) op = QUAD_EQ;
    else if (strcmp(expr->data.binary.op, "!=") == 0) op = QUAD_NE;
    else if (strcmp(expr->data.binary.op, "<") == 0) op = QUAD_LT;
    else if (strcmp(expr->data.binary.op, ">") == 0) op = QUAD_GT;
    else if (strcmp(expr->data.binary.op, "<=") == 0) op = QUAD_LE;
    else if (strcmp(expr->data.binary.op, ">=") == 0) op = QUAD_GE;
    else op = QUAD_ADD;
    
    emit(q, op, left, right, res);
    return res;
}

static char *gen_unary_op(Quadruple *q, Expr *expr) {
    char *operand = gen_expr(q, expr->data.unary.operand);
    char *res = new_temp(q);
    
    if (strcmp(expr->data.unary.op, "-") == 0) {
        emit(q, QUAD_SUB, operand, "0", res);
    } else {
        emit(q, QUAD_ASSIGN, operand, NULL, res);
    }
    return res;
}

static char *gen_expr(Quadruple *q, Expr *expr) {
    if (!expr) return strdup("");
    
    switch (expr->type) {
        case NODE_NUMBER: {
            char buf[32];
            sprintf(buf, "%d", expr->data.number.value);
            return strdup(buf);
        }
        case NODE_IDENTIFIER:
            return strdup(expr->data.ident.name);
        case NODE_BINARY_OP:
            return gen_binary_op(q, expr);
        case NODE_UNARY_OP:
            return gen_unary_op(q, expr);
        case NODE_CALL: {
            for (int i = 0; i < expr->data.call.arg_count; i++) {
                char *arg = gen_expr(q, expr->data.call.args[i]);
                emit(q, QUAD_PARAM, arg, NULL, NULL);
                free(arg);
            }
            char *res = new_temp(q);
            emit(q, QUAD_CALL, expr->data.call.name, NULL, res);
            return res;
        }
        default:
            return strdup("");
    }
}

static void gen_stmt(Quadruple *q, Statement *stmt, char **end_label);

static void gen_suite(Quadruple *q, Suite *suite, char *break_label) {
    Statement *s = suite->statements;
    while (s) {
        gen_stmt(q, s, break_label);
        s = s->next;
    }
}

static void gen_stmt(Quadruple *q, Statement *stmt, char **end_label) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case NODE_ASSIGN_STMT: {
            char *value = gen_expr(q, stmt->data.assign.value);
            emit(q, QUAD_ASSIGN, value, NULL, strdup(stmt->data.assign.target));
            break;
        }
        case NODE_RETURN_STMT: {
            char *value = gen_expr(q, stmt->data.ret.value);
            emit(q, QUAD_RETURN, value, NULL, NULL);
            free(value);
            break;
        }
        case NODE_PASS_STMT:
            break;
        case NODE_EXPR_STMT: {
            char *value = gen_expr(q, stmt->data.expr.expr);
            free(value);
            break;
        }
        case NODE_IF_STMT: {
            char *cond = gen_expr(q, stmt->data.if_stmt.condition);
            char *else_label = new_label(q);
            emit(q, QUAD_JUMP_IF, cond, NULL, else_label);
            free(cond);
            
            if (stmt->data.if_stmt.then_branch) {
                gen_suite(q, stmt->data.if_stmt.then_branch, *end_label);
            }
            
            if (stmt->data.if_stmt.else_branch) {
                char *end_if = new_label(q);
                emit(q, QUAD_JUMP, NULL, NULL, end_if);
                emit(q, QUAD_LABEL, else_label, NULL, NULL);
                gen_suite(q, stmt->data.if_stmt.else_branch, *end_label);
                emit(q, QUAD_LABEL, end_if, NULL, NULL);
                free(end_if);
            } else {
                emit(q, QUAD_LABEL, else_label, NULL, NULL);
            }
            free(else_label);
            break;
        }
        case NODE_WHILE_STMT: {
            char *start_label = new_label(q);
            char *loop_end = new_label(q);
            emit(q, QUAD_LABEL, start_label, NULL, NULL);
            
            char *cond = gen_expr(q, stmt->data.while_stmt.condition);
            emit(q, QUAD_JUMP_IF, cond, NULL, loop_end);
            free(cond);
            
            if (stmt->data.while_stmt.body) {
                gen_suite(q, stmt->data.while_stmt.body, &loop_end);
            }
            
            emit(q, QUAD_JUMP, NULL, NULL, start_label);
            emit(q, QUAD_LABEL, loop_end, NULL, NULL);
            free(start_label);
            free(loop_end);
            break;
        }
        default:
            break;
    }
}

static void gen_function(Quadruple *q, Function *func) {
    emit(q, QUAD_FUNC_BEGIN, func->name, NULL, NULL);
    
    if (func->body) {
        gen_suite(q, func->body, NULL);
    }
    
    emit(q, QUAD_FUNC_END, func->name, NULL, NULL);
}

Quadruple *generate_quadruples(Program *prog) {
    Quadruple *q = (Quadruple *)malloc(sizeof(Quadruple));
    q->head = q->tail = NULL;
    q->temp_count = 0;
    q->label_count = 0;
    
    Function *f = prog->functions;
    while (f) {
        gen_function(q, f);
        f = f->next;
    }
    
    return q;
}

static const char *op_to_str(QuadOp op) {
    switch (op) {
        case QUAD_ASSIGN: return "=";
        case QUAD_ADD: return "+";
        case QUAD_SUB: return "-";
        case QUAD_MUL: return "*";
        case QUAD_DIV: return "/";
        case QUAD_MOD: return "%";
        case QUAD_EQ: return "==";
        case QUAD_NE: return "!=";
        case QUAD_LT: return "<";
        case QUAD_GT: return ">";
        case QUAD_LE: return "<=";
        case QUAD_GE: return ">=";
        case QUAD_JUMP: return "JUMP";
        case QUAD_JUMP_IF: return "JUMP_IF";
        case QUAD_LABEL: return "LABEL";
        case QUAD_CALL: return "CALL";
        case QUAD_PARAM: return "PARAM";
        case QUAD_RETURN: return "RETURN";
        case QUAD_FUNC_BEGIN: return "FUNC_BEGIN";
        case QUAD_FUNC_END: return "FUNC_END";
        default: return "?";
    }
}

void print_quadruples(Quadruple *quads) {
    Quad *p = quads->head;
    while (p) {
        switch (p->op) {
            case QUAD_LABEL:
                printf("%s:\n", p->arg1);
                break;
            case QUAD_JUMP:
                printf("  JUMP %s\n", p->result);
                break;
            case QUAD_JUMP_IF:
                printf("  JUMP_IF %s %s\n", p->arg1, p->result);
                break;
            case QUAD_FUNC_BEGIN:
                printf("FUNCTION %s:\n", p->arg1);
                break;
            case QUAD_FUNC_END:
                printf("END_FUNCTION %s\n", p->arg1);
                break;
            case QUAD_RETURN:
                printf("  RETURN %s\n", p->arg1);
                break;
            case QUAD_PARAM:
                printf("  PARAM %s\n", p->arg1);
                break;
            case QUAD_CALL:
                printf("  %s = CALL %s\n", p->result, p->arg1);
                break;
            case QUAD_ASSIGN:
                printf("  %s = %s\n", p->result, p->arg1 ? p->arg1 : "");
                break;
            default:
                if (p->result) {
                    printf("  %s = %s %s %s\n", p->result, p->arg1 ? p->arg1 : "", 
                           op_to_str(p->op), p->arg2 ? p->arg2 : "");
                }
                break;
        }
        p = p->next;
    }
}

void free_quadruples(Quadruple *quads) {
    Quad *p = quads->head;
    while (p) {
        Quad *next = p->next;
        if (p->arg1) free(p->arg1);
        if (p->arg2) free(p->arg2);
        if (p->result) free(p->result);
        free(p);
        p = next;
    }
    free(quads);
}
