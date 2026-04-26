/*
 * parser.c – Recursive-descent parser for c0c
 *
 * Parses the C subset described in _doc/EBNF.md and builds an AST.
 * The parser uses one token of lookahead (lexer_peek).
 *
 * Type-name vs identifier ambiguity: we maintain a simple typedef set
 * (a hash set of names declared via "typedef") so that we can distinguish
 * typedef-names from ordinary identifiers in type-specifier position.
 */

#include "parser.h"
#include "ast.h"
#include "lexer.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Typedef name set ────────────────────────────────────────────────────── */
#define TYPEDEF_BUCKETS 256

typedef struct TypedefEntry {
    char               *name;
    struct TypedefEntry *next;
} TypedefEntry;

typedef struct {
    TypedefEntry *buckets[TYPEDEF_BUCKETS];
} TypedefSet;

static unsigned typedef_hash(const char *s) {
    unsigned h = 5381;
    while (*s) h = h * 33 ^ (unsigned char)*s++;
    return h % TYPEDEF_BUCKETS;
}

static int typedef_contains(TypedefSet *ts, const char *name) {
    TypedefEntry *e = ts->buckets[typedef_hash(name)];
    while (e) { if (strcmp(e->name, name) == 0) return 1; e = e->next; }
    return 0;
}

static void typedef_add(TypedefSet *ts, AstArena *a, const char *name) {
    unsigned h = typedef_hash(name);
    TypedefEntry *e = ast_arena_alloc(a, (int)sizeof(TypedefEntry));
    e->name = ast_arena_strdup(a, name);
    e->next = ts->buckets[h];
    ts->buckets[h] = e;
}

/* ── Parser struct ───────────────────────────────────────────────────────── */
struct Parser {
    Lexer      *lx;
    AstArena    arena;
    TypedefSet  typedefs;   /* typedef names */
    TypedefSet  struct_tags; /* struct/union/enum tags */
    const char *filename;
    int         errors;
};

/* ── Error / helpers ─────────────────────────────────────────────────────── */
static void parse_error(Parser *p, Token t, const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "%s:%d:%d: error: ", p->filename, t.line, t.col);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    p->errors++;
}

static Token peek(Parser *p) { return lexer_peek(p->lx); }

static Token advance(Parser *p) { return lexer_next(p->lx); }

static Token expect(Parser *p, TokenKind k) {
    Token t = peek(p);
    if (t.kind != k) {
        parse_error(p, t, "expected '%s', got '%s'",
                    token_kind_name(k), token_kind_name(t.kind));
    }
    return advance(p);
}

static int check(Parser *p, TokenKind k) { return peek(p).kind == k; }

static int match(Parser *p, TokenKind k) {
    if (check(p, k)) { advance(p); return 1; }
    return 0;
}

static Node *mknode(Parser *p, NodeKind kind) {
    Token t = peek(p);
    return ast_node(&p->arena, kind, t.line, t.col);
}

static Node *mknode_tok(Parser *p, NodeKind kind, Token t) {
    Node *n = ast_node(&p->arena, kind, t.line, t.col);
    (void)p;
    return n;
}

/* ── Forward declarations ────────────────────────────────────────────────── */
static Node *parse_expr(Parser *p);
static Node *parse_assign_expr(Parser *p);
static Node *parse_cond_expr(Parser *p);
static Node *parse_unary(Parser *p);
static Node *parse_stmt(Parser *p);
static Node *parse_compound(Parser *p);
static Node *parse_type_spec(Parser *p, Node *base);
static Node *parse_declarator(Parser *p, Node *base_type,
                               const char **out_name, int allow_abstract);
static Node *parse_decl(Parser *p, int file_scope);

/* ── Is this token the start of a declaration? ─────────────────────────── */
static int is_type_start(Parser *p) {
    TokenKind k = peek(p).kind;
    switch (k) {
        case TK_VOID: case TK_CHAR: case TK_SHORT: case TK_INT:
        case TK_LONG: case TK_FLOAT: case TK_DOUBLE:
        case TK_SIGNED: case TK_UNSIGNED:
        case TK_STRUCT: case TK_UNION: case TK_ENUM:
        case TK_CONST: case TK_VOLATILE:
        case TK_STATIC: case TK_EXTERN: case TK_AUTO: case TK_REGISTER:
        case TK_TYPEDEF:
            return 1;
        case TK_IDENT:
            return typedef_contains(&p->typedefs, peek(p).text) ||
                   typedef_contains(&p->struct_tags, peek(p).text);
        default:
            return 0;
    }
}

/* ── Type parsing ────────────────────────────────────────────────────────── */

/* Parse optional type qualifiers / storage class into node flags */
static void parse_decl_specs_flags(Parser *p, Node *n) {
    int done = 0;
    while (!done) {
        switch (peek(p).kind) {
            case TK_TYPEDEF:  advance(p); n->is_typedef  = 1; break;
            case TK_EXTERN:   advance(p); n->is_extern   = 1; break;
            case TK_STATIC:   advance(p); n->is_static   = 1; break;
            case TK_AUTO:     advance(p); break;
            case TK_REGISTER: advance(p); break;
            case TK_CONST:    advance(p); n->is_const    = 1; break;
            case TK_VOLATILE: advance(p); n->is_volatile = 1; break;
            case TK_UNSIGNED: advance(p); n->is_unsigned = 1; break;
            case TK_SIGNED:   advance(p); n->is_signed   = 1; break;
            default:          done = 1;
        }
    }
}

