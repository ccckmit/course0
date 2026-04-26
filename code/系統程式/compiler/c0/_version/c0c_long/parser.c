#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------- symbol table (typedef tracking) */

#define MAX_TYPEDEFS 512
#define MAX_ENUM_CONSTS 1024
#define MAX_STRUCT_DEFS 256

typedef struct {
    char *name;
    Type *type;
} TypedefEntry;

typedef struct {
    char    *name;
    long long value;
} EnumConst;

typedef struct {
    char *name;
    Type *type;
} StructEntry;

struct Parser {
    Lexer         *lexer;
    Token          cur;
    TypedefEntry **typedefs;
    int            n_typedefs;
    EnumConst    **enum_consts;
    int            n_enum_consts;
    StructEntry  **structs;
    int            n_structs;
};

static void register_enum_const(Parser *p, const char *name, long long val) {
    if (p->n_enum_consts >= MAX_ENUM_CONSTS) return;
    EnumConst *ec = calloc(1, sizeof(EnumConst));
    ec->name  = strdup(name);
    ec->value = val;
    p->enum_consts[p->n_enum_consts++] = ec;
}

static int lookup_enum_const(Parser *p, const char *name, long long *out) {
    for (int i = 0; i < p->n_enum_consts; i++) {
        EnumConst *ec = p->enum_consts[i];
        if (ec && strcmp(ec->name, name) == 0) {
            *out = ec->value;
            return 1;
        }
    }
    return 0;
}

static void register_struct(Parser *p, const char *name, Type *t) {
    if (p->n_structs >= MAX_STRUCT_DEFS) return;
    StructEntry *se = calloc(1, sizeof(StructEntry));
    se->name = strdup(name);
    se->type = t;
    p->structs[p->n_structs++] = se;
}

static Type *lookup_struct(Parser *p, const char *name) {
    for (int i = p->n_structs - 1; i >= 0; i--) {
        StructEntry *se = p->structs[i];
        if (se && strcmp(se->name, name) == 0)
            return se->type;
    }
    return NULL;
}

static void p_error(Parser *p, const char *msg) {
    fprintf(stderr, "parse error (line %d): %s (got '%s')\n",
            p->cur.line, msg, p->cur.text ? p->cur.text : "?");
    exit(1);
}

/* ---------------------------------------------------------------- token helpers */

static void advance(Parser *p) {
    token_free(p->cur);
    p->cur = lexer_next(p->lexer);
}

static Token peek(Parser *p) {
    return lexer_peek(p->lexer);
}

static int check(Parser *p, TokenType t) { return p->cur.type == t; }

static int match(Parser *p, TokenType t) {
    if (check(p, t)) { advance(p); return 1; }
    return 0;
}

static void expect(Parser *p, TokenType t) {
    if (!check(p, t)) {
        char msg[128];
        snprintf(msg, sizeof msg, "expected %s", token_type_name(t));
        p_error(p, msg);
    }
    advance(p);
}

static char *expect_ident(Parser *p) {
    if (!check(p, TOK_IDENT)) p_error(p, "expected identifier");
    char *s = strdup(p->cur.text);
    advance(p);
    return s;
}

/* ---------------------------------------------------------------- typedef lookup */

static void register_typedef(Parser *p, const char *name, Type *t) {
    if (p->n_typedefs >= MAX_TYPEDEFS) p_error(p, "too many typedefs");
    TypedefEntry *te = calloc(1, sizeof(TypedefEntry));
    te->name = strdup(name);
    te->type = t;
    p->typedefs[p->n_typedefs++] = te;
}

static Type *lookup_typedef(Parser *p, const char *name) {
    for (int i = p->n_typedefs - 1; i >= 0; i--) {
        TypedefEntry *te = p->typedefs[i];
        if (te && strcmp(te->name, name) == 0)
            return te->type;
    }
    return NULL;
}

static int is_gcc_extension(const char *s) {
    return strcmp(s, "__attribute__") == 0 ||
           strcmp(s, "__extension__") == 0 ||
           strcmp(s, "__asm__")       == 0 ||
           strcmp(s, "__asm")         == 0 ||
           strcmp(s, "__inline__")    == 0 ||
           strcmp(s, "__inline")      == 0 ||
           strcmp(s, "__volatile__")  == 0 ||
           strcmp(s, "__volatile")    == 0 ||
           strcmp(s, "__restrict")    == 0 ||
           strcmp(s, "__restrict__")  == 0 ||
           strcmp(s, "__const")       == 0 ||
           strcmp(s, "__const__")     == 0 ||
           strcmp(s, "__signed__")    == 0 ||
           strcmp(s, "__signed")      == 0 ||
           strcmp(s, "__typeof__")    == 0 ||
           strcmp(s, "__typeof")      == 0 ||
           strcmp(s, "__cdecl")       == 0 ||
           strcmp(s, "__declspec")    == 0 ||
           strcmp(s, "__forceinline") == 0 ||
           strcmp(s, "__nonnull")     == 0;
}

/* Skip __attribute__((...)), __asm__("..."), __typeof__(...) and bare
   extension keywords */
