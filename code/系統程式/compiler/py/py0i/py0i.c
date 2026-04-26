#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================= *
 * 前向宣告 (FORWARD DECLARATIONS)
 * ========================================================================= */

typedef struct AstNode AstNode;
typedef struct PyValue PyValue;
typedef struct PyEnv PyEnv;
typedef struct PyRuntime PyRuntime;

typedef PyValue *(*PyCFunction)(PyRuntime *rt, int argc, PyValue **argv);

/* ========================================================================= *
 * 資料結構 (DATA STRUCTURES)
 * ========================================================================= */

// --- util.h ---
typedef struct {
    void **items;
    int count;
    int capacity;
} PtrVec;

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} StrBuf;

// --- lexer.h ---
typedef enum {
    TOK_EOF, TOK_NEWLINE, TOK_INDENT, TOK_DEDENT, TOK_NAME, TOK_INT,
    TOK_FLOAT, TOK_STRING, TOK_DEF, TOK_IF, TOK_ELSE, TOK_WHILE,
    TOK_RETURN, TOK_PASS, TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET,
    TOK_RBRACKET, TOK_COMMA, TOK_COLON, TOK_DOT, TOK_PLUS, TOK_MINUS,
    TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_EQUAL, TOK_EQEQ, TOK_NE,
    TOK_LT, TOK_LE, TOK_GT, TOK_GE
} TokenKind;

typedef struct {
    TokenKind kind;
    char *lexeme;
    int line;
    int col;
} Token;

typedef struct {
    Token *items;
    int count;
    int capacity;
} TokenVec;

// --- ast_nodes.h ---
typedef enum {
    AST_MODULE, AST_EXPR_STMT, AST_ASSIGN, AST_IF, AST_WHILE,
    AST_FUNCTION_DEF, AST_RETURN, AST_PASS, AST_NAME, AST_CONSTANT,
    AST_BINOP, AST_UNARYOP, AST_COMPARE, AST_CALL, AST_ATTRIBUTE,
    AST_SUBSCRIPT
} AstKind;

typedef enum {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_EQ, OP_NE, OP_LT,
    OP_LE, OP_GT, OP_GE, OP_NEG
} OpKind;

struct AstNode {
    AstKind kind;
    int line;
    union {
        struct { PtrVec body; } module;
        struct { AstNode *expr; } expr_stmt;
        struct { char *name; AstNode *value; } assign;
        struct { AstNode *test; PtrVec body; PtrVec orelse; } if_stmt;
        struct { AstNode *test; PtrVec body; } while_stmt;
        struct { char *name; char **params; int param_count; PtrVec body; } function_def;
        struct { AstNode *value; } return_stmt;
        struct { char *name; } name;
        struct { int is_float; long int_value; double float_value; char *str_value; int is_string; } constant;
        struct { OpKind op; AstNode *left; AstNode *right; } binop;
        struct { OpKind op; AstNode *operand; } unaryop;
        struct { OpKind op; AstNode *left; AstNode *right; } compare;
        struct { AstNode *func; PtrVec args; } call;
        struct { AstNode *value; char *attr; } attribute;
        struct { AstNode *value; AstNode *index; } subscript;
    } as;
};

// --- value.h ---
typedef enum {
    PY_NONE, PY_BOOL, PY_INT, PY_FLOAT, PY_STR, PY_LIST,
    PY_SYS, PY_FUNCTION, PY_BUILTIN_FUNCTION
} PyType;

typedef struct {
    char *name;
    char **params;
    int param_count;
    AstNode *body;
    PyEnv *closure;
} PyFunction;

typedef struct {
    char *name;
    PyCFunction fn;
} PyBuiltinFunction;

typedef struct {
    PyValue **items;
    int count;
    int capacity;
} PyList;

typedef struct {
    PyValue *argv;
} PySys;

struct PyValue {
    PyType type;
    union {
        int b;
        long i;
        double f;
        char *s;
        PyList *list;
        PySys *sys;
        PyFunction *func;
        PyBuiltinFunction *cfunc;
    } as;
};

extern PyValue PY_NONE_VALUE;
extern PyValue PY_TRUE_VALUE;
extern PyValue PY_FALSE_VALUE;

// --- env.h ---
typedef struct {
    char *name;
    PyValue *value;
} PyBinding;

struct PyEnv {
    PyEnv *parent;
    PyBinding *items;
    int count;
    int capacity;
};

// --- runtime.h ---
struct PyRuntime {
    PyEnv *globals;
    PyValue *sys_value;
};

// --- exception.h ---
typedef struct {
    int has_return;
    PyValue *value;
} ExecResult;

/* ========================================================================= *
 * 函數原型 (FUNCTION PROTOTYPES)
 * ========================================================================= */

// Util
void ptrvec_init(PtrVec *vec);
void ptrvec_push(PtrVec *vec, void *item);
void ptrvec_free(PtrVec *vec);
char *xstrdup(const char *src);
char *xstrndup(const char *src, size_t n);
void strbuf_init(StrBuf *buf);
void strbuf_append_char(StrBuf *buf, char ch);
void strbuf_append_str(StrBuf *buf, const char *s);
char *strbuf_take(StrBuf *buf);
void die(const char *fmt, ...);

// Lexer
void lex_source(const char *source, TokenVec *out_tokens);
void free_tokens(TokenVec *tokens);

// Parser
AstNode *parse_tokens(TokenVec *tokens, const char *filename);

// AST Nodes
AstNode *ast_new_module(void);
AstNode *ast_new_expr_stmt(AstNode *expr, int line);
AstNode *ast_new_assign(char *name, AstNode *value, int line);
AstNode *ast_new_if(AstNode *test, int line);
AstNode *ast_new_while(AstNode *test, int line);
AstNode *ast_new_function_def(char *name, char **params, int param_count, int line);
AstNode *ast_new_return(AstNode *value, int line);
AstNode *ast_new_pass(int line);
AstNode *ast_new_name(char *name, int line);
AstNode *ast_new_int(long value, int line);
AstNode *ast_new_float(double value, int line);
AstNode *ast_new_string(char *value, int line);
AstNode *ast_new_binop(OpKind op, AstNode *left, AstNode *right, int line);
AstNode *ast_new_unaryop(OpKind op, AstNode *operand, int line);
AstNode *ast_new_compare(OpKind op, AstNode *left, AstNode *right, int line);
AstNode *ast_new_call(AstNode *func, int line);
AstNode *ast_new_attribute(AstNode *value, char *attr, int line);
AstNode *ast_new_subscript(AstNode *value, AstNode *index, int line);
void ast_free(AstNode *node);

// AST Wrap
AstNode *parse_source(const char *source, const char *filename);

