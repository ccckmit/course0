#include "codegen.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    VT_I1, VT_I8, VT_I16, VT_I32, VT_I64, VT_F32, VT_F64, VT_PTR
} ValueType;

typedef struct {
    char *reg; ValueType vt; CType cty;
} Value;

typedef struct {
    char name[64]; CType ret; int ret_struct_id; CType params[16]; int param_struct_id[16]; int param_cnt; int is_vararg;
} FuncSig;

static int reg_cnt = 0, label_cnt = 0, string_cnt = 0;
static char string_table[128][512];
static ASTNode *current_params = NULL;
static CType current_ret_ty = TY_INT;
static int break_label_stack[128], continue_label_stack[128], loop_depth = 0;
static int switch_break_stack[128], switch_depth = 0;
static FILE *out_fp = NULL;
static FuncSig func_sigs[128];
static int func_sig_cnt = 0, var_id = 0;

typedef struct { char name[64], ir[64]; CType ty; int array_len, struct_id, is_global; } VarSlot;
static VarSlot var_slots[512];
static int var_cnt = 0;
static VarSlot global_slots[256];
static int global_cnt = 0;
static int scope_marks[128], scope_depth = 0;

static int add_string_literal(const char *s) {
    if (string_cnt >= 128) error("字串常數過多");
    strcpy(string_table[string_cnt], s); return string_cnt++;
}

static void build_llvm_string(const char *src, char *dst, int *out_len) {
    int out = 0, len = 0; const unsigned char *p = (const unsigned char*)src;
    while (*p) {
        unsigned char c = *p++;
        if (c == '\n') out += sprintf(dst + out, "\\0A");
        else if (c == '\t') out += sprintf(dst + out, "\\09");
        else if (c == '\r') out += sprintf(dst + out, "\\0D");
        else if (c == '\\') out += sprintf(dst + out, "\\5C");
        else if (c == '\"') out += sprintf(dst + out, "\\22");
        else if (c < 32 || c >= 127) out += sprintf(dst + out, "\\%02X", c);
        else { dst[out++] = (char)c; dst[out] = '\0'; }
        len++;
    }
    strcat(dst, "\\00"); len++; if (out_len) *out_len = len;
}

static int is_ptr(CType ty) {
    return ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
           ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
           ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR;
}
static int is_float(CType ty) { return ty == TY_FLOAT || ty == TY_DOUBLE; }

static int is_unsigned(CType ty) { return ty == TY_UCHAR || ty == TY_USHORT || ty == TY_UINT || ty == TY_ULONG; }

static int int_rank(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return 1; if (ty == TY_SHORT || ty == TY_USHORT) return 2;
    if (ty == TY_INT || ty == TY_UINT) return 3; if (ty == TY_LONG || ty == TY_ULONG) return 4; return 0;
}

static CType int_promote(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR || ty == TY_SHORT || ty == TY_USHORT) return TY_INT; return ty;
}

static CType int_type_from_rank(int rank, int is_uns) {
    if (rank == 1) return is_uns ? TY_UCHAR : TY_CHAR; if (rank == 2) return is_uns ? TY_USHORT : TY_SHORT;
    if (rank == 3) return is_uns ? TY_UINT : TY_INT; if (rank == 4) return is_uns ? TY_ULONG : TY_LONG; return TY_INT;
}

static CType common_arith_type(CType a, CType b) {
    if (is_float(a) || is_float(b)) return (a == TY_DOUBLE || b == TY_DOUBLE) ? TY_DOUBLE : TY_FLOAT;
    a = int_promote(a); b = int_promote(b);
    int ra = int_rank(a), rb = int_rank(b), ua = is_unsigned(a), ub = is_unsigned(b);
    if (ra == rb) return int_type_from_rank(ra, ua || ub);
    if (ra > rb) return int_type_from_rank(ra, ua ? 1 : 0);
    return int_type_from_rank(rb, ub ? 1 : 0);
}

static CType base_of(CType ty) {
    if (ty == TY_CHAR_PTR) return TY_CHAR; if (ty == TY_UCHAR_PTR) return TY_UCHAR;
    if (ty == TY_SHORT_PTR) return TY_SHORT; if (ty == TY_USHORT_PTR) return TY_USHORT;
    if (ty == TY_INT_PTR) return TY_INT; if (ty == TY_UINT_PTR) return TY_UINT;
    if (ty == TY_LONG_PTR) return TY_LONG; if (ty == TY_ULONG_PTR) return TY_ULONG;
    if (ty == TY_FLOAT_PTR) return TY_FLOAT; if (ty == TY_DOUBLE_PTR) return TY_DOUBLE;
    if (ty == TY_STRUCT_PTR) return TY_STRUCT; return TY_INT;
}
static const char* llvm_type(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return "i8"; if (ty == TY_SHORT || ty == TY_USHORT) return "i16";
    if (ty == TY_INT || ty == TY_UINT) return "i32"; if (ty == TY_LONG || ty == TY_ULONG) return "i64";
    if (ty == TY_FLOAT) return "float"; if (ty == TY_DOUBLE) return "double";
    if (ty == TY_VOID) return "void"; return "ptr";
}
static const char* llvm_elem_type(CType ty) {
    CType base = is_ptr(ty) ? base_of(ty) : ty;
    if (base == TY_CHAR || base == TY_UCHAR) return "i8"; if (base == TY_SHORT || base == TY_USHORT) return "i16";
    if (base == TY_INT || base == TY_UINT) return "i32"; if (base == TY_LONG || base == TY_ULONG) return "i64";
    if (base == TY_FLOAT) return "float"; if (base == TY_DOUBLE) return "double"; return "i8";
}

