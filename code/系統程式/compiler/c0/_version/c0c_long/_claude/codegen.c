#include "codegen.h"
#include "ast.h"
#include "lexer.h"   /* for operator token types */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

/* ================================================================
   LLVM IR Code Generator
   Strategy:
   - Every expression returns a "Value" (SSA register name or constant string).
   - Local variables are alloca'd; loads/stores are explicit.
   - We use a simple counter for %tmp registers and labels.
   ================================================================ */


/* ---------------------------------------------------------------- helpers */

static int new_reg(Codegen *cg)   { return cg->reg++; }
static int new_label(Codegen *cg) { return cg->label++; }

static void emit(Codegen *cg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(cg->out, fmt, ap);
    va_end(ap);
}

/* Convert our Type to LLVM IR type string (static buffer rotation) */
#define N_TBUFS 8
static char tbufs[N_TBUFS][256];
static int  tbuf_idx = 0;

static const char *llvm_type(const Type *t) {
    char *buf = tbufs[tbuf_idx++ % N_TBUFS];
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
    assert(cg->n_locals < MAX_LOCALS);
    Local *l  = &cg->locals[cg->n_locals++];
    l->name   = strdup(name);
    int rid   = new_reg(cg);
    l->llvm_name = malloc(32);
    snprintf(l->llvm_name, 32, "%%%d", rid);
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
            case 'n': emit(cg, "\\0A"); break;
            case 't': emit(cg, "\\09"); break;
            case 'r': emit(cg, "\\0D"); break;
            case '0': emit(cg, "\\00"); break;
            case '"': emit(cg, "\\22"); break;
            case '\\': emit(cg, "\\5C"); break;
            default:  emit(cg, "\\%02X", (unsigned char)*p); break;
            }
            p++;
        } else {
            if (*p == '"') break;
            emit(cg, "%c", *p++);
        }
    }
    emit(cg, "\\00"); /* NUL terminator */
}

/* ---------------------------------------------------------------- forward decl */

typedef struct { char reg[64]; Type *type; } Val;

static Val emit_expr(Codegen *cg, Node *n);
static void emit_stmt(Codegen *cg, Node *n);
static void emit_func_def(Codegen *cg, Node *n);
static void emit_global_var(Codegen *cg, Node *n);

/* ================================================================ Expressions */

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
        return strdup(v.reg);
    }
    if (n->kind == ND_INDEX) {
        Val base_v = emit_expr(cg, n->children[0]);
        Val idx_v  = emit_expr(cg, n->children[1]);
        int r = new_reg(cg);
        Type *elem = (n->children[0]->type && n->children[0]->type->base)
                      ? n->children[0]->type->base : NULL;
        const char *et = elem ? llvm_type(elem) : "i8";
        emit(cg, "  %%%d = getelementptr %s, ptr %s, i64 %s\n",
             r, et, base_v.reg, idx_v.reg);
        char *buf = malloc(32);
        snprintf(buf, 32, "%%%d", r);
        return buf;
    }
    if (n->kind == ND_MEMBER || n->kind == ND_ARROW) {
        Val base_v;
        if (n->kind == ND_ARROW) base_v = emit_expr(cg, n->children[0]);
        else {
            char *addr = emit_lvalue_addr(cg, n->children[0]);
            base_v = make_val(addr, NULL);
            free(addr);
        }
        /* We don't have full struct layout, use i8 GEP offset 0 as placeholder */
        char *buf = malloc(64);
        snprintf(buf, 64, "%s", base_v.reg);
        return buf;
    }
    return NULL;
}