/* Parse struct/union body */
static Node *parse_struct_body(Parser *p, NodeKind kind) {
    Token tok = peek(p);
    Node *n = ast_node(&p->arena, kind, tok.line, tok.col);

    advance(p); /* consume 'struct' or 'union' */

    /* optional tag */
    if (check(p, TK_IDENT)) {
        Token tag = advance(p);
        n->sval = ast_arena_strdup(&p->arena, tag.text);
        /* register tag so sizeof(Tag) is recognised as a type */
        typedef_add(&p->struct_tags, &p->arena, tag.text);
    }

    if (!match(p, TK_LBRACE)) return n; /* forward decl */

    /* member list */
    while (!check(p, TK_RBRACE) && !check(p, TK_EOF)) {
        Node *m = mknode(p, ND_STRUCT_MEMBER);
        const char *mname = NULL;
        Node *mtype = parse_declarator(p, parse_type_spec(p, NULL), &mname, 0);
        m->left = mtype;
        m->sval = mname ? ast_arena_strdup(&p->arena, mname) : "";
        expect(p, TK_SEMICOLON);
        n->children = ast_list_append(&p->arena, n->children, m);
    }
    expect(p, TK_RBRACE);
    return n;
}

/* Parse enum */
static Node *parse_enum(Parser *p) {
    Token tok = peek(p);
    Node *n = ast_node(&p->arena, ND_TYPE_ENUM, tok.line, tok.col);
    advance(p); /* consume 'enum' */

    if (check(p, TK_IDENT)) {
        Token tag = advance(p);
        n->sval = ast_arena_strdup(&p->arena, tag.text);
        typedef_add(&p->struct_tags, &p->arena, tag.text);
    }

    if (!match(p, TK_LBRACE)) return n;

    while (!check(p, TK_RBRACE) && !check(p, TK_EOF)) {
        Token etok = expect(p, TK_IDENT);
        Node *e = ast_node(&p->arena, ND_ENUMERATOR, etok.line, etok.col);
        e->sval = ast_arena_strdup(&p->arena, etok.text);
        if (match(p, TK_EQ))
            e->left = parse_cond_expr(p);
        n->children = ast_list_append(&p->arena, n->children, e);
        if (!match(p, TK_COMMA)) break;
    }
    expect(p, TK_RBRACE);
    return n;
}

/* Parse base type specifier (no declarator yet).
   If base != NULL, copy its flags (storage/qualifiers) into the result. */
static Node *parse_type_spec(Parser *p, Node *base) {
    Token tok = peek(p);
    Node *n;

    /* storage / qualifiers */
    Node dummy; memset(&dummy, 0, sizeof(dummy));
    if (!base) base = &dummy;

    /* collect qualifiers before the actual type keyword */
    Node flags; memset(&flags, 0, sizeof(flags));
    if (base) {
        flags.is_typedef  = base->is_typedef;
        flags.is_extern   = base->is_extern;
        flags.is_static   = base->is_static;
        flags.is_const    = base->is_const;
        flags.is_volatile = base->is_volatile;
        flags.is_unsigned = base->is_unsigned;
        flags.is_signed   = base->is_signed;
    }
    parse_decl_specs_flags(p, &flags);
    tok = peek(p);

    /* "unsigned" or "signed" alone (without a following type keyword) means int */
    if ((flags.is_unsigned || flags.is_signed) &&
        tok.kind != TK_VOID && tok.kind != TK_CHAR && tok.kind != TK_SHORT &&
        tok.kind != TK_INT  && tok.kind != TK_LONG && tok.kind != TK_FLOAT &&
        tok.kind != TK_DOUBLE && tok.kind != TK_STRUCT && tok.kind != TK_UNION &&
        tok.kind != TK_ENUM) {
        n = ast_node(&p->arena, ND_TYPE_BASE, tok.line, tok.col);
        n->sval = "int";
        goto apply_flags;
    }

    switch (tok.kind) {
        case TK_VOID: case TK_CHAR: case TK_SHORT: case TK_INT:
        case TK_LONG: case TK_FLOAT: case TK_DOUBLE: {
            advance(p);
            n = ast_node(&p->arena, ND_TYPE_BASE, tok.line, tok.col);
            n->sval = ast_arena_strdup(&p->arena, tok.text);
            /* handle "long long", "unsigned int", etc. */
            if (tok.kind == TK_LONG && check(p, TK_LONG)) { advance(p); n->sval = "long long"; }
            break;
        }
        case TK_STRUCT:
            n = parse_struct_body(p, ND_TYPE_STRUCT);
            break;
        case TK_UNION:
            n = parse_struct_body(p, ND_TYPE_UNION);
            break;
        case TK_ENUM:
            n = parse_enum(p);
            break;
        case TK_IDENT:
            if (typedef_contains(&p->typedefs, tok.text)) {
                advance(p);
                n = ast_node(&p->arena, ND_TYPE_BASE, tok.line, tok.col);
                n->sval = ast_arena_strdup(&p->arena, tok.text);
            } else {
                /* No type – use implicit int (legacy) or error */
                parse_error(p, tok, "expected type, got '%s'", tok.text);
                n = ast_node(&p->arena, ND_TYPE_BASE, tok.line, tok.col);
                n->sval = "int";
            }
            break;
        default:
            parse_error(p, tok, "expected type specifier, got '%s'", tok.text);
            n = ast_node(&p->arena, ND_TYPE_BASE, tok.line, tok.col);
            n->sval = "int";
    }

    /* apply collected flags */
    apply_flags:
    n->is_typedef  = flags.is_typedef;
    n->is_extern   = flags.is_extern;
    n->is_static   = flags.is_static;
    n->is_const    = flags.is_const  || n->is_const;
    n->is_volatile = flags.is_volatile || n->is_volatile;
    n->is_unsigned = flags.is_unsigned || n->is_unsigned;
    n->is_signed   = flags.is_signed   || n->is_signed;

    /* trailing const/volatile after type keyword */
    int done = 0;
    while (!done) {
        switch (peek(p).kind) {
            case TK_CONST:    advance(p); n->is_const    = 1; break;
            case TK_VOLATILE: advance(p); n->is_volatile = 1; break;
            default: done = 1;
        }
    }

    return n;
}

