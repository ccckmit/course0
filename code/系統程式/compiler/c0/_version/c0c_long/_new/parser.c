#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char name[64];
    CType ty;
    int array_len;
    int struct_id;
} Sym;

static Sym symtab[256];
static int sym_cnt = 0;

typedef struct {
    char name[64];
    CType ret;
    int ret_struct_id;
    CType params[16];
    int param_struct_id[16];
    int param_cnt;
    int is_vararg;
} FuncSym;

static FuncSym func_tab[128];
static int func_cnt = 0;
static CType current_func_ret = TY_INT;
static int last_struct_id = -1;

static FuncSym* func_find(const char *name);

typedef struct {
    char name[64];
    CType ty;
    int struct_id;
} TypedefSym;

static TypedefSym typedef_tab[128];
static int typedef_cnt = 0;

typedef struct {
    char name[64];
    int struct_id;
} StructSym;

static StructSym struct_tab[64];
static int struct_sym_cnt = 0;

typedef struct {
    char name[64];
    int val;
} EnumSym;

static EnumSym enum_tab[256];
static int enum_cnt = 0;

typedef struct {
    char name[64];
    CType ty;
    int array_len;
    int struct_id;
} GlobalSym;

static GlobalSym gsymtab[256];
static int gsym_cnt = 0;

static void sym_reset(void) { sym_cnt = 0; }

static void sym_add(const char *name, CType ty, int array_len, int struct_id) {
    if (sym_cnt >= 256) error("符號表已滿");
    strcpy(symtab[sym_cnt].name, name);
    symtab[sym_cnt].ty = ty;
    symtab[sym_cnt].array_len = array_len;
    symtab[sym_cnt].struct_id = struct_id;
    sym_cnt++;
}

static int global_find_index(const char *name) {
    for (int i = gsym_cnt - 1; i >= 0; i--) {
        if (strcmp(gsymtab[i].name, name) == 0) return i;
    }
    return -1;
}

static void global_add(const char *name, CType ty, int array_len, int struct_id) {
    if (gsym_cnt >= 256) error("全域符號表已滿");
    if (global_find_index(name) >= 0) return; // 容許多次宣告 (外部連結 extern 支援)
    strcpy(gsymtab[gsym_cnt].name, name);
    gsymtab[gsym_cnt].ty = ty;
    gsymtab[gsym_cnt].array_len = array_len;
    gsymtab[gsym_cnt].struct_id = struct_id;
    gsym_cnt++;
}

static void func_add(const char *name, CType ret, int ret_struct_id,
                     CType *params, int *param_struct_id, int param_cnt, int is_vararg) {
    if (func_cnt >= 128) error("函式表已滿");
    for (int i = 0; i < func_cnt; i++) {
        if (strcmp(func_tab[i].name, name) == 0) return; // 已宣告
    }
    strcpy(func_tab[func_cnt].name, name);
    func_tab[func_cnt].ret = ret;
    func_tab[func_cnt].ret_struct_id = ret_struct_id;
    func_tab[func_cnt].param_cnt = param_cnt;
    func_tab[func_cnt].is_vararg = is_vararg;
    for (int i = 0; i < param_cnt; i++) {
        func_tab[func_cnt].params[i] = params[i];
        func_tab[func_cnt].param_struct_id[i] = param_struct_id[i];
    }
    func_cnt++;
}

static FuncSym* func_find(const char *name) {
    for (int i = 0; i < func_cnt; i++) {
        if (strcmp(func_tab[i].name, name) == 0) return &func_tab[i];
    }
    return NULL;
}

static void typedef_add(const char *name, CType ty, int struct_id) {
    if (typedef_cnt >= 128) error("typedef 表已滿");
    strcpy(typedef_tab[typedef_cnt].name, name);
    typedef_tab[typedef_cnt].ty = ty;
    typedef_tab[typedef_cnt].struct_id = struct_id;
    typedef_cnt++;
}

static int typedef_find(const char *name, CType *ty, int *struct_id) {
    for (int i = typedef_cnt - 1; i >= 0; i--) {
        if (strcmp(typedef_tab[i].name, name) == 0) {
            *ty = typedef_tab[i].ty;
            *struct_id = typedef_tab[i].struct_id;
            return 1;
        }
    }
    return 0;
}

static int struct_find(const char *name) {
    for (int i = 0; i < struct_sym_cnt; i++) {
        if (strcmp(struct_tab[i].name, name) == 0) return struct_tab[i].struct_id;
    }
    return -1;
}

static int add_struct_def(const char *name) {
    if (g_struct_def_cnt >= 64) error("struct 定義過多");
    int id = g_struct_def_cnt++;
    strcpy(g_struct_defs[id].name, name);
    g_struct_defs[id].size = 0;
    g_struct_defs[id].field_cnt = 0;
    if (struct_sym_cnt < 64) {
        strcpy(struct_tab[struct_sym_cnt].name, name);
        struct_tab[struct_sym_cnt].struct_id = id;
        struct_sym_cnt++;
    }
    return id;
}