static Type *default_int_type(void) {
    static Type t_int = { TY_INT, 0, 0, NULL, -1, NULL, NULL, 0, 0, NULL, NULL };
    return &t_int;
}
static Type *default_ptr_type(void) {
    static Type t_ptr  = { TY_PTR, 0, 0, NULL, -1, NULL, NULL, 0, 0, NULL, NULL };
    return &t_ptr;
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
        emit(cg, "  %%%d = fadd double 0.0, %g\n", r, n->fval);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        static Type t_double = { TY_DOUBLE, 0, 0, NULL, -1, NULL, NULL, 0, 0, NULL, NULL };
        return make_val(buf, &t_double);
    }

    case ND_CHAR_LIT: {
        char buf[32];
        snprintf(buf, sizeof buf, "%lld", n->ival);
        static Type t_char = { TY_CHAR, 0, 0, NULL, -1, NULL, NULL, 0, 0, NULL, NULL };
        return make_val(buf, &t_char);
    }

    case ND_STRING_LIT: {
        int sid = intern_string(cg, n->sval);
        int r   = new_reg(cg);
        int slen = str_literal_len(n->sval);
        emit(cg, "  %%%d = getelementptr [%d x i8], ptr @.str%d, i64 0, i64 0\n",
             r, slen, sid);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, default_ptr_type());
    }

    case ND_IDENT: {
        Local *l = find_local(cg, n->sval);
        if (l) {
            if (l->is_param) return make_val(l->llvm_name, l->type);
            int r = new_reg(cg);
            emit(cg, "  %%%d = load %s, ptr %s\n",
                 r, llvm_type(l->type), l->llvm_name);
            char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
            return make_val(buf, l->type);
        }
        Global *g = find_global(cg, n->sval);
        if (g && g->type && g->type->kind != TY_FUNC) {
            int r = new_reg(cg);
            emit(cg, "  %%%d = load %s, ptr @%s\n",
                 r, llvm_type(g->type), n->sval);
            char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
            return make_val(buf, g->type);
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
        char **arg_regs  = malloc(n->n_children * sizeof *arg_regs);
        Type **arg_types = malloc(n->n_children * sizeof *arg_types);
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
        } else {
            Val cv = emit_expr(cg, callee);
            strncpy(callee_buf, cv.reg, sizeof callee_buf - 1);
        }

        int r = new_reg(cg);
        const char *rt = llvm_type(ret_type);
        int is_void = (ret_type->kind == TY_VOID);

        if (is_void) emit(cg, "  call void %s(", callee_buf);
        else         emit(cg, "  %%%d = call %s %s(", r, rt, callee_buf);

        for (int i = 1; i < n->n_children; i++) {
            if (i > 1) emit(cg, ", ");
            emit(cg, "%s %s",
                 arg_types[i] ? llvm_type(arg_types[i]) : "i64",
                 arg_regs[i]);
            free(arg_regs[i]);
        }
        emit(cg, ")\n");

        for (int i = 1; i < n->n_children; i++) free(arg_regs[i]);
        free(arg_regs); free(arg_types);

        if (is_void) return make_val("0", ret_type);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, ret_type);
    }

    case ND_BINOP: {
        Val lv = emit_expr(cg, n->children[0]);
        Val rv = emit_expr(cg, n->children[1]);
        int r  = new_reg(cg);
        int fp = type_is_fp(lv.type) || type_is_fp(rv.type);
        int is_ptr = (lv.type && (lv.type->kind == TY_PTR || lv.type->kind == TY_ARRAY));
        const char *lt = fp ? llvm_type(lv.type) : (is_ptr ? "ptr" : "i64");

        /* Promote operands to i64 if integer */
        char lreg[64], rreg[64];
        strncpy(lreg, lv.reg, 63); strncpy(rreg, rv.reg, 63);
        if (!fp && !is_ptr) {
            /* sign-extend to i64 */
            int rL = new_reg(cg), rR = new_reg(cg);
            emit(cg, "  %%%d = sext i32 %s to i64\n", rL, lv.reg);
            emit(cg, "  %%%d = sext i32 %s to i64\n", rR, rv.reg);
            snprintf(lreg, 64, "%%%d", rL);
            snprintf(rreg, 64, "%%%d", rR);
            lt = "i64";
        }

        const char *op = NULL;
        int is_cmp = 0;
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
        case TOK_AND: {
            /* short-circuit: convert bools */
            int rA = new_reg(cg), rB = new_reg(cg), rC = new_reg(cg);
            emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rA, lreg);
            emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rB, rreg);
            emit(cg, "  %%%d = and i1 %%%d, %%%d\n", rC, rA, rB);
            int rZ = new_reg(cg);
            emit(cg, "  %%%d = zext i1 %%%d to i64\n", rZ, rC);
            char buf[32]; snprintf(buf, sizeof buf, "%%%d", rZ);
            return make_val(buf, default_int_type());
        }
        case TOK_OR: {
            int rA = new_reg(cg), rB = new_reg(cg), rC = new_reg(cg);
            emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rA, lreg);
            emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rB, rreg);
            emit(cg, "  %%%d = or i1 %%%d, %%%d\n", rC, rA, rB);
            int rZ = new_reg(cg);
            emit(cg, "  %%%d = zext i1 %%%d to i64\n", rZ, rC);
            char buf[32]; snprintf(buf, sizeof buf, "%%%d", rZ);
            return make_val(buf, default_int_type());
        }
        default:
            op = "add"; /* fallback */
        }

        if (n->op == TOK_PLUS && is_ptr) {
            emit(cg, "  %%%d = getelementptr i8, ptr %s, i64 %s\n", r, lreg, rreg);
        } else if (is_cmp) {
            emit(cg, "  %%%d = %s %s %s, %s\n", r, op, lt, lreg, rreg);
            int rZ = new_reg(cg);
            emit(cg, "  %%%d = zext i1 %%%d to i64\n", rZ, r);
            char buf[32]; snprintf(buf, sizeof buf, "%%%d", rZ);
            return make_val(buf, default_int_type());
        } else {
            emit(cg, "  %%%d = %s %s %s, %s\n", r, op, lt, lreg, rreg);
        }

        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, lv.type ? lv.type : default_int_type());
    }

    case ND_UNOP: {
        Val v = emit_expr(cg, n->children[0]);
        int r = new_reg(cg);
        int fp = type_is_fp(v.type);
        switch (n->op) {
        case TOK_MINUS:
            if (fp) emit(cg, "  %%%d = fneg double %s\n", r, v.reg);
            else    emit(cg, "  %%%d = sub i64 0, %s\n", r, v.reg);
            break;
        case TOK_BANG: {
            int rc = new_reg(cg);
            emit(cg, "  %%%d = icmp eq i64 %s, 0\n", rc, v.reg);
            emit(cg, "  %%%d = zext i1 %%%d to i64\n", r, rc);
            break;
        }
        case TOK_TILDE:
            emit(cg, "  %%%d = xor i64 %s, -1\n", r, v.reg);
            break;
        case TOK_PLUS:
            return v;
        default:
            emit(cg, "  %%%d = add i64 %s, 0\n", r, v.reg);
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, v.type);
    }

    case ND_ASSIGN: {
        Val rv_val = emit_expr(cg, n->children[1]);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) {
            /* determine store type */
            Type *lt_type = rv_val.type ? rv_val.type : default_int_type();
            emit(cg, "  store %s %s, ptr %s\n",
                 llvm_type(lt_type), rv_val.reg, addr);
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
        /* promote */
        char lr[64], rr[64];
        strncpy(lr, lv_val.reg, 63); strncpy(rr, rv_val.reg, 63);
        if (!fp) {
            int rL = new_reg(cg), rR = new_reg(cg);
            emit(cg, "  %%%d = sext i32 %s to i64\n", rL, lv_val.reg);
            emit(cg, "  %%%d = sext i32 %s to i64\n", rR, rv_val.reg);
            snprintf(lr, 64, "%%%d", rL);
            snprintf(rr, 64, "%%%d", rR);
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
        emit(cg, "  %%%d = %s %s %s, %s\n", r, op2, it, lr, rr);

        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) {
            emit(cg, "  store %s %%%d, ptr %s\n", it, r, addr);
            free(addr);
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, lv_val.type);
    }

    case ND_PRE_INC:
    case ND_PRE_DEC: {
        Val v     = emit_expr(cg, n->children[0]);
        int r     = new_reg(cg);
        const char *op3 = (n->kind == ND_PRE_INC) ? "add" : "sub";
        int rE = new_reg(cg);
        emit(cg, "  %%%d = sext i32 %s to i64\n", rE, v.reg);
        emit(cg, "  %%%d = %s i64 %%%d, 1\n", r, op3, rE);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) { emit(cg, "  store i64 %%%d, ptr %s\n", r, addr); free(addr); }
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, v.type);
    }

    case ND_POST_INC:
    case ND_POST_DEC: {
        Val v     = emit_expr(cg, n->children[0]);
        int r     = new_reg(cg);
        const char *op4 = (n->kind == ND_POST_INC) ? "add" : "sub";
        int rE = new_reg(cg);
        emit(cg, "  %%%d = sext i32 %s to i64\n", rE, v.reg);
        emit(cg, "  %%%d = %s i64 %%%d, 1\n", r, op4, rE);
        char *addr = emit_lvalue_addr(cg, n->children[0]);
        if (addr) { emit(cg, "  store i64 %%%d, ptr %s\n", r, addr); free(addr); }
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
        Type *base = (pv.type && pv.type->base) ? pv.type->base : default_int_type();
        emit(cg, "  %%%d = load %s, ptr %s\n", r, llvm_type(base), pv.reg);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, base);
    }

    case ND_INDEX: {
        Val base_v = emit_expr(cg, n->children[0]);
        Val idx_v  = emit_expr(cg, n->children[1]);
        Type *elem = (base_v.type && base_v.type->base) ? base_v.type->base : default_int_type();
        int rG = new_reg(cg);
        emit(cg, "  %%%d = getelementptr %s, ptr %s, i64 %s\n",
             rG, llvm_type(elem), base_v.reg, idx_v.reg);
        int rL = new_reg(cg);
        emit(cg, "  %%%d = load %s, ptr %%%d\n", rL, llvm_type(elem), rG);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", rL);
        return make_val(buf, elem);
    }

    case ND_CAST: {
        Val v  = emit_expr(cg, n->cast_expr);
        Type *dst = n->cast_type;
        if (!dst) return v;
        int r  = new_reg(cg);
        int fp_src = type_is_fp(v.type);
        int fp_dst = type_is_fp(dst);
        int sz_src = type_size(v.type ? v.type : default_int_type());
        int sz_dst = type_size(dst);
        if (fp_src && fp_dst) {
            if (sz_dst > sz_src) emit(cg, "  %%%d = fpext float %s to double\n", r, v.reg);
            else emit(cg, "  %%%d = fptrunc double %s to float\n", r, v.reg);
        } else if (fp_src) {
            emit(cg, "  %%%d = fptosi %s %s to %s\n", r, llvm_type(v.type), v.reg, llvm_type(dst));
        } else if (fp_dst) {
            emit(cg, "  %%%d = sitofp %s %s to %s\n", r, llvm_type(v.type), v.reg, llvm_type(dst));
        } else if (dst->kind == TY_PTR || dst->kind == TY_ARRAY) {
            emit(cg, "  %%%d = inttoptr i64 %s to ptr\n", r, v.reg);
        } else if (v.type && (v.type->kind == TY_PTR || v.type->kind == TY_ARRAY)) {
            emit(cg, "  %%%d = ptrtoint ptr %s to i64\n", r, v.reg);
        } else {
            if (sz_dst > sz_src)
                emit(cg, "  %%%d = sext %s %s to %s\n", r, llvm_type(v.type), v.reg, llvm_type(dst));
            else if (sz_dst < sz_src)
                emit(cg, "  %%%d = trunc %s %s to %s\n", r, llvm_type(v.type), v.reg, llvm_type(dst));
            else
                emit(cg, "  %%%d = bitcast %s %s to %s\n", r, llvm_type(v.type), v.reg, llvm_type(dst));
        }
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, dst);
    }

    case ND_TERNARY: {
        Val cv     = emit_expr(cg, n->cond);
        int lT     = new_label(cg);
        int lF     = new_label(cg);
        int lEnd   = new_label(cg);
        int rcond  = new_reg(cg);
        emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rcond, cv.reg);
        emit(cg, "  br i1 %%%d, label %%L%d, label %%L%d\n", rcond, lT, lF);
        emit(cg, "L%d:\n", lT);
        Val tv = emit_expr(cg, n->children[0]);
        emit(cg, "  br label %%L%d\n", lEnd);
        emit(cg, "L%d:\n", lF);
        Val fv = emit_expr(cg, n->children[1]);
        emit(cg, "  br label %%L%d\n", lEnd);
        emit(cg, "L%d:\n", lEnd);
        int rp = new_reg(cg);
        emit(cg, "  %%%d = phi i64 [ %s, %%L%d ], [ %s, %%L%d ]\n",
             rp, tv.reg, lT, fv.reg, lF);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", rp);
        return make_val(buf, tv.type);
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
        /* very simplified: just load from base ptr */
        Val bv = (n->kind == ND_ARROW)
                  ? emit_expr(cg, n->children[0])
                  : ({ char *a = emit_lvalue_addr(cg, n->children[0]); Val v2 = make_val(a, NULL); free(a); v2; });
        int r = new_reg(cg);
        emit(cg, "  %%%d = load i64, ptr %s\n", r, bv.reg);
        char buf[32]; snprintf(buf, sizeof buf, "%%%d", r);
        return make_val(buf, default_int_type());
    }

    default:
        emit(cg, "  ; unhandled expr node %d\n", n->kind);
        return make_val("0", default_int_type());
    }
}