// Values
PyValue *py_none(void);
PyValue *py_bool(int value);
PyValue *py_new_int(long value);
PyValue *py_new_float(double value);
PyValue *py_new_string(const char *value);
PyValue *py_new_list(void);
void py_list_append(PyValue *list_value, PyValue *item);
PyValue *py_list_get(PyValue *list_value, long index);
PyValue *py_new_sys(PyValue *argv_list);
PyValue *py_new_function(PyFunction *func);
PyValue *py_new_builtin(const char *name, PyCFunction fn);
int py_is_truthy(PyValue *value);
double py_as_number(PyValue *value);
char *py_to_string(PyValue *value);

// Environment
PyEnv *env_new(PyEnv *parent);
void env_set(PyEnv *env, const char *name, PyValue *value);
void env_assign(PyEnv *env, const char *name, PyValue *value);
PyValue *env_get(PyEnv *env, const char *name);

// Operator
PyValue *py_apply_binop(OpKind op, PyValue *left, PyValue *right);
PyValue *py_apply_compare(OpKind op, PyValue *left, PyValue *right);
PyValue *py_apply_unary(OpKind op, PyValue *value);

// Function creation & call
PyValue *py_make_function(const char *name, char **params, int param_count, AstNode *body, PyEnv *closure);
PyValue *py_call(PyRuntime *rt, PyValue *callable, int argc, PyValue **argv);

// Evaluation
PyValue *eval_expr(PyRuntime *rt, PyEnv *env, AstNode *node);
ExecResult exec_stmt(PyRuntime *rt, PyEnv *env, AstNode *node);
ExecResult exec_module(PyRuntime *rt, PyEnv *env, AstNode *module);
ExecResult exec_function_body(PyRuntime *rt, PyEnv *env, AstNode *function_def);

// Builtin
void builtin_install(PyRuntime *rt);
PyValue *builtin_run_path_impl(PyRuntime *rt, const char *path);

// Runtime
PyRuntime *runtime_new(void);
void runtime_set_argv(PyRuntime *rt, int argc, char **argv);
void runtime_init_builtins(PyRuntime *rt);


/* ========================================================================= *
 * 實作區塊 (IMPLEMENTATIONS)
 * ========================================================================= */

// --- 全域變數定義 ---
PyValue PY_NONE_VALUE = { PY_NONE, {0} };
PyValue PY_TRUE_VALUE = { PY_BOOL, {.b = 1} };
PyValue PY_FALSE_VALUE = { PY_BOOL, {.b = 0} };

// --- 輔助函數: 讀取檔案 ---
static char *read_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) die("cannot open %s", path);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) die("out of memory");
    if (fread(buf, 1, (size_t)size, fp) != (size_t)size) die("failed to read %s", path);
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

// --- util.c ---
void ptrvec_init(PtrVec *vec) {
    vec->items = NULL;
    vec->count = 0;
    vec->capacity = 0;
}

void ptrvec_push(PtrVec *vec, void *item) {
    if (vec->count == vec->capacity) {
        int new_cap = vec->capacity ? vec->capacity * 2 : 8;
        void **new_items = (void **)realloc(vec->items, sizeof(void *) * new_cap);
        if (!new_items) die("out of memory");
        vec->items = new_items;
        vec->capacity = new_cap;
    }
    vec->items[vec->count++] = item;
}

void ptrvec_free(PtrVec *vec) {
    free(vec->items);
    vec->items = NULL;
    vec->count = 0;
    vec->capacity = 0;
}

char *xstrdup(const char *src) {
    size_t len = strlen(src);
    char *out = (char *)malloc(len + 1);
    if (!out) die("out of memory");
    memcpy(out, src, len + 1);
    return out;
}

char *xstrndup(const char *src, size_t n) {
    char *out = (char *)malloc(n + 1);
    if (!out) die("out of memory");
    memcpy(out, src, n);
    out[n] = '\0';
    return out;
}

void strbuf_init(StrBuf *buf) {
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static void strbuf_reserve(StrBuf *buf, size_t extra) {
    size_t need = buf->len + extra + 1;
    if (need <= buf->cap) return;
    size_t new_cap = buf->cap ? buf->cap * 2 : 32;
    while (new_cap < need) new_cap *= 2;
    char *new_data = (char *)realloc(buf->data, new_cap);
    if (!new_data) die("out of memory");
    buf->data = new_data;
    buf->cap = new_cap;
}

void strbuf_append_char(StrBuf *buf, char ch) {
    strbuf_reserve(buf, 1);
    buf->data[buf->len++] = ch;
    buf->data[buf->len] = '\0';
}

void strbuf_append_str(StrBuf *buf, const char *s) {
    size_t n = strlen(s);
    strbuf_reserve(buf, n);
    memcpy(buf->data + buf->len, s, n);
    buf->len += n;
    buf->data[buf->len] = '\0';
}

char *strbuf_take(StrBuf *buf) {
    if (!buf->data) return xstrdup("");
    char *out = buf->data;
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
    return out;
}

void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(1);
}

// --- value.c ---
PyValue *py_none(void) { return &PY_NONE_VALUE; }
PyValue *py_bool(int value) { return value ? &PY_TRUE_VALUE : &PY_FALSE_VALUE; }

PyValue *py_new_int(long value) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    if (!v) die("out of memory");
    v->type = PY_INT;
    v->as.i = value;
    return v;
}

PyValue *py_new_float(double value) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    if (!v) die("out of memory");
    v->type = PY_FLOAT;
    v->as.f = value;
    return v;
}

PyValue *py_new_string(const char *value) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    if (!v) die("out of memory");
    v->type = PY_STR;
    v->as.s = xstrdup(value);
    return v;
}

PyValue *py_new_list(void) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    PyList *list = (PyList *)calloc(1, sizeof(PyList));
    if (!v || !list) die("out of memory");
    v->type = PY_LIST;
    v->as.list = list;
    return v;
}

void py_list_append(PyValue *list_value, PyValue *item) {
    if (list_value->type != PY_LIST) die("expected list");
    PyList *list = list_value->as.list;
    if (list->count == list->capacity) {
        int new_cap = list->capacity ? list->capacity * 2 : 8;
        PyValue **new_items = (PyValue **)realloc(list->items, sizeof(PyValue *) * new_cap);
        if (!new_items) die("out of memory");
        list->items = new_items;
        list->capacity = new_cap;
    }
    list->items[list->count++] = item;
}

PyValue *py_list_get(PyValue *list_value, long index) {
    if (list_value->type != PY_LIST) die("expected list");
    PyList *list = list_value->as.list;
    if (index < 0 || index >= list->count) die("list index out of range");
    return list->items[index];
}

PyValue *py_new_sys(PyValue *argv_list) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    PySys *sys = (PySys *)calloc(1, sizeof(PySys));
    if (!v || !sys) die("out of memory");
    v->type = PY_SYS;
    sys->argv = argv_list;
    v->as.sys = sys;
    return v;
}