static void skip_gcc_extension(Parser *p) {
    for (;;) {
        if (!check(p, TOK_IDENT)) break;
        if (!is_gcc_extension(p->cur.text)) break;
        const char *kw = p->cur.text;
        int has_parens = (strcmp(kw, "__attribute__") == 0 ||
                          strcmp(kw, "__asm__")       == 0 ||
                          strcmp(kw, "__asm")         == 0 ||
                          strcmp(kw, "__typeof__")    == 0 ||
                          strcmp(kw, "__typeof")      == 0 ||
                          strcmp(kw, "__declspec")    == 0);
        advance(p);
        if (has_parens && check(p, TOK_LPAREN)) {
            int depth = 1;
            advance(p);
            while (!check(p, TOK_EOF) && depth > 0) {
                if      (check(p, TOK_LPAREN)) depth++;
                else if (check(p, TOK_RPAREN)) depth--;
                advance(p);
            }
        }
    }
}

static int is_type_start(Parser *p) {
    if (check(p, TOK_IDENT) && is_gcc_extension(p->cur.text)) return 0;
    switch (p->cur.type) {
    case TOK_INT: case TOK_CHAR: case TOK_FLOAT: case TOK_DOUBLE:
    case TOK_VOID: case TOK_LONG: case TOK_SHORT: case TOK_UNSIGNED:
    case TOK_SIGNED: case TOK_STRUCT: case TOK_UNION: case TOK_ENUM:
    case TOK_CONST: case TOK_VOLATILE: case TOK_STATIC: case TOK_EXTERN:
    case TOK_TYPEDEF:
        return 1;
    case TOK_IDENT:
        return lookup_typedef(p, p->cur.text) != NULL;
    default:
        return 0;
    }
}

/* ---------------------------------------------------------------- forward decls */
static Node *parse_expr(Parser *p);
static Node *parse_stmt(Parser *p);
static Node *parse_initializer(Parser *p);
static Type *parse_type_specifier(Parser *p, int *is_typedef, int *is_static, int *is_extern);
static Type *parse_declarator(Parser *p, Type *base, char **out_name);

/* ================================================================ Type parsing */

static Type *parse_struct_union(Parser *p) {
    int is_union = (p->cur.type == TOK_UNION);
    advance(p);  /* eat struct/union */

    char *tag = NULL;
    if (check(p, TOK_IDENT)) {
        tag = strdup(p->cur.text);
        advance(p);
    }

    Type *t   = type_new(is_union ? TY_UNION : TY_STRUCT);
    t->tag    = tag;

    /* parse body */
    if (check(p, TOK_LBRACE)) {
        advance(p);
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            int dummy_td = 0, dummy_st = 0, dummy_ex = 0;
            Type *mtype = parse_type_specifier(p, &dummy_td, &dummy_st, &dummy_ex);
            if (!mtype) { advance(p); continue; }
            char *mname = NULL;
            mtype = parse_declarator(p, mtype, &mname);
            if (mname) {
                t->members = realloc(t->members, (t->n_members + 1) * sizeof(Type*));
                t->params = realloc(t->params, (t->n_members + 1) * sizeof(Param));
                t->members[t->n_members] = mtype;
                t->params[t->n_members].name = mname;
                t->params[t->n_members].type = mtype;
                t->n_members++;
            }
            expect(p, TOK_SEMICOLON);
        }
        expect(p, TOK_RBRACE);
    }
    if (tag) register_struct(p, tag, t);
    return t;
}

static Type *parse_enum_specifier(Parser *p) {
    advance(p);  /* eat enum */
    Type *t = type_new(TY_ENUM);
    if (check(p, TOK_IDENT)) {
        t->tag = strdup(p->cur.text);
        advance(p);
    }
    if (check(p, TOK_LBRACE)) {
        advance(p);
        long long val = 0;
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            if (check(p, TOK_IDENT)) {
                char *name = strdup(p->cur.text);
                advance(p);
                if (match(p, TOK_ASSIGN)) {
                    /* parse constant expression — handle simple cases */
                    if (check(p, TOK_INT_LIT)) {
                        val = (long long)strtoll(p->cur.text, NULL, 0);
                        advance(p);
                    } else if (check(p, TOK_MINUS)) {
                        advance(p);
                        if (check(p, TOK_INT_LIT)) {
                            val = -(long long)strtoll(p->cur.text, NULL, 0);
                            advance(p);
                        }
                    } else if (check(p, TOK_IDENT)) {
                        long long prev;
                        if (lookup_enum_const(p, p->cur.text, &prev)) val = prev;
                        advance(p);
                        if (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
                            int neg = (p->cur.type == TOK_MINUS);
                            advance(p);
                            if (check(p, TOK_INT_LIT)) {
                                long long off = strtoll(p->cur.text, NULL, 0);
                                val = neg ? val - off : val + off;
                                advance(p);
                            }
                        }
                    }
                }
                register_enum_const(p, name, val++);
                free(name);
            } else {
                advance(p); /* skip unexpected tokens */
            }
            if (!match(p, TOK_COMMA)) break;
        }
        expect(p, TOK_RBRACE);
    }
    return t;
}

