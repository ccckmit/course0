#include "codegen.h"
#include "ast.h"
#include "lexer.h"   /* for operator token types */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================================================================
   LLVM IR Code Generator
   Strategy:
   - Every expression returns a "Value" (SSA register name or constant string).
   - Local variables are alloca'd; loads/stores are explicit.
   - We use a simple counter for %tmp registers and labels.
   ================================================================ */

#define MAX_LOCALS  2048
#define MAX_GLOBALS 2048
#define MAX_STRINGS 2048

typedef struct {
    char *name;       /* C name */
    char *llvm_name;  /* alloca register, e.g. %x */
    Type *type;
    int   is_param;   /* parameters: already a register, no alloca needed */
} Local;

typedef struct {
    char *name;
    Type *type;
    int   is_extern;
} Global;

struct Codegen {
    FILE   *out;
    const char *source_filename;

    /* counters */
    int    reg;      /* SSA register counter */
    int    label;    /* label counter */
    int    str_id;   /* string literal counter */

    /* scope */
    Local   locals[MAX_LOCALS];
    int     n_locals;
    int     scope_stack[64];  /* n_locals at scope entry */
    int     scope_depth;

    /* globals */
    Global  globals[MAX_GLOBALS];
    int     n_globals;

    /* string literals deferred to top-level */
    char   *str_literals[MAX_STRINGS];
    int     str_ids[MAX_STRINGS];
    int     n_strings;

    /* break/continue targets */
    char   break_label[64];
    char   cont_label[64];

    /* current function return type */
    Type  *cur_ret_type;
    char   cur_func[128];
};

/* ---------------------------------------------------------------- helpers */

static int new_reg(Codegen *cg)   { return cg->reg++; }
static int new_label(Codegen *cg) { return cg->label++; }

/* Format a register reference — we use named %tN registers to avoid
   LLVM's strict sequential-numbering requirement for unnamed values */
static const char *reg_name(int r, char *buf, size_t sz) {
    snprintf(buf, sz, "%%t%d", r);
    return buf;
}
#define REGBUF(r) (reg_name((r), (char[32]){0}, 32))

/* __c0c_emit: provided by c0c_compat.c.
   Only declare for the host C compiler — c0c uses the IR header declare. */
#ifndef __C0C__
extern void __c0c_emit(FILE *out, const char *fmt, ...);
#endif

#define EMIT_BUF_SIZE 8192  /* kept for reference */

/* Convert our Type to LLVM IR type string (static buffer rotation.
   Use a global char pointer backed by a fixed array.
   Named individually so c0c initializes each as a separate global. */
#define N_TBUFS 8
#define TBUF_SIZE 256
/* __c0c_get_tbuf: provided by c0c_compat.c, returns tbuf[i%8].
   In c0c IR this becomes 'declare ptr @__c0c_get_tbuf(i32)' from the header. */
#ifndef __C0C__
extern char *__c0c_get_tbuf(int i);
#endif
static int   tbuf_idx = 0;

static const char *llvm_type(const Type *t) {
    char *buf = __c0c_get_tbuf(tbuf_idx++);
    if (!buf) buf = __c0c_get_tbuf(0);  /* safety fallback */
    if (!t) { strcpy(buf, "i32"); return buf; }
    switch (t->kind) {
    case TY_VOID:      strcpy(buf, "void");   break;
    case TY_BOOL:      strcpy(buf, "i1");     break;
    case TY_CHAR: case TY_SCHAR: case TY_UCHAR: strcpy(buf, "i8");  break;
    case TY_SHORT: case TY_USHORT: strcpy(buf, "i16"); break;
    case TY_INT: case TY_UINT: case TY_ENUM: strcpy(buf, "i32"); break;
    case TY_LONG: case TY_ULONG:
    case TY_LONGLONG: case TY_ULONGLONG: strcpy(buf, "i64"); break;
    case TY_FLOAT:     strcpy(buf, "float");  break;
    case TY_DOUBLE:    strcpy(buf, "double"); break;
    case TY_PTR:       strcpy(buf, "ptr");    break;
    case TY_ARRAY:     strcpy(buf, "ptr");    break;
    case TY_FUNC:      strcpy(buf, "ptr");    break;
    case TY_STRUCT:
    case TY_UNION:
        if (t->tag) snprintf(buf, 256, "%%struct.%s", t->tag);
        else strcpy(buf, "ptr");
        break;
    case TY_TYPEDEF_REF:
        /* best effort – treat as i64 */
        strcpy(buf, "i64");
        break;
    default: strcpy(buf, "i64"); break;
    }
    return buf;
}

static const char *llvm_ret_type(const Type *ft) {
    if (!ft || ft->kind != TY_FUNC) return "i32";
    return llvm_type(ft->ret);
}

static int type_is_fp(const Type *t) {
    if (!t) return 0;
    return t->kind == TY_FLOAT || t->kind == TY_DOUBLE;
}

/* ---------------------------------------------------------------- scope */

static void scope_push(Codegen *cg) {
    cg->scope_stack[cg->scope_depth++] = cg->n_locals;
}

static void scope_pop(Codegen *cg) {
    int prev = cg->scope_stack[--cg->scope_depth];
    for (int i = prev; i < cg->n_locals; i++) {
        free(cg->locals[i].name);
        free(cg->locals[i].llvm_name);
        /* types owned by AST */
    }
    cg->n_locals = prev;
}

static Local *find_local(Codegen *cg, const char *name) {
    for (int i = cg->n_locals - 1; i >= 0; i--)
        if (strcmp(cg->locals[i].name, name) == 0)
            return &cg->locals[i];
    return NULL;
}

static Global *find_global(Codegen *cg, const char *name) {
    for (int i = 0; i < cg->n_globals; i++)
        if (strcmp(cg->globals[i].name, name) == 0)
            return &cg->globals[i];
    return NULL;
}

static Local *add_local(Codegen *cg, const char *name, Type *type, int is_param) {
    if (cg->n_locals >= MAX_LOCALS) { fprintf(stderr, "c0c: too many locals\n"); exit(1); }
    Local *l  = &cg->locals[cg->n_locals++];
    l->name   = strdup(name);
    int rid   = new_reg(cg);
    l->llvm_name = malloc(32);
    snprintf(l->llvm_name, 32, "%%t%d", rid);
    l->type   = type;
    l->is_param = is_param;
    return l;
}

/* ---------------------------------------------------------------- string literals */

static int intern_string(Codegen *cg, const char *raw) {
    /* raw includes surrounding quotes */
    int id = cg->str_id++;
    cg->str_literals[cg->n_strings] = strdup(raw);
    cg->str_ids[cg->n_strings]      = id;
    cg->n_strings++;
    return id;
}

/* Compute byte length of a C string literal (handles \n etc.) */
static int str_literal_len(const char *raw) {
    /* raw: "hello\n" */
    int len = 0;
    const char *p = raw + 1; /* skip opening " */
    while (*p && *p != '"') {
        if (*p == '\\') { p++; if (*p) p++; }
        else p++;
        len++;
    }
    return len + 1; /* +1 for NUL */
}

/* Emit string literal content suitable for LLVM c"..." */
static void emit_str_content(Codegen *cg, const char *raw) {
    const char *p = raw + 1;
    while (*p && *p != '"') {
        if (*p == '\\' && *(p+1)) {
            p++;
            switch (*p) {
            case 'n': __c0c_emit(cg->out, "\\0A"); break;
            case 't': __c0c_emit(cg->out, "\\09"); break;
            case 'r': __c0c_emit(cg->out, "\\0D"); break;
            case '0': __c0c_emit(cg->out, "\\00"); break;
            case '"': __c0c_emit(cg->out, "\\22"); break;
            case '\\': __c0c_emit(cg->out, "\\5C"); break;
            default:  __c0c_emit(cg->out, "\\%02X", (unsigned char)*p); break;
            }
            p++;
        } else {
            if (*p == '"') break;
            __c0c_emit(cg->out, "%c", *p++);
        }
    }
    __c0c_emit(cg->out, "\\00"); /* NUL terminator */
}

/* ---------------------------------------------------------------- forward decl */

typedef struct { char reg[64]; Type *type; } Val;

static Val emit_expr(Codegen *cg, Node *n);
static void emit_stmt(Codegen *cg, Node *n);
static void emit_func_def(Codegen *cg, Node *n);
static void emit_global_var(Codegen *cg, Node *n);

/* ================================================================ Expressions */

/* Forward declarations for type helpers needed before emit_lvalue_addr */
static int val_is_64bit(Val v);
static int val_is_ptr(Val v);
static int promote_to_i64(Codegen *cg, Val v, char *out_reg, size_t out_sz);
static Type *default_int_type(void);
static Type *default_i64_type(void);
static Type *default_ptr_type(void);
static Type *default_fp_type(void);

static Val make_val(const char *reg, Type *type) {
    Val v;
    strncpy(v.reg, reg, 63); v.reg[63] = '\0';
    v.type = type;
    return v;
}

/* Compute the *address* of an lvalue node.
   Returns ptr register string, or NULL if not lvalue. */