PyValue *py_new_function(PyFunction *func) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    if (!v) die("out of memory");
    v->type = PY_FUNCTION;
    v->as.func = func;
    return v;
}

PyValue *py_new_builtin(const char *name, PyCFunction fn) {
    PyValue *v = (PyValue *)calloc(1, sizeof(PyValue));
    PyBuiltinFunction *cfunc = (PyBuiltinFunction *)calloc(1, sizeof(PyBuiltinFunction));
    if (!v || !cfunc) die("out of memory");
    cfunc->name = xstrdup(name);
    cfunc->fn = fn;
    v->type = PY_BUILTIN_FUNCTION;
    v->as.cfunc = cfunc;
    return v;
}

int py_is_truthy(PyValue *value) {
    switch (value->type) {
        case PY_NONE: return 0;
        case PY_BOOL: return value->as.b;
        case PY_INT: return value->as.i != 0;
        case PY_FLOAT: return value->as.f != 0.0;
        case PY_STR: return value->as.s[0] != '\0';
        case PY_LIST: return value->as.list->count != 0;
        case PY_SYS:
        case PY_FUNCTION:
        case PY_BUILTIN_FUNCTION:
            return 1;
    }
    return 0;
}

double py_as_number(PyValue *value) {
    switch (value->type) {
        case PY_INT: return (double)value->as.i;
        case PY_FLOAT: return value->as.f;
        case PY_BOOL: return (double)value->as.b;
        default: die("expected number");
    }
    return 0.0;
}

char *py_to_string(PyValue *value) {
    char buf[128];
    switch (value->type) {
        case PY_NONE: return xstrdup("None");
        case PY_BOOL: return xstrdup(value->as.b ? "True" : "False");
        case PY_INT: snprintf(buf, sizeof(buf), "%ld", value->as.i); return xstrdup(buf);
        case PY_FLOAT: snprintf(buf, sizeof(buf), "%.15g", value->as.f); return xstrdup(buf);
        case PY_STR: return xstrdup(value->as.s);
        case PY_LIST: return xstrdup("<list>");
        case PY_SYS: return xstrdup("<sys>");
        case PY_FUNCTION: return xstrdup("<function>");
        case PY_BUILTIN_FUNCTION: return xstrdup("<builtin-function>");
    }
    return xstrdup("<unknown>");
}

// --- env.c ---
PyEnv *env_new(PyEnv *parent) {
    PyEnv *env = (PyEnv *)calloc(1, sizeof(PyEnv));
    if (!env) die("out of memory");
    env->parent = parent;
    return env;
}

static int env_find_local(PyEnv *env, const char *name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->items[i].name, name) == 0) return i;
    }
    return -1;
}

void env_set(PyEnv *env, const char *name, PyValue *value) {
    int idx = env_find_local(env, name);
    if (idx >= 0) {
        env->items[idx].value = value;
        return;
    }
    if (env->count == env->capacity) {
        int new_cap = env->capacity ? env->capacity * 2 : 8;
        PyBinding *new_items = (PyBinding *)realloc(env->items, sizeof(PyBinding) * new_cap);
        if (!new_items) die("out of memory");
        env->items = new_items;
        env->capacity = new_cap;
    }
    env->items[env->count].name = xstrdup(name);
    env->items[env->count].value = value;
    env->count++;
}

void env_assign(PyEnv *env, const char *name, PyValue *value) {
    for (PyEnv *cur = env; cur; cur = cur->parent) {
        int idx = env_find_local(cur, name);
        if (idx >= 0) {
            cur->items[idx].value = value;
            return;
        }
    }
    env_set(env, name, value);
}

PyValue *env_get(PyEnv *env, const char *name) {
    for (PyEnv *cur = env; cur; cur = cur->parent) {
        int idx = env_find_local(cur, name);
        if (idx >= 0) {
            return cur->items[idx].value;
        }
    }
    die("name '%s' is not defined", name);
    return NULL;
}

// --- ast_nodes.c ---
static AstNode *ast_alloc(AstKind kind, int line) {
    AstNode *node = (AstNode *)calloc(1, sizeof(AstNode));
    if (!node) die("out of memory");
    node->kind = kind;
    node->line = line;
    return node;
}

AstNode *ast_new_module(void) {
    AstNode *node = ast_alloc(AST_MODULE, 1);
    ptrvec_init(&node->as.module.body);
    return node;
}

AstNode *ast_new_expr_stmt(AstNode *expr, int line) {
    AstNode *node = ast_alloc(AST_EXPR_STMT, line);
    node->as.expr_stmt.expr = expr;
    return node;
}

AstNode *ast_new_assign(char *name, AstNode *value, int line) {
    AstNode *node = ast_alloc(AST_ASSIGN, line);
    node->as.assign.name = name;
    node->as.assign.value = value;
    return node;
}

AstNode *ast_new_if(AstNode *test, int line) {
    AstNode *node = ast_alloc(AST_IF, line);
    node->as.if_stmt.test = test;
    ptrvec_init(&node->as.if_stmt.body);
    ptrvec_init(&node->as.if_stmt.orelse);
    return node;
}

AstNode *ast_new_while(AstNode *test, int line) {
    AstNode *node = ast_alloc(AST_WHILE, line);
    node->as.while_stmt.test = test;
    ptrvec_init(&node->as.while_stmt.body);
    return node;
}

AstNode *ast_new_function_def(char *name, char **params, int param_count, int line) {
    AstNode *node = ast_alloc(AST_FUNCTION_DEF, line);
    node->as.function_def.name = name;
    node->as.function_def.params = params;
    node->as.function_def.param_count = param_count;
    ptrvec_init(&node->as.function_def.body);
    return node;
}

AstNode *ast_new_return(AstNode *value, int line) {
    AstNode *node = ast_alloc(AST_RETURN, line);
    node->as.return_stmt.value = value;
    return node;
}

AstNode *ast_new_pass(int line) {
    return ast_alloc(AST_PASS, line);
}

AstNode *ast_new_name(char *name, int line) {
    AstNode *node = ast_alloc(AST_NAME, line);
    node->as.name.name = name;
    return node;
}

AstNode *ast_new_int(long value, int line) {
    AstNode *node = ast_alloc(AST_CONSTANT, line);
    node->as.constant.int_value = value;
    return node;
}

AstNode *ast_new_float(double value, int line) {
    AstNode *node = ast_alloc(AST_CONSTANT, line);
    node->as.constant.is_float = 1;
    node->as.constant.float_value = value;
    return node;
}

AstNode *ast_new_string(char *value, int line) {
    AstNode *node = ast_alloc(AST_CONSTANT, line);
    node->as.constant.is_string = 1;
    node->as.constant.str_value = value;
    return node;
}

AstNode *ast_new_binop(OpKind op, AstNode *left, AstNode *right, int line) {
    AstNode *node = ast_alloc(AST_BINOP, line);
    node->as.binop.op = op;
    node->as.binop.left = left;
    node->as.binop.right = right;
    return node;
}

