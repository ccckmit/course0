#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---------------------------------------------------------------- Node */

Node *node_new(NodeKind kind, int line) {
    Node *n = calloc(1, sizeof *n);
    if (!n) { perror("calloc"); exit(1); }
    n->kind = kind;
    n->line = line;
    return n;
}

void node_add_child(Node *parent, Node *child) {
    parent->children = realloc(parent->children,
                                (parent->n_children + 1) * sizeof(Node *));
    if (!parent->children) { perror("realloc"); exit(1); }
    parent->children[parent->n_children++] = child;
}

void node_free(Node *n) {
    if (!n) return;
    for (int i = 0; i < n->n_children; i++)
        node_free(n->children[i]);
    free(n->children);
    free(n->sval);
    free(n->func_name);
    free(n->var_name);
    free(n->typedef_name);
    /* NOTE: types owned by parser symbol table, not freed here */
    free(n);
}

/* ---------------------------------------------------------------- Type */

Type *type_new(TypeKind k) {
    Type *t = calloc(1, sizeof *t);
    if (!t) { perror("calloc"); exit(1); }
    t->kind       = k;
    t->array_size = -1;
    return t;
}

Type *type_ptr(Type *base) {
    Type *t  = type_new(TY_PTR);
    t->base  = base;
    return t;
}

Type *type_array(Type *base, long size) {
    Type *t       = type_new(TY_ARRAY);
    t->base       = base;
    t->array_size = size;
    return t;
}

void type_free(Type *t) {
    if (!t) return;
    /* don't free base recursively – shared references possible */
    free(t->tag);
    free(t->name);
    if (t->params) {
        for (int i = 0; i < t->param_count; i++)
            free(t->params[i].name);
        free(t->params);
    }
    free(t);
}

int type_is_integer(const Type *t) {
    switch (t->kind) {
    case TY_BOOL: case TY_CHAR: case TY_SCHAR: case TY_UCHAR:
    case TY_SHORT: case TY_USHORT: case TY_INT: case TY_UINT:
    case TY_LONG: case TY_ULONG: case TY_LONGLONG: case TY_ULONGLONG:
    case TY_ENUM:
        return 1;
    default:
        return 0;
    }
}

int type_is_float(const Type *t) {
    return t->kind == TY_FLOAT || t->kind == TY_DOUBLE;
}

int type_is_pointer(const Type *t) {
    return t->kind == TY_PTR || t->kind == TY_ARRAY;
}

int type_size(const Type *t) {
    switch (t->kind) {
    case TY_VOID:    return 0;
    case TY_BOOL:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:   return 1;
    case TY_SHORT:
    case TY_USHORT:  return 2;
    case TY_INT:
    case TY_UINT:
    case TY_FLOAT:
    case TY_ENUM:    return 4;
    case TY_LONG:
    case TY_ULONG:
    case TY_LONGLONG:
    case TY_ULONGLONG:
    case TY_DOUBLE:
    case TY_PTR:     return 8;
    case TY_ARRAY:
        if (t->array_size < 0) return 0;
        return (int)(t->array_size * type_size(t->base));
    default:         return 0;
    }
}