static Value gen_cond(ASTNode *node); static Value gen_expr(ASTNode *node); static Value gen_lvalue_addr(ASTNode *node);
static int struct_size(int struct_id); static void global_add(const char *name, CType ty, int array_len, int struct_id);
static int is_zero_literal(ASTNode *n) { return n && n->type == AST_NUM && n->val == 0; }
static void emit_int_const(CType ty, long long v) { fprintf(out_fp, "%s %lld", llvm_type(ty), v); }
static void emit_float_const(CType ty, double v) { fprintf(out_fp, "%s %.17g", (ty == TY_DOUBLE) ? "double" : "float", v); }

static void emit_global_scalar(ASTNode *g) {
    if (g->ty == TY_STRUCT) { fprintf(out_fp, "@%s = global[%d x i8] zeroinitializer\n", g->name, struct_size(g->struct_id)); return; }
    fprintf(out_fp, "@%s = global %s ", g->name, llvm_type(g->ty));
    if (g->init_kind == 0) {
        if (is_ptr(g->ty)) fprintf(out_fp, "null\n"); else if (is_float(g->ty)) fprintf(out_fp, "0.0\n"); else fprintf(out_fp, "0\n"); return;
    }
    ASTNode *v = g->left;
    if (is_ptr(g->ty)) {
        if (is_zero_literal(v)) fprintf(out_fp, "null\n");
        else if ((g->ty == TY_CHAR_PTR || g->ty == TY_UCHAR_PTR) && v->type == AST_STR) {
            int id = add_string_literal(v->str_val), len = (int)strlen(v->str_val) + 1;
            fprintf(out_fp, "getelementptr ([%d x i8], ptr @.str.%d, i32 0, i32 0)\n", len, id);
        } else error("全域指標初始化只支援 0 或字串");
        return;
    }
    if (is_float(g->ty)) emit_float_const(g->ty, (v->type == AST_FLOAT) ? v->fval : (double)v->val);
    else emit_int_const(g->ty, (long long)(v->type == AST_FLOAT ? (long long)v->fval : v->val));
    fprintf(out_fp, "\n");
}

static void emit_global_array(ASTNode *g) {
    CType elem = base_of(g->ty);
    if (elem == TY_STRUCT) { fprintf(out_fp, "@%s = global [%d x i8] zeroinitializer\n", g->name, g->array_len * struct_size(g->struct_id)); return; }
    if (g->init_kind == 3 && (elem == TY_CHAR || elem == TY_UCHAR)) {
        char llvm_str[2048] = {0}; int len = 0; build_llvm_string(g->str_val, llvm_str, &len);
        while (len < g->array_len) { strcat(llvm_str, "\\00"); len++; }
        fprintf(out_fp, "@%s = global [%d x i8] c\"%s\"\n", g->name, g->array_len, llvm_str); return;
    }
    fprintf(out_fp, "@%s = global [%d x %s] ", g->name, g->array_len, llvm_elem_type(g->ty));
    if (g->init_kind == 0) { fprintf(out_fp, "zeroinitializer\n"); return; }
    fprintf(out_fp, "["); ASTNode *cur = g->left;
    for (int i = 0; i < g->array_len; i++) {
        if (i > 0) fprintf(out_fp, ", ");
        if (cur) {
            if (is_float(elem)) emit_float_const(elem, (cur->type == AST_FLOAT) ? cur->fval : (double)cur->val);
            else emit_int_const(elem, (long long)(cur->type == AST_FLOAT ? (long long)cur->fval : cur->val));
            cur = cur->next;
        } else {
            if (is_float(elem)) emit_float_const(elem, 0.0); else emit_int_const(elem, 0);
        }
    }
    fprintf(out_fp, "]\n");
}

static void emit_globals(ASTNode *nodes) {
    for (ASTNode *n = nodes; n; n = n->next) {
        if (n->type != AST_GLOBAL) continue;
        global_add(n->name, n->ty, n->array_len, n->struct_id);
        if (n->is_decl) fprintf(out_fp, "@%s = external global %s\n", n->name, llvm_type(n->ty));
        else if (n->array_len > 0) emit_global_array(n); else emit_global_scalar(n);
    }
}

static int is_param(ASTNode *params, const char *name) {
    for (ASTNode *pnode = params; pnode; pnode = pnode->next) if (strcmp(pnode->name, name) == 0) return 1; return 0;
}
static FuncSig* find_func_sig(const char *name) {
    for (int i = 0; i < func_sig_cnt; i++) if (strcmp(func_sigs[i].name, name) == 0) return &func_sigs[i]; return NULL;
}
static void var_push_scope(void) { scope_marks[scope_depth++] = var_cnt; }
static void var_pop_scope(void) { if (scope_depth > 0) var_cnt = scope_marks[--scope_depth]; }

static void var_add(const char *name, const char *ir, CType ty, int array_len, int struct_id, int is_global) {
    if (var_cnt >= 512) error("變數表已滿");
    strcpy(var_slots[var_cnt].name, name); strcpy(var_slots[var_cnt].ir, ir);
    var_slots[var_cnt].ty = ty; var_slots[var_cnt].array_len = array_len;
    var_slots[var_cnt].struct_id = struct_id; var_slots[var_cnt].is_global = is_global; var_cnt++;
}

static VarSlot* var_find(const char *name) {
    for (int i = var_cnt - 1; i >= 0; i--) if (strcmp(var_slots[i].name, name) == 0) return &var_slots[i];
    for (int i = global_cnt - 1; i >= 0; i--) if (strcmp(global_slots[i].name, name) == 0) return &global_slots[i];
    error("找不到變數宣告"); return NULL;
}

static void global_add(const char *name, CType ty, int array_len, int struct_id) {
    if (global_cnt >= 256) error("全域變數表已滿");
    strcpy(global_slots[global_cnt].name, name); strcpy(global_slots[global_cnt].ir, name);
    global_slots[global_cnt].ty = ty; global_slots[global_cnt].array_len = array_len;
    global_slots[global_cnt].struct_id = struct_id; global_slots[global_cnt].is_global = 1; global_cnt++;
}