/* Parse pointer chain: "* [qualifiers]" */
static Node *parse_pointer_chain(Parser *p, Node *inner) {
    while (check(p, TK_STAR)) {
        Token t = advance(p); /* consume '*' */
        Node *ptr = ast_node(&p->arena, ND_TYPE_PTR, t.line, t.col);
        ptr->left = inner;
        /* qualifiers on pointer itself */
        int done = 0;
        while (!done) {
            switch (peek(p).kind) {
                case TK_CONST:    advance(p); ptr->is_const    = 1; break;
                case TK_VOLATILE: advance(p); ptr->is_volatile = 1; break;
                default: done = 1;
            }
        }
        inner = ptr;
    }
    return inner;
}

/* Parse parameter list "(params)" → modifies func_type->children */
static void parse_param_list(Parser *p, Node *func_type) {
    expect(p, TK_LPAREN);

    if (check(p, TK_RPAREN)) { advance(p); return; }
    if (check(p, TK_VOID) && /* peek ahead for ')' */ 1) {
        /* could be (void) */
    }

    int first = 1;
    while (!check(p, TK_RPAREN) && !check(p, TK_EOF)) {
        if (!first) expect(p, TK_COMMA);
        first = 0;

        if (check(p, TK_ELLIPSIS)) {
            Token et = advance(p);
            Node *par = ast_node(&p->arena, ND_PARAM, et.line, et.col);
            par->is_ellipsis = 1;
            func_type->children = ast_list_append(&p->arena, func_type->children, par);
            break;
        }

        /* parse type */
        if (!is_type_start(p)) {
            parse_error(p, peek(p), "expected parameter type");
            advance(p);
            break;
        }
        Node *base = parse_type_spec(p, NULL);

        /* special case: (void) with no name */
        if (base->kind == ND_TYPE_BASE && strcmp(base->sval,"void")==0 &&
            check(p, TK_RPAREN)) {
            /* "(void)" – empty param list */
            break;
        }

        const char *pname = NULL;
        Node *ptype = parse_declarator(p, base, &pname, 1); /* abstract ok */

        Token pt = peek(p);
        Node *par = ast_node(&p->arena, ND_PARAM, pt.line, pt.col);
        par->left = ptype;
        par->sval = pname ? ast_arena_strdup(&p->arena, pname) : "";
        func_type->children = ast_list_append(&p->arena, func_type->children, par);
    }
    expect(p, TK_RPAREN);
}

/*
 * parse_declarator – parse the declarator part of a declaration.
 *
 * Given the already-parsed base type, build the full type node by wrapping
 * it with pointer / array / function modifiers.
 *
 * On return:
 *   *out_name = the identifier (NULL for abstract declarators)
 *   return value = the fully-qualified type node
 *
 * allow_abstract: if 1, an identifier is optional (for type-names / params).
 */
static Node *parse_declarator(Parser *p, Node *base_type,
                               const char **out_name, int allow_abstract) {
    /* pointer prefix */
    Node *type = parse_pointer_chain(p, base_type);

    /* grouped declarator:  ( declarator ) */
    if (check(p, TK_LPAREN) && !allow_abstract) {
        /* Disambiguate: is it function call style or grouped declarator?
           If the next token after '(' is '*' or IDENT, it's grouped. */
        /* Simple heuristic: peek at token after '(' */
        /* For now, treat as grouped when we see '*' */
    }

    /* Could be "(*name)" for function pointer */
    if (check(p, TK_LPAREN)) {
        /* Look ahead: if next is '*', this is a grouped pointer declarator */
        /* We handle this by peeking; since lexer only has 1-token peek,
           we use a simpler approach: if we see '(' and then '*', it's grouped. */
        /* Actually, we just try to parse direct_declarator first. */
    }

    /* direct declarator: identifier or (declarator) */
    const char *name = NULL;

    if (check(p, TK_IDENT)) {
        Token t = advance(p);
        name = ast_arena_strdup(&p->arena, t.text);
    } else if (check(p, TK_LPAREN) && (peek(p).kind == TK_LPAREN)) {
        /* grouped – skip for now */
    }
    /* else: abstract */

    if (out_name) *out_name = name;

    /* suffix: [] and () */
    for (;;) {
        if (check(p, TK_LBRACKET)) {
            Token t = advance(p); /* [ */
            Node *arr = ast_node(&p->arena, ND_TYPE_ARRAY, t.line, t.col);
            arr->left = type;
            if (!check(p, TK_RBRACKET))
                arr->right = parse_cond_expr(p); /* size */
            expect(p, TK_RBRACKET);
            type = arr;
        } else if (check(p, TK_LPAREN)) {
            Token t = peek(p);
            Node *func = ast_node(&p->arena, ND_TYPE_FUNC, t.line, t.col);
            func->left = type;
            parse_param_list(p, func);
            type = func;
        } else {
            break;
        }
    }

    return type;
}

/* ── Expression parsing ──────────────────────────────────────────────────── */