static Type *parse_type_specifier(Parser *p, int *is_typedef_out,
                                   int *is_static_out, int *is_extern_out) {
    int is_typedef = 0, is_static = 0, is_extern = 0;
    int is_const = 0, is_volatile = 0;
    int is_unsigned = 0, is_signed = 0;
    int is_long = 0, is_longlong = 0, is_short = 0;

    TypeKind base_kind = TY_INT;
    int base_set = 0;
    Type *direct = NULL;

    for (;;) {
        /* skip GCC extension keywords */
        if (check(p, TOK_IDENT) && is_gcc_extension(p->cur.text)) {
            skip_gcc_extension(p);
            continue;
        }
        switch (p->cur.type) {
        case TOK_TYPEDEF:   is_typedef  = 1; advance(p); break;
        case TOK_STATIC:    is_static   = 1; advance(p); break;
        case TOK_EXTERN:    is_extern   = 1; advance(p); break;
        case TOK_CONST:     is_const    = 1; advance(p); break;
        case TOK_VOLATILE:  is_volatile = 1; advance(p); break;
        case TOK_UNSIGNED:  is_unsigned = 1; advance(p); break;
        case TOK_SIGNED:    is_signed   = 1; advance(p); break;
        case TOK_SHORT:     is_short    = 1; advance(p); break;
        case TOK_LONG:
            if (is_long) is_longlong = 1;
            else         is_long     = 1;
            advance(p); break;
        case TOK_VOID:   base_kind = TY_VOID;   base_set = 1; advance(p); break;
        case TOK_CHAR:   base_kind = TY_CHAR;   base_set = 1; advance(p); break;
        case TOK_INT:    base_kind = TY_INT;    base_set = 1; advance(p); break;
        case TOK_FLOAT:  base_kind = TY_FLOAT;  base_set = 1; advance(p); break;
        case TOK_DOUBLE: base_kind = TY_DOUBLE; base_set = 1; advance(p); break;
        case TOK_STRUCT:
        case TOK_UNION:
            direct   = parse_struct_union(p);
            base_set = 1;
            goto parse_type_done;
        case TOK_ENUM:
            direct   = parse_enum_specifier(p);
            base_set = 1;
            goto parse_type_done;
        case TOK_IDENT: {
            Type *td = lookup_typedef(p, p->cur.text);
            if (td) {
                /* clone reference */
                direct = type_new(TY_TYPEDEF_REF);
                direct->name = strdup(p->cur.text);
                base_set = 1;
                advance(p);
                goto parse_type_done;
            }
            Type *st = lookup_struct(p, p->cur.text);
            if (st) {
                direct = st;
                base_set = 1;
                advance(p);
                goto parse_type_done;
            }
            goto parse_type_done;
        }
        default:
            goto parse_type_done;
        }
    }
parse_type_done:;

    if (is_typedef_out)  *is_typedef_out  = is_typedef;
    if (is_static_out)   *is_static_out   = is_static;
    if (is_extern_out)   *is_extern_out   = is_extern;

    if (direct) {
        direct->is_const    = is_const;
        direct->is_volatile = is_volatile;
        return direct;
    }

    if (!base_set && !is_long && !is_short && !is_unsigned && !is_signed)
        return NULL;  /* no type found */

    /* resolve combined specifiers */
    if (base_kind == TY_CHAR) {
        if (is_unsigned) base_kind = TY_UCHAR;
        else if (is_signed) base_kind = TY_SCHAR;
    } else if (is_longlong) {
        base_kind = is_unsigned ? TY_ULONGLONG : TY_LONGLONG;
    } else if (is_long) {
        base_kind = is_unsigned ? TY_ULONG : TY_LONG;
    } else if (is_short) {
        base_kind = is_unsigned ? TY_USHORT : TY_SHORT;
    } else if (base_kind == TY_INT || !base_set) {
        if (is_unsigned) base_kind = TY_UINT;
    }

    Type *t = type_new(base_kind);
    t->is_const    = is_const;
    t->is_volatile = is_volatile;
    return t;
}