static StructDef* get_struct_def(int id) {
    if (id < 0 || id >= g_struct_def_cnt) error("struct id 錯誤");
    return &g_struct_defs[id];
}

static int type_size(CType ty, int struct_id) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return 1;
    if (ty == TY_SHORT || ty == TY_USHORT) return 2;
    if (ty == TY_INT || ty == TY_UINT) return 4;
    if (ty == TY_LONG || ty == TY_ULONG) return 8;
    if (ty == TY_FLOAT) return 4;
    if (ty == TY_DOUBLE) return 8;
    if (ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
        ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
        ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR) return 8;
    if (ty == TY_STRUCT) {
        StructDef *d = get_struct_def(struct_id);
        return d->size;
    }
    return 0;
}

static Sym* sym_find(const char *name) {
    for (int i = sym_cnt - 1; i >= 0; i--) {
        if (strcmp(symtab[i].name, name) == 0) return &symtab[i];
    }
    int gi = global_find_index(name);
    if (gi >= 0) {
        static Sym gsym;
        strcpy(gsym.name, gsymtab[gi].name);
        gsym.ty = gsymtab[gi].ty;
        gsym.array_len = gsymtab[gi].array_len;
        gsym.struct_id = gsymtab[gi].struct_id;
        return &gsym;
    }
    error("找不到變數宣告");
    return NULL;
}

static int is_ptr(CType ty) {
    return ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
           ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
           ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR;
}
static int is_float(CType ty) { return ty == TY_FLOAT || ty == TY_DOUBLE; }
static int is_unsigned(CType ty) {
    return ty == TY_UCHAR || ty == TY_USHORT || ty == TY_UINT || ty == TY_ULONG;
}
static int int_rank(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return 1;
    if (ty == TY_SHORT || ty == TY_USHORT) return 2;
    if (ty == TY_INT || ty == TY_UINT) return 3;
    if (ty == TY_LONG || ty == TY_ULONG) return 4;
    return 0;
}
static CType int_promote(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR || ty == TY_SHORT || ty == TY_USHORT) return TY_INT;
    return ty;
}
static CType int_type_from_rank(int rank, int is_uns) {
    if (rank == 1) return is_uns ? TY_UCHAR : TY_CHAR;
    if (rank == 2) return is_uns ? TY_USHORT : TY_SHORT;
    if (rank == 3) return is_uns ? TY_UINT : TY_INT;
    if (rank == 4) return is_uns ? TY_ULONG : TY_LONG;
    return TY_INT;
}
static CType common_arith_type(CType a, CType b) {
    if (is_float(a) || is_float(b)) {
        if (a == TY_DOUBLE || b == TY_DOUBLE) return TY_DOUBLE;
        return TY_FLOAT;
    }
    a = int_promote(a);
    b = int_promote(b);
    int ra = int_rank(a);
    int rb = int_rank(b);
    int ua = is_unsigned(a);
    int ub = is_unsigned(b);
    if (ra == rb) return int_type_from_rank(ra, ua || ub);
    if (ra > rb) {
        if (ua) return int_type_from_rank(ra, 1);
        if (ub) return int_type_from_rank(ra, 0);
        return int_type_from_rank(ra, 0);
    } else {
        if (ub) return int_type_from_rank(rb, 1);
        if (ua) return int_type_from_rank(rb, 0);
        return int_type_from_rank(rb, 0);
    }
}
static CType ptr_of(CType ty) {
    if (ty == TY_CHAR) return TY_CHAR_PTR;
    if (ty == TY_UCHAR) return TY_UCHAR_PTR;
    if (ty == TY_SHORT) return TY_SHORT_PTR;
    if (ty == TY_USHORT) return TY_USHORT_PTR;
    if (ty == TY_INT) return TY_INT_PTR;
    if (ty == TY_UINT) return TY_UINT_PTR;
    if (ty == TY_LONG) return TY_LONG_PTR;
    if (ty == TY_ULONG) return TY_ULONG_PTR;
    if (ty == TY_FLOAT) return TY_FLOAT_PTR;
    if (ty == TY_DOUBLE) return TY_DOUBLE_PTR;
    if (ty == TY_STRUCT) return TY_STRUCT_PTR;
    return TY_LONG_PTR; // 任何雙重指標或多重指標統一作為 8 bytes 長指標處理
}
static CType base_of(CType ty) {
    if (ty == TY_CHAR_PTR) return TY_CHAR;
    if (ty == TY_UCHAR_PTR) return TY_UCHAR;
    if (ty == TY_SHORT_PTR) return TY_SHORT;
    if (ty == TY_USHORT_PTR) return TY_USHORT;
    if (ty == TY_INT_PTR) return TY_INT;
    if (ty == TY_UINT_PTR) return TY_UINT;
    if (ty == TY_LONG_PTR) return TY_LONG;
    if (ty == TY_ULONG_PTR) return TY_ULONG;
    if (ty == TY_FLOAT_PTR) return TY_FLOAT;
    if (ty == TY_DOUBLE_PTR) return TY_DOUBLE;
    if (ty == TY_STRUCT_PTR) return TY_STRUCT;
    return TY_INT;
}

