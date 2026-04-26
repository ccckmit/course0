/*
 * ast.c – AST arena allocator and ast_to_c() pretty-printer
 *
 * ast_to_c() walks the AST and reproduces valid C source.
 * The output is intended to be compilable and semantically identical
 * to the original source (modulo whitespace / comment loss).
 */

#include "ast.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Arena ───────────────────────────────────────────────────────────────── */
#define ARENA_BLOCK_SZ 65536

void *ast_arena_alloc(AstArena *a, int n) {
    /* Round up to 16-byte alignment */
    int aligned = (n + 15) & ~15;
    if (!a->head || a->head->used + aligned > a->head->cap) {
        int cap = aligned > ARENA_BLOCK_SZ ? aligned : ARENA_BLOCK_SZ;
        AstArenaBlock *b = malloc(sizeof(AstArenaBlock));
        if (!b) { perror("malloc"); exit(1); }
        b->data = malloc((size_t)cap);
        if (!b->data) { perror("malloc"); exit(1); }
        b->used = 0;
        b->cap  = cap;
        b->next = a->head;
        a->head = b;
    }
    void *p = a->head->data + a->head->used;
    a->head->used += aligned;
    return p;
}

char *ast_arena_strdup(AstArena *a, const char *s) {
    int len = (int)strlen(s);
    char *p = ast_arena_alloc(a, len + 1);
    memcpy(p, s, (size_t)len + 1);
    return p;
}

void ast_arena_free(AstArena *a) {
    AstArenaBlock *b = a->head;
    while (b) {
        AstArenaBlock *next = b->next;
        free(b->data);
        free(b);
        b = next;
    }
    a->head = NULL;
}

/* ── Node constructors ───────────────────────────────────────────────────── */
Node *ast_node(AstArena *a, NodeKind kind, int line, int col) {
    Node *n = ast_arena_alloc(a, (int)sizeof(Node));
    memset(n, 0, sizeof(Node));
    n->kind = kind;
    n->line = line;
    n->col  = col;
    return n;
}

NodeList *ast_list_append(AstArena *a, NodeList *list, Node *node) {
    NodeList *item = ast_arena_alloc(a, (int)sizeof(NodeList));
    item->node = node;
    item->next = NULL;
    if (!list) return item;
    NodeList *p = list;
    while (p->next) p = p->next;
    p->next = item;
    return list;
}

/* ── Debug dump ──────────────────────────────────────────────────────────── */
static const char *node_kind_name(NodeKind k) {
    switch (k) {
#define X(n) case n: return #n;
        X(ND_TRANSLATION_UNIT) X(ND_FUNC_DEF) X(ND_DECL) X(ND_PARAM)
        X(ND_TYPE_BASE) X(ND_TYPE_PTR) X(ND_TYPE_ARRAY) X(ND_TYPE_FUNC)
        X(ND_TYPE_STRUCT) X(ND_TYPE_UNION) X(ND_TYPE_ENUM)
        X(ND_STRUCT_MEMBER) X(ND_ENUMERATOR)
        X(ND_COMPOUND) X(ND_IF) X(ND_WHILE) X(ND_DO_WHILE) X(ND_FOR)
        X(ND_RETURN) X(ND_BREAK) X(ND_CONTINUE) X(ND_GOTO) X(ND_LABEL)
        X(ND_SWITCH) X(ND_CASE) X(ND_DEFAULT)
        X(ND_EXPR_STMT) X(ND_EMPTY_STMT)
        X(ND_ASSIGN) X(ND_TERNARY) X(ND_BINARY)
        X(ND_UNARY_PRE) X(ND_UNARY_POST)
        X(ND_CAST) X(ND_SIZEOF_EXPR) X(ND_SIZEOF_TYPE)
        X(ND_CALL) X(ND_INDEX) X(ND_MEMBER) X(ND_ARROW)
        X(ND_IDENT) X(ND_INT_LIT) X(ND_FLOAT_LIT) X(ND_CHAR_LIT) X(ND_STR_LIT)
        X(ND_INIT_LIST) X(ND_COMMA_EXPR)
#undef X
        default: return "ND_UNKNOWN";
    }
}