/* Parse pointer prefixes and array/function suffixes around a name */
static Type *parse_declarator(Parser *p, Type *base, char **out_name) {
    /* Collect pointer prefixes: they modify the return/element type */
    int ptr_count = 0;
    int ptr_const[16] = {0};
    while (check(p, TOK_STAR) && ptr_count < 16) {
        advance(p);
        ptr_const[ptr_count] = 0;
        while (check(p, TOK_CONST) || check(p, TOK_VOLATILE)) {
            if (check(p, TOK_CONST)) ptr_const[ptr_count] = 1;
            advance(p);
        }
        ptr_count++;
    }

    /* Apply pointer layers to base FIRST (so ptr wraps the base type,
       becoming the return type of any following function declarator) */
    for (int i = ptr_count - 1; i >= 0; i--) {
        Type *pt = type_ptr(base);
        pt->is_const = ptr_const[i];
        base = pt;
    }

    /* direct declarator: name or grouped */
    if (out_name) *out_name = NULL;
    skip_gcc_extension(p);
    if (check(p, TOK_IDENT) && !is_gcc_extension(p->cur.text)) {
        if (out_name) *out_name = strdup(p->cur.text);
        advance(p);
    } else if (check(p, TOK_LPAREN)) {
        /* Could be grouped declarator like (*fp)(args) */
        /* Peek: if next is * or ident that looks like a declarator name,
           treat as grouped; otherwise it's a function call suffix handled below */
        advance(p);
        /* save pos and try grouped declarator */
        base = parse_declarator(p, base, out_name);
        expect(p, TOK_RPAREN);
    }
    skip_gcc_extension(p);

    /* suffixes: array [] and function () */
    for (;;) {
        if (check(p, TOK_IDENT) && is_gcc_extension(p->cur.text)) {
            skip_gcc_extension(p);
            continue;
        }
        if (check(p, TOK_LBRACKET)) {
            advance(p);
            long sz = -1;
            if (!check(p, TOK_RBRACKET)) {
                if (check(p, TOK_INT_LIT)) {
                    sz = (long)atol(p->cur.text);
                    advance(p);
                } else {
                    int depth = 0;
                    while (!check(p, TOK_EOF)) {
                        if (check(p, TOK_LBRACKET)) depth++;
                        if (check(p, TOK_RBRACKET)) { if (depth == 0) break; depth--; }
                        advance(p);
                    }
                }
            }
            expect(p, TOK_RBRACKET);
            base = type_array(base, sz);
        } else if (check(p, TOK_LPAREN)) {
            advance(p);
            Type *ft = type_new(TY_FUNC);
            ft->ret = base;   /* base is already pointer-wrapped return type */
            Param *params = NULL;
            int n = 0;
            int variadic = 0;
            while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
                if (check(p, TOK_ELLIPSIS)) {
                    variadic = 1; advance(p); break;
                }
                int dummy_td = 0, dummy_st = 0, dummy_ex = 0;
                Type *pt = parse_type_specifier(p, &dummy_td, &dummy_st, &dummy_ex);
                if (!pt) break;
                char *pname = NULL;
                pt = parse_declarator(p, pt, &pname);
                params = realloc(params, (n + 1) * sizeof(Param));
                if (!params) { perror("realloc"); exit(1); }
                params[n].name = pname;
                params[n].type = pt;
                n++;
                if (!match(p, TOK_COMMA)) break;
            }
            expect(p, TOK_RPAREN);
            ft->params      = params;
            ft->param_count = n;
            ft->variadic    = variadic;
            base = ft;
        } else {
            break;
        }
    }

    return base;
}

/* ================================================================ Expressions */

static Node *parse_primary(Parser *p);
static Node *parse_postfix(Parser *p);
static Node *parse_unary(Parser *p);
static Node *parse_cast(Parser *p);
static Node *parse_mul(Parser *p);
static Node *parse_add(Parser *p);
static Node *parse_shift(Parser *p);
static Node *parse_relational(Parser *p);
static Node *parse_equality(Parser *p);
static Node *parse_bitand(Parser *p);
static Node *parse_bitxor(Parser *p);
static Node *parse_bitor(Parser *p);
static Node *parse_logand(Parser *p);
static Node *parse_logor(Parser *p);
static Node *parse_ternary(Parser *p);
static Node *parse_assign(Parser *p);

static Node *parse_primary(Parser *p) {
    int line = p->cur.line;
    if (check(p, TOK_INT_LIT)) {
        Node *n = node_new(ND_INT_LIT, line);
        n->ival = (long long)strtoll(p->cur.text, NULL, 0);
        advance(p);
        return n;
    }
    if (check(p, TOK_FLOAT_LIT)) {
        Node *n = node_new(ND_FLOAT_LIT, line);
        n->fval = atof(p->cur.text);
        advance(p);
        return n;
    }
    if (check(p, TOK_CHAR_LIT)) {
        Node *n = node_new(ND_CHAR_LIT, line);
        const char *s = p->cur.text;
        if (s[0] == '\'' && s[1] == '\\') {
            switch (s[2]) {
            case 'n': n->ival = '\n'; break;
            case 't': n->ival = '\t'; break;
            case 'r': n->ival = '\r'; break;
            case '0': n->ival = '\0'; break;
            default:  n->ival = s[2]; break;
            }
        } else {
            n->ival = (unsigned char)s[1];
        }
        advance(p);
        return n;
    }
    if (check(p, TOK_STRING_LIT)) {
        Node *n  = node_new(ND_STRING_LIT, line);
        /* start with first string (strip closing quote) */
        size_t len = strlen(p->cur.text);
        char *combined = malloc(len + 1);
        memcpy(combined, p->cur.text, len - 1); /* drop closing " */
        combined[len - 1] = '\0';
        advance(p);
        /* concatenate any adjacent string literals */
        while (check(p, TOK_STRING_LIT)) {
            const char *next = p->cur.text + 1; /* skip opening " */
            size_t nlen = strlen(next);          /* includes closing " */
            size_t cur_len = strlen(combined);
            combined = realloc(combined, cur_len + nlen + 1);
            memcpy(combined + cur_len, next, nlen);
            combined[cur_len + nlen] = '\0';
            advance(p);
        }
        /* re-add closing quote */
        size_t fl = strlen(combined);
        combined = realloc(combined, fl + 2);
        combined[fl]   = '"';
        combined[fl+1] = '\0';
        n->sval = combined;
        return n;
    }
    if (check(p, TOK_IDENT)) {
        /* check if it's a known enum constant */
        long long eval;
        if (lookup_enum_const(p, p->cur.text, &eval)) {
            Node *n = node_new(ND_INT_LIT, line);
            n->ival = eval;
            advance(p);
            return n;
        }
        Node *n  = node_new(ND_IDENT, line);
        n->sval  = strdup(p->cur.text);
        advance(p);
        return n;
    }
    if (match(p, TOK_LPAREN)) {
        /* check for cast: (type) expr */
        if (is_type_start(p)) {
            int dummy_td = 0, dummy_st = 0, dummy_ex = 0;
            Type *ct = parse_type_specifier(p, &dummy_td, &dummy_st, &dummy_ex);
            if (ct) {
                char *dummy_name = NULL;
                ct = parse_declarator(p, ct, &dummy_name);
                free(dummy_name);
                if (match(p, TOK_RPAREN)) {
                    Node *cast = node_new(ND_CAST, line);
                    cast->cast_type = ct;
                    cast->cast_expr = parse_cast(p);
                    return cast;
                }
            }
        }
        Node *inner = parse_expr(p);
        expect(p, TOK_RPAREN);
        return inner;
    }
    if (check(p, TOK_SIZEOF)) {
        advance(p);
        if (match(p, TOK_LPAREN)) {
            if (is_type_start(p)) {
                int dummy_td = 0, dummy_st = 0, dummy_ex = 0;
                Type *st = parse_type_specifier(p, &dummy_td, &dummy_st, &dummy_ex);
                char *dummy_name = NULL;
                st = parse_declarator(p, st, &dummy_name);
                free(dummy_name);
                expect(p, TOK_RPAREN);
                Node *n    = node_new(ND_SIZEOF_TYPE, line);
                n->cast_type = st;
                return n;
            }
            Node *e = parse_expr(p);
            expect(p, TOK_RPAREN);
            Node *n    = node_new(ND_SIZEOF_EXPR, line);
            node_add_child(n, e);
            return n;
        }
        Node *e = parse_unary(p);
        Node *n = node_new(ND_SIZEOF_EXPR, line);
        node_add_child(n, e);
        return n;
    }
    p_error(p, "expected primary expression");
    return NULL;
}

