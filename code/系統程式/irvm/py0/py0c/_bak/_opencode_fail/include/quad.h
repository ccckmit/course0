#ifndef QUAD_H
#define QUAD_H

#include "../include/ast.h"

typedef enum {
    QUAD_ASSIGN,
    QUAD_ADD,
    QUAD_SUB,
    QUAD_MUL,
    QUAD_DIV,
    QUAD_MOD,
    QUAD_EQ,
    QUAD_NE,
    QUAD_LT,
    QUAD_GT,
    QUAD_LE,
    QUAD_GE,
    QUAD_JUMP,
    QUAD_JUMP_IF,
    QUAD_LABEL,
    QUAD_CALL,
    QUAD_PARAM,
    QUAD_RETURN,
    QUAD_FUNC_BEGIN,
    QUAD_FUNC_END
} QuadOp;

typedef struct Quad {
    QuadOp op;
    char *arg1;
    char *arg2;
    char *result;
    struct Quad *next;
} Quad;

typedef struct {
    Quad *head;
    Quad *tail;
    int temp_count;
    int label_count;
} Quadruple;

Quadruple *generate_quadruples(Program *prog);
void print_quadruples(Quadruple *quads);
void free_quadruples(Quadruple *quads);

#endif