static CType parse_type_allow_void(int allow_void) {
    CType base;
    last_struct_id = -1;
    while (cur_tok.type == TK_CONST) next_token();
    if (allow_void && cur_tok.type == TK_VOID) { next_token(); base = TY_VOID; goto ptr_check; }
    if (cur_tok.type == TK_STRUCT) {
        next_token();
        if (cur_tok.type != TK_IDENT) error("預期 struct 名稱");
        int sid = struct_find(cur_tok.name);
        if (sid < 0) error("未知的 struct");
        next_token();
        base = TY_STRUCT;
        last_struct_id = sid;
        goto ptr_check;
    }
    if (cur_tok.type == TK_IDENT) {
        CType t;
        int sid = -1;
        if (typedef_find(cur_tok.name, &t, &sid)) {
            next_token();
            last_struct_id = sid;
            base = t;
            goto ptr_check;
        }
    }

    int is_uns = 0, is_short = 0, is_long = 0, has_type = 0;
    CType builtin = TY_INT;
    while (1) {
        if (cur_tok.type == TK_CONST) { next_token(); continue; }
        if (cur_tok.type == TK_UNSIGNED) { is_uns = 1; next_token(); continue; }
        if (cur_tok.type == TK_SHORT) { is_short = 1; next_token(); continue; }
        if (cur_tok.type == TK_LONG) { is_long = 1; next_token(); continue; }
        if (cur_tok.type == TK_INT) { has_type = 1; builtin = TY_INT; next_token(); continue; }
        if (cur_tok.type == TK_CHAR) { has_type = 1; builtin = TY_CHAR; next_token(); continue; }
        if (cur_tok.type == TK_FLOAT) { has_type = 1; builtin = TY_FLOAT; next_token(); continue; }
        if (cur_tok.type == TK_DOUBLE) { has_type = 1; builtin = TY_DOUBLE; next_token(); continue; }
        break;
    }
    if (!has_type && (is_uns || is_short || is_long)) { builtin = TY_INT; has_type = 1; }
    if (!has_type) { error("預期型別"); return TY_INT; }
    if (builtin == TY_FLOAT || builtin == TY_DOUBLE) base = builtin;
    else if (builtin == TY_CHAR) base = is_uns ? TY_UCHAR : TY_CHAR;
    else {
        if (is_short) base = is_uns ? TY_USHORT : TY_SHORT;
        else if (is_long) base = is_uns ? TY_ULONG : TY_LONG;
        else base = is_uns ? TY_UINT : TY_INT;
    }

ptr_check:
    while (cur_tok.type == TK_MUL) {
        next_token();
        if (base == TY_VOID) base = TY_CHAR_PTR;
        else base = ptr_of(base);
    }
    return base;
}

static CType parse_type(void) {
    return parse_type_allow_void(0);
}

static ASTNode* make_var_node(const char *name) {
    Sym *s = sym_find(name);
    ASTNode *n = new_node(AST_VAR);
    strcpy(n->name, name);
    n->ty = s->ty;
    n->array_len = s->array_len;
    n->struct_id = s->struct_id;
    return n;
}

static ASTNode* parse_expr();
static ASTNode* parse_stmt();
static ASTNode* parse_block();
static ASTNode* parse_decl_stmt(int expect_semi);

static int is_struct_def_ahead(void) {
    char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
    next_token();
    if (cur_tok.type != TK_IDENT) { p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; return 0; }
    next_token();
    int is_def = (cur_tok.type == TK_LBRACE);
    p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
    return is_def;
}

static int is_typedef_name(void) {
    if (cur_tok.type != TK_IDENT) return 0;
    CType t; int sid;
    return typedef_find(cur_tok.name, &t, &sid);
}

static int is_type_start(void) {
    if (cur_tok.type == TK_INT || cur_tok.type == TK_CHAR || cur_tok.type == TK_FLOAT ||
        cur_tok.type == TK_DOUBLE || cur_tok.type == TK_UNSIGNED || cur_tok.type == TK_SHORT ||
        cur_tok.type == TK_LONG || cur_tok.type == TK_CONST || cur_tok.type == TK_STRUCT) return 1;
    return is_typedef_name();
}

static int is_func_def_ahead(void) {
    char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col; int saved_struct = last_struct_id;
    if (!is_type_start()) return 0;
    parse_type_allow_void(1);
    if (cur_tok.type != TK_IDENT) { p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; last_struct_id = saved_struct; return 0; }
    next_token();
    int is_func = (cur_tok.type == TK_LPAREN);
    p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; last_struct_id = saved_struct;
    return is_func;
}