static Node *parse_primary(Parser *p) {
    Token t = peek(p);

    switch (t.kind) {
        case TK_IDENT: {
            advance(p);
            Node *n = mknode_tok(p, ND_IDENT, t);
            n->sval = ast_arena_strdup(&p->arena, t.text);
            return n;
        }
        case TK_INT_LIT: {
            advance(p);
            Node *n = mknode_tok(p, ND_INT_LIT, t);
            n->sval = ast_arena_strdup(&p->arena, t.text);
            n->ival = strtoll(t.text, NULL, 0);
            return n;
        }
        case TK_FLOAT_LIT: {
            advance(p);
            Node *n = mknode_tok(p, ND_FLOAT_LIT, t);
            n->sval = ast_arena_strdup(&p->arena, t.text);
            n->fval = strtod(t.text, NULL);
            return n;
        }
        case TK_CHAR_LIT: {
            advance(p);
            Node *n = mknode_tok(p, ND_CHAR_LIT, t);
            n->sval = ast_arena_strdup(&p->arena, t.text);
            return n;
        }
        case TK_STR_LIT: {
            advance(p);
            Node *n = mknode_tok(p, ND_STR_LIT, t);
            /* concatenate adjacent string literals */
            char buf[4096];
            /* strip trailing '"', will re-add after concat */
            int blen = (int)strlen(t.text) - 1; /* drop final '"' */
            if (blen < 0) blen = 0;
            strncpy(buf, t.text, (size_t)blen < sizeof(buf) ? (size_t)blen : sizeof(buf)-1);
            buf[blen < (int)(sizeof(buf)-1) ? blen : (int)(sizeof(buf)-1)] = '\0';
            while (check(p, TK_STR_LIT)) {
                Token nt = advance(p);
                /* append inner content (strip leading '"') */
                const char *inner = nt.text + 1; /* skip '"' */
                int ilen = (int)strlen(inner) - 1; /* drop trailing '"' */
                if (ilen > 0) {
                    int cur_len = (int)strlen(buf);
                    if (cur_len + ilen < (int)sizeof(buf) - 1) {
                        strncat(buf, inner, (size_t)ilen);
                    }
                }
            }
            /* close the string */
            int cur_len = (int)strlen(buf);
            if (cur_len < (int)sizeof(buf) - 1) { buf[cur_len] = '"'; buf[cur_len+1] = '\0'; }
            n->sval = ast_arena_strdup(&p->arena, buf);
            return n;
        }
        case TK_LPAREN: {
            Token lp = advance(p); /* consume '(' */
            /* cast or grouped expr? */
            if (is_type_start(p)) {
                Node *type = parse_type_spec(p, NULL);
                const char *dummy = NULL;
                type = parse_declarator(p, type, &dummy, 1);
                expect(p, TK_RPAREN);
                Token ct = peek(p);
                Node *cast = ast_node(&p->arena, ND_CAST, ct.line, ct.col);
                cast->left  = type;
                cast->right = parse_unary(p);
                return cast;
            }
            (void)lp;
            Node *e = parse_expr(p);
            expect(p, TK_RPAREN);
            return e;
        }
        default:
            parse_error(p, t, "unexpected token '%s' in expression", t.text);
            advance(p);
            Node *n = mknode_tok(p, ND_INT_LIT, t);
            n->sval = "0"; n->ival = 0;
            return n;
    }
}

static Node *parse_postfix(Parser *p) {
    Node *n = parse_primary(p);
    for (;;) {
        Token t = peek(p);
        if (t.kind == TK_LBRACKET) {
            advance(p);
            Node *idx = mknode_tok(p, ND_INDEX, t);
            idx->left  = n;
            idx->right = parse_expr(p);
            expect(p, TK_RBRACKET);
            n = idx;
        } else if (t.kind == TK_LPAREN) {
            advance(p);
            Node *call = mknode_tok(p, ND_CALL, t);
            call->left = n;
            if (!check(p, TK_RPAREN)) {
                int first = 1;
                while (!check(p, TK_RPAREN) && !check(p, TK_EOF)) {
                    if (!first) expect(p, TK_COMMA);
                    first = 0;
                    call->children = ast_list_append(&p->arena,
                                        call->children, parse_assign_expr(p));
                }
            }
            expect(p, TK_RPAREN);
            n = call;
        } else if (t.kind == TK_DOT) {
            advance(p);
            Token mt = expect(p, TK_IDENT);
            Node *m = mknode_tok(p, ND_MEMBER, t);
            m->left = n;
            m->sval = ast_arena_strdup(&p->arena, mt.text);
            n = m;
        } else if (t.kind == TK_ARROW) {
            advance(p);
            Token mt = expect(p, TK_IDENT);
            Node *m = mknode_tok(p, ND_ARROW, t);
            m->left = n;
            m->sval = ast_arena_strdup(&p->arena, mt.text);
            n = m;
        } else if (t.kind == TK_PLUS_PLUS) {
            advance(p);
            Node *u = mknode_tok(p, ND_UNARY_POST, t);
            u->left = n; u->sval = "++";
            n = u;
        } else if (t.kind == TK_MINUS_MINUS) {
            advance(p);
            Node *u = mknode_tok(p, ND_UNARY_POST, t);
            u->left = n; u->sval = "--";
            n = u;
        } else {
            break;
        }
    }
    return n;
}

static Node *parse_unary(Parser *p) {
    Token t = peek(p);
    switch (t.kind) {
        case TK_PLUS_PLUS: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "++"; n->left = parse_unary(p); return n;
        }
        case TK_MINUS_MINUS: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "--"; n->left = parse_unary(p); return n;
        }
        case TK_AMP: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "&"; n->left = parse_unary(p); return n; /* was cast_expr, unary is fine */
        }
        case TK_STAR: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "*"; n->left = parse_unary(p); return n;
        }
        case TK_PLUS: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "+"; n->left = parse_unary(p); return n;
        }
        case TK_MINUS: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "-"; n->left = parse_unary(p); return n;
        }
        case TK_TILDE: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "~"; n->left = parse_unary(p); return n;
        }
        case TK_BANG: {
            advance(p);
            Node *n = mknode_tok(p, ND_UNARY_PRE, t);
            n->sval = "!"; n->left = parse_unary(p); return n;
        }
        case TK_SIZEOF: {
            advance(p); /* consume 'sizeof' */
            if (check(p, TK_LPAREN)) {
                advance(p); /* consume '(' */
                if (is_type_start(p)) {
                    Node *sz = mknode_tok(p, ND_SIZEOF_TYPE, t);
                    Node *type = parse_type_spec(p, NULL);
                    const char *dn = NULL;
                    sz->left = parse_declarator(p, type, &dn, 1);
                    expect(p, TK_RPAREN);
                    return sz;
                }
                /* sizeof(expr) */
                Node *sz = mknode_tok(p, ND_SIZEOF_EXPR, t);
                sz->left = parse_expr(p);
                expect(p, TK_RPAREN);
                return sz;
            }
            /* sizeof expr (no parens) */
            Node *sz = mknode_tok(p, ND_SIZEOF_EXPR, t);
            sz->left = parse_unary(p);
            return sz;
        }
        default:
            return parse_postfix(p);
    }
}

