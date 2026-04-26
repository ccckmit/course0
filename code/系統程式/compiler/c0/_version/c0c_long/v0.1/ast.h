#ifndef C0C_AST_H
#define C0C_AST_H

typedef enum {
    AST_FUNC, AST_GLOBAL, AST_DECL, AST_ASSIGN, AST_BINOP, AST_UNARY, AST_CAST, AST_VAR, AST_NUM, AST_FLOAT, AST_RETURN,
    AST_STR, AST_CALL, AST_EXPR_STMT, AST_IF, AST_WHILE, AST_FOR, AST_BREAK, AST_CONTINUE, AST_INCDEC,
    AST_ADDR, AST_DEREF, AST_INDEX, AST_SIZEOF, AST_BLOCK, AST_SWITCH, AST_CASE, AST_DO, AST_MEMBER
} ASTNodeType;

typedef enum {
    TY_INT,
    TY_UINT,
    TY_SHORT,
    TY_USHORT,
    TY_LONG,
    TY_ULONG,
    TY_CHAR,
    TY_UCHAR,
    TY_FLOAT,
    TY_DOUBLE,
    TY_INT_PTR,
    TY_UINT_PTR,
    TY_SHORT_PTR,
    TY_USHORT_PTR,
    TY_LONG_PTR,
    TY_ULONG_PTR,
    TY_CHAR_PTR,
    TY_UCHAR_PTR,
    TY_FLOAT_PTR,
    TY_DOUBLE_PTR,
    TY_VOID,
    TY_STRUCT,
    TY_STRUCT_PTR
} CType;

typedef struct ASTNode {
    ASTNodeType type;
    CType ty;
    int val;
    double fval;
    char name[64];
    char str_val[256];
    int op;
    int array_len;
    int init_kind;
    int is_default;
    int is_decl;
    int struct_id;
    struct ASTNode *left, *right;
    struct ASTNode *cond, *then_body, *else_body;
    struct ASTNode *init, *update, *body;
    int is_prefix;
    struct ASTNode *next;
} ASTNode;

ASTNode* new_node(ASTNodeType type);

typedef struct {
    char name[64];
    CType ty;
    int offset;
    int struct_id;
} StructField;

typedef struct {
    char name[64];
    int size;
    StructField fields[64];
    int field_cnt;
} StructDef;

extern StructDef g_struct_defs[64];
extern int g_struct_def_cnt;

#endif