static const char* slot_prefix(VarSlot *slot) { return slot->is_global ? "@" : "%"; }
static char* slot_ref(VarSlot *slot) { char *res = malloc(64); sprintf(res, "%s%s", slot_prefix(slot), slot->ir); return res; }
static int struct_size(int struct_id) { return (struct_id >= 0 && struct_id < g_struct_def_cnt) ? g_struct_defs[struct_id].size : 0; }
static int elem_size(CType ptr_ty, int struct_id) {
    if (!is_ptr(ptr_ty)) return 0; CType b = base_of(ptr_ty);
    if (b == TY_CHAR || b == TY_UCHAR) return 1; if (b == TY_SHORT || b == TY_USHORT) return 2;
    if (b == TY_INT || b == TY_UINT || b == TY_FLOAT) return 4;
    if (b == TY_LONG || b == TY_ULONG || b == TY_DOUBLE || is_ptr(b)) return 8;
    if (b == TY_STRUCT) return struct_size(struct_id); return 0;
}

static void build_func_sigs(ASTNode *funcs) {
    func_sig_cnt = 0;
    for (ASTNode *f = funcs; f; f = f->next) {
        if (f->type != AST_FUNC) continue;
        FuncSig *sig = &func_sigs[func_sig_cnt++];
        strcpy(sig->name, f->name); sig->ret = f->ty; sig->ret_struct_id = f->struct_id;
        sig->is_vararg = f->val; sig->param_cnt = 0;
        for (ASTNode *p = f->left; p && sig->param_cnt < 16; p = p->next) {
            sig->params[sig->param_cnt] = p->ty; sig->param_struct_id[sig->param_cnt++] = p->struct_id;
        }
    }
}

static int func_has_def(ASTNode *funcs, const char *name) {
    for (ASTNode *f = funcs; f; f = f->next) if (f->type == AST_FUNC && !f->is_decl && strcmp(f->name, name) == 0) return 1; return 0;
}

static ValueType vt_from_ctype(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return VT_I8; if (ty == TY_SHORT || ty == TY_USHORT) return VT_I16;
    if (ty == TY_INT || ty == TY_UINT) return VT_I32; if (ty == TY_LONG || ty == TY_ULONG) return VT_I64;
    if (ty == TY_FLOAT) return VT_F32; if (ty == TY_DOUBLE) return VT_F64; if (is_ptr(ty)) return VT_PTR; return VT_I32;
}

static const char* llvm_type_from_vt(ValueType vt) {
    if (vt == VT_I1) return "i1"; if (vt == VT_I8) return "i8"; if (vt == VT_I16) return "i16";
    if (vt == VT_I32) return "i32"; if (vt == VT_I64) return "i64";
    if (vt == VT_F32) return "float"; if (vt == VT_F64) return "double"; return "ptr";
}

static Value value_from_raw(char *reg, ValueType vt, CType cty) { Value v = {reg, vt, cty}; return v; }
static Value value_from_ctype(char *reg, CType ty) { return value_from_raw(reg, vt_from_ctype(ty), ty); }

static Value cast_value(Value v, CType to) {
    if (v.reg == NULL) error("使用了 void 表達式");
    ValueType from_vt = v.vt; ValueType to_vt = vt_from_ctype(to);
    
    if (from_vt == VT_PTR && to_vt != VT_PTR) {
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = ptrtoint ptr %s to %s\n", r, v.reg, llvm_type_from_vt(to_vt));
        free(v.reg);
        char *res = malloc(64); sprintf(res, "%%%d", r);
        v = value_from_raw(res, to_vt, to);
        from_vt = to_vt;
    }
    if (v.cty == to && v.vt != VT_I1) return v;
    
    if (is_ptr(to)) {
        if (v.vt == VT_PTR) { v.cty = to; return v; }
        if (v.vt == VT_I32 || v.vt == VT_I64) {
            int r = reg_cnt++;
            fprintf(out_fp, "  %%%d = inttoptr %s %s to ptr\n", r, llvm_type_from_vt(v.vt), v.reg);
            free(v.reg);
            char *res = malloc(64); sprintf(res, "%%%d", r);
            return value_from_ctype(res, to);
        }
        error("不支援轉換為指標");
    }
    
    int r = reg_cnt++;
    if (to_vt == VT_F32 || to_vt == VT_F64) {
        const char *to_ty = (to_vt == VT_F64) ? "double" : "float";
        if (from_vt == VT_F32 || from_vt == VT_F64) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, (to_vt == VT_F64 && from_vt == VT_F32) ? "fpext" : "fptrunc", (from_vt == VT_F64) ? "double" : "float", v.reg, to_ty);
        else fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, is_unsigned(v.cty) ? "uitofp" : "sitofp", llvm_type_from_vt(from_vt), v.reg, to_ty);
    } else {
        const char *to_ty = llvm_type(to);
        if (from_vt == VT_F32 || from_vt == VT_F64) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, is_unsigned(to) ? "fptoui" : "fptosi", (from_vt == VT_F64) ? "double" : "float", v.reg, to_ty);
        else {
            int fb = (from_vt == VT_I1) ? 1 : (from_vt == VT_I8) ? 8 : (from_vt == VT_I16) ? 16 : (from_vt == VT_I64) ? 64 : 32;
            int tb = (to_vt == VT_I8) ? 8 : (to_vt == VT_I16) ? 16 : (to_vt == VT_I64) ? 64 : 32;
            if (fb == tb) { v.cty = to; return v; }
            else if (fb < tb) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, (from_vt == VT_I1 || is_unsigned(v.cty)) ? "zext" : "sext", llvm_type_from_vt(from_vt), v.reg, to_ty);
            else fprintf(out_fp, "  %%%d = trunc %s %s to %s\n", r, llvm_type_from_vt(from_vt), v.reg, to_ty);
        }
    }
    free(v.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, to);
}