/* Binary operator parsing via Pratt-style precedence table */
typedef struct { TokenKind tk; const char *op; int prec; int right_assoc; } BinOp;
static const BinOp BINOPS[] = {
    {TK_PIPE_PIPE,  "||", 4, 0},
    {TK_AMP_AMP,   "&&", 5, 0},
    {TK_PIPE,      "|",  6, 0},
    {TK_CARET,     "^",  7, 0},
    {TK_AMP,       "&",  8, 0},
    {TK_EQ_EQ,     "==", 9, 0},
    {TK_BANG_EQ,   "!=", 9, 0},
    {TK_LT,        "<", 10, 0},
    {TK_GT,        ">", 10, 0},
    {TK_LT_EQ,    "<=", 10, 0},
    {TK_GT_EQ,    ">=", 10, 0},
    {TK_LT_LT,    "<<", 11, 0},
    {TK_GT_GT,    ">>", 11, 0},
    {TK_PLUS,      "+", 12, 0},
    {TK_MINUS,     "-", 12, 0},
    {TK_STAR,      "*", 13, 0},
    {TK_SLASH,     "/", 13, 0},
    {TK_PERCENT,   "%", 13, 0},
    {0, NULL, 0, 0}
};

static const BinOp *find_binop(TokenKind k) {
    for (int i = 0; BINOPS[i].op; i++)
        if (BINOPS[i].tk == k) return &BINOPS[i];
    return NULL;
}

static Node *parse_binary(Parser *p, int min_prec) {
    Node *lhs = parse_unary(p);
    for (;;) {
        const BinOp *op = find_binop(peek(p).kind);
        if (!op || op->prec < min_prec) break;
        Token t = advance(p);
        int next_prec = op->right_assoc ? op->prec : op->prec + 1;
        Node *rhs = parse_binary(p, next_prec);
        Node *bin = mknode_tok(p, ND_BINARY, t);
        bin->sval  = op->op;
        bin->left  = lhs;
        bin->right = rhs;
        lhs = bin;
    }
    return lhs;
}

static Node *parse_cond_expr(Parser *p) {
    Node *cond = parse_binary(p, 4); /* start at lowest binary prec */
    if (!check(p, TK_QUESTION)) return cond;
    Token qt = advance(p); /* ? */
    Node *then_e = parse_expr(p);
    expect(p, TK_COLON);
    Node *else_e = parse_cond_expr(p);
    Node *tern = mknode_tok(p, ND_TERNARY, qt);
    tern->left  = cond;
    tern->right = then_e;
    tern->extra = else_e;
    return tern;
}

static int is_assign_op(TokenKind k) {
    switch (k) {
        case TK_EQ: case TK_PLUS_EQ: case TK_MINUS_EQ: case TK_STAR_EQ:
        case TK_SLASH_EQ: case TK_PERCENT_EQ: case TK_AMP_EQ:
        case TK_PIPE_EQ: case TK_CARET_EQ: case TK_LT_LT_EQ: case TK_GT_GT_EQ:
            return 1;
        default: return 0;
    }
}

static Node *parse_assign_expr(Parser *p) {
    /* We parse lhs as cond_expr then check for assignment operator.
       This handles the case where lhs is not an lvalue (parser can't tell). */
    Node *lhs = parse_cond_expr(p);
    Token t = peek(p);
    if (is_assign_op(t.kind)) {
        advance(p);
        Node *rhs = parse_assign_expr(p);
        Node *a = mknode_tok(p, ND_ASSIGN, t);
        a->sval  = ast_arena_strdup(&p->arena, t.text);
        a->left  = lhs;
        a->right = rhs;
        return a;
    }
    return lhs;
}

static Node *parse_expr(Parser *p) {
    Node *e = parse_assign_expr(p);
    while (check(p, TK_COMMA)) {
        Token t = advance(p);
        Node *rhs = parse_assign_expr(p);
        Node *comma = mknode_tok(p, ND_COMMA_EXPR, t);
        comma->left  = e;
        comma->right = rhs;
        e = comma;
    }
    return e;
}

/* ── Initializer ─────────────────────────────────────────────────────────── */
static Node *parse_initializer(Parser *p) {
    if (check(p, TK_LBRACE)) {
        Token t = advance(p);
        Node *il = ast_node(&p->arena, ND_INIT_LIST, t.line, t.col);
        while (!check(p, TK_RBRACE) && !check(p, TK_EOF)) {
            /* C99 designators: [expr] = val  or  .name = val */
            if (check(p, TK_LBRACKET)) {
                advance(p); /* [ */
                Node *des = mknode(p, ND_INDEX); /* reuse INDEX as designator */
                des->left = parse_cond_expr(p);
                expect(p, TK_RBRACKET);
                expect(p, TK_EQ);
                des->right = parse_initializer(p);
                il->children = ast_list_append(&p->arena, il->children, des);
            } else if (check(p, TK_DOT)) {
                advance(p); /* . */
                Token nt = expect(p, TK_IDENT);
                Node *des = mknode_tok(p, ND_MEMBER, nt); /* reuse MEMBER */
                des->sval = ast_arena_strdup(&p->arena, nt.text);
                expect(p, TK_EQ);
                des->left = parse_initializer(p);
                il->children = ast_list_append(&p->arena, il->children, des);
            } else {
                il->children = ast_list_append(&p->arena, il->children,
                                                parse_initializer(p));
            }
            if (!match(p, TK_COMMA)) break;
        }
        expect(p, TK_RBRACE);
        return il;
    }
    return parse_assign_expr(p);
}