void ast_dump(Node *n, int depth, FILE *out) {
    if (!n) return;
    for (int i = 0; i < depth * 2; i++) fputc(' ', out);
    fprintf(out, "[%s]", node_kind_name(n->kind));
    if (n->sval) fprintf(out, " sval=%s", n->sval);
    if (n->ival) fprintf(out, " ival=%lld", n->ival);
    fprintf(out, "\n");
    ast_dump(n->left,  depth + 1, out);
    ast_dump(n->right, depth + 1, out);
    ast_dump(n->extra, depth + 1, out);
    for (NodeList *c = n->children; c; c = c->next)
        ast_dump(c->node, depth + 1, out);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * ast_to_c() – pretty-printer
 * ═══════════════════════════════════════════════════════════════════════════ */

typedef struct {
    FILE *out;
    int   indent;
} Printer;

static void pr_indent(Printer *p) {
    for (int i = 0; i < p->indent; i++) fputs("    ", p->out);
}

static void pr(Printer *p, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(p->out, fmt, ap);
    va_end(ap);
}

/* Forward declarations */
static void emit_type_prefix(Printer *p, Node *type, int top);
static void emit_type_suffix(Printer *p, Node *type);
static void emit_decl(Printer *p, Node *type, const char *name);
static void emit_expr(Printer *p, Node *n, int prec);
static void emit_stmt(Printer *p, Node *n);
static void emit_block(Printer *p, Node *n);

/* ── Operator precedence (for parenthesisation) ────────────────────────── */
static int expr_prec(Node *n) {
    if (!n) return 100;
    switch (n->kind) {
        case ND_COMMA_EXPR:  return 1;
        case ND_ASSIGN:      return 2;
        case ND_TERNARY:     return 3;
        case ND_BINARY: {
            const char *op = n->sval;
            if (!op) return 4;
            if (strcmp(op,"||")==0) return 4;
            if (strcmp(op,"&&")==0) return 5;
            if (strcmp(op,"|")==0)  return 6;
            if (strcmp(op,"^")==0)  return 7;
            if (strcmp(op,"&")==0)  return 8;
            if (strcmp(op,"==")==0||strcmp(op,"!=")==0) return 9;
            if (strcmp(op,"<")==0||strcmp(op,">")==0||
                strcmp(op,"<=")==0||strcmp(op,">=")==0) return 10;
            if (strcmp(op,"<<")==0||strcmp(op,">>")==0) return 11;
            if (strcmp(op,"+")==0||strcmp(op,"-")==0) return 12;
            if (strcmp(op,"*")==0||strcmp(op,"/")==0||
                strcmp(op,"%")==0)  return 13;
            return 13;
        }
        case ND_CAST:
        case ND_UNARY_PRE:
        case ND_SIZEOF_EXPR:
        case ND_SIZEOF_TYPE:  return 14;
        case ND_UNARY_POST:
        case ND_CALL:
        case ND_INDEX:
        case ND_MEMBER:
        case ND_ARROW:        return 15;
        default:              return 16;
    }
}

/* ── Type emission ────────────────────────────────────────────────────────
 *
 * C type syntax is inside-out: for "pointer to array 3 of int":
 *   prefix: "int (*"   suffix: ")[3]"
 *
 * We split each type node into prefix and suffix parts.
 * emit_decl(type, name) prints:  <prefix> name <suffix>
 */

static void emit_storage(Printer *p, Node *t) {
    if (t->is_typedef)  pr(p, "typedef ");
    if (t->is_extern)   pr(p, "extern ");
    if (t->is_static)   pr(p, "static ");
}

/* emit the leftmost part of the type (stops before name) */
static void emit_type_prefix(Printer *p, Node *t, int top) {
    if (!t) return;
    switch (t->kind) {
        case ND_TYPE_BASE:
            if (top) emit_storage(p, t);
            if (t->is_const)    pr(p, "const ");
            if (t->is_volatile) pr(p, "volatile ");
            if (t->is_unsigned) pr(p, "unsigned ");
            if (t->is_signed)   pr(p, "signed ");
            pr(p, "%s", t->sval);
            break;

        case ND_TYPE_PTR:
            emit_type_prefix(p, t->left, top);
            /* insert '*' between base and declarator;
               if inner is array/function we need parens */
            if (t->left && (t->left->kind == ND_TYPE_ARRAY ||
                            t->left->kind == ND_TYPE_FUNC))
                pr(p, " (*");
            else
                pr(p, " *");
            if (t->is_const)    pr(p, "const ");
            if (t->is_volatile) pr(p, "volatile ");
            break;

        case ND_TYPE_ARRAY:
            emit_type_prefix(p, t->left, top);
            break;

        case ND_TYPE_FUNC:
            emit_type_prefix(p, t->left, top);
            break;

        case ND_TYPE_STRUCT:
        case ND_TYPE_UNION: {
            if (top) emit_storage(p, t);
            if (t->is_const)    pr(p, "const ");
            if (t->is_volatile) pr(p, "volatile ");
            const char *kw = (t->kind == ND_TYPE_STRUCT) ? "struct" : "union";
            pr(p, "%s", kw);
            if (t->sval) pr(p, " %s", t->sval);
            if (t->children) {
                pr(p, " {\n");
                p->indent++;
                for (NodeList *c = t->children; c; c = c->next) {
                    Node *m = c->node; /* ND_STRUCT_MEMBER */
                    pr_indent(p);
                    emit_decl(p, m->left, m->sval);
                    pr(p, ";\n");
                }
                p->indent--;
                pr_indent(p);
                pr(p, "}");
            }
            break;
        }

        case ND_TYPE_ENUM: {
            if (top) emit_storage(p, t);
            pr(p, "enum");
            if (t->sval) pr(p, " %s", t->sval);
            if (t->children) {
                pr(p, " {\n");
                p->indent++;
                for (NodeList *c = t->children; c; c = c->next) {
                    Node *e = c->node; /* ND_ENUMERATOR */
                    pr_indent(p);
                    pr(p, "%s", e->sval);
                    if (e->left) { pr(p, " = "); emit_expr(p, e->left, 0); }
                    if (c->next) pr(p, ",");
                    pr(p, "\n");
                }
                p->indent--;
                pr_indent(p);
                pr(p, "}");
            }
            break;
        }

        default:
            pr(p, "/*?type%d*/", t->kind);
    }
}

/* emit the rightmost suffix part (array sizes, param lists) */
static void emit_type_suffix(Printer *p, Node *t) {
    if (!t) return;
    switch (t->kind) {
        case ND_TYPE_PTR:
            if (t->left && (t->left->kind == ND_TYPE_ARRAY ||
                            t->left->kind == ND_TYPE_FUNC))
                pr(p, ")");
            emit_type_suffix(p, t->left);
            break;

        case ND_TYPE_ARRAY:
            pr(p, "[");
            if (t->right) emit_expr(p, t->right, 0);
            pr(p, "]");
            emit_type_suffix(p, t->left);
            break;

        case ND_TYPE_FUNC: {
            pr(p, "(");
            int first = 1;
            for (NodeList *c = t->children; c; c = c->next) {
                if (!first) pr(p, ", ");
                first = 0;
                Node *par = c->node;
                if (par->is_ellipsis) { pr(p, "..."); continue; }
                emit_decl(p, par->left, par->sval);
            }
            pr(p, ")");
            emit_type_suffix(p, t->left);
            break;
        }

        default:
            break; /* base types have no suffix */
    }
}

/* Full declarator: prefix <space> name suffix */
static void emit_decl(Printer *p, Node *type, const char *name) {
    emit_type_prefix(p, type, 0);
    if (name && *name) pr(p, " %s", name);
    emit_type_suffix(p, type);
}

/* ── Expression emission ─────────────────────────────────────────────────── */

static void emit_expr(Printer *p, Node *n, int outer_prec) {
    if (!n) return;
    int my_prec = expr_prec(n);
    int paren   = (my_prec < outer_prec) ||
                  /* assignment is right-assoc; force paren on left nest */
                  ((n->kind == ND_ASSIGN) && (my_prec == outer_prec));
    if (paren) pr(p, "(");

    switch (n->kind) {
        case ND_IDENT:     pr(p, "%s", n->sval); break;
        case ND_INT_LIT:   pr(p, "%s", n->sval); break;
        case ND_FLOAT_LIT: pr(p, "%s", n->sval); break;
        case ND_CHAR_LIT:  pr(p, "%s", n->sval); break;
        case ND_STR_LIT:   pr(p, "%s", n->sval); break;

        case ND_BINARY: {
            /* Determine effective outer-prec for children.
               Use my_prec+1 to force parens on same-prec right-child
               (left-associativity).
               Use FORCE_PAREN (999) to force parens when mixing operators
               that trigger -Wparentheses even if mathematically correct:
                 ||  with &&  child
                 &   with +/- child                                          */
            #define FORCE_PAREN 999
            int lprec = my_prec + 1;
            int rprec = my_prec + 1;
            const char *op = n->sval ? n->sval : "";
            if (n->right && n->right->kind == ND_BINARY) {
                const char *rop = n->right->sval ? n->right->sval : "";
                if (strcmp(op,"||")==0 && strcmp(rop,"&&")==0) rprec = FORCE_PAREN;
                if (strcmp(op,"&")==0  &&
                    (strcmp(rop,"+")==0 || strcmp(rop,"-")==0)) rprec = FORCE_PAREN;
            }
            if (n->left && n->left->kind == ND_BINARY) {
                const char *lop = n->left->sval ? n->left->sval : "";
                if (strcmp(op,"||")==0 && strcmp(lop,"&&")==0) lprec = FORCE_PAREN;
                if (strcmp(op,"&")==0  &&
                    (strcmp(lop,"+")==0 || strcmp(lop,"-")==0)) lprec = FORCE_PAREN;
            }
            emit_expr(p, n->left,  lprec);
            pr(p, " %s ", op);
            emit_expr(p, n->right, rprec);
            #undef FORCE_PAREN
            break;
        }

        case ND_ASSIGN:
            emit_expr(p, n->left,  my_prec + 1);
            pr(p, " %s ", n->sval);
            emit_expr(p, n->right, my_prec);
            break;

        case ND_COMMA_EXPR:
            emit_expr(p, n->left,  my_prec);
            pr(p, ", ");
            emit_expr(p, n->right, my_prec);
            break;

        case ND_TERNARY:
            emit_expr(p, n->left,  my_prec + 1);
            pr(p, " ? ");
            emit_expr(p, n->right, 0);
            pr(p, " : ");
            emit_expr(p, n->extra, my_prec);
            break;

        case ND_UNARY_PRE:
            pr(p, "%s", n->sval);
            emit_expr(p, n->left, my_prec);
            break;

        case ND_UNARY_POST:
            emit_expr(p, n->left, my_prec);
            pr(p, "%s", n->sval);
            break;

        case ND_CAST:
            pr(p, "(");
            emit_type_prefix(p, n->left, 0);
            emit_type_suffix(p, n->left);
            pr(p, ")");
            emit_expr(p, n->right, my_prec);
            break;

        case ND_SIZEOF_EXPR:
            pr(p, "sizeof ");
            emit_expr(p, n->left, my_prec);
            break;

        case ND_SIZEOF_TYPE:
            pr(p, "sizeof(");
            emit_type_prefix(p, n->left, 0);
            emit_type_suffix(p, n->left);
            pr(p, ")");
            break;

        case ND_CALL: {
            emit_expr(p, n->left, my_prec);
            pr(p, "(");
            int first = 1;
            for (NodeList *c = n->children; c; c = c->next) {
                if (!first) pr(p, ", ");
                first = 0;
                emit_expr(p, c->node, 2); /* above comma */
            }
            pr(p, ")");
            break;
        }

        case ND_INDEX:
            /* When used as a designated initializer (has right child = value),
               it was stored as [idx]=val by parse_initializer.
               Normal array index: left[right], no right->right.
               Designated: recognised in ND_INIT_LIST below.            */
            emit_expr(p, n->left,  my_prec);
            pr(p, "[");
            emit_expr(p, n->right, 0);
            pr(p, "]");
            break;

        case ND_MEMBER:
            emit_expr(p, n->left, my_prec);
            pr(p, ".%s", n->sval);
            break;

        case ND_ARROW:
            emit_expr(p, n->left, my_prec);
            pr(p, "->%s", n->sval);
            break;

        case ND_INIT_LIST: {
            pr(p, "{");
            int first = 1;
            for (NodeList *c = n->children; c; c = c->next) {
                if (!first) pr(p, ", ");
                first = 0;
                Node *ch = c->node;
                /* Designated index: [idx] = val  → stored as ND_INDEX{left=idx,right=val} */
                if (ch->kind == ND_INDEX && ch->right) {
                    pr(p, "[");
                    emit_expr(p, ch->left, 0);
                    pr(p, "] = ");
                    emit_expr(p, ch->right, 2);
                /* Designated member: .name = val  → stored as ND_MEMBER{sval=name,left=val} */
                } else if (ch->kind == ND_MEMBER && ch->left && !ch->right) {
                    pr(p, ".%s = ", ch->sval);
                    emit_expr(p, ch->left, 2);
                } else {
                    emit_expr(p, ch, 2);
                }
            }
            pr(p, "}");
            break;
        }

        default:
            pr(p, "/*?expr%d*/", n->kind);
    }

    if (paren) pr(p, ")");
}

/* ── Statement emission ──────────────────────────────────────────────────── */

static void emit_block(Printer *p, Node *n) {
    /* If already a compound statement, emit its braces at current indent.
       Otherwise emit as a single-statement block. */
    if (n && n->kind == ND_COMPOUND) {
        pr(p, " {\n");
        p->indent++;
        for (NodeList *c = n->children; c; c = c->next)
            emit_stmt(p, c->node);
        p->indent--;
        pr_indent(p);
        pr(p, "}");
    } else {
        pr(p, "\n");
        p->indent++;
        emit_stmt(p, n);
        p->indent--;
    }
}

static void emit_stmt(Printer *p, Node *n) {
    if (!n) return;
    switch (n->kind) {

        case ND_EMPTY_STMT:
            pr_indent(p); pr(p, ";\n");
            break;

        case ND_EXPR_STMT:
            pr_indent(p);
            emit_expr(p, n->left, 0);
            pr(p, ";\n");
            break;

        case ND_DECL: {
            pr_indent(p);
            /* storage / type qualifiers from type node */
            emit_type_prefix(p, n->left, 1);
            if (n->sval && *n->sval) {
                pr(p, " %s", n->sval);
                emit_type_suffix(p, n->left);
            } else {
                emit_type_suffix(p, n->left);
            }
            if (n->right) { pr(p, " = "); emit_expr(p, n->right, 2); }
            pr(p, ";\n");
            break;
        }

        case ND_COMPOUND:
            pr_indent(p); pr(p, "{\n");
            p->indent++;
            for (NodeList *c = n->children; c; c = c->next)
                emit_stmt(p, c->node);
            p->indent--;
            pr_indent(p); pr(p, "}\n");
            break;

        case ND_IF:
            pr_indent(p); pr(p, "if (");
            emit_expr(p, n->left, 0);
            pr(p, ")");
            emit_block(p, n->right);
            if (n->extra) {
                pr(p, "\n");
                pr_indent(p); pr(p, "else");
                if (n->extra->kind == ND_IF) {
                    pr(p, " ");
                    /* reuse emit_stmt but without leading indent */
                    p->indent--;
                    emit_stmt(p, n->extra);
                    p->indent++;
                } else {
                    emit_block(p, n->extra);
                    pr(p, "\n");
                }
            } else {
                pr(p, "\n");
            }
            break;

        case ND_WHILE:
            pr_indent(p); pr(p, "while (");
            emit_expr(p, n->left, 0);
            pr(p, ")");
            emit_block(p, n->right);
            pr(p, "\n");
            break;

        case ND_DO_WHILE:
            pr_indent(p); pr(p, "do");
            emit_block(p, n->left);
            pr(p, " while (");
            emit_expr(p, n->right, 0);
            pr(p, ");\n");
            break;

        case ND_FOR:
            pr_indent(p); pr(p, "for (");
            /* left=init, right=cond, extra=step, children[0]=body */
            if (n->left) {
                if (n->left->kind == ND_DECL) {
                    /* variable declaration in for-init */
                    emit_type_prefix(p, n->left->left, 1);
                    if (n->left->sval && *n->left->sval) {
                        pr(p, " %s", n->left->sval);
                        emit_type_suffix(p, n->left->left);
                    } else {
                        emit_type_suffix(p, n->left->left);
                    }
                    if (n->left->right) {
                        pr(p, " = ");
                        emit_expr(p, n->left->right, 2);
                    }
                } else {
                    emit_expr(p, n->left, 0);
                }
            }
            pr(p, "; ");
            if (n->right) emit_expr(p, n->right, 0);
            pr(p, "; ");
            if (n->extra) emit_expr(p, n->extra, 0);
            pr(p, ")");
            /* body is first child */
            if (n->children) emit_block(p, n->children->node);
            pr(p, "\n");
            break;

        case ND_RETURN:
            pr_indent(p); pr(p, "return");
            if (n->left) { pr(p, " "); emit_expr(p, n->left, 0); }
            pr(p, ";\n");
            break;

        case ND_BREAK:
            pr_indent(p); pr(p, "break;\n");
            break;

        case ND_CONTINUE:
            pr_indent(p); pr(p, "continue;\n");
            break;

        case ND_GOTO:
            pr_indent(p); pr(p, "goto %s;\n", n->sval);
            break;

        case ND_LABEL:
            /* labels are outdented one level */
            p->indent--;
            pr_indent(p); pr(p, "%s:\n", n->sval);
            p->indent++;
            emit_stmt(p, n->left);
            break;

        case ND_SWITCH:
            pr_indent(p); pr(p, "switch (");
            emit_expr(p, n->left, 0);
            pr(p, ")");
            emit_block(p, n->right);
            pr(p, "\n");
            break;

        case ND_CASE:
            p->indent--;
            pr_indent(p); pr(p, "case ");
            emit_expr(p, n->left, 0);
            pr(p, ":\n");
            p->indent++;
            emit_stmt(p, n->right);
            break;

        case ND_DEFAULT:
            p->indent--;
            pr_indent(p); pr(p, "default:\n");
            p->indent++;
            emit_stmt(p, n->left);
            break;

        default:
            pr_indent(p);
            pr(p, "/*?stmt%d*/;\n", n->kind);
    }
}

/* ── Top-level emission ───────────────────────────────────────────────────── */

static void emit_func_def(Printer *p, Node *n) {
    /* n->left  = return type (ND_TYPE_*)
     * n->sval  = function name
     * n->children = params (ND_PARAM nodes)
     * n->right = body (ND_COMPOUND)
     */
    /* storage from return type */
    emit_type_prefix(p, n->left, 1);

    /* name + param list */
    pr(p, " %s(", n->sval);
    int first = 1;
    for (NodeList *c = n->children; c; c = c->next) {
        if (!first) pr(p, ", ");
        first = 0;
        Node *par = c->node;
        if (par->is_ellipsis) { pr(p, "..."); continue; }
        emit_decl(p, par->left, par->sval);
    }
    pr(p, ")");

    emit_type_suffix(p, n->left);
    pr(p, "\n");

    /* body */
    pr_indent(p); pr(p, "{\n");
    p->indent++;
    for (NodeList *c = n->right->children; c; c = c->next)
        emit_stmt(p, c->node);
    p->indent--;
    pr(p, "}\n");
}

static void emit_global_decl(Printer *p, Node *n) {
    /* ND_DECL at file scope */
    emit_type_prefix(p, n->left, 1);
    if (n->sval && *n->sval) {
        pr(p, " %s", n->sval);
        emit_type_suffix(p, n->left);
    } else {
        emit_type_suffix(p, n->left);
    }
    if (n->right) { pr(p, " = "); emit_expr(p, n->right, 2); }
    pr(p, ";\n");
}

void ast_to_c(Node *root, FILE *out) {
    if (!root) return;
    Printer pr_s = { out, 0 };
    Printer *p   = &pr_s;

    assert(root->kind == ND_TRANSLATION_UNIT);
    for (NodeList *c = root->children; c; c = c->next) {
        Node *ext = c->node;
        if (ext->kind == ND_FUNC_DEF)
            emit_func_def(p, ext);
        else if (ext->kind == ND_DECL)
            emit_global_decl(p, ext);
        else {
            pr(p, "/*?external%d*/\n", ext->kind);
        }
        pr(p, "\n");
    }
}