AstNode *ast_new_unaryop(OpKind op, AstNode *operand, int line) {
    AstNode *node = ast_alloc(AST_UNARYOP, line);
    node->as.unaryop.op = op;
    node->as.unaryop.operand = operand;
    return node;
}

AstNode *ast_new_compare(OpKind op, AstNode *left, AstNode *right, int line) {
    AstNode *node = ast_alloc(AST_COMPARE, line);
    node->as.compare.op = op;
    node->as.compare.left = left;
    node->as.compare.right = right;
    return node;
}

AstNode *ast_new_call(AstNode *func, int line) {
    AstNode *node = ast_alloc(AST_CALL, line);
    node->as.call.func = func;
    ptrvec_init(&node->as.call.args);
    return node;
}

AstNode *ast_new_attribute(AstNode *value, char *attr, int line) {
    AstNode *node = ast_alloc(AST_ATTRIBUTE, line);
    node->as.attribute.value = value;
    node->as.attribute.attr = attr;
    return node;
}

AstNode *ast_new_subscript(AstNode *value, AstNode *index, int line) {
    AstNode *node = ast_alloc(AST_SUBSCRIPT, line);
    node->as.subscript.value = value;
    node->as.subscript.index = index;
    return node;
}

static void ast_free_vec(PtrVec *vec) {
    for (int i = 0; i < vec->count; i++) {
        ast_free((AstNode *)vec->items[i]);
    }
    ptrvec_free(vec);
}

void ast_free(AstNode *node) {
    if (!node) return;
    switch (node->kind) {
        case AST_MODULE:
            ast_free_vec(&node->as.module.body); break;
        case AST_EXPR_STMT:
            ast_free(node->as.expr_stmt.expr); break;
        case AST_ASSIGN:
            free(node->as.assign.name); ast_free(node->as.assign.value); break;
        case AST_IF:
            ast_free(node->as.if_stmt.test);
            ast_free_vec(&node->as.if_stmt.body);
            ast_free_vec(&node->as.if_stmt.orelse); break;
        case AST_WHILE:
            ast_free(node->as.while_stmt.test);
            ast_free_vec(&node->as.while_stmt.body); break;
        case AST_FUNCTION_DEF:
            free(node->as.function_def.name);
            for (int i = 0; i < node->as.function_def.param_count; i++) {
                free(node->as.function_def.params[i]);
            }
            free(node->as.function_def.params);
            ast_free_vec(&node->as.function_def.body); break;
        case AST_RETURN:
            ast_free(node->as.return_stmt.value); break;
        case AST_NAME:
            free(node->as.name.name); break;
        case AST_CONSTANT:
            if (node->as.constant.is_string) free(node->as.constant.str_value);
            break;
        case AST_BINOP:
            ast_free(node->as.binop.left); ast_free(node->as.binop.right); break;
        case AST_UNARYOP:
            ast_free(node->as.unaryop.operand); break;
        case AST_COMPARE:
            ast_free(node->as.compare.left); ast_free(node->as.compare.right); break;
        case AST_CALL:
            ast_free(node->as.call.func); ast_free_vec(&node->as.call.args); break;
        case AST_ATTRIBUTE:
            ast_free(node->as.attribute.value); free(node->as.attribute.attr); break;
        case AST_SUBSCRIPT:
            ast_free(node->as.subscript.value); ast_free(node->as.subscript.index); break;
        case AST_PASS:
            break;
    }
    free(node);
}

// --- lexer.c ---
static void tokenvec_push(TokenVec *vec, Token tok) {
    if (vec->count == vec->capacity) {
        int new_cap = vec->capacity ? vec->capacity * 2 : 64;
        Token *new_items = (Token *)realloc(vec->items, sizeof(Token) * new_cap);
        if (!new_items) die("out of memory");
        vec->items = new_items;
        vec->capacity = new_cap;
    }
    vec->items[vec->count++] = tok;
}

static void emit(TokenVec *tokens, TokenKind kind, char *lexeme, int line, int col) {
    Token tok;
    tok.kind = kind; tok.lexeme = lexeme; tok.line = line; tok.col = col;
    tokenvec_push(tokens, tok);
}

static TokenKind keyword_kind(const char *text) {
    if (strcmp(text, "def") == 0) return TOK_DEF;
    if (strcmp(text, "if") == 0) return TOK_IF;
    if (strcmp(text, "else") == 0) return TOK_ELSE;
    if (strcmp(text, "while") == 0) return TOK_WHILE;
    if (strcmp(text, "return") == 0) return TOK_RETURN;
    if (strcmp(text, "pass") == 0) return TOK_PASS;
    return TOK_NAME;
}

static void lex_line(const char *line, int line_no, int start_col, TokenVec *tokens) {
    int i = start_col;
    while (line[i] && line[i] != '\n' && line[i] != '\r') {
        char c = line[i];
        if (c == '#') break;
        if (c == ' ' || c == '\t') { i++; continue; }
        if (isalpha((unsigned char)c) || c == '_') {
            int start = i; i++;
            while (isalnum((unsigned char)line[i]) || line[i] == '_') i++;
            char *text = xstrndup(line + start, (size_t)(i - start));
            emit(tokens, keyword_kind(text), text, line_no, start + 1);
            continue;
        }
        if (isdigit((unsigned char)c)) {
            int start = i; int is_float = 0; i++;
            while (isdigit((unsigned char)line[i])) i++;
            if (line[i] == '.') {
                is_float = 1; i++;
                while (isdigit((unsigned char)line[i])) i++;
            }
            char *text = xstrndup(line + start, (size_t)(i - start));
            emit(tokens, is_float ? TOK_FLOAT : TOK_INT, text, line_no, start + 1);
            continue;
        }
        if (c == '\'' || c == '"') {
            char quote = c; int start = i;
            StrBuf buf; strbuf_init(&buf); i++;
            while (line[i] && line[i] != quote) {
                if (line[i] == '\\') {
                    i++; if (!line[i]) break;
                    switch (line[i]) {
                        case 'n': strbuf_append_char(&buf, '\n'); break;
                        case 't': strbuf_append_char(&buf, '\t'); break;
                        case '\\': strbuf_append_char(&buf, '\\'); break;
                        case '\'': strbuf_append_char(&buf, '\''); break;
                        case '"': strbuf_append_char(&buf, '"'); break;
                        default: strbuf_append_char(&buf, line[i]); break;
                    }
                    i++;
                } else {
                    strbuf_append_char(&buf, line[i]); i++;
                }
            }
            if (line[i] != quote) die("unterminated string at line %d", line_no);
            i++;
            emit(tokens, TOK_STRING, strbuf_take(&buf), line_no, start + 1);
            continue;
        }
        if (c == '=' && line[i + 1] == '=') { emit(tokens, TOK_EQEQ, xstrdup("=="), line_no, i + 1); i += 2; continue; }
        if (c == '!' && line[i + 1] == '=') { emit(tokens, TOK_NE, xstrdup("!="), line_no, i + 1); i += 2; continue; }
        if (c == '<' && line[i + 1] == '=') { emit(tokens, TOK_LE, xstrdup("<="), line_no, i + 1); i += 2; continue; }
        if (c == '>' && line[i + 1] == '=') { emit(tokens, TOK_GE, xstrdup(">="), line_no, i + 1); i += 2; continue; }
        TokenKind kind = TOK_EOF;
        switch (c) {
            case '(': kind = TOK_LPAREN; break; case ')': kind = TOK_RPAREN; break;
            case '[': kind = TOK_LBRACKET; break; case ']': kind = TOK_RBRACKET; break;
            case ',': kind = TOK_COMMA; break; case ':': kind = TOK_COLON; break;
            case '.': kind = TOK_DOT; break; case '+': kind = TOK_PLUS; break;
            case '-': kind = TOK_MINUS; break; case '*': kind = TOK_STAR; break;
            case '/': kind = TOK_SLASH; break; case '%': kind = TOK_PERCENT; break;
            case '=': kind = TOK_EQUAL; break; case '<': kind = TOK_LT; break;
            case '>': kind = TOK_GT; break;
            default: die("unexpected character '%c' at line %d", c, line_no);
        }
        char *text = xstrndup(&line[i], 1);
        emit(tokens, kind, text, line_no, i + 1); i++;
    }
}