static Value to_i1(Value v) {
    if (v.vt == VT_I1) return v; if (v.reg == NULL) error("使用了 void 表達式");
    int r = reg_cnt++;
    if (v.vt == VT_PTR) fprintf(out_fp, "  %%%d = icmp ne ptr %s, null\n", r, v.reg);
    else if (v.vt == VT_F32 || v.vt == VT_F64) fprintf(out_fp, "  %%%d = fcmp one %s %s, 0.0\n", r, (v.vt == VT_F64) ? "double" : "float", v.reg);
    else fprintf(out_fp, "  %%%d = icmp ne %s %s, 0\n", r, llvm_type(v.cty), v.reg);
    free(v.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT);
}

static Value to_i32(Value v) { return cast_value(v, TY_INT); }

static Value gen_lvalue_addr(ASTNode *node) {
    if (node->type == AST_VAR) {
        VarSlot *slot = var_find(node->name); const char *pref = slot_prefix(slot);
        if (slot->array_len > 0) {
            int r = reg_cnt++;
            if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr[%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
            else fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
            char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, slot->ty);
        }
        return value_from_raw(slot_ref(slot), VT_PTR, slot->ty);
    }
    if (node->type == AST_DEREF) return gen_expr(node->left);
    if (node->type == AST_INDEX) {
        ASTNode *base = node->left; Value base_ptr;
        if (base->type == AST_VAR) {
            VarSlot *slot = var_find(base->name); const char *pref = slot_prefix(slot);
            if (slot->array_len > 0) {
                int r = reg_cnt++;
                if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr [%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
                else fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
                char *res = malloc(64); sprintf(res, "%%%d", r); base_ptr = value_from_ctype(res, slot->ty);
            } else base_ptr = gen_expr(base);
        } else base_ptr = gen_expr(base);
        Value idx = to_i32(gen_expr(node->right));
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r, llvm_elem_type(base->ty), base_ptr.reg, idx.reg);
        free(base_ptr.reg); free(idx.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_INT_PTR);
    }
    if (node->type == AST_MEMBER) {
        Value base_ptr = node->op ? gen_expr(node->left) : gen_lvalue_addr(node->left);
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = getelementptr i8, ptr %s, i32 %d\n", r, base_ptr.reg, node->val);
        free(base_ptr.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_INT_PTR);
    }
    error("不支援的取址"); return value_from_raw(NULL, VT_PTR, TY_INT_PTR);
}