/* ================================================================ Statements */

static void emit_stmt(Codegen *cg, Node *n) {
    if (!n) return;
    switch (n->kind) {

    case ND_BLOCK:
        scope_push(cg);
        for (int i = 0; i < n->n_children; i++)
            emit_stmt(cg, n->children[i]);
        scope_pop(cg);
        break;

    case ND_VAR_DECL: {
        Type *vt = n->var_type ? n->var_type : default_int_type();
        const char *lt = llvm_type(vt);
        int r = new_reg(cg);
        emit(cg, "  %%%d = alloca %s\n", r, lt);
        Local *l = add_local(cg, n->var_name ? n->var_name : "__anon", vt, 0);
        /* Fix up the llvm_name to the alloca register */
        free(l->llvm_name);
        l->llvm_name = malloc(32);
        snprintf(l->llvm_name, 32, "%%%d", r);

        if (n->n_children > 0) {
            Val iv = emit_expr(cg, n->children[0]);
            emit(cg, "  store %s %s, ptr %%%d\n", lt, iv.reg, r);
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
            emit(cg, "  ret %s %s\n",
                 llvm_type(cg->cur_ret_type), rv.reg);
        } else {
            emit(cg, "  ret void\n");
        }
        /* unreachable block */
        int dead = new_label(cg);
        emit(cg, "L%d:\n", dead);
        break;
    }

    case ND_IF: {
        Val cv    = emit_expr(cg, n->cond);
        int rcond = new_reg(cg);
        emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rcond, cv.reg);
        int lT   = new_label(cg);
        int lF   = new_label(cg);
        int lEnd = new_label(cg);
        emit(cg, "  br i1 %%%d, label %%L%d, label %%L%d\n",
             rcond, lT, n->else_branch ? lF : lEnd);
        emit(cg, "L%d:\n", lT);
        emit_stmt(cg, n->then_branch);
        emit(cg, "  br label %%L%d\n", lEnd);
        if (n->else_branch) {
            emit(cg, "L%d:\n", lF);
            emit_stmt(cg, n->else_branch);
            emit(cg, "  br label %%L%d\n", lEnd);
        }
        emit(cg, "L%d:\n", lEnd);
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

        emit(cg, "  br label %%L%d\n", lCond);
        emit(cg, "L%d:\n", lCond);
        Val cv    = emit_expr(cg, n->loop_cond);
        int rcond = new_reg(cg);
        emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rcond, cv.reg);
        emit(cg, "  br i1 %%%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        emit(cg, "L%d:\n", lBody);
        emit_stmt(cg, n->loop_body);
        emit(cg, "  br label %%L%d\n", lCond);
        emit(cg, "L%d:\n", lEnd);

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

        emit(cg, "  br label %%L%d\n", lBody);
        emit(cg, "L%d:\n", lBody);
        emit_stmt(cg, n->loop_body);
        emit(cg, "  br label %%L%d\n", lCond);
        emit(cg, "L%d:\n", lCond);
        Val cv    = emit_expr(cg, n->loop_cond);
        int rcond = new_reg(cg);
        emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rcond, cv.reg);
        emit(cg, "  br i1 %%%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        emit(cg, "L%d:\n", lEnd);

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
        emit(cg, "  br label %%L%d\n", lCond);
        emit(cg, "L%d:\n", lCond);
        if (n->for_cond) {
            Val cv    = emit_expr(cg, n->for_cond);
            int rcond = new_reg(cg);
            emit(cg, "  %%%d = icmp ne i64 %s, 0\n", rcond, cv.reg);
            emit(cg, "  br i1 %%%d, label %%L%d, label %%L%d\n", rcond, lBody, lEnd);
        } else {
            emit(cg, "  br label %%L%d\n", lBody);
        }
        emit(cg, "L%d:\n", lBody);
        emit_stmt(cg, n->for_body);
        emit(cg, "  br label %%L%d\n", lPost);
        emit(cg, "L%d:\n", lPost);
        if (n->for_post) emit_expr(cg, n->for_post);
        emit(cg, "  br label %%L%d\n", lCond);
        emit(cg, "L%d:\n", lEnd);
        scope_pop(cg);

        strcpy(cg->break_label, old_break);
        strcpy(cg->cont_label,  old_cont);
        break;
    }

    case ND_BREAK:
        emit(cg, "  br label %%%s\n", cg->break_label);
        { int dead = new_label(cg); emit(cg, "L%d:\n", dead); }
        break;

    case ND_CONTINUE:
        emit(cg, "  br label %%%s\n", cg->cont_label);
        { int dead = new_label(cg); emit(cg, "L%d:\n", dead); }
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

    emit(cg, "define %s %s @%s(",
         linkage, llvm_ret_type(ft),
         n->func_name ? n->func_name : "anon");

    /* parameters */
    scope_push(cg);
    for (int i = 0; i < ft->param_count; i++) {
        if (i) emit(cg, ", ");
        const char *pt = llvm_type(ft->params[i].type);
        int pr = new_reg(cg);
        emit(cg, "%s %%%d", pt, pr);
        /* register param as a value (no alloca needed for simple use) */
        if (n->param_names && n->param_names[i]) {
            Local *l = add_local(cg, n->param_names[i], ft->params[i].type, 1);
            free(l->llvm_name);
            l->llvm_name = malloc(32);
            snprintf(l->llvm_name, 32, "%%%d", pr);
        }
    }
    if (ft->variadic) { if (ft->param_count) emit(cg, ", "); emit(cg, "..."); }
    emit(cg, ") {\n");
    emit(cg, "entry:\n");

    /* body */
    emit_stmt(cg, n->loop_body);

    /* implicit return */
    if (ft->ret && ft->ret->kind == TY_VOID)
        emit(cg, "  ret void\n");
    else
        emit(cg, "  ret %s 0\n", llvm_ret_type(ft));

    emit(cg, "}\n\n");
    scope_pop(cg);
}