static Node *parse_postfix(Parser *p) {
    Node *n = parse_primary(p);
    for (;;) {
        int line = p->cur.line;
        if (match(p, TOK_LPAREN)) {
            /* function call */
            Node *call = node_new(ND_CALL, line);
            node_add_child(call, n);
            while (!check(p, TOK_RPAREN) && !check(p, TOK_EOF)) {
                node_add_child(call, parse_assign(p));
                if (!match(p, TOK_COMMA)) break;
            }
            expect(p, TOK_RPAREN);
            n = call;
        } else if (match(p, TOK_LBRACKET)) {
            Node *idx = node_new(ND_INDEX, line);
            node_add_child(idx, n);
            node_add_child(idx, parse_expr(p));
            expect(p, TOK_RBRACKET);
            n = idx;
        } else if (match(p, TOK_DOT)) {
            Node *m = node_new(ND_MEMBER, line);
            m->sval = expect_ident(p);
            node_add_child(m, n);
            n = m;
        } else if (match(p, TOK_ARROW)) {
            Node *m = node_new(ND_ARROW, line);
            m->sval = expect_ident(p);
            node_add_child(m, n);
            n = m;
        } else if (check(p, TOK_INC)) {
            advance(p);
            Node *u = node_new(ND_POST_INC, line);
            node_add_child(u, n);
            n = u;
        } else if (check(p, TOK_DEC)) {
            advance(p);
            Node *u = node_new(ND_POST_DEC, line);
            node_add_child(u, n);
            n = u;
        } else {
            break;
        }
    }
    return n;
}

static Node *parse_unary(Parser *p) {
    int line = p->cur.line;
    if (check(p, TOK_INC)) { advance(p); Node *n = node_new(ND_PRE_INC, line); node_add_child(n, parse_unary(p)); return n; }
    if (check(p, TOK_DEC)) { advance(p); Node *n = node_new(ND_PRE_DEC, line); node_add_child(n, parse_unary(p)); return n; }
    if (check(p, TOK_AMP)) {
        advance(p);
        Node *n = node_new(ND_ADDR, line);
        node_add_child(n, parse_cast(p));
        return n;
    }
    if (check(p, TOK_STAR)) {
        advance(p);
        Node *n = node_new(ND_DEREF, line);
        node_add_child(n, parse_cast(p));
        return n;
    }
    if (check(p, TOK_MINUS) || check(p, TOK_PLUS) ||
        check(p, TOK_BANG)  || check(p, TOK_TILDE)) {
        int op = p->cur.type;
        advance(p);
        Node *n = node_new(ND_UNOP, line);
        n->op   = op;
        node_add_child(n, parse_cast(p));
        return n;
    }
    return parse_postfix(p);
}

static Node *parse_cast(Parser *p) {
    return parse_unary(p);
}

#define BINOP_LEVEL(name, next, ...)                                  \
static Node *name(Parser *p) {                                        \
    Node *left = next(p);                                             \
    TokenType ops[] = { __VA_ARGS__, TOK_EOF };                       \
    for (;;) {                                                        \
        int found = 0;                                                \
        for (int i = 0; ops[i] != TOK_EOF; i++) {                    \
            if (p->cur.type == ops[i]) {                              \
                int line = p->cur.line;                               \
                int op   = p->cur.type;                               \
                advance(p);                                           \
                Node *right = next(p);                                \
                Node *n     = node_new(ND_BINOP, line);               \
                n->op       = op;                                     \
                node_add_child(n, left);                              \
                node_add_child(n, right);                             \
                left  = n;                                            \
                found = 1;                                            \
                break;                                                \
            }                                                         \
        }                                                             \
        if (!found) break;                                            \
    }                                                                 \
    return left;                                                      \
}