void lex_source(const char *source, TokenVec *out_tokens) {
    out_tokens->items = NULL; out_tokens->count = 0; out_tokens->capacity = 0;
    int indent_stack[128]; int indent_top = 0; indent_stack[0] = 0;
    const char *cursor = source; int line_no = 1;
    while (*cursor) {
        const char *line_start = cursor;
        while (*cursor && *cursor != '\n') cursor++;
        size_t len = (size_t)(cursor - line_start);
        if (*cursor == '\n') cursor++;
        char *line = xstrndup(line_start, len);
        int col = 0; int indent = 0;
        while (line[col] == ' ' || line[col] == '\t') {
            indent += (line[col] == '\t') ? 4 : 1; col++;
        }
        int blank = 1;
        for (int i = col; line[i]; i++) {
            if (line[i] == '#') break;
            if (!isspace((unsigned char)line[i])) { blank = 0; break; }
        }
        if (!blank) {
            if (indent > indent_stack[indent_top]) {
                indent_stack[++indent_top] = indent;
                emit(out_tokens, TOK_INDENT, xstrdup("<INDENT>"), line_no, 1);
            } else {
                while (indent < indent_stack[indent_top]) {
                    indent_top--; emit(out_tokens, TOK_DEDENT, xstrdup("<DEDENT>"), line_no, 1);
                }
                if (indent != indent_stack[indent_top]) die("inconsistent indentation at line %d", line_no);
            }
            lex_line(line, line_no, col, out_tokens);
            emit(out_tokens, TOK_NEWLINE, xstrdup("<NEWLINE>"), line_no, (int)len + 1);
        }
        free(line); line_no++;
    }
    while (indent_top > 0) { indent_top--; emit(out_tokens, TOK_DEDENT, xstrdup("<DEDENT>"), line_no, 1); }
    emit(out_tokens, TOK_EOF, xstrdup("<EOF>"), line_no, 1);
}

void free_tokens(TokenVec *tokens) {
    for (int i = 0; i < tokens->count; i++) free(tokens->items[i].lexeme);
    free(tokens->items);
    tokens->items = NULL; tokens->count = 0; tokens->capacity = 0;
}

// --- parser.c ---
typedef struct {
    TokenVec *tokens;
    int pos;
    const char *filename;
} Parser;

static Token *peek(Parser *p) { return &p->tokens->items[p->pos]; }
static Token *prev(Parser *p) { return &p->tokens->items[p->pos - 1]; }
static int match(Parser *p, TokenKind kind) {
    if (peek(p)->kind == kind) { p->pos++; return 1; }
    return 0;
}
static Token *expect(Parser *p, TokenKind kind, const char *message) {
    if (peek(p)->kind != kind) die("%s:%d:%d: %s", p->filename, peek(p)->line, peek(p)->col, message);
    p->pos++; return prev(p);
}
static void skip_newlines(Parser *p) { while (match(p, TOK_NEWLINE)) {} }

static AstNode *parse_expr(Parser *p);
static AstNode *parse_stmt(Parser *p);

static AstNode *parse_primary(Parser *p) {
    Token *tok = peek(p);
    if (match(p, TOK_INT)) return ast_new_int(strtol(prev(p)->lexeme, NULL, 10), tok->line);
    if (match(p, TOK_FLOAT)) return ast_new_float(strtod(prev(p)->lexeme, NULL), tok->line);
    if (match(p, TOK_STRING)) return ast_new_string(xstrdup(prev(p)->lexeme), tok->line);
    if (match(p, TOK_NAME)) {
        AstNode *expr = ast_new_name(xstrdup(prev(p)->lexeme), tok->line);
        for (;;) {
            if (match(p, TOK_LPAREN)) {
                AstNode *call = ast_new_call(expr, tok->line);
                if (!match(p, TOK_RPAREN)) {
                    do { ptrvec_push(&call->as.call.args, parse_expr(p)); } while (match(p, TOK_COMMA));
                    expect(p, TOK_RPAREN, "expected ')'");
                }
                expr = call; continue;
            }
            if (match(p, TOK_DOT)) {
                Token *name = expect(p, TOK_NAME, "expected attribute name");
                expr = ast_new_attribute(expr, xstrdup(name->lexeme), tok->line); continue;
            }
            if (match(p, TOK_LBRACKET)) {
                AstNode *index = parse_expr(p);
                expect(p, TOK_RBRACKET, "expected ']'");
                expr = ast_new_subscript(expr, index, tok->line); continue;
            }
            break;
        }
        return expr;
    }
    if (match(p, TOK_LPAREN)) {
        AstNode *expr = parse_expr(p);
        expect(p, TOK_RPAREN, "expected ')'");
        for (;;) {
            if (match(p, TOK_LPAREN)) {
                AstNode *call = ast_new_call(expr, tok->line);
                if (!match(p, TOK_RPAREN)) {
                    do { ptrvec_push(&call->as.call.args, parse_expr(p)); } while (match(p, TOK_COMMA));
                    expect(p, TOK_RPAREN, "expected ')'");
                }
                expr = call; continue;
            }
            if (match(p, TOK_DOT)) {
                Token *name = expect(p, TOK_NAME, "expected attribute name");
                expr = ast_new_attribute(expr, xstrdup(name->lexeme), tok->line); continue;
            }
            if (match(p, TOK_LBRACKET)) {
                AstNode *index = parse_expr(p);
                expect(p, TOK_RBRACKET, "expected ']'");
                expr = ast_new_subscript(expr, index, tok->line); continue;
            }
            break;
        }
        return expr;
    }
    die("%s:%d:%d: expected expression", p->filename, tok->line, tok->col);
    return NULL;
}

