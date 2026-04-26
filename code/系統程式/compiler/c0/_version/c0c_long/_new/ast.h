#ifndef C0C_AST_H
#define C0C_AST_H

typedef enum {
    TY_INT, TY_CHAR, TY_VOID, TY_FLOAT, TY_DOUBLE, 
    TY_UCHAR, TY_SHORT, TY_USHORT, TY_UINT, TY_LONG, TY_ULONG,
    TY_INT_PTR, TY_CHAR_PTR, TY_VOID_PTR, TY_FLOAT_PTR, TY_DOUBLE_PTR,
    TY_UCHAR_PTR, TY_SHORT_PTR, TY_USHORT_PTR, TY_UINT_PTR, TY_LONG_PTR, TY_ULONG_PTR,
    TY_STRUCT, TY_STRUCT_PTR
} CType;

typedef enum {
    AST_NUM, AST_FLOAT, AST_STR, AST_VAR, AST_CALL, AST_ADDR, AST_DEREF,
    AST_INDEX, AST_MEMBER, AST_BINOP, AST_UNARY, AST_CAST, AST_SIZEOF,
    AST_INCDEC, AST_DECL, AST_ASSIGN, AST_EXPR_STMT, AST_BLOCK, AST_IF,
    AST_WHILE, AST_FOR, AST_DO, AST_SWITCH, AST_CASE, AST_BREAK,
    AST_CONTINUE, AST_RETURN, AST_FUNC, AST_GLOBAL
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *next;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;
    struct ASTNode *then_body;
    struct ASTNode *else_body;
    struct ASTNode *body;
    struct ASTNode *init;
    struct ASTNode *update;
    
    int op;
    int val; // 也在函數中用來標記 is_vararg
    double fval;
    char name[64];
    char str_val[256];
    
    CType ty;
    int array_len;
    int struct_id;
    int init_kind;
    int is_default;
    int is_prefix;
    int is_decl; // 也用來標記 extern
} ASTNode;

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

ASTNode* new_node(ASTNodeType type);

#endif