static char *emit_lvalue_addr(Codegen *cg, Node *n) {
    if (n->kind == ND_IDENT) {
        Local *l = find_local(cg, n->sval);
        if (l) return strdup(l->llvm_name);
        Global *g = find_global(cg, n->sval);
        if (g) {
            char *buf = malloc(128);
            snprintf(buf, 128, "@%s", n->sval);
            return buf;
        }
        /* external / undeclared – treat as global */
        char *buf = malloc(128);
        snprintf(buf, 128, "@%s", n->sval);
        return buf;
    }
    if (n->kind == ND_DEREF) {
        Val v = emit_expr(cg, n->children[0]);
        if (val_is_ptr(v)) return strdup(v.reg);
        /* convert i64 to ptr */
        int rp = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rp, v.reg);
        char *buf = malloc(32); snprintf(buf, 32, "%%t%d", rp);
        return buf;
    }
    if (n->kind == ND_INDEX) {
        Val base_v = emit_expr(cg, n->children[0]);
        Val idx_v  = emit_expr(cg, n->children[1]);
        int r = new_reg(cg);
        Type *elem = (n->children[0]->type && n->children[0]->type->base)
                      ? n->children[0]->type->base : NULL;
        /* Default to ptr element stride when type unknown — safer for pointer arrays */
        const char *et = elem ? llvm_type(elem) : "ptr";
        /* ensure base is ptr */
        char base_r[64];
        if (val_is_ptr(base_v)) { strncpy(base_r, base_v.reg, 63); base_r[63] = '\0'; }
        else { int rp = new_reg(cg); __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rp, base_v.reg); snprintf(base_r, 64, "%%t%d", rp); }
        /* ensure idx is i64 */
        char idx_r[64]; promote_to_i64(cg, idx_v, idx_r, 64);
        __c0c_emit(cg->out, "  %%t%d = getelementptr %s, ptr %s, i64 %s\n", r, et, base_r, idx_r);
        char *buf = malloc(32);
        snprintf(buf, 32, "%%t%d", r);
        return buf;
    }
    if (n->kind == ND_MEMBER || n->kind == ND_ARROW) {
        Val base_v;
        if (n->kind == ND_ARROW) base_v = emit_expr(cg, n->children[0]);
        else {
            char *addr = emit_lvalue_addr(cg, n->children[0]);
            if (addr) {
                base_v = make_val(addr, default_ptr_type());
                free(addr);
            } else {
                base_v = emit_expr(cg, n->children[0]);
                if (!val_is_ptr(base_v)) {
                    int rp = new_reg(cg);
                    char promoted[64]; promote_to_i64(cg, base_v, promoted, 64);
                    __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rp, promoted);
                    char tmp[32]; snprintf(tmp, 32, "%%t%d", rp);
                    base_v = make_val(tmp, default_ptr_type());
                }
            }
        }
        char *memname = n->sval;
        Type *basety = n->children[0] ? n->children[0]->type : NULL;
        long offset = 0;
        if (basety && (basety->kind == TY_STRUCT || basety->kind == TY_UNION) && basety->params) {
            for (int i = 0; i < basety->n_members; i++) {
                if (basety->params[i].name && strcmp(basety->params[i].name, memname) == 0) {
                    break;
                }
                if (basety->members && basety->members[i]) {
                    offset += type_size(basety->members[i]);
                }
            }
        }
        {
            int r = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = getelementptr i8, ptr %s, i64 %ld\n", r, base_v.reg, offset);
            char *buf = malloc(32);
            snprintf(buf, 32, "%%t%d", r);
            return buf;
        }
    }
    return NULL;
}