BINOP_LEVEL(parse_mul,        parse_cast,        TOK_STAR, TOK_SLASH, TOK_PERCENT)
BINOP_LEVEL(parse_add,        parse_mul,         TOK_PLUS, TOK_MINUS)
BINOP_LEVEL(parse_shift,      parse_add,         TOK_LSHIFT, TOK_RSHIFT)
BINOP_LEVEL(parse_relational, parse_shift,       TOK_LT, TOK_GT, TOK_LEQ, TOK_GEQ)
BINOP_LEVEL(parse_equality,   parse_relational,  TOK_EQ, TOK_NEQ)
BINOP_LEVEL(parse_bitand,     parse_equality,    TOK_AMP)
BINOP_LEVEL(parse_bitxor,     parse_bitand,      TOK_CARET)
BINOP_LEVEL(parse_bitor,      parse_bitxor,      TOK_PIPE)
BINOP_LEVEL(parse_logand,     parse_bitor,       TOK_AND)
BINOP_LEVEL(parse_logor,      parse_logand,      TOK_OR)

static Node *parse_ternary(Parser *p) {
    Node *cond = parse_logor(p);
    if (!check(p, TOK_QUESTION)) return cond;
    int line = p->cur.line;
    advance(p);
    Node *then_e = parse_expr(p);
    expect(p, TOK_COLON);
    Node *else_e = parse_ternary(p);
    Node *n      = node_new(ND_TERNARY, line);
    n->cond = cond;
    node_add_child(n, then_e);
    node_add_child(n, else_e);
    return n;
}

static Node *parse_assign(Parser *p) {
    Node *left = parse_ternary(p);
    int line   = p->cur.line;

    /* Check each assignment operator explicitly — avoids static array init issues */
    {
        TokenType t = p->cur.type;
        if (t == TOK_ASSIGN || t == TOK_PLUS_ASSIGN || t == TOK_MINUS_ASSIGN ||
            t == TOK_STAR_ASSIGN || t == TOK_SLASH_ASSIGN || t == TOK_PERCENT_ASSIGN ||
            t == TOK_AMP_ASSIGN  || t == TOK_PIPE_ASSIGN  || t == TOK_CARET_ASSIGN  ||
            t == TOK_LSHIFT_ASSIGN || t == TOK_RSHIFT_ASSIGN) {
            int op = t;
            advance(p);
            Node *right = parse_assign(p);
            NodeKind nk = (op == TOK_ASSIGN) ? ND_ASSIGN : ND_COMPOUND_ASSIGN;
            Node *n = node_new(nk, line);
            n->op   = op;
            node_add_child(n, left);
            node_add_child(n, right);
            return n;
        }
    }
    return left;
}

static Node *parse_expr(Parser *p) {
    Node *n = parse_assign(p);
    if (check(p, TOK_COMMA)) {
        int line = p->cur.line;
        Node *comma = node_new(ND_COMMA, line);
        node_add_child(comma, n);
        while (match(p, TOK_COMMA))
            node_add_child(comma, parse_assign(p));
        return comma;
    }
    return n;
}

/* ================================================================ Statements */

static Node *parse_block(Parser *p);

static Node *parse_local_decl(Parser *p) {
    int line = p->cur.line;
    int is_typedef = 0, is_static = 0, is_extern = 0;
    Type *base = parse_type_specifier(p, &is_typedef, &is_static, &is_extern);
    if (!base) return NULL;

    /* multiple declarators */
    Node *block_wrap = node_new(ND_BLOCK, line);
    do {
        char *name = NULL;
        Type *t    = parse_declarator(p, base, &name);
        if (is_typedef && name) {
            register_typedef(p, name, t);
            Node *td = node_new(ND_TYPEDEF, line);
            td->typedef_name = name;
            td->typedef_type = t;
            node_add_child(block_wrap, td);
        } else {
            Node *vd = node_new(ND_VAR_DECL, line);
            vd->var_name  = name;
            vd->var_type  = t;
            vd->is_static = is_static;
            vd->is_extern = is_extern;
            /* optional initializer */
            if (match(p, TOK_ASSIGN)) {
                node_add_child(vd, parse_initializer(p));
            }
            node_add_child(block_wrap, vd);
        }
    } while (match(p, TOK_COMMA));
    expect(p, TOK_SEMICOLON);

    /* unwrap if only one decl */
    if (block_wrap->n_children == 1) {
        Node *child = block_wrap->children[0];
        block_wrap->n_children = 0;
        free(block_wrap->children);
        free(block_wrap);
        return child;
    }
    return block_wrap;
}

/* Parse an initializer: either an expression or a { ... } list.
   Returns a node representing the initializer value. */
static Node *parse_initializer(Parser *p) {
    int line = p->cur.line;
    if (!check(p, TOK_LBRACE))
        return parse_assign(p);

    /* brace-enclosed initializer list: skip entirely, emit 0 placeholder */
    advance(p); /* eat { */
    int depth = 1;
    while (!check(p, TOK_EOF) && depth > 0) {
        if      (check(p, TOK_LBRACE)) depth++;
        else if (check(p, TOK_RBRACE)) { depth--; if (depth == 0) break; }
        advance(p);
    }
    expect(p, TOK_RBRACE);
    /* represent as int literal 0 — codegen emits zeroinitializer */
    Node *n = node_new(ND_INT_LIT, line);
    n->ival = 0;
    n->sval = strdup("{init}");
    return n;
}