static AstNode *parse_unary(Parser *p) {
    if (match(p, TOK_MINUS)) return ast_new_unaryop(OP_NEG, parse_unary(p), prev(p)->line);
    return parse_primary(p);
}

static AstNode *parse_factor(Parser *p) {
    AstNode *expr = parse_unary(p);
    for (;;) {
        OpKind op;
        if (match(p, TOK_STAR)) op = OP_MUL; else if (match(p, TOK_SLASH)) op = OP_DIV; else if (match(p, TOK_PERCENT)) op = OP_MOD; else break;
        expr = ast_new_binop(op, expr, parse_unary(p), prev(p)->line);
    }
    return expr;
}

static AstNode *parse_term(Parser *p) {
    AstNode *expr = parse_factor(p);
    for (;;) {
        OpKind op;
        if (match(p, TOK_PLUS)) op = OP_ADD; else if (match(p, TOK_MINUS)) op = OP_SUB; else break;
        expr = ast_new_binop(op, expr, parse_factor(p), prev(p)->line);
    }
    return expr;
}

static AstNode *parse_comparison(Parser *p) {
    AstNode *expr = parse_term(p);
    for (;;) {
        OpKind op;
        if (match(p, TOK_EQEQ)) op = OP_EQ; else if (match(p, TOK_NE)) op = OP_NE; else if (match(p, TOK_LT)) op = OP_LT;
        else if (match(p, TOK_LE)) op = OP_LE; else if (match(p, TOK_GT)) op = OP_GT; else if (match(p, TOK_GE)) op = OP_GE; else break;
        expr = ast_new_compare(op, expr, parse_term(p), prev(p)->line);
    }
    return expr;
}

static AstNode *parse_expr(Parser *p) { return parse_comparison(p); }

static void parse_block(Parser *p, PtrVec *body) {
    expect(p, TOK_NEWLINE, "expected newline after ':'");
    expect(p, TOK_INDENT, "expected indented block");
    skip_newlines(p);
    while (peek(p)->kind != TOK_DEDENT && peek(p)->kind != TOK_EOF) {
        ptrvec_push(body, parse_stmt(p));
        skip_newlines(p);
    }
    expect(p, TOK_DEDENT, "expected dedent");
}

static AstNode *parse_function_def(Parser *p) {
    Token *name = expect(p, TOK_NAME, "expected function name");
    expect(p, TOK_LPAREN, "expected '(' after function name");
    char **params = NULL; int param_count = 0; int param_cap = 0;
    if (!match(p, TOK_RPAREN)) {
        do {
            Token *param = expect(p, TOK_NAME, "expected parameter name");
            if (param_count == param_cap) {
                int new_cap = param_cap ? param_cap * 2 : 4;
                char **new_params = (char **)realloc(params, sizeof(char *) * new_cap);
                if (!new_params) die("out of memory");
                params = new_params; param_cap = new_cap;
            }
            params[param_count++] = xstrdup(param->lexeme);
        } while (match(p, TOK_COMMA));
        expect(p, TOK_RPAREN, "expected ')'");
    }
    expect(p, TOK_COLON, "expected ':' after function definition");
    AstNode *fn = ast_new_function_def(xstrdup(name->lexeme), params, param_count, name->line);
    parse_block(p, &fn->as.function_def.body);
    return fn;
}

static AstNode *parse_if(Parser *p) {
    Token *tok = prev(p);
    AstNode *test = parse_expr(p);
    expect(p, TOK_COLON, "expected ':' after if condition");
    AstNode *node = ast_new_if(test, tok->line);
    parse_block(p, &node->as.if_stmt.body);
    skip_newlines(p);
    if (match(p, TOK_ELSE)) {
        expect(p, TOK_COLON, "expected ':' after else");
        parse_block(p, &node->as.if_stmt.orelse);
    }
    return node;
}

static AstNode *parse_while(Parser *p) {
    Token *tok = prev(p);
    AstNode *test = parse_expr(p);
    expect(p, TOK_COLON, "expected ':' after while condition");
    AstNode *node = ast_new_while(test, tok->line);
    parse_block(p, &node->as.while_stmt.body);
    return node;
}

static AstNode *parse_stmt(Parser *p) {
    if (match(p, TOK_DEF)) return parse_function_def(p);
    if (match(p, TOK_IF)) return parse_if(p);
    if (match(p, TOK_WHILE)) return parse_while(p);
    if (match(p, TOK_RETURN)) {
        int line = prev(p)->line;
        if (peek(p)->kind == TOK_NEWLINE) { expect(p, TOK_NEWLINE, "expected newline"); return ast_new_return(NULL, line); }
        AstNode *expr = parse_expr(p);
        expect(p, TOK_NEWLINE, "expected newline after return");
        return ast_new_return(expr, line);
    }
    if (match(p, TOK_PASS)) {
        int line = prev(p)->line;
        expect(p, TOK_NEWLINE, "expected newline after pass");
        return ast_new_pass(line);
    }
    if (peek(p)->kind == TOK_NAME && p->tokens->items[p->pos + 1].kind == TOK_EQUAL) {
        Token *name = expect(p, TOK_NAME, "expected name");
        expect(p, TOK_EQUAL, "expected '='");
        AstNode *value = parse_expr(p);
        expect(p, TOK_NEWLINE, "expected newline after assignment");
        return ast_new_assign(xstrdup(name->lexeme), value, name->line);
    }
    AstNode *expr = parse_expr(p);
    expect(p, TOK_NEWLINE, "expected newline after expression");
    return ast_new_expr_stmt(expr, expr->line);
}

AstNode *parse_tokens(TokenVec *tokens, const char *filename) {
    Parser p; p.tokens = tokens; p.pos = 0; p.filename = filename;
    AstNode *module = ast_new_module();
    skip_newlines(&p);
    while (peek(&p)->kind != TOK_EOF) {
        ptrvec_push(&module->as.module.body, parse_stmt(&p));
        skip_newlines(&p);
    }
    return module;
}

// --- ast.c ---
AstNode *parse_source(const char *source, const char *filename) {
    TokenVec tokens;
    lex_source(source, &tokens);
    AstNode *module = parse_tokens(&tokens, filename);
    free_tokens(&tokens);
    return module;
}

// --- operator.c ---
static int both_ints(PyValue *a, PyValue *b) {
    return a->type == PY_INT && b->type == PY_INT;
}