static Type *default_int_type(void) {
    static Type t_int = { .kind = TY_INT, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
    return &t_int;
}
static Type *default_i64_type(void) {
    static Type t_i64 = { .kind = TY_LONG, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
    return &t_i64;
}
static Type *default_ptr_type(void) {
    static Type t_ptr = { .kind = TY_PTR, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
    return &t_ptr;
}
static Type *default_fp_type(void) {
    static Type t_double = { .kind = TY_DOUBLE, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
    return &t_double;
}
/* Return the effective LLVM type for a Val — used to decide sext/truncation */
static int val_is_64bit(Val v) {
    if (!v.type) return 0;
    switch (v.type->kind) {
    case TY_LONG: case TY_ULONG: case TY_LONGLONG: case TY_ULONGLONG:
    case TY_PTR:  case TY_ARRAY: case TY_DOUBLE:
        return 1;
    default:
        return 0;
    }
}
static int val_is_ptr(Val v) {
    if (!v.type) return 0;
    return v.type->kind == TY_PTR || v.type->kind == TY_ARRAY;
}

/* Promote any value to i64 for arithmetic. Handles ptr, i64, i32. */
static int promote_to_i64(Codegen *cg, Val v, char *out_reg, size_t out_sz) {
    if (val_is_ptr(v)) {
        int r = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = ptrtoint ptr %s to i64\n", r, v.reg);
        snprintf(out_reg, out_sz, "%%t%d", r);
        return r;
    } else if (val_is_64bit(v)) {
        strncpy(out_reg, v.reg, out_sz - 1); out_reg[out_sz-1] = '\0';
        return -1;
    } else {
        int r = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = sext i32 %s to i64\n", r, v.reg);
        snprintf(out_reg, out_sz, "%%t%d", r);
        return r;
    }
}

/* Emit a truthiness check: returns register number of i1 result */
static int emit_cond(Codegen *cg, Val cv) {
    int r = new_reg(cg);
    if (val_is_ptr(cv)) {
        __c0c_emit(cg->out, "  %%t%d = icmp ne ptr %s, null\n", r, cv.reg);
    } else if (type_is_fp(cv.type)) {
        __c0c_emit(cg->out, "  %%t%d = fcmp one double %s, 0.0\n", r, cv.reg);
    } else {
        char promoted[64];
        promote_to_i64(cg, cv, promoted, 64);
        __c0c_emit(cg->out, "  %%t%d = icmp ne i64 %s, 0\n", r, promoted);
    }
    return r;
}

static Val emit_expr(Codegen *cg, Node *n) {
    if (!n) return make_val("0", default_int_type());
    int line = n->line;
    (void)line;

    switch (n->kind) {

    case ND_INT_LIT: {
        char buf[32];
        snprintf(buf, sizeof buf, "%lld", n->ival);
        return make_val(buf, default_int_type());
    }

    case ND_FLOAT_LIT: {
        /* LLVM needs hex float for precise representation */
        int r = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = fadd double 0.0, %g\n", r, n->fval);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        static Type t_double = { .kind = TY_DOUBLE, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
        return make_val(buf, &t_double);
    }

    case ND_CHAR_LIT: {
        char buf[32];
        snprintf(buf, sizeof buf, "%lld", n->ival);
        static Type t_char = { .kind = TY_CHAR, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
        return make_val(buf, &t_char);
    }

    case ND_STRING_LIT: {
        int sid = intern_string(cg, n->sval);
        int r   = new_reg(cg);
        int slen = str_literal_len(n->sval);
        __c0c_emit(cg->out, "  %%t%d = getelementptr [%d x i8], ptr @.str%d, i64 0, i64 0\n",
             r, slen, sid);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        return make_val(buf, default_ptr_type());
    }

    case ND_IDENT: {
        Local *l = find_local(cg, n->sval);
        if (l) {
            if (l->is_param) return make_val(l->llvm_name, l->type);
            int r = new_reg(cg);
            /* Determine actual load type: we store i64 for ints, ptr for pointers */
            const char *load_t;
            Type *ret_t;
            if (l->type && (l->type->kind == TY_PTR || l->type->kind == TY_ARRAY)) {
                load_t = "ptr"; ret_t = default_ptr_type();
            } else if (l->type && type_is_fp(l->type)) {
                load_t = llvm_type(l->type); ret_t = l->type;
            } else {
                load_t = "i64";
                /* Preserve original C type (TY_INT, TY_CHAR etc.) so varargs
                   truncation works correctly for printf("%d", int_var) */
                ret_t = (l->type) ? l->type : default_i64_type();
            }
            __c0c_emit(cg->out, "  %%t%d = load %s, ptr %s\n", r, load_t, l->llvm_name);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
            return make_val(buf, ret_t);
        }
        Global *g = find_global(cg, n->sval);
        if (g && g->type && g->type->kind != TY_FUNC) {
            int r = new_reg(cg);
            const char *load_t;
            Type *ret_t;
            if (g->type->kind == TY_PTR || g->type->kind == TY_ARRAY) {
                load_t = "ptr"; ret_t = default_ptr_type();
            } else if (type_is_fp(g->type)) {
                load_t = llvm_type(g->type); ret_t = g->type;
            } else {
                load_t = "i64"; ret_t = g->type ? g->type : default_i64_type();
            }
            __c0c_emit(cg->out, "  %%t%d = load %s, ptr @%s\n", r, load_t, n->sval);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
            return make_val(buf, ret_t);
        }
        /* function or undeclared: return pointer to global */
        char buf[128]; snprintf(buf, sizeof buf, "@%s", n->sval);
        return make_val(buf, default_ptr_type());
    }

    case ND_CALL: {
        /* children[0] = callee, children[1..] = args */
        Node *callee = n->children[0];
        /* Try to determine return type */
        Type *ret_type = default_int_type();

        /* Collect args */
        char **arg_regs  = malloc(n->n_children * 8);
        Type **arg_types = malloc(n->n_children * 8);
        for (int i = 1; i < n->n_children; i++) {
            Val av = emit_expr(cg, n->children[i]);
            arg_regs[i]  = strdup(av.reg);
            arg_types[i] = av.type;
        }

        /* get callee */
        char callee_buf[128] = {0};
        if (callee->kind == ND_IDENT) {
            snprintf(callee_buf, sizeof callee_buf, "@%s", callee->sval);
            Global *g = find_global(cg, callee->sval);
            if (g && g->type && g->type->kind == TY_FUNC)
                ret_type = g->type->ret;
            /* well-known libc functions that return ptr */
            else {
                const char *ptr_funcs[] = {
                    "malloc","calloc","realloc","strdup","strndup",
                    "memcpy","memmove","memset","strcpy","strncpy",
                    "strcat","strncat","strchr","strrchr","strstr",
                    "fopen","fdopen","freopen","tmpfile",
                    "getenv","setlocale","strtok","strerror",
                    "node_new","type_new","type_ptr","type_array",
                    "parser_new","lexer_new","codegen_new",
                    "macro_preprocess","read_file",
                    "__c0c_stderr","__c0c_stdout","__c0c_stdin",
                    "__c0c_get_tbuf","__c0c_get_td_name",
                    NULL
                };
                for (int pi = 0; ptr_funcs[pi]; pi++) {
                    if (strcmp(callee->sval, ptr_funcs[pi]) == 0) {
                        ret_type = default_ptr_type();
                        break;
                    }
                }
                /* functions returning i64 */
                const char *i64_funcs[] = {
                    "strlen","strtol","strtoll","atol","atoll",
                    "ftell","fread","fwrite","fseek",
                    "__c0c_get_td_kind",
                    NULL
                };
                for (int pi = 0; i64_funcs[pi]; pi++) {
                    if (strcmp(callee->sval, i64_funcs[pi]) == 0) {
                        ret_type = default_i64_type();
                        break;
                    }
                }
                /* functions returning void */
                static Type t_void = { .kind = TY_VOID, .is_const = 0, .is_volatile = 0, .base = NULL, .array_size = -1, .ret = NULL, .params = NULL, .param_count = 0, .variadic = 0, .tag = NULL, .members = NULL, .n_members = 0, .name = NULL };
                const char *void_funcs[] = {
                    "__c0c_va_start","__c0c_va_end","__c0c_va_copy",
                    "__c0c_emit",
                    "free","exit","perror","assert",
                    NULL
                };
                for (int pi = 0; void_funcs[pi]; pi++) {
                    if (strcmp(callee->sval, void_funcs[pi]) == 0) {
                        ret_type = &t_void;
                        break;
                    }
                }
            }
        } else {
            Val cv = emit_expr(cg, callee);
            strncpy(callee_buf, cv.reg, sizeof callee_buf - 1);
        }

        /* Is this a varargs function? printf/fprintf/sprintf/snprintf need i32 for int args */
        int is_varargs_call = 0;
        if (callee->kind == ND_IDENT) {
            const char *va_funcs[] = {
                "printf","fprintf","sprintf","snprintf","dprintf",
                "vprintf","vfprintf","vsprintf","vsnprintf",
                "__c0c_emit",
                NULL
            };
            for (int pi = 0; va_funcs[pi]; pi++)
                if (strcmp(callee->sval, va_funcs[pi]) == 0) { is_varargs_call = 1; break; }
        }

        /* For varargs calls on arm64/x86-64, integer args are passed as 64-bit.
           Do NOT truncate to i32 — keep all integers as i64 for varargs.
           The format string (%d vs %lld) is handled by printf internally. */

        int r = new_reg(cg);
        const char *rt = llvm_type(ret_type);
        int is_void = (ret_type->kind == TY_VOID);

        if (is_void && is_varargs_call)
            __c0c_emit(cg->out, "  call void (ptr, ...) %s(", callee_buf);
        else if (is_void)
            __c0c_emit(cg->out, "  call void %s(", callee_buf);
        else if (is_varargs_call)
            __c0c_emit(cg->out, "  %%t%d = call %s (ptr, ...) %s(", r, rt, callee_buf);
        else
            __c0c_emit(cg->out, "  %%t%d = call %s %s(", r, rt, callee_buf);

        for (int i = 1; i < n->n_children; i++) {
            if (i > 1) __c0c_emit(cg->out, ", ");
            const char *at;
            if (arg_types[i] && (arg_types[i]->kind == TY_PTR || arg_types[i]->kind == TY_ARRAY))
                at = "ptr";
            else if (arg_types[i] && type_is_fp(arg_types[i]))
                at = llvm_type(arg_types[i]);
            else
                at = "i64";  /* all integers as i64 — arm64 varargs promotes to 64-bit */
            __c0c_emit(cg->out, "%s %s", at, arg_regs[i]);
        }
        __c0c_emit(cg->out, ")\n");

        for (int i = 1; i < n->n_children; i++) free(arg_regs[i]);
        free(arg_regs); free(arg_types);

        if (is_void) return make_val("0", ret_type);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);

        /* Normalize return: if call emits i32/i8/etc but we want i64, add sext.
           Only sext if: not ptr, not fp, not void, and actual emitted type is < 64 bits */
        Type *val_ret = ret_type;
        if (!type_is_fp(ret_type) && ret_type->kind != TY_PTR &&
            ret_type->kind != TY_ARRAY && ret_type->kind != TY_VOID) {
            int ret_sz = type_size(ret_type);
            /* Only sext if the emitted type is actually < i64
               (ret_sz == 0 means struct/unknown -> already emitted as i64) */
            if (ret_sz > 0 && ret_sz < 8 && strcmp(rt, "i64") != 0) {
                int rs = new_reg(cg);
                __c0c_emit(cg->out, "  %%t%d = sext %s %%t%d to i64\n", rs, rt, r);
                snprintf(buf, sizeof buf, "%%t%d", rs);
            }
            val_ret = default_i64_type();
        }
        return make_val(buf, val_ret);
    }

    case ND_BINOP: {
        /* Short-circuit operators must be handled before evaluating both operands */
        if (n->op == TOK_AND) {
            Val lv = emit_expr(cg, n->children[0]);
            int lTrue = new_label(cg), lFalse = new_label(cg), lEnd = new_label(cg);
            char la[64]; promote_to_i64(cg, lv, la, 64);
            int rA = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = icmp ne i64 %s, 0\n", rA, la);
            __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rA, lTrue, lFalse);
            __c0c_emit(cg->out, "L%d:\n", lTrue);
            Val rv = emit_expr(cg, n->children[1]);
            char lb[64]; promote_to_i64(cg, rv, lb, 64);
            int rB = new_reg(cg), rBext = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = icmp ne i64 %s, 0\n", rB, lb);
            __c0c_emit(cg->out, "  %%t%d = zext i1 %%t%d to i64\n", rBext, rB);
            __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
            __c0c_emit(cg->out, "L%d:\n", lFalse);
            __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
            __c0c_emit(cg->out, "L%d:\n", lEnd);
            int rZ = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = phi i64 [ %%t%d, %%L%d ], [ 0, %%L%d ]\n",
                       rZ, rBext, lTrue, lFalse);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", rZ);
            return make_val(buf, default_i64_type());
        }
        if (n->op == TOK_OR) {
            Val lv = emit_expr(cg, n->children[0]);
            int lTrue = new_label(cg), lFalse = new_label(cg), lEnd = new_label(cg);
            char la[64]; promote_to_i64(cg, lv, la, 64);
            int rA = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = icmp ne i64 %s, 0\n", rA, la);
            __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rA, lTrue, lFalse);
            __c0c_emit(cg->out, "L%d:\n", lTrue);
            __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
            __c0c_emit(cg->out, "L%d:\n", lFalse);
            Val rv = emit_expr(cg, n->children[1]);
            char lb[64]; promote_to_i64(cg, rv, lb, 64);
            int rB = new_reg(cg), rBext = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = icmp ne i64 %s, 0\n", rB, lb);
            __c0c_emit(cg->out, "  %%t%d = zext i1 %%t%d to i64\n", rBext, rB);
            __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
            __c0c_emit(cg->out, "L%d:\n", lEnd);
            int rZ = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = phi i64 [ 1, %%L%d ], [ %%t%d, %%L%d ]\n",
                       rZ, lTrue, rBext, lFalse);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", rZ);
            return make_val(buf, default_i64_type());
        }

        Val lv = emit_expr(cg, n->children[0]);
        Val rv = emit_expr(cg, n->children[1]);
        int r  = new_reg(cg);
        int fp = type_is_fp(lv.type) || type_is_fp(rv.type);
        int is_ptr = val_is_ptr(lv);
        const char *lt = fp ? llvm_type(lv.type) : "i64";

        /* For pointer arithmetic: convert ptrs to i64 first, then do arithmetic */
        char lreg[64], rreg[64];
        lreg[0] = '\0'; rreg[0] = '\0';
        int is_cmp = 0;
        switch (n->op) {
        case TOK_EQ: case TOK_NEQ:
        case TOK_LT: case TOK_GT: case TOK_LEQ: case TOK_GEQ:
            is_cmp = 1; break;
        default: break;
        }
        if (!fp) {
            promote_to_i64(cg, lv, lreg, 64);
            promote_to_i64(cg, rv, rreg, 64);
            lt = "i64";
        } else {
            strncpy(lreg, lv.reg, 63); lreg[63] = '\0';
            /* For fp comparison with integer, need to convert int to double */
            if (is_cmp && rv.type && !type_is_fp(rv.type)) {
                int rconv = new_reg(cg);
                __c0c_emit(cg->out, "  %%t%d = sitofp i64 %s to double\n", rconv, rv.reg);
                snprintf(rreg, 64, "%%t%d", rconv);
            } else {
                strncpy(rreg, rv.reg, 63); rreg[63] = '\0';
            }
        }

        const char *op = NULL;
        switch (n->op) {
        case TOK_PLUS:    op = fp ? "fadd" : (is_ptr ? "getelementptr" : "add");  break;
        case TOK_MINUS:   op = fp ? "fsub" : "sub";   break;
        case TOK_STAR:    op = fp ? "fmul" : "mul";   break;
        case TOK_SLASH:   op = fp ? "fdiv" : "sdiv";  break;
        case TOK_PERCENT: op = fp ? "frem" : "srem";  break;
        case TOK_AMP:     op = "and"; break;
        case TOK_PIPE:    op = "or";  break;
        case TOK_CARET:   op = "xor"; break;
        case TOK_LSHIFT:  op = "shl"; break;
        case TOK_RSHIFT:  op = "ashr"; break;
        case TOK_EQ:   op = fp ? "fcmp oeq" : "icmp eq";  is_cmp = 1; break;
        case TOK_NEQ:  op = fp ? "fcmp one" : "icmp ne";  is_cmp = 1; break;
        case TOK_LT:   op = fp ? "fcmp olt" : "icmp slt"; is_cmp = 1; break;
        case TOK_GT:   op = fp ? "fcmp ogt" : "icmp sgt"; is_cmp = 1; break;
        case TOK_LEQ:  op = fp ? "fcmp ole" : "icmp sle"; is_cmp = 1; break;
        case TOK_GEQ:  op = fp ? "fcmp oge" : "icmp sge"; is_cmp = 1; break;
        case TOK_AND: case TOK_OR:
            /* handled above with short-circuit; should not reach here */
            op = "add"; break;
        default:
            op = "add"; /* fallback */
        }

        if (n->op == TOK_PLUS && is_ptr) {
            /* ptr + int: convert i64 back to ptr for GEP */
            int rptr = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rptr, lreg);
            __c0c_emit(cg->out, "  %%t%d = getelementptr i8, ptr %%t%d, i64 %s\n", r, rptr, rreg);
        } else if (is_cmp) {
            __c0c_emit(cg->out, "  %%t%d = %s %s %s, %s\n", r, op, lt, lreg, rreg);
            /* fcmp already returns i1, just zext to i64 */
            int rZ = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = zext i1 %%t%d to i64\n", rZ, r);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", rZ);
            return make_val(buf, default_i64_type());
        } else {
            __c0c_emit(cg->out, "  %%t%d = %s %s %s, %s\n", r, op, lt, lreg, rreg);
        }

        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        /* ptr+int returns ptr (GEP result), ptr arithmetic returns i64, everything else i64 */
        if (n->op == TOK_PLUS && is_ptr)
            return make_val(buf, default_ptr_type());
        if (is_ptr)  /* ptr - ptr, ptr * int etc: all i64 */
            return make_val(buf, default_i64_type());
        if (fp)
            return make_val(buf, default_fp_type());
        return make_val(buf, default_i64_type());
    }

    case ND_UNOP: {
        Val v = emit_expr(cg, n->children[0]);
        int r = new_reg(cg);
        int fp = type_is_fp(v.type);
        char vp[64];
        if (!fp) promote_to_i64(cg, v, vp, 64);
        switch (n->op) {
        case TOK_MINUS:
            if (fp) __c0c_emit(cg->out, "  %%t%d = fneg double %s\n", r, v.reg);
            else    __c0c_emit(cg->out, "  %%t%d = sub i64 0, %s\n", r, vp);
            break;
        case TOK_BANG: {
            int rc = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = icmp eq i64 %s, 0\n", rc, vp);
            __c0c_emit(cg->out, "  %%t%d = zext i1 %%t%d to i64\n", r, rc);
            break;
        }
        case TOK_TILDE:
            __c0c_emit(cg->out, "  %%t%d = xor i64 %s, -1\n", r, vp);
            break;
        case TOK_PLUS:
            return v;
        default:
            __c0c_emit(cg->out, "  %%t%d = add i64 %s, 0\n", r, vp);
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        return make_val(buf, fp ? v.type : default_i64_type());
    }

    case ND_ASSIGN: {
        Val rv_val = emit_expr(cg, n->children[1]);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) {
            /* Use the actual LLVM type of the value, not the C type */
            const char *st;
            if (val_is_ptr(rv_val))       st = "ptr";
            else if (type_is_fp(rv_val.type)) st = llvm_type(rv_val.type);
            else                           st = "i64";
            /* If value might be i32, promote first */
            char stored[64];
            if (!val_is_ptr(rv_val) && !val_is_64bit(rv_val) && !type_is_fp(rv_val.type)) {
                int rp = new_reg(cg);
                __c0c_emit(cg->out, "  %%t%d = sext i32 %s to i64\n", rp, rv_val.reg);
                snprintf(stored, 64, "%%t%d", rp);
            } else {
                strncpy(stored, rv_val.reg, 63); stored[63] = '\0';
            }
            __c0c_emit(cg->out, "  store %s %s, ptr %s\n", st, stored, addr);
            free(addr);
        }
        return rv_val;
    }

    case ND_COMPOUND_ASSIGN: {
        /* e.g.  x += y  =>  x = x op y */
        Val lv_val = emit_expr(cg, n->children[0]);
        Val rv_val = emit_expr(cg, n->children[1]);
        int r      = new_reg(cg);
        int fp     = type_is_fp(lv_val.type) || type_is_fp(rv_val.type);
        const char *it = fp ? "double" : "i64";
        char lr[64], rr[64];
        if (!fp) {
            promote_to_i64(cg, lv_val, lr, 64);
            promote_to_i64(cg, rv_val, rr, 64);
        } else {
            strncpy(lr, lv_val.reg, 63); lr[63] = '\0';
            strncpy(rr, rv_val.reg, 63); rr[63] = '\0';
        }
        const char *op2;
        switch (n->op) {
        case TOK_PLUS_ASSIGN:   op2 = fp ? "fadd" : "add";  break;
        case TOK_MINUS_ASSIGN:  op2 = fp ? "fsub" : "sub";  break;
        case TOK_STAR_ASSIGN:   op2 = fp ? "fmul" : "mul";  break;
        case TOK_SLASH_ASSIGN:  op2 = fp ? "fdiv" : "sdiv"; break;
        case TOK_PERCENT_ASSIGN:op2 = "srem"; break;
        case TOK_AMP_ASSIGN:    op2 = "and";  break;
        case TOK_PIPE_ASSIGN:   op2 = "or";   break;
        case TOK_CARET_ASSIGN:  op2 = "xor";  break;
        case TOK_LSHIFT_ASSIGN: op2 = "shl";  break;
        case TOK_RSHIFT_ASSIGN: op2 = "ashr"; break;
        default: op2 = "add";
        }
        __c0c_emit(cg->out, "  %%t%d = %s %s %s, %s\n", r, op2, it, lr, rr);

        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) {
            __c0c_emit(cg->out, "  store %s %%t%d, ptr %s\n", it, r, addr);
            free(addr);
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        return make_val(buf, fp ? lv_val.type : default_i64_type());
    }

    case ND_PRE_INC:
    case ND_PRE_DEC: {
        Val v = emit_expr(cg, n->children[0]);
        int r = new_reg(cg);
        const char *op3 = (n->kind == ND_PRE_INC) ? "add" : "sub";
        char vr[64];
        promote_to_i64(cg, v, vr, 64);
        __c0c_emit(cg->out, "  %%t%d = %s i64 %s, 1\n", r, op3, vr);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) { __c0c_emit(cg->out, "  store i64 %%t%d, ptr %s\n", r, addr); free(addr); }
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        return make_val(buf, default_i64_type());
    }

    case ND_POST_INC:
    case ND_POST_DEC: {
        Val v = emit_expr(cg, n->children[0]);
        int r = new_reg(cg);
        const char *op4 = (n->kind == ND_POST_INC) ? "add" : "sub";
        char vr[64];
        promote_to_i64(cg, v, vr, 64);
        __c0c_emit(cg->out, "  %%t%d = %s i64 %s, 1\n", r, op4, vr);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) { __c0c_emit(cg->out, "  store i64 %%t%d, ptr %s\n", r, addr); free(addr); }
        return v; /* return old value */
    }

    case ND_ADDR: {
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (!addr) return make_val("null", default_ptr_type());
        Val v = make_val(addr, default_ptr_type());
        free(addr);
        return v;
    }

    case ND_DEREF: {
        Val pv = emit_expr(cg, n->children[0]);
        int r  = new_reg(cg);
        /* Ensure we have a ptr to load from */
        char ptr_r[64];
        if (val_is_ptr(pv)) {
            strncpy(ptr_r, pv.reg, 63); ptr_r[63] = '\0';
        } else {
            int rp = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rp, pv.reg);
            snprintf(ptr_r, 64, "%%t%d", rp);
        }
        /* Load: use ptr type for pointer targets, i64 for everything else */
        Type *base = (pv.type && pv.type->base) ? pv.type->base : default_int_type();
        int base_is_ptr = (base->kind == TY_PTR || base->kind == TY_ARRAY);
        const char *load_t = base_is_ptr ? "ptr" : "i64";
        Type *ret_t = base_is_ptr ? default_ptr_type() : default_i64_type();
        __c0c_emit(cg->out, "  %%t%d = load %s, ptr %s\n", r, load_t, ptr_r);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        return make_val(buf, ret_t);
    }

    case ND_INDEX: {
        Val base_v = emit_expr(cg, n->children[0]);
        Val idx_v  = emit_expr(cg, n->children[1]);
        /* Element type: use base's element type if known.
           If base is a ptr with unknown base, default to ptr (8-byte stride)
           rather than i32 (4-byte stride) — safer for pointer arrays. */
        Type *elem = (base_v.type && base_v.type->base) ? base_v.type->base : NULL;
        /* Determine GEP element type and load type */
        int elem_is_ptr = (elem && (elem->kind == TY_PTR || elem->kind == TY_ARRAY));
        int elem_is_fp  = (elem && type_is_fp(elem));
        const char *gep_et;
        const char *load_t;
        Type *ret_elem;
        if (!elem) {
            /* Unknown element type — use ptr stride (safer for pointer arrays) */
            gep_et = "ptr"; load_t = "ptr"; ret_elem = default_ptr_type();
        } else if (elem_is_ptr) {
            gep_et = "ptr"; load_t = "ptr"; ret_elem = default_ptr_type();
        } else if (elem_is_fp) {
            gep_et = llvm_type(elem); load_t = llvm_type(elem); ret_elem = elem;
        } else {
            gep_et = "i64"; load_t = "i64"; ret_elem = default_i64_type();
        }
        /* Ensure base is ptr and index is i64 */
        char base_r[64], idx_r[64];
        if (val_is_ptr(base_v)) strncpy(base_r, base_v.reg, 63);
        else { int rb = new_reg(cg); __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rb, base_v.reg); snprintf(base_r, 64, "%%t%d", rb); }
        promote_to_i64(cg, idx_v, idx_r, 64);
        base_r[63] = '\0';
        int rG = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = getelementptr %s, ptr %s, i64 %s\n",
             rG, gep_et, base_r, idx_r);
        int rL = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = load %s, ptr %%t%d\n", rL, load_t, rG);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", rL);
        return make_val(buf, ret_elem);
    }

    case ND_CAST: {
        Val v  = emit_expr(cg, n->cast_expr);
        Type *dst = n->cast_type;
        if (!dst) return v;
        int r  = new_reg(cg);
        int fp_src = type_is_fp(v.type);
        int fp_dst = type_is_fp(dst);
        int is_dst_ptr = (dst->kind == TY_PTR || dst->kind == TY_ARRAY);
        int is_src_ptr = val_is_ptr(v);
        if (fp_src && fp_dst) {
            int sz_src = type_size(v.type); int sz_dst = type_size(dst);
            if (sz_dst > sz_src) __c0c_emit(cg->out, "  %%t%d = fpext float %s to double\n", r, v.reg);
            else __c0c_emit(cg->out, "  %%t%d = fptrunc double %s to float\n", r, v.reg);
        } else if (fp_src && !fp_dst) {
            __c0c_emit(cg->out, "  %%t%d = fptosi double %s to i64\n", r, v.reg);
        } else if (!fp_src && fp_dst) {
            char promoted[64]; promote_to_i64(cg, v, promoted, 64);
            __c0c_emit(cg->out, "  %%t%d = sitofp i64 %s to %s\n", r, promoted, llvm_type(dst));
        } else if (is_dst_ptr && !is_src_ptr) {
            /* int -> ptr */
            char promoted[64]; promote_to_i64(cg, v, promoted, 64);
            __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", r, promoted);
        } else if (!is_dst_ptr && is_src_ptr) {
            /* ptr -> int */
            __c0c_emit(cg->out, "  %%t%d = ptrtoint ptr %s to i64\n", r, v.reg);
        } else if (is_dst_ptr && is_src_ptr) {
            /* ptr -> ptr: no-op */
            __c0c_emit(cg->out, "  %%t%d = bitcast ptr %s to ptr\n", r, v.reg);
        } else {
            /* int -> int: use i64 throughout */
            char promoted[64]; promote_to_i64(cg, v, promoted, 64);
            __c0c_emit(cg->out, "  %%t%d = add i64 %s, 0\n", r, promoted);  /* no-op promote */
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r);
        if (is_dst_ptr) return make_val(buf, default_ptr_type());
        if (fp_dst)     return make_val(buf, dst);
        return make_val(buf, default_i64_type());
    }

    case ND_TERNARY: {
        Val cv     = emit_expr(cg, n->cond);
        int lT     = new_label(cg);
        int lF     = new_label(cg);
        int lEnd   = new_label(cg);
        int rcond = emit_cond(cg, cv);
        __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rcond, lT, lF);
        __c0c_emit(cg->out, "L%d:\n", lT);
        Val tv = emit_expr(cg, n->children[0]);
        char tv_r[64]; promote_to_i64(cg, tv, tv_r, 64);
        __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
        __c0c_emit(cg->out, "L%d:\n", lF);
        Val fv = emit_expr(cg, n->children[1]);
        char fv_r[64]; promote_to_i64(cg, fv, fv_r, 64);
        __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
        __c0c_emit(cg->out, "L%d:\n", lEnd);
        int rp = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = phi i64 [ %s, %%L%d ], [ %s, %%L%d ]\n",
             rp, tv_r, lT, fv_r, lF);
        char buf[32]; snprintf(buf, sizeof buf, "%%t%d", rp);
        return make_val(buf, default_i64_type());
    }

    case ND_SIZEOF_TYPE: {
        int sz = n->cast_type ? type_size(n->cast_type) : 0;
        char buf[32]; snprintf(buf, sizeof buf, "%d", sz);
        return make_val(buf, default_int_type());
    }

    case ND_SIZEOF_EXPR: {
        int sz = n->children[0]->type ? type_size(n->children[0]->type) : 8;
        char buf[32]; snprintf(buf, sizeof buf, "%d", sz);
        return make_val(buf, default_int_type());
    }

    case ND_COMMA: {
        Val v = make_val("0", default_int_type());
        for (int i = 0; i < n->n_children; i++)
            v = emit_expr(cg, n->children[i]);
        return v;
    }

    case ND_MEMBER:
    case ND_ARROW: {
        Val bv;
        if (n->kind == ND_ARROW) {
            bv = emit_expr(cg, n->children[0]);
        } else {
            char *a = emit_lvalue_addr(cg, n->children[0]);
            if (a) {
                bv = make_val(a, default_ptr_type());
                free(a);
            } else {
                bv = emit_expr(cg, n->children[0]);
            }
        }
        char base_ptr[64];
        if (val_is_ptr(bv)) {
            strncpy(base_ptr, bv.reg, 63); base_ptr[63] = '\0';
        } else {
            int rp = new_reg(cg);
            char promoted[64]; promote_to_i64(cg, bv, promoted, 64);
            __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rp, promoted);
            snprintf(base_ptr, 64, "%%t%d", rp);
        }
        char *memname = n->sval;
        Type *basety = n->children[0] ? n->children[0]->type : NULL;
        long offset = 0;
        if (basety && (basety->kind == TY_STRUCT || basety->kind == TY_UNION) && basety->params) {
            for (int i = 0; i < basety->n_members; i++) {
                if (basety->params[i].name && strcmp(basety->params[i].name, memname) == 0) {
                    break;
                }
                if (basety->members && basety->members[i]) {
                    offset += type_size(basety->members[i]);
                }
            }
        }
        {
            int r = new_reg(cg);
            if (offset > 0) {
                __c0c_emit(cg->out, "  %%t%d = getelementptr i8, ptr %s, i64 %ld\n", r, base_ptr, offset);
            } else {
                __c0c_emit(cg->out, "  %%t%d = getelementptr i8, ptr %s, i64 0\n", r, base_ptr);
            }
            int r2 = new_reg(cg);
            __c0c_emit(cg->out, "  %%t%d = load i64, ptr %%t%d\n", r2, r);
            char buf[32]; snprintf(buf, sizeof buf, "%%t%d", r2);
            return make_val(buf, default_i64_type());
        }
    }

    default:
        __c0c_emit(cg->out, "  ; unhandled expr node %d\n", n->kind);
        return make_val("0", default_int_type());
    }
}