static Value gen_expr(ASTNode *node) {
    if (node->type == AST_NUM) { char *res = malloc(64); sprintf(res, "%d", node->val); return value_from_ctype(res, node->ty); }
    if (node->type == AST_FLOAT) { char *res = malloc(64); if (node->ty == TY_FLOAT) snprintf(res, 64, "%.9f", node->fval); else snprintf(res, 64, "%.17f", node->fval); return value_from_ctype(res, node->ty); }
    if (node->type == AST_STR) { int id = add_string_literal(node->str_val), len = (int)strlen(node->str_val) + 1, r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr ([%d x i8], ptr @.str.%d, i32 0, i32 0)\n", r, len, id); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_CHAR_PTR); }
    if (node->type == AST_VAR) {
        VarSlot *slot = var_find(node->name); const char *pref = slot_prefix(slot);
        if (slot->array_len > 0) {
            int r = reg_cnt++;
            if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr [%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
            else fprintf(out_fp, "  %%%d = getelementptr[%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
            char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, slot->ty);
        }
        if (slot->ty == TY_STRUCT) return value_from_raw(slot_ref(slot), VT_PTR, TY_STRUCT_PTR);
        int r = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s%s\n", r, llvm_type(slot->ty), pref, slot->ir);
        char *res = malloc(64); sprintf(res, "%%%d", r); return is_ptr(slot->ty) ? value_from_raw(res, VT_PTR, slot->ty) : value_from_ctype(res, slot->ty);
    }
    if (node->type == AST_CALL) {
        Value raw_vals[16], final_vals[16]; CType call_types[16]; int arg_count = 0;
        for (ASTNode *arg = node->left; arg; arg = arg->next) { raw_vals[arg_count] = gen_expr(arg); call_types[arg_count] = arg->ty; arg_count++; }
        FuncSig *sig = find_func_sig(node->name);
        int is_vararg = (sig && sig->is_vararg) || (strcmp(node->name, "printf") == 0) || (strcmp(node->name, "sprintf") == 0) || (strcmp(node->name, "snprintf") == 0) || (strcmp(node->name, "fprintf") == 0);
        const char *ret_ty = (sig && sig->ret == TY_VOID) ? "void" : llvm_type(sig ? sig->ret : TY_INT);
        for (int i = 0; i < arg_count; i++) {
            if (is_vararg && i >= (sig ? sig->param_cnt : 0)) {
                CType ct = call_types[i];
                if (ct == TY_FLOAT) ct = TY_DOUBLE;
                if (ct == TY_CHAR || ct == TY_UCHAR || ct == TY_SHORT || ct == TY_USHORT) ct = TY_INT;
                final_vals[i] = is_ptr(ct) ? raw_vals[i] : cast_value(raw_vals[i], ct); call_types[i] = ct;
            } else if (sig && i < sig->param_cnt) {
                CType pt = sig->params[i];
                final_vals[i] = is_ptr(pt) ? (is_ptr(call_types[i]) ? raw_vals[i] : cast_value(raw_vals[i], pt)) : cast_value(raw_vals[i], pt);
                call_types[i] = pt;
            } else final_vals[i] = raw_vals[i];
        }
        int r = -1;
        if (is_vararg) {
            if (!sig || sig->ret != TY_VOID) { r = reg_cnt++; fprintf(out_fp, "  %%%d = call %s (", r, ret_ty); }
            else fprintf(out_fp, "  call void (");
            for (int i = 0; i < (sig ? sig->param_cnt : 0); i++) fprintf(out_fp, "%s%s", (i > 0) ? ", " : "", llvm_type(sig->params[i]));
            fprintf(out_fp, "%s...) @%s(", (sig && sig->param_cnt > 0) ? ", " : "", node->name);
        } else {
            if (sig && sig->ret == TY_VOID) fprintf(out_fp, "  call void @%s(", node->name);
            else { r = reg_cnt++; fprintf(out_fp, "  %%%d = call %s @%s(", r, ret_ty, node->name); }
        }
        for (int i = 0; i < arg_count; i++) {
            if (i > 0) fprintf(out_fp, ", ");
            fprintf(out_fp, "%s %s", is_ptr(call_types[i]) ? "ptr" : llvm_type(call_types[i]), final_vals[i].reg); free(final_vals[i].reg);
        }
        fprintf(out_fp, ")\n");
        if ((!is_vararg && sig && sig->ret == TY_VOID) || (is_vararg && sig && sig->ret == TY_VOID)) return value_from_raw(NULL, VT_I32, TY_INT);
        char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, sig ? sig->ret : TY_INT);
    }
    if (node->type == AST_ADDR) return gen_lvalue_addr(node->left);
    if (node->type == AST_DEREF || node->type == AST_INDEX || node->type == AST_MEMBER) {
        Value addr = (node->type == AST_DEREF) ? gen_expr(node->left) : gen_lvalue_addr(node);
        if (node->ty == TY_STRUCT) return addr;
        int r = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r, llvm_elem_type(node->ty), addr.reg);
        free(addr.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, node->ty);
    }
    if (node->type == AST_BINOP) {
        if (node->op == TK_ANDAND || node->op == TK_OROR) {
            int tmp = reg_cnt++, rhs_label = label_cnt++, short_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  %%%d = alloca i1\n", tmp); Value left_cond = gen_cond(node->left);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", left_cond.reg, (node->op == TK_ANDAND) ? rhs_label : short_label, (node->op == TK_ANDAND) ? short_label : rhs_label);
            free(left_cond.reg); fprintf(out_fp, "L%d:\n", rhs_label); Value right_cond = gen_cond(node->right);
            fprintf(out_fp, "  store i1 %s, ptr %%%d\n  br label %%L%d\nL%d:\n  store i1 %s, ptr %%%d\n  br label %%L%d\nL%d:\n", right_cond.reg, tmp, end_label, short_label, (node->op == TK_ANDAND) ? "0" : "1", tmp, end_label, end_label);
            free(right_cond.reg); int r = reg_cnt++; fprintf(out_fp, "  %%%d = load i1, ptr %%%d\n", r, tmp); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_EQ || node->op == TK_NE) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            Value l = is_ptr(node->left->ty) ? gen_expr(node->left) : cast_value(gen_expr(node->left), TY_INT_PTR);
            Value r = is_ptr(node->right->ty) ? gen_expr(node->right) : cast_value(gen_expr(node->right), TY_INT_PTR);
            int rcmp = reg_cnt++; fprintf(out_fp, "  %%%d = icmp %s ptr %s, %s\n", rcmp, (node->op == TK_EQ) ? "eq" : "ne", l.reg, r.reg);
            free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rcmp); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            Value l = is_ptr(node->left->ty) ? gen_expr(node->left) : cast_value(gen_expr(node->left), TY_INT_PTR);
            Value r = is_ptr(node->right->ty) ? gen_expr(node->right) : cast_value(gen_expr(node->right), TY_INT_PTR);
            int rl = reg_cnt++, rr = reg_cnt++, rcmp = reg_cnt++;
            fprintf(out_fp, "  %%%d = ptrtoint ptr %s to i64\n  %%%d = ptrtoint ptr %s to i64\n", rl, l.reg, rr, r.reg);
            fprintf(out_fp, "  %%%d = icmp %s i64 %%%d, %%%d\n", rcmp, (node->op == TK_LT) ? "slt" : (node->op == TK_LE) ? "sle" : (node->op == TK_GT) ? "sgt" : "sge", rl, rr);
            free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rcmp); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_PLUS || node->op == TK_MINUS) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            if (is_ptr(node->left->ty) && is_ptr(node->right->ty)) {
                Value l = gen_expr(node->left), r = gen_expr(node->right);
                int rl = reg_cnt++, rr = reg_cnt++, rdiff = reg_cnt++, esz = elem_size(node->left->ty, node->left->struct_id), rdiv = rdiff, rtr = reg_cnt++;
                fprintf(out_fp, "  %%%d = ptrtoint ptr %s to i64\n  %%%d = ptrtoint ptr %s to i64\n  %%%d = sub i64 %%%d, %%%d\n", rl, l.reg, rr, r.reg, rdiff, rl, rr);
                if (esz > 1) { rdiv = reg_cnt++; fprintf(out_fp, "  %%%d = sdiv i64 %%%d, %d\n", rdiv, rdiff, esz); }
                fprintf(out_fp, "  %%%d = trunc i64 %%%d to i32\n", rtr, rdiv); free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rtr); return value_from_ctype(res, TY_INT);
            } else {
                ASTNode *ptr_node = is_ptr(node->left->ty) ? node->left : node->right, *int_node = is_ptr(node->left->ty) ? node->right : node->left;
                Value base = gen_expr(ptr_node), idx = to_i32(gen_expr(int_node));
                if (node->op == TK_MINUS && is_ptr(node->left->ty)) {
                    int rneg = reg_cnt++; fprintf(out_fp, "  %%%d = sub i32 0, %s\n", rneg, idx.reg); free(idx.reg); char *neg = malloc(64); sprintf(neg, "%%%d", rneg); idx = value_from_raw(neg, VT_I32, TY_INT);
                }
                int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r, llvm_elem_type(ptr_node->ty), base.reg, idx.reg);
                free(base.reg); free(idx.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, ptr_node->ty);
            }
        }
        CType ct = common_arith_type(node->left->ty, node->right->ty);
        Value left = cast_value(gen_expr(node->left), ct), right = cast_value(gen_expr(node->right), ct);
        int r = reg_cnt++;
        if (is_float(ct)) {
            const char *fty = (ct == TY_DOUBLE) ? "double" : "float";
            if (node->op == '+') fprintf(out_fp, "  %%%d = fadd %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '-') fprintf(out_fp, "  %%%d = fsub %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '*') fprintf(out_fp, "  %%%d = fmul %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '/') fprintf(out_fp, "  %%%d = fdiv %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) {
                fprintf(out_fp, "  %%%d = fcmp %s %s %s, %s\n", r, (node->op == TK_LT) ? "olt" : (node->op == TK_LE) ? "ole" : (node->op == TK_GT) ? "ogt" : (node->op == TK_GE) ? "oge" : (node->op == TK_EQ) ? "oeq" : "one", fty, left.reg, right.reg);
            }
        } else {
            const char *ity = llvm_type(ct); int uns = is_unsigned(ct);
            if (node->op == '+') fprintf(out_fp, "  %%%d = add %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '-') fprintf(out_fp, "  %%%d = sub %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '*') fprintf(out_fp, "  %%%d = mul %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '/') fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r, uns ? "udiv" : "sdiv", ity, left.reg, right.reg);
            if (node->op == TK_MOD) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r, uns ? "urem" : "srem", ity, left.reg, right.reg);
            if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) {
                fprintf(out_fp, "  %%%d = icmp %s %s %s, %s\n", r, (node->op == TK_LT) ? (uns ? "ult" : "slt") : (node->op == TK_GT) ? (uns ? "ugt" : "sgt") : (node->op == TK_LE) ? (uns ? "ule" : "sle") : (node->op == TK_GE) ? (uns ? "uge" : "sge") : (node->op == TK_EQ) ? "eq" : "ne", ity, left.reg, right.reg);
            }
        }
        free(left.reg); free(right.reg); char *res = malloc(64); sprintf(res, "%%%d", r);
        if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) return value_from_raw(res, VT_I1, TY_INT);
        return value_from_ctype(res, ct);
    }
    if (node->type == AST_UNARY) {
        if (node->op == TK_NOT) { Value val = gen_cond(node->left); int r = reg_cnt++; fprintf(out_fp, "  %%%d = xor i1 %s, 1\n", r, val.reg); free(val.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT); }
        if (node->op == TK_MINUS || node->op == TK_PLUS) {
            Value val = cast_value(gen_expr(node->left), node->ty);
            if (node->op == TK_PLUS) return val;
            int r = reg_cnt++;
            if (is_float(node->ty)) fprintf(out_fp, "  %%%d = fsub %s 0.0, %s\n", r, (node->ty == TY_DOUBLE) ? "double" : "float", val.reg);
            else fprintf(out_fp, "  %%%d = sub %s 0, %s\n", r, llvm_type(node->ty), val.reg);
            free(val.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, node->ty);
        }
    }
    if (node->type == AST_CAST) {
        Value val = gen_expr(node->left);
        return cast_value(val, node->ty);
    }
    if (node->type == AST_SIZEOF) { char *res = malloc(64); sprintf(res, "%d", node->val); return value_from_ctype(res, TY_INT); }
    if (node->type == AST_INCDEC) {
        int is_inc = (node->op == TK_PLUSPLUS); Value addr = gen_lvalue_addr(node->left);
        const char *ty = llvm_type(node->ty); int r_old = reg_cnt++;
        fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r_old, ty, addr.reg);
        char *old_reg = malloc(64); sprintf(old_reg, "%%%d", r_old); Value old_val = value_from_ctype(old_reg, node->ty);
        int r_new = reg_cnt++;
        if (is_ptr(node->ty)) fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r_new, llvm_elem_type(node->ty), old_val.reg, is_inc ? "1" : "-1");
        else if (is_float(node->ty)) fprintf(out_fp, "  %%%d = %s %s %s, 1.0\n", r_new, is_inc ? "fadd" : "fsub", (node->ty == TY_DOUBLE) ? "double" : "float", old_val.reg);
        else fprintf(out_fp, "  %%%d = %s %s %s, 1\n", r_new, is_inc ? "add" : "sub", ty, old_val.reg);
        char *new_reg = malloc(64); sprintf(new_reg, "%%%d", r_new); Value new_val = value_from_ctype(new_reg, node->ty);
        fprintf(out_fp, "  store %s %s, ptr %s\n", ty, new_val.reg, addr.reg); free(addr.reg);
        if (node->is_prefix) return value_from_ctype(new_val.reg, node->ty);
        free(new_val.reg); return value_from_ctype(old_val.reg, node->ty);
    }
    error("未知的表達式"); return value_from_raw(NULL, VT_I32, TY_INT);
}