static int is_lvalue_node(ASTNode *n) {
    if (!n) return 0;
    return n->type == AST_VAR || n->type == AST_INDEX || n->type == AST_DEREF || n->type == AST_MEMBER;
}

static ASTNode* parse_primary() {
    if (cur_tok.type == TK_NUM) {
        ASTNode *n = new_node(AST_NUM);
        n->val = cur_tok.val;
        n->ty = TY_INT;
        next_token();
        return n;
    } else if (cur_tok.type == TK_FLOAT_LIT) {
        ASTNode *n = new_node(AST_FLOAT);
        n->fval = cur_tok.fval;
        n->ty = (cur_tok.val == 1) ? TY_FLOAT : TY_DOUBLE;
        next_token();
        return n;
    } else if (cur_tok.type == TK_CHAR_LIT) {
        ASTNode *n = new_node(AST_NUM);
        n->val = cur_tok.val;
        n->ty = TY_CHAR;
        next_token();
        return n;
    } else if (cur_tok.type == TK_STR) {
        ASTNode *n = new_node(AST_STR);
        strcpy(n->str_val, cur_tok.str_val);
        n->ty = TY_CHAR_PTR;
        next_token();
        return n;
    } else if (cur_tok.type == TK_IDENT) {
        char name[64];
        strcpy(name, cur_tok.name);
        for (int i=0; i<enum_cnt; i++) {
            if (strcmp(enum_tab[i].name, name) == 0) {
                next_token();
                ASTNode *n = new_node(AST_NUM);
                n->val = enum_tab[i].val;
                n->ty = TY_INT;
                return n;
            }
        }
        next_token();
        if (cur_tok.type == TK_LPAREN) {
            next_token();
            ASTNode *n = new_node(AST_CALL);
            strcpy(n->name, name);
            FuncSym *fs = func_find(name);
            n->ty = fs ? fs->ret : TY_INT;
            n->struct_id = fs ? fs->ret_struct_id : -1;
            ASTNode *head = NULL, *tail = NULL;
            if (cur_tok.type != TK_RPAREN) {
                head = tail = parse_expr();
                while (cur_tok.type == TK_COMMA) {
                    next_token();
                    tail->next = parse_expr();
                    tail = tail->next;
                }
            }
            expect(TK_RPAREN, "預期 ')'");
            n->left = head;
            return n;
        } else {
            return make_var_node(name);
        }
    } else if (cur_tok.type == TK_LPAREN) {
        next_token();
        ASTNode *n = parse_expr();
        expect(TK_RPAREN, "預期 ')'");
        return n;
    }
    error("預期 表達式 (Expression)");
    return NULL;
}

static ASTNode* parse_postfix() {
    ASTNode *n = parse_primary();
    while (1) {
        if (cur_tok.type == '[') {
            next_token();
            ASTNode *idx = parse_expr();
            expect(']', "預期 ']'");
            ASTNode *nidx = new_node(AST_INDEX);
            nidx->left = n; nidx->right = idx;
            nidx->ty = base_of(n->ty);
            nidx->struct_id = n->struct_id;
            n = nidx;
            continue;
        }
        if (cur_tok.type == '.' || cur_tok.type == TK_ARROW) {
            int is_arrow = (cur_tok.type == TK_ARROW);
            next_token();
            ASTNode *m = new_node(AST_MEMBER);
            m->left = n; strcpy(m->name, cur_tok.name); m->op = is_arrow;
            StructDef *def = get_struct_def(n->struct_id);
            int found = 0;
            for (int i = 0; i < def->field_cnt; i++) {
                if (strcmp(def->fields[i].name, m->name) == 0) {
                    m->ty = def->fields[i].ty;
                    m->val = def->fields[i].offset;
                    m->struct_id = def->fields[i].struct_id;
                    found = 1; break;
                }
            }
            if (!found) error("找不到 struct 欄位");
            next_token();
            n = m;
            continue;
        }
        if (cur_tok.type == TK_PLUSPLUS || cur_tok.type == TK_MINUSMINUS) {
            ASTNode *inc = new_node(AST_INCDEC);
            inc->op = cur_tok.type; inc->is_prefix = 0; inc->left = n; inc->ty = n->ty;
            next_token(); n = inc;
            break;
        }
        break;
    }
    return n;
}