/* ================================================================ Statements */

static void emit_stmt(Codegen *cg, Node *n) {
    if (!n) return;
    switch (n->kind) {

    case ND_BLOCK:
        if (n->ival) scope_push(cg);  /* ival=1 means explicit { } block */
        for (int i = 0; i < n->n_children; i++)
            emit_stmt(cg, n->children[i]);
        if (n->ival) scope_pop(cg);
        break;

    case ND_VAR_DECL: {
        Type *vt = n->var_type ? n->var_type : default_int_type();
        const char *lt;
        Type *stored_vt;
        if (vt->kind == TY_ARRAY && vt->base && (vt->base->kind == TY_STRUCT || vt->base->kind == TY_UNION)) {
            int elem_sz = type_size(vt->base);
            int total_sz = (int)vt->array_size * elem_sz;
            if (total_sz <= 0) total_sz = 8;
            char size_buf[32];
            snprintf(size_buf, sizeof(size_buf), "[%d x i8]", total_sz);
            lt = size_buf;
            stored_vt = default_ptr_type();
        } else if (vt->kind == TY_STRUCT || vt->kind == TY_UNION) {
            int sz = type_size(vt);
            if (sz <= 0) sz = 8;
            char size_buf[32];
            snprintf(size_buf, sizeof(size_buf), "[%d x i8]", sz);
            lt = size_buf;
            stored_vt = vt;
        } else if (vt->kind == TY_PTR || vt->kind == TY_ARRAY) {
            lt = "ptr"; stored_vt = default_ptr_type();
        } else if (type_is_fp(vt)) {
            lt = "double"; stored_vt = default_fp_type();
        } else {
            lt = "i64"; stored_vt = vt;
        }
        int r = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = alloca %s\n", r, lt);
        if (cg->n_locals >= MAX_LOCALS) { fprintf(stderr, "c0c: too many locals\n"); exit(1); }
        Local *l = &cg->locals[cg->n_locals++];
        l->name      = strdup(n->var_name ? n->var_name : "__anon");
        l->llvm_name = malloc(32);
        snprintf(l->llvm_name, 32, "%%t%d", r);
        l->type      = stored_vt;
        l->is_param  = 0;

        if (n->n_children > 0) {
            Val iv = emit_expr(cg, n->children[0]);
            const char *st;
            if (val_is_ptr(iv))       st = "ptr";
            else if (type_is_fp(iv.type)) st = llvm_type(iv.type);
            else                       st = "i64";
            char stored[64];
            if (!val_is_ptr(iv) && !val_is_64bit(iv) && !type_is_fp(iv.type)) {
                int rp = new_reg(cg);
                __c0c_emit(cg->out, "  %%t%d = sext i32 %s to i64\n", rp, iv.reg);
                snprintf(stored, 64, "%%t%d", rp);
            } else {
                strncpy(stored, iv.reg, 63); stored[63] = '\0';
            }
            __c0c_emit(cg->out, "  store %s %s, ptr %%t%d\n", st, stored, r);
        }
        /* handle chained decls */
        for (int i = 1; i < n->n_children; i++)
            emit_stmt(cg, n->children[i]);
        break;
    }

    case ND_EXPR_STMT:
        if (n->n_children > 0) emit_expr(cg, n->children[0]);
        break;

    case ND_RETURN: {
        if (n->ret_val) {
            Val rv = emit_expr(cg, n->ret_val);
            Type *rt = cg->cur_ret_type;
            int rt_is_ptr  = (rt->kind == TY_PTR || rt->kind == TY_ARRAY);
            int rt_is_void = (rt->kind == TY_VOID);
            int rt_is_fp   = type_is_fp(rt);
            const char *ret_type_str = llvm_type(rt);

            if (rt_is_void) {
                __c0c_emit(cg->out, "  ret void\n");
            } else if (rt_is_fp) {
                __c0c_emit(cg->out, "  ret %s %s\n", ret_type_str, rv.reg);
            } else if (rt_is_ptr) {
                /* Return ptr: coerce if needed */
                if (val_is_ptr(rv)) {
                    __c0c_emit(cg->out, "  ret ptr %s\n", rv.reg);
                } else {
                    int rc = new_reg(cg);
                    char pr[64]; promote_to_i64(cg, rv, pr, 64);
                    __c0c_emit(cg->out, "  %%t%d = inttoptr i64 %s to ptr\n", rc, pr);
                    __c0c_emit(cg->out, "  ret ptr %%t%d\n", rc);
                }
            } else {
                /* Return integer: use the declared LLVM return type */
                char pr[64]; promote_to_i64(cg, rv, pr, 64);
                if (strcmp(ret_type_str, "i8") == 0) {
                    int rc = new_reg(cg);
                    __c0c_emit(cg->out, "  %%t%d = trunc i64 %s to i8\n", rc, pr);
                    __c0c_emit(cg->out, "  ret i8 %%t%d\n", rc);
                } else if (strcmp(ret_type_str, "i16") == 0) {
                    int rc = new_reg(cg);
                    __c0c_emit(cg->out, "  %%t%d = trunc i64 %s to i16\n", rc, pr);
                    __c0c_emit(cg->out, "  ret i16 %%t%d\n", rc);
                } else if (strcmp(ret_type_str, "i32") == 0) {
                    int rc = new_reg(cg);
                    __c0c_emit(cg->out, "  %%t%d = trunc i64 %s to i32\n", rc, pr);
                    __c0c_emit(cg->out, "  ret i32 %%t%d\n", rc);
                } else {
                    /* i64 or typedef_ref (mapped to i64) */
                    __c0c_emit(cg->out, "  ret i64 %s\n", pr);
                }
            }
        } else {
            __c0c_emit(cg->out, "  ret void\n");
        }
        int dead = new_label(cg);
        __c0c_emit(cg->out, "L%d:\n", dead);
        break;
    }

    case ND_IF: {
        Val cv    = emit_expr(cg, n->cond);
        int rcond = emit_cond(cg, cv);
        int lT   = new_label(cg);
        int lF   = new_label(cg);
        int lEnd = new_label(cg);
        __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n",
             rcond, lT, n->else_branch ? lF : lEnd);
        __c0c_emit(cg->out, "L%d:\n", lT);
        emit_stmt(cg, n->then_branch);
        __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
        if (n->else_branch) {
            __c0c_emit(cg->out, "L%d:\n", lF);
            emit_stmt(cg, n->else_branch);
            __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
        }
        __c0c_emit(cg->out, "L%d:\n", lEnd);
        break;
    }

    case ND_WHILE: {
        int lCond = new_label(cg);
        int lBody = new_label(cg);
        int lEnd  = new_label(cg);
        char old_break[64], old_cont[64];
        strcpy(old_break, cg->break_label);
        strcpy(old_cont,  cg->cont_label);
        snprintf(cg->break_label, 64, "L%d", lEnd);
        snprintf(cg->cont_label,  64, "L%d", lCond);

        __c0c_emit(cg->out, "  br label %%L%d\n", lCond);
        __c0c_emit(cg->out, "L%d:\n", lCond);
        Val cv    = emit_expr(cg, n->loop_cond);
        int rcond = emit_cond(cg, cv);
        __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        __c0c_emit(cg->out, "L%d:\n", lBody);
        emit_stmt(cg, n->loop_body);
        __c0c_emit(cg->out, "  br label %%L%d\n", lCond);
        __c0c_emit(cg->out, "L%d:\n", lEnd);

        strcpy(cg->break_label, old_break);
        strcpy(cg->cont_label,  old_cont);
        break;
    }

    case ND_DO_WHILE: {
        int lBody = new_label(cg);
        int lCond = new_label(cg);
        int lEnd  = new_label(cg);
        char old_break[64], old_cont[64];
        strcpy(old_break, cg->break_label);
        strcpy(old_cont,  cg->cont_label);
        snprintf(cg->break_label, 64, "L%d", lEnd);
        snprintf(cg->cont_label,  64, "L%d", lCond);

        __c0c_emit(cg->out, "  br label %%L%d\n", lBody);
        __c0c_emit(cg->out, "L%d:\n", lBody);
        emit_stmt(cg, n->loop_body);
        __c0c_emit(cg->out, "  br label %%L%d\n", lCond);
        __c0c_emit(cg->out, "L%d:\n", lCond);
        Val cv    = emit_expr(cg, n->loop_cond);
        int rcond = emit_cond(cg, cv);
        __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        __c0c_emit(cg->out, "L%d:\n", lEnd);

        strcpy(cg->break_label, old_break);
        strcpy(cg->cont_label,  old_cont);
        break;
    }

    case ND_FOR: {
        int lCond = new_label(cg);
        int lBody = new_label(cg);
        int lPost = new_label(cg);
        int lEnd  = new_label(cg);
        char old_break[64], old_cont[64];
        strcpy(old_break, cg->break_label);
        strcpy(old_cont,  cg->cont_label);
        snprintf(cg->break_label, 64, "L%d", lEnd);
        snprintf(cg->cont_label,  64, "L%d", lPost);

        scope_push(cg);
        if (n->for_init) emit_stmt(cg, n->for_init);
        __c0c_emit(cg->out, "  br label %%L%d\n", lCond);
        __c0c_emit(cg->out, "L%d:\n", lCond);
        if (n->for_cond) {
            Val cv    = emit_expr(cg, n->for_cond);
            int rcond = emit_cond(cg, cv);
            __c0c_emit(cg->out, "  br i1 %%t%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        } else {
            __c0c_emit(cg->out, "  br label %%L%d\n", lBody);
        }
        __c0c_emit(cg->out, "L%d:\n", lBody);
        emit_stmt(cg, n->for_body);
        __c0c_emit(cg->out, "  br label %%L%d\n", lPost);
        __c0c_emit(cg->out, "L%d:\n", lPost);
        if (n->for_post) emit_expr(cg, n->for_post);
        __c0c_emit(cg->out, "  br label %%L%d\n", lCond);
        __c0c_emit(cg->out, "L%d:\n", lEnd);
        scope_pop(cg);

        strcpy(cg->break_label, old_break);
        strcpy(cg->cont_label,  old_cont);
        break;
    }

    case ND_BREAK:
        __c0c_emit(cg->out, "  br label %%%s\n", cg->break_label);
        { int dead = new_label(cg); __c0c_emit(cg->out, "L%d:\n", dead); }
        break;

    case ND_CONTINUE:
        __c0c_emit(cg->out, "  br label %%%s\n", cg->cont_label);
        { int dead = new_label(cg); __c0c_emit(cg->out, "L%d:\n", dead); }
        break;

    case ND_SWITCH: {
        /* Emit switch using LLVM 'switch' instruction.
           We do a two-pass: first collect all case values/labels,
           then emit the switch, then emit case bodies. */
        Val sv = emit_expr(cg, n->cond);
        int lEnd = new_label(cg);
        char old_break[64];
        strcpy(old_break, cg->break_label);
        snprintf(cg->break_label, 64, "L%d", lEnd);

        /* collect case labels from the switch body block */
        Node *body = n->loop_body;
        int n_cases = 0;
        int case_labels[256];
        long long case_vals[256];
        int default_label = lEnd;

        /* pre-allocate labels */
        for (int i = 0; i < body->n_children && n_cases < 256; i++) {
            Node *ch = body->children[i];
            if (ch->kind == ND_CASE) {
                case_labels[n_cases] = new_label(cg);
                case_vals[n_cases]   = ch->cond ? ch->cond->ival : 0;
                n_cases++;
            } else if (ch->kind == ND_DEFAULT) {
                default_label = new_label(cg);
            }
        }

        /* promote switch value to i64 */
        char sv_promoted[64];
        promote_to_i64(cg, sv, sv_promoted, 64);
        int rs = new_reg(cg);
        __c0c_emit(cg->out, "  %%t%d = add i64 %s, 0\n", rs, sv_promoted);
        __c0c_emit(cg->out, "  switch i64 %%t%d, label %%L%d [\n", rs, default_label);
        int ci = 0;
        for (int i = 0; i < body->n_children; i++) {
            Node *ch = body->children[i];
            if (ch->kind == ND_CASE && ci < n_cases) {
                __c0c_emit(cg->out, "    i64 %lld, label %%L%d\n", case_vals[ci], case_labels[ci]);
                ci++;
            }
        }
        __c0c_emit(cg->out, "  ]\n");

        /* emit case bodies */
        ci = 0;
        int def_ci = 0;
        for (int i = 0; i < body->n_children; i++) {
            Node *ch = body->children[i];
            if (ch->kind == ND_CASE && ci < n_cases) {
                __c0c_emit(cg->out, "L%d:\n", case_labels[ci++]);
                if (ch->n_children > 0) emit_stmt(cg, ch->children[0]);
                /* fallthrough: jump to next label or end */
                int next = (ci < n_cases) ? case_labels[ci] : lEnd;
                __c0c_emit(cg->out, "  br label %%L%d\n", next);
            } else if (ch->kind == ND_DEFAULT) {
                __c0c_emit(cg->out, "L%d:\n", default_label);
                if (ch->n_children > 0) emit_stmt(cg, ch->children[0]);
                __c0c_emit(cg->out, "  br label %%L%d\n", lEnd);
                def_ci++;
            } else {
                emit_stmt(cg, ch);
            }
        }
        __c0c_emit(cg->out, "L%d:\n", lEnd);
        strcpy(cg->break_label, old_break);
        break;
    }

    case ND_CASE:
        /* handled inside ND_SWITCH; if encountered standalone emit body */
        if (n->n_children > 0) emit_stmt(cg, n->children[0]);
        break;

    case ND_DEFAULT:
        if (n->n_children > 0) emit_stmt(cg, n->children[0]);
        break;

    case ND_LABEL: {
        /* Named label for goto — emit a branch to it first so any preceding
           dead block (from break/goto) has a proper terminator */
        __c0c_emit(cg->out, "  br label %%%s\n", n->sval);
        __c0c_emit(cg->out, "%s:\n", n->sval);
        if (n->n_children > 0) emit_stmt(cg, n->children[0]);
        break;
    }

    case ND_GOTO:
        __c0c_emit(cg->out, "  br label %%%s\n", n->sval);
        { int dead = new_label(cg); __c0c_emit(cg->out, "L%d:\n", dead); }
        break;

    case ND_TYPEDEF:
        /* nothing to emit */
        break;

    default:
        /* try as expression */
        emit_expr(cg, n);
        break;
    }
}