static Value gen_cond(ASTNode *node) { Value val = gen_expr(node); if (val.vt == VT_I1) return val; return to_i1(val); }

static void gen_stmt(ASTNode *node) {
    while (node) {
        if (node->type == AST_DECL) {
            char ir_name[64]; snprintf(ir_name, sizeof(ir_name), "v%d", var_id++);
            var_add(node->name, ir_name, node->ty, node->array_len, node->struct_id, 0);
            if (node->array_len > 0) {
                if (base_of(node->ty) == TY_STRUCT) fprintf(out_fp, "  %%%s = alloca [%d x i8]\n", ir_name, node->array_len * struct_size(node->struct_id));
                else {
                    fprintf(out_fp, "  %%%s = alloca [%d x %s]\n", ir_name, node->array_len, llvm_elem_type(node->ty));
                    if (node->init_kind == 2) {
                        int idx = 0;
                        for (ASTNode *cur = node->left; cur && idx < node->array_len; cur = cur->next, idx++) {
                            Value val = gen_expr(cur), store_val = cast_value(val, base_of(node->ty)); int r = reg_cnt++;
                            fprintf(out_fp, "  %%%d = getelementptr[%d x %s], ptr %%%s, i32 0, i32 %d\n  store %s %s, ptr %%%d\n", r, node->array_len, llvm_elem_type(node->ty), ir_name, idx, llvm_elem_type(node->ty), store_val.reg, r); free(store_val.reg);
                        }
                        for (int i = idx; i < node->array_len; i++) {
                            int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %%%s, i32 0, i32 %d\n  store %s 0, ptr %%%d\n", r, node->array_len, llvm_elem_type(node->ty), ir_name, i, llvm_elem_type(node->ty), r);
                        }
                    } else if (node->init_kind == 3) {
                        int len = (int)strlen(node->str_val);
                        for (int i = 0; i < node->array_len; i++) {
                            int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr[%d x i8], ptr %%%s, i32 0, i32 %d\n  store i8 %d, ptr %%%d\n", r, node->array_len, ir_name, i, (i < len) ? (unsigned char)node->str_val[i] : 0, r);
                        }
                    }
                }
            } else if (node->ty == TY_STRUCT) fprintf(out_fp, "  %%%s = alloca [%d x i8]\n", ir_name, struct_size(node->struct_id));
            else if (is_param(current_params, node->name)) {
                fprintf(out_fp, "  %%%s = alloca %s\n  store %s %%%s, ptr %%%s\n", ir_name, llvm_type(node->ty), llvm_type(node->ty), node->name, ir_name);
            } else {
                fprintf(out_fp, "  %%%s = alloca %s\n", ir_name, llvm_type(node->ty));
                if (node->left) { Value val = gen_expr(node->left), store_val = is_ptr(node->ty) ? cast_value(val, node->ty) : cast_value(val, node->ty); fprintf(out_fp, "  store %s %s, ptr %%%s\n", llvm_type(node->ty), store_val.reg, ir_name); free(store_val.reg); }
            }
        } else if (node->type == AST_ASSIGN) {
            Value addr = gen_lvalue_addr(node->left);
            if (node->op == TK_ASSIGN) {
                Value val = gen_expr(node->right), store_val = cast_value(val, node->ty);
                fprintf(out_fp, "  store %s %s, ptr %s\n", llvm_type(node->ty), store_val.reg, addr.reg); free(store_val.reg);
            } else {
                int r_old = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r_old, llvm_type(node->ty), addr.reg);
                char *old_reg = malloc(64); sprintf(old_reg, "%%%d", r_old); Value old_val = value_from_ctype(old_reg, node->ty);
                Value rhs = cast_value(gen_expr(node->right), is_ptr(node->ty) ? TY_INT : node->ty); int r_new = reg_cnt++;
                if (is_ptr(node->ty)) {
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r_new, llvm_elem_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) { int r_neg = reg_cnt++; fprintf(out_fp, "  %%%d = sub i32 0, %s\n  %%%d = getelementptr %s, ptr %s, i32 %%%d\n", r_neg, rhs.reg, r_new, llvm_elem_type(node->ty), old_val.reg, r_neg); }
                    else error("指標不支援此複合指定");
                } else if (is_float(node->ty)) {
                    const char *fty = (node->ty == TY_DOUBLE) ? "double" : "float";
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = fadd %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) fprintf(out_fp, "  %%%d = fsub %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_MULEQ) fprintf(out_fp, "  %%%d = fmul %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_DIVEQ) fprintf(out_fp, "  %%%d = fdiv %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                } else {
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = add %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) fprintf(out_fp, "  %%%d = sub %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MULEQ) fprintf(out_fp, "  %%%d = mul %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_DIVEQ) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r_new, is_unsigned(node->ty) ? "udiv" : "sdiv", llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MODEQ) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r_new, is_unsigned(node->ty) ? "urem" : "srem", llvm_type(node->ty), old_val.reg, rhs.reg);
                }
                free(old_val.reg); free(rhs.reg); char *new_reg = malloc(64); sprintf(new_reg, "%%%d", r_new); Value new_val = value_from_ctype(new_reg, node->ty);
                fprintf(out_fp, "  store %s %s, ptr %s\n", llvm_type(node->ty), new_val.reg, addr.reg); free(new_val.reg);
            }
            free(addr.reg);
        } else if (node->type == AST_EXPR_STMT) { if (node->left) { Value val = gen_expr(node->left); if (val.reg) free(val.reg); } }
        else if (node->type == AST_BLOCK) { var_push_scope(); gen_stmt(node->left); var_pop_scope(); }
        else if (node->type == AST_IF) {
            Value cond_val = gen_cond(node->cond);
            int then_label = label_cnt++, else_label = node->else_body ? label_cnt++ : label_cnt++, end_label = node->else_body ? label_cnt++ : else_label;
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", cond_val.reg, then_label, else_label); free(cond_val.reg);
            fprintf(out_fp, "L%d:\n", then_label); gen_stmt(node->then_body); fprintf(out_fp, "  br label %%L%d\n", end_label);
            if (node->else_body) { fprintf(out_fp, "L%d:\n", else_label); gen_stmt(node->else_body); fprintf(out_fp, "  br label %%L%d\n", end_label); }
            fprintf(out_fp, "L%d:\n", end_label);
        } else if (node->type == AST_WHILE) {
            int cond_label = label_cnt++, body_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, cond_label); Value cond_val = gen_cond(node->cond);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\nL%d:\n", cond_val.reg, body_label, end_label, body_label); free(cond_val.reg);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = cond_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", cond_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", end_label);
        } else if (node->type == AST_FOR) {
            if (node->init) gen_stmt(node->init);
            int cond_label = label_cnt++, body_label = label_cnt++, update_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, cond_label);
            if (node->cond) { Value cond_val = gen_cond(node->cond); fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", cond_val.reg, body_label, end_label); free(cond_val.reg); }
            else fprintf(out_fp, "  br label %%L%d\n", body_label);
            fprintf(out_fp, "L%d:\n", body_label);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = update_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", update_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", update_label); if (node->update) gen_stmt(node->update); fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, end_label);
        } else if (node->type == AST_DO) {
            int body_label = label_cnt++, cond_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", body_label, body_label);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = cond_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", cond_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", cond_label); Value cond_val = gen_cond(node->cond);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\nL%d:\n", cond_val.reg, body_label, end_label, end_label); free(cond_val.reg);
        } else if (node->type == AST_SWITCH) {
            Value cond_val = cast_value(gen_expr(node->cond), TY_INT);
            int end_label = label_cnt++, case_count = 0;
            for (ASTNode *c = node->left; c; c = c->next) case_count++;
            int *case_labels = malloc(sizeof(int) * case_count), *case_is_default = malloc(sizeof(int) * case_count);
            ASTNode **case_nodes = malloc(sizeof(ASTNode*) * case_count);
            int default_label = -1, idx = 0;
            for (ASTNode *c = node->left; c; c = c->next) {
                case_labels[idx] = label_cnt++; case_is_default[idx] = c->is_default;
                if (c->is_default) default_label = case_labels[idx];
                case_nodes[idx] = c; idx++;
            }
            int check_label = label_cnt++; fprintf(out_fp, "  br label %%L%d\n", check_label);
            for (int i = 0; i < case_count; i++) {
                if (case_is_default[i]) continue;
                fprintf(out_fp, "L%d:\n", check_label);
                int cmp = reg_cnt++, next = label_cnt++;
                fprintf(out_fp, "  %%%d = icmp eq i32 %s, %d\n  br i1 %%%d, label %%L%d, label %%L%d\n", cmp, cond_val.reg, case_nodes[i]->val, cmp, case_labels[i], next);
                check_label = next;
            }
            fprintf(out_fp, "L%d:\n", check_label);
            if (default_label >= 0) fprintf(out_fp, "  br label %%L%d\n", default_label); else fprintf(out_fp, "  br label %%L%d\n", end_label);
            switch_break_stack[switch_depth++] = end_label;
            for (int i = 0; i < case_count; i++) {
                fprintf(out_fp, "L%d:\n", case_labels[i]); gen_stmt(case_nodes[i]->left);
                int next_label = (i + 1 < case_count) ? case_labels[i + 1] : end_label;
                fprintf(out_fp, "  br label %%L%d\n", next_label);
            }
            switch_depth--; fprintf(out_fp, "L%d:\n", end_label);
            free(cond_val.reg); free(case_labels); free(case_is_default); free(case_nodes);
        } else if (node->type == AST_BREAK) {
            if (loop_depth > 0) fprintf(out_fp, "  br label %%L%d\n", break_label_stack[loop_depth - 1]);
            else if (switch_depth > 0) fprintf(out_fp, "  br label %%L%d\n", switch_break_stack[switch_depth - 1]);
            else error("break 不在迴圈或 switch 內");
        } else if (node->type == AST_CONTINUE) {
            if (loop_depth == 0) error("continue 不在迴圈內");
            fprintf(out_fp, "  br label %%L%d\n", continue_label_stack[loop_depth - 1]);
        } else if (node->type == AST_RETURN) {
            if (node->left == NULL) fprintf(out_fp, "  ret void\n");
            else {
                Value val = gen_expr(node->left); Value ret_val = cast_value(val, current_ret_ty);
                fprintf(out_fp, "  ret %s %s\n", llvm_type(current_ret_ty), ret_val.reg); free(ret_val.reg);
            }
        }
        node = node->next;
    }
}