static ASTNode* parse_unary() {
    if (cur_tok.type == TK_SIZEOF) {
        next_token();
        ASTNode *n = new_node(AST_SIZEOF);
        n->ty = TY_INT;
        if (cur_tok.type == TK_LPAREN) {
            char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
            next_token();
            int is_type = 0;
            if (is_type_start()) is_type = 1;
            if (is_type) {
                p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
                expect(TK_LPAREN, "預期 '('");
                CType ty = parse_type_allow_void(0);
                expect(TK_RPAREN, "預期 ')'");
                n->val = type_size(ty, last_struct_id);
                return n;
            }
            p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
        }
        n->left = parse_unary();
        if (n->left && n->left->type == AST_VAR && n->left->array_len > 0) {
            int esz = type_size(base_of(n->left->ty), n->left->struct_id);
            n->val = n->left->array_len * esz;
        } else n->val = type_size(n->left->ty, n->left->struct_id);
        return n;
    }
    if (cur_tok.type == TK_LPAREN) {
        char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
        next_token();
        if (is_type_start()) {
            p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
            expect(TK_LPAREN, "預期 '('");
            CType ty = parse_type_allow_void(0);
            expect(TK_RPAREN, "預期 ')'");
            ASTNode *n = new_node(AST_CAST);
            n->ty = ty; n->struct_id = last_struct_id;
            n->left = parse_unary();
            return n;
        }
        p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
    }
    if (cur_tok.type == TK_MINUS || cur_tok.type == TK_NOT ||
        cur_tok.type == TK_PLUSPLUS || cur_tok.type == TK_MINUSMINUS ||
        cur_tok.type == TK_PLUS || cur_tok.type == TK_MUL || cur_tok.type == '&') {
        TokenType op = cur_tok.type; next_token();
        ASTNode *operand = parse_unary();
        if (op == '&') {
            ASTNode *n = new_node(AST_ADDR); n->left = operand;
            n->ty = ptr_of(operand->ty); n->struct_id = operand->struct_id; return n;
        }
        if (op == TK_MUL) {
            ASTNode *n = new_node(AST_DEREF); n->left = operand;
            n->ty = base_of(operand->ty); n->struct_id = operand->struct_id; return n;
        }
        if (op == TK_PLUSPLUS || op == TK_MINUSMINUS) {
            ASTNode *inc = new_node(AST_INCDEC); inc->op = op; inc->is_prefix = 1;
            inc->left = operand; inc->ty = operand->ty; return inc;
        }
        ASTNode *n = new_node(AST_UNARY); n->op = op; n->left = operand;
        n->ty = (op == TK_MINUS || op == TK_PLUS) ? operand->ty : TY_INT;
        return n;
    }
    return parse_postfix();
}

static ASTNode* parse_mul() {
    ASTNode *n = parse_unary();
    while (cur_tok.type == TK_MUL || cur_tok.type == TK_DIV || cur_tok.type == TK_MOD) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_unary(); n = p;
        p->ty = common_arith_type(p->left->ty, p->right->ty);
    }
    return n;
}

static ASTNode* parse_add() {
    ASTNode *n = parse_mul();
    while (cur_tok.type == TK_PLUS || cur_tok.type == TK_MINUS) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_mul(); n = p;
        if (is_ptr(p->left->ty) && is_ptr(p->right->ty) && p->op == TK_MINUS) p->ty = TY_INT;
        else if (is_ptr(p->left->ty)) { p->ty = p->left->ty; p->struct_id = p->left->struct_id; }
        else if (is_ptr(p->right->ty)) { p->ty = p->right->ty; p->struct_id = p->right->struct_id; }
        else p->ty = common_arith_type(p->left->ty, p->right->ty);
    }
    return n;
}

