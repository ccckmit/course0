#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ---------------------------------------------------------------- symbol table (typedef tracking) */

#define MAX_TYPEDEFS 512

typedef struct {
    char *name;
    Type *type;
} TypedefEntry;

struct Parser {
    Lexer       *lexer;
    Token        cur;
    TypedefEntry typedefs[MAX_TYPEDEFS];
    int          n_typedefs;
};

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
    p->typedefs[p->n_typedefs].name = strdup(name);
    p->typedefs[p->n_typedefs].type = t;
    p->n_typedefs++;
}

static Type *lookup_typedef(Parser *p, const char *name) {
    for (int i = p->n_typedefs - 1; i >= 0; i--)
        if (strcmp(p->typedefs[i].name, name) == 0)
            return p->typedefs[i].type;
    return NULL;
}

static int is_type_start(Parser *p) {
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

    /* optional body – we skip it for now (just track the tag) */
    if (check(p, TOK_LBRACE)) {
        advance(p);
        int depth = 1;
        while (!check(p, TOK_EOF) && depth > 0) {
            if      (check(p, TOK_LBRACE)) depth++;
            else if (check(p, TOK_RBRACE)) depth--;
            advance(p);
        }
    }
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
        long val = 0;
        while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
            /* just skip enum body */
            if (check(p, TOK_COMMA)) advance(p);
            else advance(p);
            (void)val;
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
            goto done;
        case TOK_ENUM:
            direct   = parse_enum_specifier(p);
            base_set = 1;
            goto done;
        case TOK_IDENT: {
            Type *td = lookup_typedef(p, p->cur.text);
            if (td) {
                /* clone reference */
                direct = type_new(TY_TYPEDEF_REF);
                direct->name = strdup(p->cur.text);
                base_set = 1;
                advance(p);
                goto done;
            }
            goto done;
        }
        default:
            goto done;
        }
    }
done:;

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
    /* pointer prefixes */
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

    /* direct declarator: name or grouped */
    if (out_name) *out_name = NULL;
    if (check(p, TOK_IDENT)) {
        if (out_name) *out_name = strdup(p->cur.text);
        advance(p);
    } else if (check(p, TOK_LPAREN)) {
        /* grouped declarator – handle minimally */
        advance(p);
        base = parse_declarator(p, base, out_name);
        expect(p, TOK_RPAREN);
    }

    /* suffixes: array [] and function () */
    for (;;) {
        if (check(p, TOK_LBRACKET)) {
            advance(p);
            long sz = -1;
            if (!check(p, TOK_RBRACKET)) {
                /* parse constant expr minimally */
                if (check(p, TOK_INT_LIT)) {
                    sz = (long)atol(p->cur.text);
                    advance(p);
                } else {
                    /* skip expression */
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
            ft->ret = base;
            /* parse params */
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
                params = realloc(params, (n + 1) * sizeof *params);
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

    /* apply pointer layers (outermost first) */
    for (int i = ptr_count - 1; i >= 0; i--) {
        base = type_ptr(base);
        base->is_const = ptr_const[i];
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
        n->sval  = strdup(p->cur.text);
        advance(p);
        return n;
    }
    if (check(p, TOK_IDENT)) {
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

    static const TokenType assign_ops[] = {
        TOK_ASSIGN,
        TOK_PLUS_ASSIGN, TOK_MINUS_ASSIGN, TOK_STAR_ASSIGN, TOK_SLASH_ASSIGN,
        TOK_PERCENT_ASSIGN, TOK_AMP_ASSIGN, TOK_PIPE_ASSIGN, TOK_CARET_ASSIGN,
        TOK_LSHIFT_ASSIGN, TOK_RSHIFT_ASSIGN,
        TOK_EOF
    };

    for (int i = 0; assign_ops[i] != TOK_EOF; i++) {
        if (p->cur.type == assign_ops[i]) {
            int op = p->cur.type;
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
                node_add_child(vd, parse_assign(p));
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
    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF))
        node_add_child(block, parse_stmt(p));
    expect(p, TOK_RBRACE);
    return block;
}

/* ================================================================ Top-level */

static Node *parse_toplevel(Parser *p) {
    int line = p->cur.line;

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
        node_add_child(vd, parse_assign(p));
    /* handle multiple declarators on same line */
    while (match(p, TOK_COMMA)) {
        char *n2 = NULL;
        Type *t2 = parse_declarator(p, base, &n2);
        Node *vd2 = node_new(ND_VAR_DECL, line);
        vd2->var_name = n2; vd2->var_type = t2; vd2->is_global = 1;
        if (match(p, TOK_ASSIGN)) node_add_child(vd2, parse_assign(p));
        node_add_child(vd, vd2); /* chain */
    }
    expect(p, TOK_SEMICOLON);
    return vd;
}

/* ================================================================ Public API */

Parser *parser_new(Lexer *lexer) {
    Parser *p = calloc(1, sizeof *p);
    if (!p) { perror("calloc"); exit(1); }
    p->lexer = lexer;
    p->cur   = lexer_next(lexer);
    return p;
}

void parser_free(Parser *p) {
    token_free(p->cur);
    for (int i = 0; i < p->n_typedefs; i++) free(p->typedefs[i].name);
    free(p);
}

Node *parser_parse(Parser *p) {
    Node *tu = node_new(ND_TRANSLATION_UNIT, 0);
    while (!check(p, TOK_EOF))
        node_add_child(tu, parse_toplevel(p));
    return tu;
}