PyValue *py_apply_binop(OpKind op, PyValue *left, PyValue *right) {
    if (op == OP_ADD && left->type == PY_STR && right->type == PY_STR) {
        StrBuf buf; strbuf_init(&buf);
        strbuf_append_str(&buf, left->as.s); strbuf_append_str(&buf, right->as.s);
        char *joined = strbuf_take(&buf);
        PyValue *out = py_new_string(joined); free(joined); return out;
    }
    if (both_ints(left, right)) {
        long a = left->as.i; long b = right->as.i;
        switch (op) {
            case OP_ADD: return py_new_int(a + b);
            case OP_SUB: return py_new_int(a - b);
            case OP_MUL: return py_new_int(a * b);
            case OP_DIV: return py_new_float((double)a / (double)b);
            case OP_MOD: return py_new_int(a % b);
            default: break;
        }
    }
    double a = py_as_number(left); double b = py_as_number(right);
    switch (op) {
        case OP_ADD: return py_new_float(a + b);
        case OP_SUB: return py_new_float(a - b);
        case OP_MUL: return py_new_float(a * b);
        case OP_DIV: return py_new_float(a / b);
        case OP_MOD: return py_new_float((long)a % (long)b);
        default: die("unsupported binary operator");
    }
    return py_none();
}

PyValue *py_apply_compare(OpKind op, PyValue *left, PyValue *right) {
    if (left->type == PY_STR && right->type == PY_STR) {
        int cmp = strcmp(left->as.s, right->as.s);
        switch (op) {
            case OP_EQ: return py_bool(cmp == 0); case OP_NE: return py_bool(cmp != 0);
            case OP_LT: return py_bool(cmp < 0); case OP_LE: return py_bool(cmp <= 0);
            case OP_GT: return py_bool(cmp > 0); case OP_GE: return py_bool(cmp >= 0);
            default: break;
        }
    }
    double a = py_as_number(left); double b = py_as_number(right);
    switch (op) {
        case OP_EQ: return py_bool(a == b); case OP_NE: return py_bool(a != b);
        case OP_LT: return py_bool(a < b); case OP_LE: return py_bool(a <= b);
        case OP_GT: return py_bool(a > b); case OP_GE: return py_bool(a >= b);
        default: die("unsupported comparison operator");
    }
    return py_bool(0);
}

PyValue *py_apply_unary(OpKind op, PyValue *value) {
    switch (op) {
        case OP_NEG:
            if (value->type == PY_INT) return py_new_int(-value->as.i);
            return py_new_float(-py_as_number(value));
        default: die("unsupported unary operator");
    }
    return py_none();
}

// --- function.c ---
PyValue *py_make_function(const char *name, char **params, int param_count, AstNode *body, PyEnv *closure) {
    PyFunction *fn = (PyFunction *)calloc(1, sizeof(PyFunction));
    if (!fn) die("out of memory");
    fn->name = xstrdup(name); fn->params = params; fn->param_count = param_count;
    fn->body = body; fn->closure = closure;
    return py_new_function(fn);
}

// --- call.c ---
PyValue *py_call(PyRuntime *rt, PyValue *callable, int argc, PyValue **argv) {
    if (callable->type == PY_BUILTIN_FUNCTION) {
        return callable->as.cfunc->fn(rt, argc, argv);
    }
    if (callable->type == PY_FUNCTION) {
        PyFunction *fn = callable->as.func;
        if (argc != fn->param_count) die("%s() expected %d arguments, got %d", fn->name, fn->param_count, argc);
        PyEnv *local = env_new(fn->closure);
        for (int i = 0; i < argc; i++) env_set(local, fn->params[i], argv[i]);
        ExecResult result = exec_function_body(rt, local, fn->body);
        if (result.has_return) return result.value;
        return py_none();
    }
    die("object is not callable");
    return py_none();
}

// --- eval.c ---
static ExecResult exec_block(PyRuntime *rt, PyEnv *env, PtrVec *body) {
    ExecResult result = {0, NULL};
    for (int i = 0; i < body->count; i++) {
        result = exec_stmt(rt, env, (AstNode *)body->items[i]);
        if (result.has_return) return result;
    }
    return result;
}

PyValue *eval_expr(PyRuntime *rt, PyEnv *env, AstNode *node) {
    (void)rt;
    switch (node->kind) {
        case AST_NAME:
            return env_get(env, node->as.name.name);
        case AST_CONSTANT:
            if (node->as.constant.is_string) return py_new_string(node->as.constant.str_value);
            if (node->as.constant.is_float) return py_new_float(node->as.constant.float_value);
            return py_new_int(node->as.constant.int_value);
        case AST_BINOP: {
            PyValue *left = eval_expr(rt, env, node->as.binop.left);
            PyValue *right = eval_expr(rt, env, node->as.binop.right);
            return py_apply_binop(node->as.binop.op, left, right);
        }
        case AST_UNARYOP: {
            PyValue *value = eval_expr(rt, env, node->as.unaryop.operand);
            return py_apply_unary(node->as.unaryop.op, value);
        }
        case AST_COMPARE: {
            PyValue *left = eval_expr(rt, env, node->as.compare.left);
            PyValue *right = eval_expr(rt, env, node->as.compare.right);
            return py_apply_compare(node->as.compare.op, left, right);
        }
        case AST_CALL: {
            PyValue *func = eval_expr(rt, env, node->as.call.func);
            int argc = node->as.call.args.count;
            PyValue **argv = argc ? (PyValue **)calloc((size_t)argc, sizeof(PyValue *)) : NULL;
            for (int i = 0; i < argc; i++) argv[i] = eval_expr(rt, env, (AstNode *)node->as.call.args.items[i]);
            PyValue *result = py_call(rt, func, argc, argv);
            free(argv); return result;
        }
        case AST_ATTRIBUTE: {
            PyValue *value = eval_expr(rt, env, node->as.attribute.value);
            if (value->type == PY_SYS && strcmp(node->as.attribute.attr, "argv") == 0) return value->as.sys->argv;
            die("unsupported attribute access: %s", node->as.attribute.attr);
        }
        case AST_SUBSCRIPT: {
            PyValue *value = eval_expr(rt, env, node->as.subscript.value);
            PyValue *index = eval_expr(rt, env, node->as.subscript.index);
            if (index->type != PY_INT) die("subscript index must be int");
            if (value->type == PY_LIST) return py_list_get(value, index->as.i);
            if (value->type == PY_STR) {
                long i = index->as.i; size_t len = strlen(value->as.s);
                if (i < 0 || (size_t)i >= len) die("string index out of range");
                char text[2]; text[0] = value->as.s[i]; text[1] = '\0';
                return py_new_string(text);
            }
            die("unsupported subscript target");
        }
        default: die("unsupported expression kind %d", node->kind);
    }
    return py_none();
}