/* ── Statement parsing ───────────────────────────────────────────────────── */

static Node *parse_decl(Parser *p, int file_scope) {
    (void)file_scope;
    Node *base = parse_type_spec(p, NULL);
    /* may have multiple declarators */
    Node *first = NULL;

    /* Struct/union/enum with no declarator (type-only decl) */
    if (check(p, TK_SEMICOLON)) {
        Node *d = mknode(p, ND_DECL);
        d->left = base;
        d->sval = "";
        return d;
    }

    const char *name = NULL;
    Node *type = parse_declarator(p, base, &name, 0);

    /* function definition check: handled by caller (parse_external_decl) */
    Node *d = mknode(p, ND_DECL);
    d->left = type;
    d->sval = name ? name : "";

    /* Copy typedef/extern/static flags from type to decl */
    d->is_typedef = base->is_typedef;

    /* initializer */
    if (match(p, TK_EQ))
        d->right = parse_initializer(p);

    /* If typedef, register the name */
    if (d->is_typedef && name)
        typedef_add(&p->typedefs, &p->arena, name);

    first = d;

    /* Additional declarators:  int a, b, c; */
    /* For simplicity, we emit multiple ND_DECL nodes chained via 'extra' */
    Node *prev = first;
    while (check(p, TK_COMMA)) {
        advance(p);
        const char *n2 = NULL;
        Node *t2 = parse_declarator(p, base, &n2, 0);
        Node *d2 = mknode(p, ND_DECL);
        d2->left = t2;
        d2->sval = n2 ? n2 : "";
        d2->is_typedef = base->is_typedef;
        if (match(p, TK_EQ))
            d2->right = parse_initializer(p);
        if (d2->is_typedef && n2)
            typedef_add(&p->typedefs, &p->arena, n2);
        prev->extra = d2;
        prev = d2;
    }

    return first;
}

static Node *parse_compound(Parser *p) {
    Token t = expect(p, TK_LBRACE);
    Node *block = ast_node(&p->arena, ND_COMPOUND, t.line, t.col);

    while (!check(p, TK_RBRACE) && !check(p, TK_EOF)) {
        Node *item;
        if (is_type_start(p)) {
            /* declaration – may be chained (int a, b;) */
            Node *d = parse_decl(p, 0);
            expect(p, TK_SEMICOLON);
            /* flatten chained decls */
            for (Node *cur = d; cur; cur = cur->extra)
                block->children = ast_list_append(&p->arena, block->children, cur);
        } else {
            item = parse_stmt(p);
            block->children = ast_list_append(&p->arena, block->children, item);
        }
    }
    expect(p, TK_RBRACE);
    return block;
}