/* ================================================================ Functions */

static void emit_func_def(Codegen *cg, Node *n) {
    Type *ft = n->func_type;
    if (!ft || ft->kind != TY_FUNC) return;

    cg->n_locals = 0;
    cg->reg      = 0;
    cg->label    = 0;
    cg->cur_ret_type = ft->ret ? ft->ret : default_int_type();
    strncpy(cg->cur_func, n->func_name ? n->func_name : "anon", 127);

    /* linkage */
    const char *linkage = n->is_static ? "internal" : "dso_local";

    __c0c_emit(cg->out, "define %s %s @%s(",
         linkage, llvm_ret_type(ft),
         n->func_name ? n->func_name : "anon");

    /* parameters */
    scope_push(cg);
    int emitted_params = 0;
    for (int i = 0; i < ft->param_count; i++) {
        Type *pt_type = ft->params[i].type;
        /* skip void-only parameter list like f(void) */
        if (pt_type && pt_type->kind == TY_VOID && ft->param_count == 1) break;
        if (emitted_params) __c0c_emit(cg->out, ", ");
        /* Use i64 for all integer params, ptr for pointers and structs */
        const char *pt;
        Type *stored_type;
        if (!pt_type || type_is_fp(pt_type)) {
            pt = pt_type ? llvm_type(pt_type) : "i64";
            stored_type = pt_type;
        } else if (pt_type->kind == TY_PTR || pt_type->kind == TY_ARRAY) {
            pt = "ptr"; stored_type = default_ptr_type();
        } else if (pt_type->kind == TY_STRUCT || pt_type->kind == TY_UNION ||
                   pt_type->kind == TY_TYPEDEF_REF) {
            /* Struct/typedef params treated as ptr (our simplified ABI) */
            pt = "ptr"; stored_type = default_ptr_type();
        } else {
            pt = "i64"; stored_type = default_i64_type();
        }
        int pr = new_reg(cg);
        __c0c_emit(cg->out, "%s %%t%d", pt, pr);
        emitted_params++;
        if (n->param_names && n->param_names[i]) {
            if (cg->n_locals >= MAX_LOCALS) { fprintf(stderr, "c0c: too many locals\n"); exit(1); }
            Local *l = &cg->locals[cg->n_locals++];
            l->name      = strdup(n->param_names[i]);
            l->llvm_name = malloc(32);
            snprintf(l->llvm_name, 32, "%%t%d", pr);
            l->type      = stored_type;
            l->is_param  = 1;
        }
    }
    if (ft->variadic) { if (emitted_params) __c0c_emit(cg->out, ", "); __c0c_emit(cg->out, "..."); }
    __c0c_emit(cg->out, ") {\n");
    __c0c_emit(cg->out, "entry:\n");

    /* body */
    emit_stmt(cg, n->loop_body);

    /* implicit return — always needed since last block may have fallen through */
    if (!ft->ret || ft->ret->kind == TY_VOID) {
        __c0c_emit(cg->out, "  ret void\n");
    } else if (ft->ret->kind == TY_PTR || ft->ret->kind == TY_ARRAY) {
        __c0c_emit(cg->out, "  ret ptr null\n");
    } else if (type_is_fp(ft->ret)) {
        __c0c_emit(cg->out, "  ret %s 0.0\n", llvm_type(ft->ret));
    } else {
        /* Integer return: match the declared LLVM return type */
        const char *rts = llvm_type(ft->ret);
        if      (strcmp(rts,"i8")  == 0) __c0c_emit(cg->out, "  ret i8 0\n");
        else if (strcmp(rts,"i16") == 0) __c0c_emit(cg->out, "  ret i16 0\n");
        else if (strcmp(rts,"i32") == 0) __c0c_emit(cg->out, "  ret i32 0\n");
        else                             __c0c_emit(cg->out, "  ret i64 0\n");
    }

    __c0c_emit(cg->out, "}\n\n");
    scope_pop(cg);
}