static ASTNode* parse_rel() {
    ASTNode *n = parse_add();
    while (cur_tok.type == TK_LT || cur_tok.type == TK_GT || cur_tok.type == TK_LE || cur_tok.type == TK_GE) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_add(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_eq() {
    ASTNode *n = parse_rel();
    while (cur_tok.type == TK_EQ || cur_tok.type == TK_NE) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_rel(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_and() {
    ASTNode *n = parse_eq();
    while (cur_tok.type == TK_ANDAND) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_eq(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_or() {
    ASTNode *n = parse_and();
    while (cur_tok.type == TK_OROR) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_and(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_expr() { return parse_or(); }

static ASTNode* parse_lvalue() {
    ASTNode *n = parse_unary();
    if (n->type == AST_VAR || n->type == AST_INDEX || n->type == AST_DEREF || n->type == AST_MEMBER) return n;
    error("預期左值");
    return NULL;
}

static ASTNode* parse_block() {
    expect(TK_LBRACE, "預期 '{'");
    int scope_mark = sym_cnt;
    ASTNode *head = NULL, *tail = NULL;
    while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
        ASTNode *stmt = parse_stmt();
        if (!head) head = tail = stmt; else { tail->next = stmt; tail = stmt; }
    }
    expect(TK_RBRACE, "預期 '}'");
    sym_cnt = scope_mark;
    return head;
}

static ASTNode* parse_struct_decl_or_def() {
    expect(TK_STRUCT, "預期 'struct'");
    char struct_name[64];
    strcpy(struct_name, cur_tok.name); next_token();
    if (cur_tok.type == TK_LBRACE) {
        int sid = struct_find(struct_name);
        if (sid < 0) sid = add_struct_def(struct_name);
        StructDef *def = get_struct_def(sid);
        next_token();
        while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            CType fty = parse_type_allow_void(0);
            StructField *f = &def->fields[def->field_cnt++];
            strcpy(f->name, cur_tok.name);
            f->ty = fty; f->offset = def->size; f->struct_id = last_struct_id;
            def->size += type_size(fty, f->struct_id);
            next_token(); expect(TK_SEMI, "預期 ';'");
        }
        expect(TK_RBRACE, "預期 '}'"); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    error("struct 宣告需有定義"); return NULL;
}

static ASTNode* parse_typedef_stmt() {
    expect(TK_TYPEDEF, "預期 'typedef'");
    if (cur_tok.type == TK_STRUCT) {
        expect(TK_STRUCT, "預期 'struct'");
        char struct_name[64] = {0};
        if (cur_tok.type == TK_IDENT) { strcpy(struct_name, cur_tok.name); next_token(); }
        int sid = -1;
        if (cur_tok.type == TK_LBRACE) {
            if (struct_name[0] == '\0') snprintf(struct_name, sizeof(struct_name), "__anon%d", g_struct_def_cnt);
            sid = struct_find(struct_name);
            if (sid < 0) sid = add_struct_def(struct_name);
            StructDef *def = get_struct_def(sid); next_token();
            while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
                CType fty = parse_type_allow_void(0);
                StructField *f = &def->fields[def->field_cnt++];
                strcpy(f->name, cur_tok.name); f->ty = fty; f->offset = def->size; f->struct_id = last_struct_id;
                def->size += type_size(fty, f->struct_id); next_token(); expect(TK_SEMI, "預期 ';'");
            }
            expect(TK_RBRACE, "預期 '}'");
        } else sid = struct_find(struct_name);
        typedef_add(cur_tok.name, TY_STRUCT, sid);
        next_token(); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    if (cur_tok.type == TK_ENUM) {
        next_token();
        if (cur_tok.type == TK_IDENT) next_token();
        expect(TK_LBRACE, "預期 '{'");
        int enum_val = 0;
        while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            strcpy(enum_tab[enum_cnt].name, cur_tok.name); next_token();
            if (cur_tok.type == TK_ASSIGN) {
                next_token();
                if (cur_tok.type == TK_MINUS) { next_token(); enum_val = -cur_tok.val; }
                else enum_val = cur_tok.val;
                next_token();
            }
            enum_tab[enum_cnt++].val = enum_val++;
            if (cur_tok.type == TK_COMMA) next_token();
        }
        expect(TK_RBRACE, "預期 '}'");
        typedef_add(cur_tok.name, TY_INT, -1);
        next_token(); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    CType ty = parse_type_allow_void(0);
    typedef_add(cur_tok.name, ty, -1);
    next_token(); expect(TK_SEMI, "預期 ';'");
    return NULL;
}

static ASTNode* parse_decl_stmt(int expect_semi) {
    CType decl_ty = parse_type();
    ASTNode *n = new_node(AST_DECL);
    strcpy(n->name, cur_tok.name);
    expect(TK_IDENT, "預期變數名稱");
    n->ty = decl_ty; n->struct_id = last_struct_id; n->array_len = 0; n->init_kind = 0;

    if (cur_tok.type == '[') {
        next_token();
        if (cur_tok.type == ']') { n->array_len = -1; next_token(); }
        else { n->array_len = cur_tok.val; next_token(); expect(']', "預期 ']'"); }
        n->ty = ptr_of(decl_ty);
    }

    if (cur_tok.type == TK_ASSIGN) {
        next_token();
        if (n->array_len != 0) {
            if (cur_tok.type == TK_STR) { n->init_kind = 3; strcpy(n->str_val, cur_tok.str_val); next_token(); }
            else if (cur_tok.type == '{') {
                n->init_kind = 2; next_token();
                ASTNode *head = NULL, *tail = NULL; int count = 0;
                if (cur_tok.type != '}') {
                    head = tail = parse_expr(); count++;
                    while (cur_tok.type == TK_COMMA) { next_token(); if (cur_tok.type == '}') break; tail->next = parse_expr(); tail = tail->next; count++; }
                }
                expect('}', "預期 '}'"); n->left = head;
                if (n->array_len == -1) n->array_len = count;
            }
            if (n->array_len == -1 && n->init_kind == 3) n->array_len = (int)strlen(n->str_val) + 1;
        } else {
            n->init_kind = 1; n->left = parse_expr();
        }
    }
    if (expect_semi) expect(TK_SEMI, "預期 ';'");
    return n;
}

static ASTNode* parse_if_stmt() {
    expect(TK_IF, "預期 'if'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'");
    ASTNode *then_body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt();
    ASTNode *else_body = NULL;
    if (cur_tok.type == TK_ELSE) { next_token(); else_body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); }
    ASTNode *n = new_node(AST_IF); n->cond = cond; n->then_body = then_body; n->else_body = else_body; return n;
}

static ASTNode* parse_while_stmt() {
    expect(TK_WHILE, "預期 'while'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'");
    ASTNode *n = new_node(AST_WHILE); n->cond = cond; n->body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); return n;
}

static ASTNode* parse_for_stmt() {
    expect(TK_FOR, "預期 'for'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *init = NULL, *cond = NULL, *update = NULL;
    if (cur_tok.type != TK_SEMI) {
        if (is_type_start()) { init = parse_decl_stmt(0); sym_add(init->name, init->ty, init->array_len, init->struct_id); }
        else { init = new_node(AST_EXPR_STMT); init->left = parse_expr(); }
    }
    expect(TK_SEMI, "預期 ';'");
    if (cur_tok.type != TK_SEMI) cond = parse_expr();
    expect(TK_SEMI, "預期 ';'");
    if (cur_tok.type != TK_RPAREN) { update = parse_expr(); }
    expect(TK_RPAREN, "預期 ')'");
    ASTNode *n = new_node(AST_FOR); n->init = init; n->cond = cond; n->update = update;
    n->body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); return n;
}

static ASTNode* parse_switch_stmt() {
    expect(TK_SWITCH, "預期 'switch'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'"); expect(TK_LBRACE, "預期 '{'");
    ASTNode *case_head = NULL, *case_tail = NULL;
    while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
        ASTNode *c = new_node(AST_CASE);
        if (cur_tok.type == TK_CASE) { next_token(); c->val = cur_tok.val; c->is_default = 0; next_token(); expect(':', "預期 ':'"); }
        else if (cur_tok.type == TK_DEFAULT) { next_token(); c->is_default = 1; expect(':', "預期 ':'"); }
        ASTNode *head = NULL, *tail = NULL;
        while (cur_tok.type != TK_CASE && cur_tok.type != TK_DEFAULT && cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            ASTNode *stmt = parse_stmt();
            if (!head) head = tail = stmt; else { tail->next = stmt; tail = stmt; }
        }
        c->left = head;
        if (!case_head) case_head = case_tail = c; else { case_tail->next = c; case_tail = c; }
    }
    expect(TK_RBRACE, "預期 '}'");
    ASTNode *sw = new_node(AST_SWITCH); sw->cond = cond; sw->left = case_head; return sw;
}

static ASTNode* parse_do_stmt() {
    expect(TK_DO, "預期 'do'");
    ASTNode *body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt();
    expect(TK_WHILE, "預期 'while'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'"); expect(TK_SEMI, "預期 ';'");
    ASTNode *n = new_node(AST_DO); n->cond = cond; n->body = body; return n;
}

static ASTNode* parse_stmt() {
    if (cur_tok.type == TK_TYPEDEF) { ASTNode *n = parse_typedef_stmt(); if (n) return n; ASTNode *e = new_node(AST_EXPR_STMT); e->left = NULL; return e; }
    if (cur_tok.type == TK_STRUCT && is_struct_def_ahead()) { parse_struct_decl_or_def(); ASTNode *e = new_node(AST_EXPR_STMT); e->left = NULL; return e; }
    if (is_type_start()) { ASTNode *n = parse_decl_stmt(1); sym_add(n->name, n->ty, n->array_len, n->struct_id); return n; }
    if (cur_tok.type == TK_LBRACE) { ASTNode *n = new_node(AST_BLOCK); n->left = parse_block(); return n; }
    if (cur_tok.type == TK_IF) return parse_if_stmt();
    if (cur_tok.type == TK_SWITCH) return parse_switch_stmt();
    if (cur_tok.type == TK_DO) return parse_do_stmt();
    if (cur_tok.type == TK_WHILE) return parse_while_stmt();
    if (cur_tok.type == TK_FOR) return parse_for_stmt();
    if (cur_tok.type == TK_BREAK) { next_token(); ASTNode *n = new_node(AST_BREAK); expect(TK_SEMI, "預期 ';'"); return n; }
    if (cur_tok.type == TK_CONTINUE) { next_token(); ASTNode *n = new_node(AST_CONTINUE); expect(TK_SEMI, "預期 ';'"); return n; }
    if (cur_tok.type == TK_RETURN) {
        next_token(); ASTNode *n = new_node(AST_RETURN);
        if (cur_tok.type == TK_SEMI) { n->left = NULL; n->ty = TY_VOID; next_token(); }
        else { n->left = parse_expr(); n->ty = current_func_ret; expect(TK_SEMI, "預期 ';'"); }
        return n;
    }
    if (cur_tok.type == TK_IDENT || cur_tok.type == TK_MUL) {
        char *s_p = p; Token s_t = cur_tok; int s_l = cur_line, s_c = cur_col;
        ASTNode *cand = parse_unary();
        int is_assign = is_lvalue_node(cand) && (cur_tok.type == TK_ASSIGN || cur_tok.type == TK_PLUSEQ || cur_tok.type == TK_MINUSEQ || cur_tok.type == TK_MULEQ || cur_tok.type == TK_DIVEQ || cur_tok.type == TK_MODEQ);
        p = s_p; cur_tok = s_t; cur_line = s_l; cur_col = s_c;
        if (is_assign) {
            ASTNode *lv = parse_lvalue(); ASTNode *n = new_node(AST_ASSIGN);
            n->left = lv; n->ty = lv->ty; n->op = cur_tok.type; next_token(); n->right = parse_expr(); expect(TK_SEMI, "預期 ';'"); return n;
        }
        ASTNode *n = new_node(AST_EXPR_STMT); n->left = parse_expr(); expect(TK_SEMI, "預期 ';'"); return n;
    }
    error("未知的陳述式"); return NULL;
}

static ASTNode* parse_func() {
    CType ret_ty = parse_type_allow_void(1);
    int ret_struct_id = last_struct_id;
    ASTNode *func = new_node(AST_FUNC);
    func->ty = ret_ty; strcpy(func->name, cur_tok.name);
    expect(TK_IDENT, "預期函數名稱"); expect(TK_LPAREN, "預期 '('");

    sym_reset();
    ASTNode *param_head = NULL, *param_tail = NULL;
    CType param_types[16]; int param_struct_ids[16]; int param_cnt = 0; int is_vararg = 0;
    if (cur_tok.type != TK_RPAREN && cur_tok.type != TK_VOID) {
        while (1) {
            if (cur_tok.type == TK_ELLIPSIS) { is_vararg = 1; next_token(); break; }
            CType pty = parse_type(); ASTNode *param = new_node(AST_DECL);
            if (cur_tok.type == TK_IDENT) { strcpy(param->name, cur_tok.name); next_token(); }
            else sprintf(param->name, ".unnamed%d", param_cnt);
            param->ty = pty; param->array_len = 0; param->struct_id = last_struct_id;
            param_types[param_cnt] = pty; param_struct_ids[param_cnt++] = last_struct_id;
            sym_add(param->name, pty, 0, param->struct_id);
            if (!param_head) param_head = param_tail = param; else { param_tail->next = param; param_tail = param; }
            if (cur_tok.type == TK_COMMA) { next_token(); continue; }
            break;
        }
    } else if (cur_tok.type == TK_VOID) next_token();
    expect(TK_RPAREN, "預期 ')'");
    func_add(func->name, ret_ty, ret_struct_id, param_types, param_struct_ids, param_cnt, is_vararg);
    func->val = is_vararg;
    if (cur_tok.type == TK_SEMI) { next_token(); func->left = param_head; func->is_decl = 1; return func; }
    current_func_ret = ret_ty; func->left = param_head; func->right = parse_block(); func->is_decl = 0; return func;
}

ASTNode* parse_program(void) {
    ASTNode *head = NULL, *tail = NULL;
    while (cur_tok.type != TK_EOF) {
        int is_extern = 0;
        if (cur_tok.type == TK_EXTERN) { is_extern = 1; next_token(); }
        if (cur_tok.type == TK_STRUCT && is_struct_def_ahead()) { parse_struct_decl_or_def(); continue; }
        if (cur_tok.type == TK_TYPEDEF) { parse_typedef_stmt(); continue; }
        if (cur_tok.type == TK_ENUM) {
            next_token();
            if (cur_tok.type == TK_IDENT) next_token();
            expect(TK_LBRACE, "預期 '{'");
            int enum_val = 0;
            while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
                strcpy(enum_tab[enum_cnt].name, cur_tok.name); next_token();
                if (cur_tok.type == TK_ASSIGN) { next_token(); if (cur_tok.type == TK_MINUS) { next_token(); enum_val = -cur_tok.val; } else enum_val = cur_tok.val; next_token(); }
                enum_tab[enum_cnt++].val = enum_val++;
                if (cur_tok.type == TK_COMMA) next_token();
            }
            expect(TK_RBRACE, "預期 '}'"); expect(TK_SEMI, "預期 ';'");
            continue;
        }
        if (is_type_start() && !is_func_def_ahead()) {
            ASTNode *g = parse_decl_stmt(1);
            g->type = AST_GLOBAL; g->is_decl = is_extern;
            global_add(g->name, g->ty, g->array_len, g->struct_id);
            if (!head) head = tail = g; else { tail->next = g; tail = g; }
            continue;
        }
        ASTNode *func = parse_func();
        func->is_decl = func->is_decl || is_extern;
        if (!head) head = tail = func; else { tail->next = func; tail = func; }
    }
    return head;
}