/* ================================================================ Global variables */

static void emit_global_var(Codegen *cg, Node *n) {
    if (!n->var_name) return;

    /* register */
    int exists = 0;
    for (int i = 0; i < cg->n_globals; i++)
        if (strcmp(cg->globals[i].name, n->var_name) == 0) { exists = 1; break; }
    if (!exists && cg->n_globals < MAX_GLOBALS) {
        cg->globals[cg->n_globals].name      = strdup(n->var_name);
        cg->globals[cg->n_globals].type      = n->var_type;
        cg->globals[cg->n_globals].is_extern = n->is_extern;
        cg->n_globals++;
    }

    if (n->is_extern) {
        emit(cg, "@%s = external global %s\n",
             n->var_name, llvm_type(n->var_type));
        return;
    }

    const char *linkage = n->is_static ? "internal" : "dso_local";
    const char *lt = llvm_type(n->var_type);
    /* default zeroinitializer */
    emit(cg, "@%s = %s global %s zeroinitializer\n", n->var_name, linkage, lt);
}

/* ================================================================ Public API */

Codegen *codegen_new(FILE *out, const char *source_filename) {
    Codegen *cg = calloc(1, sizeof *cg);
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
    emit(cg, "; ModuleID = '%s'\n", cg->source_filename);
    emit(cg, "source_filename = \"%s\"\n", cg->source_filename);
    emit(cg, "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n");
    emit(cg, "target triple = \"x86_64-unknown-linux-gnu\"\n\n");

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
    emit(cg, "\n");

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
        emit(cg, "@.str%d = private unnamed_addr constant [%d x i8] c\"",
             cg->str_ids[i], slen);
        emit_str_content(cg, cg->str_literals[i]);
        emit(cg, "\"\n");
    }
}