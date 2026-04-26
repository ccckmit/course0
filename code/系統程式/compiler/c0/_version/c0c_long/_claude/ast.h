#ifndef AST_H
#define AST_H

#include <stddef.h>

/* ---------------------------------------------------------------- types */

typedef enum {
    TY_VOID, TY_BOOL,
    TY_CHAR, TY_SCHAR, TY_UCHAR,
    TY_SHORT, TY_USHORT,
    TY_INT, TY_UINT,
    TY_LONG, TY_ULONG,
    TY_LONGLONG, TY_ULONGLONG,
    TY_FLOAT, TY_DOUBLE,
    TY_PTR, TY_ARRAY, TY_FUNC,
    TY_STRUCT, TY_UNION, TY_ENUM,
    TY_TYPEDEF_REF
} TypeKind;

typedef struct Type Type;
typedef struct Param Param;

struct Type {
    TypeKind  kind;
    int       is_const;
    int       is_volatile;
    /* PTR / ARRAY */
    Type     *base;
    long      array_size;   /* -1 = unspecified */
    /* FUNC */
    Type     *ret;
    Param    *params;
    int       param_count;
    int       variadic;
    /* STRUCT/UNION/ENUM */
    char     *tag;
    /* TYPEDEF_REF */
    char     *name;
};

struct Param {
    char *name;  /* may be NULL */
    Type *type;
};

/* ---------------------------------------------------------------- AST nodes */

typedef enum {
    /* Declarations */
    ND_TRANSLATION_UNIT,
    ND_FUNC_DEF,
    ND_VAR_DECL,
    ND_TYPEDEF,
    ND_STRUCT_DEF,
    /* Statements */
    ND_BLOCK,
    ND_IF,
    ND_WHILE,
    ND_DO_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_BREAK,
    ND_CONTINUE,
    ND_EXPR_STMT,
    /* Expressions */
    ND_INT_LIT,
    ND_FLOAT_LIT,
    ND_CHAR_LIT,
    ND_STRING_LIT,
    ND_IDENT,
    ND_CALL,
    ND_BINOP,
    ND_UNOP,
    ND_ASSIGN,
    ND_COMPOUND_ASSIGN,
    ND_CAST,
    ND_TERNARY,
    ND_SIZEOF_TYPE,
    ND_SIZEOF_EXPR,
    ND_INDEX,        /* a[i]  */
    ND_MEMBER,       /* a.b   */
    ND_ARROW,        /* a->b  */
    ND_ADDR,         /* &x    */
    ND_DEREF,        /* *x    */
    ND_PRE_INC, ND_PRE_DEC,
    ND_POST_INC, ND_POST_DEC,
    ND_INIT_LIST,
    ND_COMMA,        /* a, b  (expression) */
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind  kind;
    int       line;
    Type     *type;   /* resolved type (filled by codegen) */

    /* children – union-style, all flat */
    Node    **children;
    int       n_children;

    /* leaf data */
    long long ival;
    double    fval;
    char     *sval;    /* string / ident name / member name / op string */

    /* for ND_FUNC_DEF */
    char     *func_name;
    Type     *func_type;
    char    **param_names;

    /* for ND_VAR_DECL */
    char     *var_name;
    Type     *var_type;
    int       is_global;
    int       is_static;
    int       is_extern;

    /* for ND_BINOP / ND_UNOP / ND_COMPOUND_ASSIGN */
    int       op;   /* TokenType value */

    /* for ND_TYPEDEF */
    char     *typedef_name;
    Type     *typedef_type;

    /* for ND_IF */
    Node     *cond;
    Node     *then_branch;
    Node     *else_branch;

    /* for ND_WHILE / ND_DO_WHILE */
    Node     *loop_cond;
    Node     *loop_body;

    /* for ND_FOR */
    Node     *for_init;
    Node     *for_cond;
    Node     *for_post;
    Node     *for_body;

    /* for ND_RETURN */
    Node     *ret_val;

    /* for ND_CAST */
    Type     *cast_type;
    Node     *cast_expr;
};

Node *node_new(NodeKind kind, int line);
void  node_free(Node *n);
void  node_add_child(Node *parent, Node *child);

Type *type_new(TypeKind k);
Type *type_ptr(Type *base);
Type *type_array(Type *base, long size);
void  type_free(Type *t);
int   type_is_integer(const Type *t);
int   type_is_float(const Type *t);
int   type_is_pointer(const Type *t);
int   type_size(const Type *t);    /* size in bytes */

#endif /* AST_H */