#ifndef AST_H
#define AST_H

#include <stdio.h>

/*
 * ast.h – Abstract Syntax Tree for c0c
 *
 * All nodes are allocated from a single Arena owned by the Parser.
 * No individual node is freed; the whole tree is released at once.
 */

/* ── Node kinds ──────────────────────────────────────────────────────────── */
typedef enum {
    /* Declarations */
    ND_TRANSLATION_UNIT,  /* root: list of external_decl               */
    ND_FUNC_DEF,          /* return_type name params body               */
    ND_DECL,              /* type declarator [init]                     */
    ND_PARAM,             /* type declarator (in param list)            */

    /* Types */
    ND_TYPE_BASE,         /* "int", "char", "void", …                  */
    ND_TYPE_PTR,          /* pointer to inner type                      */
    ND_TYPE_ARRAY,        /* array of inner type [size]                 */
    ND_TYPE_FUNC,         /* function type (ret, params)                */
    ND_TYPE_STRUCT,       /* struct { members }  or  struct Tag         */
    ND_TYPE_UNION,        /* union  { members }  or  union  Tag         */
    ND_TYPE_ENUM,         /* enum   { enumerators }                     */
    ND_STRUCT_MEMBER,     /* one member decl inside struct/union        */
    ND_ENUMERATOR,        /* NAME [= value]                             */

    /* Statements */
    ND_COMPOUND,          /* { item* }                                  */
    ND_IF,                /* cond then [else]                           */
    ND_WHILE,             /* cond body                                  */
    ND_DO_WHILE,          /* body cond                                  */
    ND_FOR,               /* init cond step body                        */
    ND_RETURN,            /* [expr]                                     */
    ND_BREAK,
    ND_CONTINUE,
    ND_GOTO,              /* label name                                 */
    ND_LABEL,             /* name: stmt                                 */
    ND_SWITCH,            /* expr body                                  */
    ND_CASE,              /* expr: stmt                                 */
    ND_DEFAULT,           /* default: stmt                              */
    ND_EXPR_STMT,         /* expr ;                                     */
    ND_EMPTY_STMT,        /* ;                                          */

    /* Expressions */
    ND_ASSIGN,            /* lhs op rhs  (op stored in Node.op)        */
    ND_TERNARY,           /* cond ? then : else                         */
    ND_BINARY,            /* lhs op rhs                                 */
    ND_UNARY_PRE,         /* op expr  (prefix)                          */
    ND_UNARY_POST,        /* expr op  (postfix)                         */
    ND_CAST,              /* (type) expr                                */
    ND_SIZEOF_EXPR,       /* sizeof expr                                */
    ND_SIZEOF_TYPE,       /* sizeof(type)                               */
    ND_CALL,              /* callee ( args )                            */
    ND_INDEX,             /* arr [ idx ]                                */
    ND_MEMBER,            /* expr . name                                */
    ND_ARROW,             /* expr -> name                               */
    ND_IDENT,             /* identifier                                 */
    ND_INT_LIT,           /* integer literal                            */
    ND_FLOAT_LIT,         /* float literal                              */
    ND_CHAR_LIT,          /* char literal                               */
    ND_STR_LIT,           /* string literal                             */
    ND_INIT_LIST,         /* { expr, expr, ... }  initializer           */
    ND_COMMA_EXPR,        /* expr , expr                                */

    ND_COUNT
} NodeKind;

/* ── Forward declaration ─────────────────────────────────────────────────── */
typedef struct Node Node;

/* ── Node list (singly-linked) ───────────────────────────────────────────── */
typedef struct NodeList {
    Node           *node;
    struct NodeList *next;
} NodeList;

/* ── Main AST node ───────────────────────────────────────────────────────── */
struct Node {
    NodeKind  kind;
    int       line;   /* source line for error messages */
    int       col;

    /* Generic children – meaning depends on kind (see below) */
    Node     *left;   /* primary child / type / condition / callee     */
    Node     *right;  /* secondary child / body / init / rhs           */
    Node     *extra;  /* tertiary child (else-branch, step, etc.)      */

    /* Child list (params, args, members, stmts, …) */
    NodeList *children;

    /* Leaf data */
    const char *sval;  /* identifier name / operator text / literal    */
    long long   ival;  /* integer literal value                         */
    double      fval;  /* float literal value                           */

    /* Type modifiers */
    int is_const;
    int is_volatile;
    int is_static;
    int is_extern;
    int is_typedef;
    int is_unsigned;
    int is_signed;
    int is_ellipsis; /* for ND_PARAM representing "..." */
};

/* ── Arena (shared with parser) ─────────────────────────────────────────── */
/* Declared here so ast.c can use the same type. */
typedef struct AstArenaBlock {
    char                  *data;
    int                    used;
    int                    cap;
    struct AstArenaBlock  *next;
} AstArenaBlock;

typedef struct {
    AstArenaBlock *head;
} AstArena;

/* Allocate from arena (used by parser, exposed so ast.c can share it) */
void  *ast_arena_alloc(AstArena *a, int n);
char  *ast_arena_strdup(AstArena *a, const char *s);

/* ── Node constructors ───────────────────────────────────────────────────── */
Node     *ast_node(AstArena *a, NodeKind kind, int line, int col);
NodeList *ast_list_append(AstArena *a, NodeList *list, Node *node);

/* ── Pretty-print to C source ────────────────────────────────────────────── */
void ast_to_c(Node *root, FILE *out);

/* ── Debug dump ──────────────────────────────────────────────────────────── */
void ast_dump(Node *root, int depth, FILE *out);

/* ── Arena free ──────────────────────────────────────────────────────────── */
void ast_arena_free(AstArena *a);

#endif /* AST_H */