static Node *parse_stmt(Parser *p) {
    Token t = peek(p);

    /* labelled statement: IDENT ':' */
    if (t.kind == TK_IDENT) {
        /* peek 2 tokens ahead: if next is ':', it's a label */
        Token saved = lexer_next(p->lx);
        Token colon = lexer_peek(p->lx);
        if (colon.kind == TK_COLON) {
            /* It IS a label */
            lexer_next(p->lx); /* consume ':' */
            Node *label = ast_node(&p->arena, ND_LABEL, t.line, t.col);
            label->sval  = ast_arena_strdup(&p->arena, saved.text);
            label->left  = parse_stmt(p);
            return label;
        }
        /* Not a label – put the two tokens back via a trick:
           we can't un-consume from our lexer.  Use a simple approach:
           re-create the expression starting with 'saved' as primary. */
        /* Build the ident node manually and continue parsing postfix/binary */
        Node *ident = ast_node(&p->arena, ND_IDENT, saved.line, saved.col);
        ident->sval = ast_arena_strdup(&p->arena, saved.text);

        /* parse rest of expr starting from postfix */
        /* We feed the already-consumed token by using a wrapper: */
        /* Actually we just handle the common case: the ident is the entire
           primary, then we parse postfix/binary from current position. */
        Node *e = ident;
        /* postfix */
        for (;;) {
            Token pt = peek(p);
            if (pt.kind == TK_LBRACKET) {
                advance(p);
                Node *idx = mknode_tok(p, ND_INDEX, pt);
                idx->left = e; idx->right = parse_expr(p);
                expect(p, TK_RBRACKET);
                e = idx;
            } else if (pt.kind == TK_LPAREN) {
                advance(p);
                Node *call = mknode_tok(p, ND_CALL, pt);
                call->left = e;
                if (!check(p, TK_RPAREN)) {
                    int first = 1;
                    while (!check(p, TK_RPAREN) && !check(p, TK_EOF)) {
                        if (!first) expect(p, TK_COMMA);
                        first = 0;
                        call->children = ast_list_append(&p->arena,
                                            call->children, parse_assign_expr(p));
                    }
                }
                expect(p, TK_RPAREN);
                e = call;
            } else if (pt.kind == TK_DOT) {
                advance(p);
                Token mt = expect(p, TK_IDENT);
                Node *m = mknode_tok(p, ND_MEMBER, pt);
                m->left = e; m->sval = ast_arena_strdup(&p->arena, mt.text);
                e = m;
            } else if (pt.kind == TK_ARROW) {
                advance(p);
                Token mt = expect(p, TK_IDENT);
                Node *m = mknode_tok(p, ND_ARROW, pt);
                m->left = e; m->sval = ast_arena_strdup(&p->arena, mt.text);
                e = m;
            } else if (pt.kind == TK_PLUS_PLUS) {
                advance(p);
                Node *u = mknode_tok(p, ND_UNARY_POST, pt);
                u->left = e; u->sval = "++"; e = u;
            } else if (pt.kind == TK_MINUS_MINUS) {
                advance(p);
                Node *u = mknode_tok(p, ND_UNARY_POST, pt);
                u->left = e; u->sval = "--"; e = u;
            } else {
                break;
            }
        }
        /* binary ops */
        {
            const BinOp *op;
            while ((op = find_binop(peek(p).kind)) != NULL) {
                Token bt = advance(p);
                Node *rhs = parse_binary(p, op->prec + 1);
                Node *bin = mknode_tok(p, ND_BINARY, bt);
                bin->sval = op->op; bin->left = e; bin->right = rhs;
                e = bin;
            }
        }
        /* ternary */
        if (check(p, TK_QUESTION)) {
            Token qt = advance(p);
            Node *then_e = parse_expr(p);
            expect(p, TK_COLON);
            Node *else_e = parse_cond_expr(p);
            Node *tern = mknode_tok(p, ND_TERNARY, qt);
            tern->left = e; tern->right = then_e; tern->extra = else_e;
            e = tern;
        }
        /* assignment */
        if (is_assign_op(peek(p).kind)) {
            Token at = advance(p);
            Node *rhs = parse_assign_expr(p);
            Node *a = mknode_tok(p, ND_ASSIGN, at);
            a->sval = ast_arena_strdup(&p->arena, at.text);
            a->left = e; a->right = rhs;
            e = a;
        }
        /* comma */
        while (check(p, TK_COMMA)) {
            Token ct = advance(p);
            Node *rhs = parse_assign_expr(p);
            Node *comma = mknode_tok(p, ND_COMMA_EXPR, ct);
            comma->left = e; comma->right = rhs;
            e = comma;
        }
        expect(p, TK_SEMICOLON);
        Node *es = mknode(p, ND_EXPR_STMT);
        es->left = e;
        return es;
    }

    switch (t.kind) {
        case TK_LBRACE:
            return parse_compound(p);

        case TK_SEMICOLON:
            advance(p);
            return mknode_tok(p, ND_EMPTY_STMT, t);

        case TK_IF: {
            advance(p);
            Node *n = mknode_tok(p, ND_IF, t);
            expect(p, TK_LPAREN);
            n->left = parse_expr(p);
            expect(p, TK_RPAREN);
            n->right = parse_stmt(p);
            if (check(p, TK_ELSE)) { advance(p); n->extra = parse_stmt(p); }
            return n;
        }

        case TK_WHILE: {
            advance(p);
            Node *n = mknode_tok(p, ND_WHILE, t);
            expect(p, TK_LPAREN);
            n->left = parse_expr(p);
            expect(p, TK_RPAREN);
            n->right = parse_stmt(p);
            return n;
        }

        case TK_DO: {
            advance(p);
            Node *n = mknode_tok(p, ND_DO_WHILE, t);
            n->left = parse_stmt(p);
            expect(p, TK_WHILE);
            expect(p, TK_LPAREN);
            n->right = parse_expr(p);
            expect(p, TK_RPAREN);
            expect(p, TK_SEMICOLON);
            return n;
        }

        case TK_FOR: {
            advance(p);
            Node *n = mknode_tok(p, ND_FOR, t);
            expect(p, TK_LPAREN);
            /* init */
            if (check(p, TK_SEMICOLON)) { advance(p); }
            else if (is_type_start(p)) {
                n->left = parse_decl(p, 0);
                expect(p, TK_SEMICOLON);
            } else {
                Node *es = mknode(p, ND_EXPR_STMT);
                es->left = parse_expr(p);
                n->left = es->left;
                expect(p, TK_SEMICOLON);
            }
            /* cond */
            if (!check(p, TK_SEMICOLON)) n->right = parse_expr(p);
            expect(p, TK_SEMICOLON);
            /* step */
            if (!check(p, TK_RPAREN)) n->extra = parse_expr(p);
            expect(p, TK_RPAREN);
            /* body */
            n->children = ast_list_append(&p->arena, n->children, parse_stmt(p));
            return n;
        }

        case TK_RETURN: {
            advance(p);
            Node *n = mknode_tok(p, ND_RETURN, t);
            if (!check(p, TK_SEMICOLON)) n->left = parse_expr(p);
            expect(p, TK_SEMICOLON);
            return n;
        }

        case TK_BREAK: {
            advance(p);
            expect(p, TK_SEMICOLON);
            return mknode_tok(p, ND_BREAK, t);
        }

        case TK_CONTINUE: {
            advance(p);
            expect(p, TK_SEMICOLON);
            return mknode_tok(p, ND_CONTINUE, t);
        }

        case TK_GOTO: {
            advance(p);
            Node *n = mknode_tok(p, ND_GOTO, t);
            Token lt = expect(p, TK_IDENT);
            n->sval = ast_arena_strdup(&p->arena, lt.text);
            expect(p, TK_SEMICOLON);
            return n;
        }

        case TK_SWITCH: {
            advance(p);
            Node *n = mknode_tok(p, ND_SWITCH, t);
            expect(p, TK_LPAREN);
            n->left = parse_expr(p);
            expect(p, TK_RPAREN);
            n->right = parse_stmt(p);
            return n;
        }

        case TK_CASE: {
            advance(p);
            Node *n = mknode_tok(p, ND_CASE, t);
            n->left = parse_cond_expr(p);
            expect(p, TK_COLON);
            n->right = parse_stmt(p);
            return n;
        }

        case TK_DEFAULT: {
            advance(p);
            Node *n = mknode_tok(p, ND_DEFAULT, t);
            expect(p, TK_COLON);
            n->left = parse_stmt(p);
            return n;
        }

        default: {
            /* expression statement */
            Node *e = parse_expr(p);
            expect(p, TK_SEMICOLON);
            Node *es = mknode_tok(p, ND_EXPR_STMT, t);
            es->left = e;
            return es;
        }
    }
}