void gen_llvm_ir(ASTNode *funcs, FILE *out) {
    out_fp = out;
    fprintf(out_fp, "; ModuleID = 'c0c'\n");
    string_cnt = 0; global_cnt = 0;
    build_func_sigs(funcs);
    emit_globals(funcs);
    for (ASTNode *func = funcs; func; func = func->next) {
        if (func->type != AST_FUNC) continue;
        const char *ret_ty = llvm_type(func->ty);
        if (func->is_decl) {
            if (func_has_def(funcs, func->name)) continue;
            fprintf(out_fp, "declare %s @%s(", ret_ty, func->name);
        } else fprintf(out_fp, "define dso_local %s @%s(", ret_ty, func->name);
        
        int first = 1;
        for (ASTNode *param = func->left; param; param = param->next) {
            if (!first) fprintf(out_fp, ", ");
            fprintf(out_fp, "%s %%%s", llvm_type(param->ty), param->name); first = 0;
        }
        if (func->val) fprintf(out_fp, "%s...", first ? "" : ", ");
        if (func->is_decl) { fprintf(out_fp, ")\n\n"); continue; }
        
        fprintf(out_fp, ") {\nentry:\n");
        reg_cnt = 0; var_id = 0; var_cnt = 0; scope_depth = 0; var_push_scope();
        current_params = func->left; current_ret_ty = func->ty;
        for (ASTNode *pnode = current_params; pnode; pnode = pnode->next) gen_stmt(pnode);
        gen_stmt(func->right);
        if (current_ret_ty == TY_VOID) fprintf(out_fp, "  ret void\n");
        else fprintf(out_fp, "  ret %s 0\n", llvm_type(current_ret_ty));
        fprintf(out_fp, "}\n\n");
    }

    for (int i = 0; i < string_cnt; i++) {
        char llvm_str[2048] = {0}; int len = 0;
        build_llvm_string(string_table[i], llvm_str, &len);
        fprintf(out_fp, "@.str.%d = private unnamed_addr constant [%d x i8] c\"%s\", align 1\n", i, len, llvm_str);
    }
}