static Node *parse_stmt(Parser *p) {
    int line = p->cur.line;

    if (check(p, TOK_LBRACE))
        return parse_block(p);

    if (check(p, TOK_IF)) {
        advance(p);
        Node *n = node_new(ND_IF, line);
        expect(p, TOK_LPAREN);
        n->cond = parse_expr(p);
        expect(p, TOK_RPAREN);
        n->then_branch = parse_stmt(p);
        if (match(p, TOK_ELSE))
            n->else_branch = parse_stmt(p);
        return n;
    }

    if (check(p, TOK_WHILE)) {
        advance(p);
        Node *n = node_new(ND_WHILE, line);
        expect(p, TOK_LPAREN);
        n->loop_cond = parse_expr(p);
        expect(p, TOK_RPAREN);
        n->loop_body = parse_stmt(p);
        return n;
    }

    if (check(p, TOK_DO)) {
        advance(p);
        Node *n = node_new(ND_DO_WHILE, line);
        n->loop_body = parse_stmt(p);
        expect(p, TOK_WHILE);
        expect(p, TOK_LPAREN);
        n->loop_cond = parse_expr(p);
        expect(p, TOK_RPAREN);
        expect(p, TOK_SEMICOLON);
        return n;
    }

    if (check(p, TOK_FOR)) {
        advance(p);
        Node *n = node_new(ND_FOR, line);
        expect(p, TOK_LPAREN);
        if (!check(p, TOK_SEMICOLON)) {
            if (is_type_start(p)) n->for_init = parse_local_decl(p);
            else { n->for_init = node_new(ND_EXPR_STMT, line); node_add_child(n->for_init, parse_expr(p)); expect(p, TOK_SEMICOLON); }
        } else advance(p);
        if (!check(p, TOK_SEMICOLON)) n->for_cond = parse_expr(p);
        expect(p, TOK_SEMICOLON);
        if (!check(p, TOK_RPAREN)) n->for_post = parse_expr(p);
        expect(p, TOK_RPAREN);
        n->for_body = parse_stmt(p);
        return n;
    }

    if (check(p, TOK_RETURN)) {
        advance(p);
        Node *n = node_new(ND_RETURN, line);
        if (!check(p, TOK_SEMICOLON))
            n->ret_val = parse_expr(p);
        expect(p, TOK_SEMICOLON);
        return n;
    }

    if (check(p, TOK_BREAK)) {
        advance(p); expect(p, TOK_SEMICOLON);
        return node_new(ND_BREAK, line);
    }

    if (check(p, TOK_CONTINUE)) {
        advance(p); expect(p, TOK_SEMICOLON);
        return node_new(ND_CONTINUE, line);
    }

    if (check(p, TOK_SWITCH)) {
        advance(p);
        Node *n = node_new(ND_SWITCH, line);
        expect(p, TOK_LPAREN);
        n->cond = parse_expr(p);
        expect(p, TOK_RPAREN);
        n->loop_body = parse_stmt(p);
        return n;
    }

    if (check(p, TOK_CASE)) {
        advance(p);
        Node *n = node_new(ND_CASE, line);
        n->cond = parse_expr(p);   /* case value */
        expect(p, TOK_COLON);
        /* collect body statements until next case/default/} */
        Node *body = node_new(ND_BLOCK, line);
        while (!check(p, TOK_CASE) && !check(p, TOK_DEFAULT) &&
               !check(p, TOK_RBRACE) && !check(p, TOK_EOF))
            node_add_child(body, parse_stmt(p));
        node_add_child(n, body);
        return n;
    }

    if (check(p, TOK_DEFAULT)) {
        advance(p);
        expect(p, TOK_COLON);
        Node *n = node_new(ND_DEFAULT, line);
        Node *body = node_new(ND_BLOCK, line);
        while (!check(p, TOK_CASE) && !check(p, TOK_DEFAULT) &&
               !check(p, TOK_RBRACE) && !check(p, TOK_EOF))
            node_add_child(body, parse_stmt(p));
        node_add_child(n, body);
        return n;
    }

    if (check(p, TOK_GOTO)) {
        advance(p);
        Node *n  = node_new(ND_GOTO, line);
        n->sval  = expect_ident(p);
        expect(p, TOK_SEMICOLON);
        return n;
    }

    /* label: ident ':' stmt  (must check before expr-stmt) */
    if (check(p, TOK_IDENT) && peek(p).type == TOK_COLON) {
        Node *n = node_new(ND_LABEL, line);
        n->sval = strdup(p->cur.text);
        advance(p); advance(p); /* eat ident and colon */
        node_add_child(n, parse_stmt(p));
        return n;
    }

    /* local decl */
    if (is_type_start(p))
        return parse_local_decl(p);

    /* expression statement */
    if (check(p, TOK_SEMICOLON)) { advance(p); return node_new(ND_BLOCK, line); }

    Node *n = node_new(ND_EXPR_STMT, line);
    node_add_child(n, parse_expr(p));
    expect(p, TOK_SEMICOLON);
    return n;
}