ExecResult exec_stmt(PyRuntime *rt, PyEnv *env, AstNode *node) {
    ExecResult result = {0, NULL};
    switch (node->kind) {
        case AST_EXPR_STMT:
            (void)eval_expr(rt, env, node->as.expr_stmt.expr); return result;
        case AST_ASSIGN: {
            PyValue *value = eval_expr(rt, env, node->as.assign.value);
            env_assign(env, node->as.assign.name, value); return result;
        }
        case AST_IF: {
            PyValue *cond = eval_expr(rt, env, node->as.if_stmt.test);
            if (py_is_truthy(cond)) return exec_block(rt, env, &node->as.if_stmt.body);
            return exec_block(rt, env, &node->as.if_stmt.orelse);
        }
        case AST_WHILE:
            while (py_is_truthy(eval_expr(rt, env, node->as.while_stmt.test))) {
                result = exec_block(rt, env, &node->as.while_stmt.body);
                if (result.has_return) return result;
            }
            return result;
        case AST_FUNCTION_DEF: {
            PyValue *fn = py_make_function(node->as.function_def.name, node->as.function_def.params,
                                           node->as.function_def.param_count, node, env);
            env_set(env, node->as.function_def.name, fn); return result;
        }
        case AST_RETURN:
            result.has_return = 1;
            result.value = node->as.return_stmt.value ? eval_expr(rt, env, node->as.return_stmt.value) : py_none();
            return result;
        case AST_PASS:
            return result;
        default: die("unsupported statement kind %d", node->kind);
    }
    return result;
}

ExecResult exec_module(PyRuntime *rt, PyEnv *env, AstNode *module) {
    if (module->kind != AST_MODULE) die("expected module");
    return exec_block(rt, env, &module->as.module.body);
}

ExecResult exec_function_body(PyRuntime *rt, PyEnv *env, AstNode *function_def) {
    if (function_def->kind != AST_FUNCTION_DEF) die("expected function definition");
    return exec_block(rt, env, &function_def->as.function_def.body);
}

// --- builtin.c ---
static PyValue *builtin_print(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    for (int i = 0; i < argc; i++) {
        char *text = py_to_string(argv[i]);
        if (i > 0) fputc(' ', stdout);
        fputs(text, stdout); free(text);
    }
    fputc('\n', stdout); return py_none();
}

static PyValue *builtin_len(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1) die("len() expects 1 argument");
    if (argv[0]->type == PY_STR) return py_new_int((long)strlen(argv[0]->as.s));
    if (argv[0]->type == PY_LIST) return py_new_int((long)argv[0]->as.list->count);
    die("len() unsupported for this type"); return py_none();
}

static PyValue *builtin_int(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1) die("int() expects 1 argument");
    if (argv[0]->type == PY_INT) return argv[0];
    if (argv[0]->type == PY_FLOAT) return py_new_int((long)argv[0]->as.f);
    if (argv[0]->type == PY_BOOL) return py_new_int((long)argv[0]->as.b);
    die("int() unsupported for this type"); return py_none();
}

static PyValue *builtin_float(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1) die("float() expects 1 argument");
    return py_new_float(py_as_number(argv[0]));
}

static PyValue *builtin_str(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1) die("str() expects 1 argument");
    char *s = py_to_string(argv[0]); PyValue *out = py_new_string(s);
    free(s); return out;
}

static PyValue *builtin_bool(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1) die("bool() expects 1 argument");
    return py_bool(py_is_truthy(argv[0]));
}

static PyValue *builtin_import(PyRuntime *rt, int argc, PyValue **argv) {
    (void)rt;
    if (argc != 1 || argv[0]->type != PY_STR) die("__import__() expects one string argument");
    if (strcmp(argv[0]->as.s, "sys") == 0) return rt->sys_value;
    die("unsupported import: %s", argv[0]->as.s); return py_none();
}

PyValue *builtin_run_path_impl(PyRuntime *rt, const char *path) {
    char *source = read_file(path);
    AstNode *module = parse_source(source, path);
    PyValue *old_argv = rt->sys_value->as.sys->argv;
    PyValue *new_argv = py_new_list();
    PyList *old_list = old_argv->as.list;
    for (int i = 1; i < old_list->count; i++) py_list_append(new_argv, old_list->items[i]);
    rt->sys_value->as.sys->argv = new_argv;
    PyEnv *module_env = env_new(rt->globals);
    env_set(module_env, "__name__", py_new_string("__main__"));
    env_set(module_env, "__file__", py_new_string(path));
    ExecResult result = exec_module(rt, module_env, module);
    (void)result;
    rt->sys_value->as.sys->argv = old_argv;
    ast_free(module); free(source); return py_none();
}

static PyValue *builtin_run_path(PyRuntime *rt, int argc, PyValue **argv) {
    if (argc != 1 || argv[0]->type != PY_STR) die("run_path() expects one string argument");
    return builtin_run_path_impl(rt, argv[0]->as.s);
}

void builtin_install(PyRuntime *rt) {
    env_set(rt->globals, "print", py_new_builtin("print", builtin_print));
    env_set(rt->globals, "len", py_new_builtin("len", builtin_len));
    env_set(rt->globals, "int", py_new_builtin("int", builtin_int));
    env_set(rt->globals, "float", py_new_builtin("float", builtin_float));
    env_set(rt->globals, "str", py_new_builtin("str", builtin_str));
    env_set(rt->globals, "bool", py_new_builtin("bool", builtin_bool));
    env_set(rt->globals, "__import__", py_new_builtin("__import__", builtin_import));
    env_set(rt->globals, "run_path", py_new_builtin("run_path", builtin_run_path));
}

// --- runtime.c ---
PyRuntime *runtime_new(void) {
    PyRuntime *rt = (PyRuntime *)calloc(1, sizeof(PyRuntime));
    if (!rt) die("out of memory");
    rt->globals = env_new(NULL);
    runtime_init_builtins(rt);
    return rt;
}

void runtime_set_argv(PyRuntime *rt, int argc, char **argv) {
    PyValue *list = py_new_list();
    for (int i = 0; i < argc; i++) py_list_append(list, py_new_string(argv[i]));
    rt->sys_value = py_new_sys(list);
    env_set(rt->globals, "sys", rt->sys_value);
}

void runtime_init_builtins(PyRuntime *rt) {
    builtin_install(rt);
}

// --- py0i.c (主程式) ---
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./py0i <script.py>\n");
        return 1;
    }

    PyRuntime *rt = runtime_new();
    runtime_set_argv(rt, argc - 1, argv + 1);
    env_set(rt->globals, "__name__", py_new_string("__main__"));
    env_set(rt->globals, "__file__", py_new_string(argv[1]));

    char *source = read_file(argv[1]);
    AstNode *module = parse_source(source, argv[1]);
    ExecResult result = exec_module(rt, rt->globals, module);
    (void)result;
    
    free(source);
    ast_free(module);
    
    return 0;
}