/* ================================================================ Global variables */

static void emit_global_var(Codegen *cg, Node *n) {
    if (!n->var_name) return;

    Type *vt = n->var_type;

    /* Function-type declarations: emit as 'declare' not global variable */
    if (vt && vt->kind == TY_FUNC) {
        /* Check if already defined (function body present) — if so, skip declare */
        int found = 0;
        for (int i = 0; i < cg->n_globals; i++)
            if (strcmp(cg->globals[i].name, n->var_name) == 0) { found = 1; break; }
        if (found) return;  /* already registered as defined — don't emit declare */
        /* Register as extern */
        if (cg->n_globals < MAX_GLOBALS) {
            cg->globals[cg->n_globals].name      = strdup(n->var_name);
            cg->globals[cg->n_globals].type      = vt;
            cg->globals[cg->n_globals].is_extern = 1;
            cg->n_globals++;
        }
        /* Build param list string — must match define param types */
        char params_buf[512] = {0};
        int pos = 0;
        for (int i = 0; i < vt->param_count && pos < 480; i++) {
            Type *pt = vt->params[i].type;
            if (pt && pt->kind == TY_VOID && vt->param_count == 1) break;
            if (i) pos += snprintf(params_buf + pos, 512 - pos, ", ");
            /* Match the param type rules from emit_func_def */
            const char *pt_str;
            if (!pt || type_is_fp(pt)) {
                pt_str = pt ? llvm_type(pt) : "i64";
            } else if (pt->kind == TY_PTR || pt->kind == TY_ARRAY) {
                pt_str = "ptr";
            } else if (pt->kind == TY_STRUCT || pt->kind == TY_UNION ||
                       pt->kind == TY_TYPEDEF_REF) {
                pt_str = "ptr";
            } else {
                pt_str = "i64";
            }
            pos += snprintf(params_buf + pos, 512 - pos, "%s", pt_str);
        }
        if (vt->variadic) {
            if (vt->param_count) pos += snprintf(params_buf + pos, 512 - pos, ", ");
            pos += snprintf(params_buf + pos, 512 - pos, "...");
        }
        __c0c_emit(cg->out, "declare %s @%s(%s)\n", llvm_ret_type(vt), n->var_name, params_buf);
        return;
    }

    /* register */
    int exists = 0;
    for (int i = 0; i < cg->n_globals; i++)
        if (strcmp(cg->globals[i].name, n->var_name) == 0) { exists = 1; break; }
    if (!exists && cg->n_globals < MAX_GLOBALS) {
        cg->globals[cg->n_globals].name      = strdup(n->var_name);
        cg->globals[cg->n_globals].type      = vt;
        cg->globals[cg->n_globals].is_extern = n->is_extern;
        cg->n_globals++;
    }

    if (n->is_extern) {
        __c0c_emit(cg->out, "@%s = external global %s\n",
             n->var_name, llvm_type(vt));
        return;
    }

    const char *linkage = n->is_static ? "internal" : "dso_local";
    const char *lt = llvm_type(vt);
    __c0c_emit(cg->out, "@%s = %s global %s zeroinitializer\n", n->var_name, linkage, lt);
}