/* ── Top-level ───────────────────────────────────────────────────────────── */

static Node *parse_external_decl(Parser *p) {
    /* Parse decl_specifiers + first declarator */
    Node *base = parse_type_spec(p, NULL);

    /* struct/union/enum with no declarator */
    if (check(p, TK_SEMICOLON)) {
        advance(p);
        Node *d = ast_node(&p->arena, ND_DECL, base->line, base->col);
        d->left = base;
        d->sval = "";
        return d;
    }

    const char *name = NULL;
    Node *type = parse_declarator(p, base, &name, 0);

    /* Function definition: declarator followed by '{' */
    if (check(p, TK_LBRACE) && type->kind == ND_TYPE_FUNC) {
        Node *fn = ast_node(&p->arena, ND_FUNC_DEF, base->line, base->col);
        fn->left  = type->left; /* return type */
        fn->sval  = name ? name : "";
        /* params: steal children from type node */
        fn->children = type->children;
        /* copy storage flags from base */
        fn->is_static = base->is_static;
        fn->is_extern = base->is_extern;
        fn->right = parse_compound(p);
        return fn;
    }

    /* Global variable declaration */
    Node *d = ast_node(&p->arena, ND_DECL, base->line, base->col);
    d->left = type;
    d->sval = name ? name : "";
    d->is_typedef = base->is_typedef;
    d->is_static  = base->is_static;
    d->is_extern  = base->is_extern;

    if (match(p, TK_EQ))
        d->right = parse_initializer(p);

    if (d->is_typedef && name)
        typedef_add(&p->typedefs, &p->arena, name);

    /* additional declarators */
    Node *prev = d;
    while (check(p, TK_COMMA)) {
        advance(p);
        const char *n2 = NULL;
        Node *t2 = parse_declarator(p, base, &n2, 0);
        Node *d2 = ast_node(&p->arena, ND_DECL, t2->line, t2->col);
        d2->left = t2;
        d2->sval = n2 ? n2 : "";
        d2->is_typedef = base->is_typedef;
        if (match(p, TK_EQ))
            d2->right = parse_initializer(p);
        if (d2->is_typedef && n2)
            typedef_add(&p->typedefs, &p->arena, n2);
        prev->extra = d2;
        prev = d2;
    }

    expect(p, TK_SEMICOLON);
    return d;
}

Node *parser_parse(Parser *p) {
    Token t = peek(p);
    Node *root = ast_node(&p->arena, ND_TRANSLATION_UNIT, t.line, t.col);

    while (!check(p, TK_EOF)) {
        Node *ext = parse_external_decl(p);
        /* flatten chained decls */
        for (Node *cur = ext; cur; cur = cur->extra)
            root->children = ast_list_append(&p->arena, root->children, cur);
        if (p->errors > 20) {
            fprintf(stderr, "too many errors, aborting\n");
            break;
        }
    }
    return root;
}

/* ── Public API ──────────────────────────────────────────────────────────── */

Parser *parser_new_file(const char *filename) {
    Lexer *lx = lexer_new_file(filename);
    if (!lx) return NULL;
    Parser *p = calloc(1, sizeof(Parser));
    if (!p) { perror("calloc"); exit(1); }
    p->lx       = lx;
    p->filename = filename;
    memset(&p->arena, 0, sizeof(p->arena));
    memset(&p->typedefs, 0, sizeof(p->typedefs));
    memset(&p->struct_tags, 0, sizeof(p->struct_tags));
    /* Pre-register common standard typedefs */
    static const char *std_typedefs[] = {
        "size_t", "ssize_t", "ptrdiff_t", "intptr_t", "uintptr_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "int8_t",  "int16_t",  "int32_t",  "int64_t",
        "off_t", "pid_t", "uid_t", "gid_t",
        "FILE", "DIR", "va_list", "__builtin_va_list",
        "bool", "wchar_t", "wint_t",
        NULL
    };
    for (int i = 0; std_typedefs[i]; i++)
        typedef_add(&p->typedefs, &p->arena, std_typedefs[i]);
    return p;
}

int parser_errors(Parser *p) { return p ? p->errors : 0; }

void parser_free(Parser *p) {
    if (!p) return;
    lexer_free(p->lx);
    ast_arena_free(&p->arena);
    free(p);
}

/* ── Self-test ───────────────────────────────────────────────────────────── */
int parser_test(const char *filename) {
    Parser *p = parser_new_file(filename);
    if (!p) return 1;

    Node *root = parser_parse(p);
    if (!root || p->errors) {
        fprintf(stderr, "parser_test: parse errors in %s\n", filename);
        parser_free(p);
        return 1;
    }

    /* Count top-level decls */
    int count = 0;
    for (NodeList *nl = root->children; nl; nl = nl->next) count++;

    /* Dump to stderr for inspection */
    fprintf(stderr, "parser_test: %s → %d top-level decls\n",
            filename, count);

    parser_free(p);
    return 0;
}

/* ── main ────────────────────────────────────────────────────────────────── */
#ifdef PARSER_STANDALONE
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: parser_test <file.c> ...\n");
        return 1;
    }
    int rc = 0;
    for (int i = 1; i < argc; i++) {
        fprintf(stderr, "=== Parsing %s ===\n", argv[i]);
        Parser *p = parser_new_file(argv[i]);
        if (!p) { rc = 1; continue; }
        Node *root = parser_parse(p);
        if (root && parser_errors(p) == 0) {
            ast_to_c(root, stdout);
        }
        if (parser_errors(p)) rc = 1;
        parser_free(p);
    }
    return rc;
}
#endif /* PARSER_STANDALONE */