static Node *parse_block(Parser *p) {
    int line = p->cur.line;
    expect(p, TOK_LBRACE);
    Node *block = node_new(ND_BLOCK, line);
    block->ival = 1;  /* 1 = create new scope */
    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF))
        node_add_child(block, parse_stmt(p));
    expect(p, TOK_RBRACE);
    return block;
}

/* ================================================================ Top-level */

static Node *parse_toplevel(Parser *p) {
    int line = p->cur.line;

    /* skip leading GCC extensions/attributes */
    skip_gcc_extension(p);

    int is_typedef = 0, is_static = 0, is_extern = 0;
    Type *base = parse_type_specifier(p, &is_typedef, &is_static, &is_extern);
    if (!base) {
        p_error(p, "expected declaration");
        return NULL;
    }

    /* handle bare struct/enum with just semicolon */
    if (check(p, TOK_SEMICOLON)) { advance(p); return node_new(ND_BLOCK, line); }

    char *name = NULL;
    Type *t    = parse_declarator(p, base, &name);

    /* skip __attribute__ after declarator, before { or ; */
    skip_gcc_extension(p);

    if (is_typedef) {
        if (name) register_typedef(p, name, t);
        Node *td = node_new(ND_TYPEDEF, line);
        td->typedef_name = name;
        td->typedef_type = t;
        expect(p, TOK_SEMICOLON);
        return td;
    }

    /* function definition */
    if (t->kind == TY_FUNC && check(p, TOK_LBRACE)) {
        Node *fn       = node_new(ND_FUNC_DEF, line);
        fn->func_name  = name;
        fn->func_type  = t;
        fn->is_static  = is_static;
        fn->is_extern  = is_extern;
        /* store param names */
        fn->param_names = malloc(t->param_count * sizeof(char *));
        for (int i = 0; i < t->param_count; i++)
            fn->param_names[i] = t->params[i].name ? strdup(t->params[i].name) : NULL;
        fn->loop_body = parse_block(p);
        return fn;
    }

    /* global variable declaration */
    Node *vd     = node_new(ND_VAR_DECL, line);
    vd->var_name = name;
    vd->var_type = t;
    vd->is_global  = 1;
    vd->is_static  = is_static;
    vd->is_extern  = is_extern;
    if (match(p, TOK_ASSIGN))
        node_add_child(vd, parse_initializer(p));
    /* handle multiple declarators on same line */
    while (match(p, TOK_COMMA)) {
        char *n2 = NULL;
        Type *t2 = parse_declarator(p, base, &n2);
        Node *vd2 = node_new(ND_VAR_DECL, line);
        vd2->var_name = n2; vd2->var_type = t2; vd2->is_global = 1;
        if (match(p, TOK_ASSIGN)) node_add_child(vd2, parse_initializer(p));
        node_add_child(vd, vd2); /* chain */
    }
    expect(p, TOK_SEMICOLON);
    return vd;
}

/* ================================================================ Public API */

Parser *parser_new(Lexer *lexer) {
    Parser *p = calloc(1, sizeof(Parser));
    if (!p) { perror("calloc"); exit(1); }
    p->lexer = lexer;
    p->cur   = lexer_next(lexer);
    /* Allocate pointer arrays for typedef and enum tables */
    p->typedefs    = calloc(MAX_TYPEDEFS,    sizeof(TypedefEntry*));
    p->enum_consts = calloc(MAX_ENUM_CONSTS, sizeof(EnumConst*));
    p->structs     = calloc(MAX_STRUCT_DEFS, sizeof(StructEntry*));

    /* pre-register common standard typedefs so system headers don't need to be parsed.
       Use separate arrays to avoid struct array stride issues in self-hosted IR. */
    {
        /* td_names/td_kinds are provided by c0c_compat.c (static initializers
           compile to null in c0c IR — we use accessor functions instead) */
#ifndef __C0C__
        extern const char *__c0c_get_td_name(int i);
        extern int         __c0c_get_td_kind(int i);
#endif
        for (int i = 0; __c0c_get_td_name(i); i++) {
            Type *t = type_new(__c0c_get_td_kind(i));
            register_typedef(p, __c0c_get_td_name(i), t);
        }
    }
    return p;
}

void parser_free(Parser *p) {
    token_free(p->cur);
    for (int i = 0; i < p->n_typedefs; i++) {
        TypedefEntry *te = p->typedefs[i];
        if (te) { free(te->name); free(te); }
    }
    free(p->typedefs);
    for (int i = 0; i < p->n_enum_consts; i++) {
        EnumConst *ec = p->enum_consts[i];
        if (ec) { free(ec->name); free(ec); }
    }
    free(p->enum_consts);
    for (int i = 0; i < p->n_structs; i++) {
        StructEntry *se = p->structs[i];
        if (se) { free(se->name); free(se); }
    }
    free(p->structs);
    free(p);
}

Node *parser_parse(Parser *p) {
    Node *tu = node_new(ND_TRANSLATION_UNIT, 0);
    while (!check(p, TOK_EOF)) {
        /* skip stray semicolons between declarations */
        while (match(p, TOK_SEMICOLON)) {}
        skip_gcc_extension(p);
        if (check(p, TOK_EOF)) break;
        node_add_child(tu, parse_toplevel(p));
    }
    return tu;
}