/* ================================================================ Public API */

Codegen *codegen_new(FILE *out, const char *source_filename) {
    Codegen *cg = calloc(1, sizeof(Codegen));
    if (!cg) { perror("calloc"); exit(1); }
    cg->out             = out;
    cg->source_filename = source_filename;
    return cg;
}

void codegen_free(Codegen *cg) {
    for (int i = 0; i < cg->n_globals; i++) free(cg->globals[i].name);
    for (int i = 0; i < cg->n_strings; i++) free(cg->str_literals[i]);
    free(cg);
}

void codegen_emit(Codegen *cg, Node *tu) {
    if (!tu) return;

    /* ---- module header ---- */
    __c0c_emit(cg->out, "; ModuleID = '%s'\n", cg->source_filename);
    __c0c_emit(cg->out, "source_filename = \"%s\"\n", cg->source_filename);
    __c0c_emit(cg->out, "target datalayout = \"e-m:o-i64:64-i128:128-n32:64-S128\"\n");
    __c0c_emit(cg->out, "target triple = \"arm64-apple-macosx15.0.0\"\n\n");

    /* ---- standard library declarations ---- */
    __c0c_emit(cg->out, "; stdlib declarations\n");
    __c0c_emit(cg->out, "declare ptr @malloc(i64)\n");
    __c0c_emit(cg->out, "declare ptr @calloc(i64, i64)\n");
    __c0c_emit(cg->out, "declare ptr @realloc(ptr, i64)\n");
    __c0c_emit(cg->out, "declare void @free(ptr)\n");
    __c0c_emit(cg->out, "declare i64 @strlen(ptr)\n");
    __c0c_emit(cg->out, "declare ptr @strdup(ptr)\n");
    __c0c_emit(cg->out, "declare ptr @strndup(ptr, i64)\n");
    __c0c_emit(cg->out, "declare ptr @strcpy(ptr, ptr)\n");
    __c0c_emit(cg->out, "declare ptr @strncpy(ptr, ptr, i64)\n");
    __c0c_emit(cg->out, "declare ptr @strcat(ptr, ptr)\n");
    __c0c_emit(cg->out, "declare ptr @strchr(ptr, i64)\n");
    __c0c_emit(cg->out, "declare ptr @strstr(ptr, ptr)\n");
    __c0c_emit(cg->out, "declare i32 @strcmp(ptr, ptr)\n");
    __c0c_emit(cg->out, "declare i32 @strncmp(ptr, ptr, i64)\n");
    __c0c_emit(cg->out, "declare ptr @memcpy(ptr, ptr, i64)\n");
    __c0c_emit(cg->out, "declare ptr @memset(ptr, i32, i64)\n");
    __c0c_emit(cg->out, "declare i32 @memcmp(ptr, ptr, i64)\n");
    __c0c_emit(cg->out, "declare i32 @printf(ptr, ...)\n");
    __c0c_emit(cg->out, "declare i32 @fprintf(ptr, ptr, ...)\n");
    __c0c_emit(cg->out, "declare i32 @sprintf(ptr, ptr, ...)\n");
    __c0c_emit(cg->out, "declare i32 @snprintf(ptr, i64, ptr, ...)\n");
    __c0c_emit(cg->out, "declare i32 @vfprintf(ptr, ptr, ptr)\n");
    __c0c_emit(cg->out, "declare i32 @vsnprintf(ptr, i64, ptr, ptr)\n");
    __c0c_emit(cg->out, "declare ptr @fopen(ptr, ptr)\n");
    __c0c_emit(cg->out, "declare i32 @fclose(ptr)\n");
    __c0c_emit(cg->out, "declare i64 @fread(ptr, i64, i64, ptr)\n");
    __c0c_emit(cg->out, "declare i64 @fwrite(ptr, i64, i64, ptr)\n");
    __c0c_emit(cg->out, "declare i32 @fseek(ptr, i64, i32)\n");
    __c0c_emit(cg->out, "declare i64 @ftell(ptr)\n");
    __c0c_emit(cg->out, "declare void @perror(ptr)\n");
    __c0c_emit(cg->out, "declare void @exit(i32)\n");
    __c0c_emit(cg->out, "declare ptr @getenv(ptr)\n");
    __c0c_emit(cg->out, "declare i32 @atoi(ptr)\n");
    __c0c_emit(cg->out, "declare i64 @atol(ptr)\n");
    __c0c_emit(cg->out, "declare i64 @strtol(ptr, ptr, i32)\n");
    __c0c_emit(cg->out, "declare i64 @strtoll(ptr, ptr, i32)\n");
    __c0c_emit(cg->out, "declare double @atof(ptr)\n");
    __c0c_emit(cg->out, "declare i32 @isspace(i32)\n");
    __c0c_emit(cg->out, "declare i32 @isdigit(i32)\n");
    __c0c_emit(cg->out, "declare i32 @isalpha(i32)\n");
    __c0c_emit(cg->out, "declare i32 @isalnum(i32)\n");
    __c0c_emit(cg->out, "declare i32 @isxdigit(i32)\n");
    __c0c_emit(cg->out, "declare i32 @isupper(i32)\n");
    __c0c_emit(cg->out, "declare i32 @islower(i32)\n");
    __c0c_emit(cg->out, "declare i32 @toupper(i32)\n");
    __c0c_emit(cg->out, "declare i32 @tolower(i32)\n");
    __c0c_emit(cg->out, "declare i32 @assert(i32)\n");
    /* c0c_compat.c provides these real functions. Declare them in the IR. */
    __c0c_emit(cg->out, "declare ptr @__c0c_stderr()\n");
    __c0c_emit(cg->out, "declare ptr @__c0c_stdout()\n");
    __c0c_emit(cg->out, "declare ptr @__c0c_stdin()\n");
    __c0c_emit(cg->out, "declare ptr @__c0c_get_tbuf(i32)\n");
    __c0c_emit(cg->out, "declare ptr @__c0c_get_td_name(i64)\n");
    __c0c_emit(cg->out, "declare i64 @__c0c_get_td_kind(i64)\n");
    __c0c_emit(cg->out, "declare void @__c0c_emit(ptr, ptr, ...)\n");
    __c0c_emit(cg->out, "\n");

    /* Register stdlib in globals table so calls don't re-declare */
    const char *stdlib_names[] = {
        "malloc","calloc","realloc","free","strlen","strdup","strndup",
        "strcpy","strncpy","strcat","strchr","strstr","strcmp","strncmp",
        "memcpy","memset","memcmp","printf","fprintf","sprintf","snprintf","vfprintf","vsnprintf",
        "fopen","fclose","fread","fwrite","fseek","ftell","perror","exit",
        "getenv","atoi","atol","strtol","strtoll","atof",
        "isspace","isdigit","isalpha","isalnum","isxdigit","isupper","islower",
        "toupper","tolower","assert",
        "va_start","va_end","va_copy",
        "__c0c_va_start","__c0c_va_end","__c0c_va_copy",
        "__c0c_stderr","__c0c_stdout","__c0c_stdin",
        "__c0c_emit","__c0c_get_tbuf",
        "__c0c_get_td_name","__c0c_get_td_kind",
        "stderr","stdout","stdin",
        NULL
    };
    for (int si = 0; stdlib_names[si]; si++) {
        int found = 0;
        for (int j = 0; j < cg->n_globals; j++)
            if (strcmp(cg->globals[j].name, stdlib_names[si]) == 0) { found = 1; break; }
        if (!found && cg->n_globals < MAX_GLOBALS) {
            cg->globals[cg->n_globals].name = strdup(stdlib_names[si]);
            cg->globals[cg->n_globals].type = NULL;
            cg->globals[cg->n_globals].is_extern = 1;
            cg->n_globals++;
        }
    }

    /* ---- pass 1: collect extern/function declarations and globals ---- */
    for (int i = 0; i < tu->n_children; i++) {
        Node *child = tu->children[i];
        if (!child) continue;
        if (child->kind == ND_FUNC_DEF) {
            /* register in globals table */
            int found = 0;
            for (int j = 0; j < cg->n_globals; j++)
                if (strcmp(cg->globals[j].name, child->func_name ? child->func_name : "") == 0)
                    { found = 1; break; }
            if (!found && cg->n_globals < MAX_GLOBALS) {
                cg->globals[cg->n_globals].name = strdup(child->func_name ? child->func_name : "__anon");
                cg->globals[cg->n_globals].type = child->func_type;
                cg->globals[cg->n_globals].is_extern = 0;
                cg->n_globals++;
            }
        }
    }

    /* ---- pass 2: emit global variables ---- */
    for (int i = 0; i < tu->n_children; i++) {
        Node *child = tu->children[i];
        if (!child) continue;
        if (child->kind == ND_VAR_DECL)
            emit_global_var(cg, child);
    }
    __c0c_emit(cg->out, "\n");

    /* ---- pass 3: emit function definitions ---- */
    for (int i = 0; i < tu->n_children; i++) {
        Node *child = tu->children[i];
        if (!child) continue;
        if (child->kind == ND_FUNC_DEF)
            emit_func_def(cg, child);
    }

    /* ---- pass 4: emit string literals ---- */
    for (int i = 0; i < cg->n_strings; i++) {
        int slen = str_literal_len(cg->str_literals[i]);
        __c0c_emit(cg->out, "@.str%d = private unnamed_addr constant [%d x i8] c\"",
             cg->str_ids[i], slen);
        emit_str_content(cg, cg->str_literals[i]);
        __c0c_emit(cg->out, "\"\n");
    }
}
