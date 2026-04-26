typedef enum {
    ND_TRANSLATION_UNIT,
    ND_FUNC_DEF,
    ND_DECL,
    ND_PARAM,
    ND_TYPE_BASE,
    ND_TYPE_PTR,
    ND_TYPE_ARRAY,
    ND_TYPE_FUNC,
    ND_TYPE_STRUCT,
    ND_TYPE_UNION,
    ND_TYPE_ENUM,
    ND_STRUCT_MEMBER,
    ND_ENUMERATOR,
    ND_COMPOUND,
    ND_IF,
    ND_WHILE,
    ND_DO_WHILE,
    ND_FOR,
    ND_RETURN,
    ND_BREAK,
    ND_CONTINUE,
    ND_GOTO,
    ND_LABEL,
    ND_SWITCH,
    ND_CASE,
    ND_DEFAULT,
    ND_EXPR_STMT,
    ND_EMPTY_STMT,
    ND_ASSIGN,
    ND_TERNARY,
    ND_BINARY,
    ND_UNARY_PRE,
    ND_UNARY_POST,
    ND_CAST,
    ND_SIZEOF_EXPR,
    ND_SIZEOF_TYPE,
    ND_CALL,
    ND_INDEX,
    ND_MEMBER,
    ND_ARROW,
    ND_IDENT,
    ND_INT_LIT,
    ND_FLOAT_LIT,
    ND_CHAR_LIT,
    ND_STR_LIT,
    ND_INIT_LIST,
    ND_COMMA_EXPR,
    ND_COUNT
} NodeKind;

typedef struct Node Node;

typedef struct NodeList {
    Node * node;
    struct NodeList * next;
} NodeList;

struct Node {
    NodeKind kind;
    int line;
    int col;
    Node * left;
    Node * right;
    Node * extra;
    NodeList * children;
    const char * sval;
    long long ival;
    double fval;
    int is_const;
    int is_volatile;
    int is_static;
    int is_extern;
    int is_typedef;
    int is_unsigned;
    int is_signed;
    int is_ellipsis;
};

typedef struct AstArenaBlock {
    char * data;
    int used;
    int cap;
    struct AstArenaBlock * next;
} AstArenaBlock;

typedef struct {
    AstArenaBlock * head;
} AstArena;

void * ast_arena_alloc(AstArena * a, int n);

char * ast_arena_strdup(AstArena * a, const char * s);

Node * ast_node(AstArena * a, NodeKind kind, int line, int col);

NodeList * ast_list_append(AstArena * a, NodeList * list, Node * node);

void ast_to_c(Node * root, FILE * out);

void ast_dump(Node * root, int depth, FILE * out);

void ast_arena_free(AstArena * a);

void * ast_arena_alloc(AstArena * a, int n)
{
    int aligned = (n + 15) & ~15;
    if (!a->head || a->head->used + aligned > a->head->cap) {
        int cap = aligned > 65536 ? aligned : 65536;
        AstArenaBlock * b = malloc(sizeof(AstArenaBlock));
        if (!b) {
            perror("malloc");
            exit(1);
        }
        b->data = malloc((size_t)cap);
        if (!b->data) {
            perror("malloc");
            exit(1);
        }
        b->used = 0;
        b->cap = cap;
        b->next = a->head;
        a->head = b;
    }
    void * p = a->head->data + a->head->used;
    a->head->used += aligned;
    return p;
}

char * ast_arena_strdup(AstArena * a, const char * s)
{
    int len = (int)strlen(s);
    char * p = ast_arena_alloc(a, len + 1);
    memcpy(p, s, (size_t)len + 1);
    return p;
}

void ast_arena_free(AstArena * a)
{
    AstArenaBlock * b = a->head;
    while (b) {
        AstArenaBlock * next = b->next;
        free(b->data);
        free(b);
        b = next;
    }
    a->head = NULL;
}

Node * ast_node(AstArena * a, NodeKind kind, int line, int col)
{
    Node * n = ast_arena_alloc(a, (int)sizeof(Node));
    memset(n, 0, sizeof(Node));
    n->kind = kind;
    n->line = line;
    n->col = col;
    return n;
}

NodeList * ast_list_append(AstArena * a, NodeList * list, Node * node)
{
    NodeList * item = ast_arena_alloc(a, (int)sizeof(NodeList));
    item->node = node;
    item->next = NULL;
    if (!list)
        return item;

    NodeList * p = list;
    while (p->next)
        p = p->next;

    p->next = item;
    return list;
}

static const char * node_kind_name(NodeKind k)
{
    switch (k) {
    case ND_TRANSLATION_UNIT:
        return "ND_TRANSLATION_UNIT";
    case ND_FUNC_DEF:
        return "ND_FUNC_DEF";
    case ND_DECL:
        return "ND_DECL";
    case ND_PARAM:
        return "ND_PARAM";
    case ND_TYPE_BASE:
        return "ND_TYPE_BASE";
    case ND_TYPE_PTR:
        return "ND_TYPE_PTR";
    case ND_TYPE_ARRAY:
        return "ND_TYPE_ARRAY";
    case ND_TYPE_FUNC:
        return "ND_TYPE_FUNC";
    case ND_TYPE_STRUCT:
        return "ND_TYPE_STRUCT";
    case ND_TYPE_UNION:
        return "ND_TYPE_UNION";
    case ND_TYPE_ENUM:
        return "ND_TYPE_ENUM";
    case ND_STRUCT_MEMBER:
        return "ND_STRUCT_MEMBER";
    case ND_ENUMERATOR:
        return "ND_ENUMERATOR";
    case ND_COMPOUND:
        return "ND_COMPOUND";
    case ND_IF:
        return "ND_IF";
    case ND_WHILE:
        return "ND_WHILE";
    case ND_DO_WHILE:
        return "ND_DO_WHILE";
    case ND_FOR:
        return "ND_FOR";
    case ND_RETURN:
        return "ND_RETURN";
    case ND_BREAK:
        return "ND_BREAK";
    case ND_CONTINUE:
        return "ND_CONTINUE";
    case ND_GOTO:
        return "ND_GOTO";
    case ND_LABEL:
        return "ND_LABEL";
    case ND_SWITCH:
        return "ND_SWITCH";
    case ND_CASE:
        return "ND_CASE";
    case ND_DEFAULT:
        return "ND_DEFAULT";
    case ND_EXPR_STMT:
        return "ND_EXPR_STMT";
    case ND_EMPTY_STMT:
        return "ND_EMPTY_STMT";
    case ND_ASSIGN:
        return "ND_ASSIGN";
    case ND_TERNARY:
        return "ND_TERNARY";
    case ND_BINARY:
        return "ND_BINARY";
    case ND_UNARY_PRE:
        return "ND_UNARY_PRE";
    case ND_UNARY_POST:
        return "ND_UNARY_POST";
    case ND_CAST:
        return "ND_CAST";
    case ND_SIZEOF_EXPR:
        return "ND_SIZEOF_EXPR";
    case ND_SIZEOF_TYPE:
        return "ND_SIZEOF_TYPE";
    case ND_CALL:
        return "ND_CALL";
    case ND_INDEX:
        return "ND_INDEX";
    case ND_MEMBER:
        return "ND_MEMBER";
    case ND_ARROW:
        return "ND_ARROW";
    case ND_IDENT:
        return "ND_IDENT";
    case ND_INT_LIT:
        return "ND_INT_LIT";
    case ND_FLOAT_LIT:
        return "ND_FLOAT_LIT";
    case ND_CHAR_LIT:
        return "ND_CHAR_LIT";
    case ND_STR_LIT:
        return "ND_STR_LIT";
    case ND_INIT_LIST:
        return "ND_INIT_LIST";
    case ND_COMMA_EXPR:
        return "ND_COMMA_EXPR";
    default:
        return "ND_UNKNOWN";
    }
}

void ast_dump(Node * n, int depth, FILE * out)
{
    if (!n)
        return;

    for (int i = 0; i < depth * 2; i++)
        fputc(' ', out);

    fprintf(out, "[%s]", node_kind_name(n->kind));
    if (n->sval)
        fprintf(out, " sval=%s", n->sval);

    if (n->ival)
        fprintf(out, " ival=%lld", n->ival);

    fprintf(out, "\n");
    ast_dump(n->left, depth + 1, out);
    ast_dump(n->right, depth + 1, out);
    ast_dump(n->extra, depth + 1, out);
    for (NodeList * c = n->children; c; c = c->next)
        ast_dump(c->node, depth + 1, out);

}

typedef struct {
    FILE * out;
    int indent;
} Printer;

static void pr_indent(Printer * p)
{
    for (int i = 0; i < p->indent; i++)
        fputs("    ", p->out);

}

static void pr(Printer * p, const char * fmt, ...)
{
    va_list ap;
    __builtin_va_start(ap, fmt);
    vfprintf(p->out, fmt, ap);
    __builtin_va_end(ap);
}

static void emit_type_prefix(Printer * p, Node * type, int top);

static void emit_type_suffix(Printer * p, Node * type);

static void emit_decl(Printer * p, Node * type, const char * name);

static void emit_expr(Printer * p, Node * n, int prec);

static void emit_stmt(Printer * p, Node * n);

static void emit_block(Printer * p, Node * n);

static int expr_prec(Node * n)
{
    if (!n)
        return 100;

    switch (n->kind) {
    case ND_COMMA_EXPR:
        return 1;
    case ND_ASSIGN:
        return 2;
    case ND_TERNARY:
        return 3;
    case ND_BINARY:
        {
            const char * op = n->sval;
            if (!op)
                return 4;

            if (strcmp(op, "||") == 0)
                return 4;

            if (strcmp(op, "&&") == 0)
                return 5;

            if (strcmp(op, "|") == 0)
                return 6;

            if (strcmp(op, "^") == 0)
                return 7;

            if (strcmp(op, "&") == 0)
                return 8;

            if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0)
                return 9;

            if (((strcmp(op, "<") == 0 || strcmp(op, ">") == 0) || strcmp(op, "<=") == 0) || strcmp(op, ">=") == 0)
                return 10;

            if (strcmp(op, "<<") == 0 || strcmp(op, ">>") == 0)
                return 11;

            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0)
                return 12;

            if ((strcmp(op, "*") == 0 || strcmp(op, "/") == 0) || strcmp(op, "%") == 0)
                return 13;

            return 13;
        }
    case ND_CAST:
    case ND_UNARY_PRE:
    case ND_SIZEOF_EXPR:
    case ND_SIZEOF_TYPE:
        return 14;
    case ND_UNARY_POST:
    case ND_CALL:
    case ND_INDEX:
    case ND_MEMBER:
    case ND_ARROW:
        return 15;
    default:
        return 16;
    }
}

static void emit_storage(Printer * p, Node * t)
{
    if (t->is_typedef)
        pr(p, "typedef ");

    if (t->is_extern)
        pr(p, "extern ");

    if (t->is_static)
        pr(p, "static ");

}

static void emit_type_prefix(Printer * p, Node * t, int top)
{
    if (!t)
        return;

    switch (t->kind) {
    case ND_TYPE_BASE:
        if (top)
            emit_storage(p, t);

        if (t->is_const)
            pr(p, "const ");

        if (t->is_volatile)
            pr(p, "volatile ");

        if (t->is_unsigned)
            pr(p, "unsigned ");

        if (t->is_signed)
            pr(p, "signed ");

        pr(p, "%s", t->sval);
        break;
    case ND_TYPE_PTR:
        emit_type_prefix(p, t->left, top);
        if (t->left && (t->left->kind == ND_TYPE_ARRAY || t->left->kind == ND_TYPE_FUNC))
            pr(p, " (*");

        else
            pr(p, " *");

        if (t->is_const)
            pr(p, "const ");

        if (t->is_volatile)
            pr(p, "volatile ");

        break;
    case ND_TYPE_ARRAY:
        emit_type_prefix(p, t->left, top);
        break;
    case ND_TYPE_FUNC:
        emit_type_prefix(p, t->left, top);
        break;
    case ND_TYPE_STRUCT:
    case ND_TYPE_UNION:
        {
            if (top)
                emit_storage(p, t);

            if (t->is_const)
                pr(p, "const ");

            if (t->is_volatile)
                pr(p, "volatile ");

            const char * kw = t->kind == ND_TYPE_STRUCT ? "struct" : "union";
            pr(p, "%s", kw);
            if (t->sval)
                pr(p, " %s", t->sval);

            if (t->children) {
                pr(p, " {\n");
                p->indent++;
                for (NodeList * c = t->children; c; c = c->next) {
                    Node * m = c->node;
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
    case ND_TYPE_ENUM:
        {
            if (top)
                emit_storage(p, t);

            pr(p, "enum");
            if (t->sval)
                pr(p, " %s", t->sval);

            if (t->children) {
                pr(p, " {\n");
                p->indent++;
                for (NodeList * c = t->children; c; c = c->next) {
                    Node * e = c->node;
                    pr_indent(p);
                    pr(p, "%s", e->sval);
                    if (e->left) {
                        pr(p, " = ");
                        emit_expr(p, e->left, 0);
                    }
                    if (c->next)
                        pr(p, ",");

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

static void emit_type_suffix(Printer * p, Node * t)
{
    if (!t)
        return;

    switch (t->kind) {
    case ND_TYPE_PTR:
        if (t->left && (t->left->kind == ND_TYPE_ARRAY || t->left->kind == ND_TYPE_FUNC))
            pr(p, ")");

        emit_type_suffix(p, t->left);
        break;
    case ND_TYPE_ARRAY:
        pr(p, "[");
        if (t->right)
            emit_expr(p, t->right, 0);

        pr(p, "]");
        emit_type_suffix(p, t->left);
        break;
    case ND_TYPE_FUNC:
        {
            pr(p, "(");
            int first = 1;
            for (NodeList * c = t->children; c; c = c->next) {
                if (!first)
                    pr(p, ", ");

                first = 0;
                Node * par = c->node;
                if (par->is_ellipsis) {
                    pr(p, "...");
                    continue;
                }
                emit_decl(p, par->left, par->sval);
            }
            pr(p, ")");
            emit_type_suffix(p, t->left);
            break;
        }
    default:
        break;
    }
}

static void emit_decl(Printer * p, Node * type, const char * name)
{
    emit_type_prefix(p, type, 0);
    if (name && *name)
        pr(p, " %s", name);

    emit_type_suffix(p, type);
}

static void emit_expr(Printer * p, Node * n, int outer_prec)
{
    if (!n)
        return;

    int my_prec = expr_prec(n);
    int paren = my_prec < outer_prec || (n->kind == ND_ASSIGN && my_prec == outer_prec);
    if (paren)
        pr(p, "(");

    switch (n->kind) {
    case ND_IDENT:
        pr(p, "%s", n->sval);
        break;
    case ND_INT_LIT:
        pr(p, "%s", n->sval);
        break;
    case ND_FLOAT_LIT:
        pr(p, "%s", n->sval);
        break;
    case ND_CHAR_LIT:
        pr(p, "%s", n->sval);
        break;
    case ND_STR_LIT:
        pr(p, "%s", n->sval);
        break;
    case ND_BINARY:
        {
            int lprec = my_prec + 1;
            int rprec = my_prec + 1;
            const char * op = n->sval ? n->sval : "";
            if (n->right && n->right->kind == ND_BINARY) {
                const char * rop = n->right->sval ? n->right->sval : "";
                if (strcmp(op, "||") == 0 && strcmp(rop, "&&") == 0)
                    rprec = 999;

                if (strcmp(op, "&") == 0 && (strcmp(rop, "+") == 0 || strcmp(rop, "-") == 0))
                    rprec = 999;

            }
            if (n->left && n->left->kind == ND_BINARY) {
                const char * lop = n->left->sval ? n->left->sval : "";
                if (strcmp(op, "||") == 0 && strcmp(lop, "&&") == 0)
                    lprec = 999;

                if (strcmp(op, "&") == 0 && (strcmp(lop, "+") == 0 || strcmp(lop, "-") == 0))
                    lprec = 999;

            }
            emit_expr(p, n->left, lprec);
            pr(p, " %s ", op);
            emit_expr(p, n->right, rprec);
            break;
        }
    case ND_ASSIGN:
        emit_expr(p, n->left, my_prec + 1);
        pr(p, " %s ", n->sval);
        emit_expr(p, n->right, my_prec);
        break;
    case ND_COMMA_EXPR:
        emit_expr(p, n->left, my_prec);
        pr(p, ", ");
        emit_expr(p, n->right, my_prec);
        break;
    case ND_TERNARY:
        emit_expr(p, n->left, my_prec + 1);
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
    case ND_CALL:
        {
            emit_expr(p, n->left, my_prec);
            pr(p, "(");
            int first = 1;
            for (NodeList * c = n->children; c; c = c->next) {
                if (!first)
                    pr(p, ", ");

                first = 0;
                emit_expr(p, c->node, 2);
            }
            pr(p, ")");
            break;
        }
    case ND_INDEX:
        emit_expr(p, n->left, my_prec);
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
    case ND_INIT_LIST:
        {
            pr(p, "{");
            int first = 1;
            for (NodeList * c = n->children; c; c = c->next) {
                if (!first)
                    pr(p, ", ");

                first = 0;
                Node * ch = c->node;
                if (ch->kind == ND_INDEX && ch->right) {
                    pr(p, "[");
                    emit_expr(p, ch->left, 0);
                    pr(p, "] = ");
                    emit_expr(p, ch->right, 2);
                }
                else             if ((ch->kind == ND_MEMBER && ch->left) && !ch->right) {
                pr(p, ".%s = ", ch->sval);
                emit_expr(p, ch->left, 2);
            }
            else {
                emit_expr(p, ch, 2);
            }
            }
            pr(p, "}");
            break;
        }
    default:
        pr(p, "/*?expr%d*/", n->kind);
    }
    if (paren)
        pr(p, ")");

}

static void emit_block(Printer * p, Node * n)
{
    if (n && n->kind == ND_COMPOUND) {
        pr(p, " {\n");
        p->indent++;
        for (NodeList * c = n->children; c; c = c->next)
            emit_stmt(p, c->node);

        p->indent--;
        pr_indent(p);
        pr(p, "}");
    }
    else {
        pr(p, "\n");
        p->indent++;
        emit_stmt(p, n);
        p->indent--;
    }
}

static void emit_stmt(Printer * p, Node * n)
{
    if (!n)
        return;

    switch (n->kind) {
    case ND_EMPTY_STMT:
        pr_indent(p);
        pr(p, ";\n");
        break;
    case ND_EXPR_STMT:
        pr_indent(p);
        emit_expr(p, n->left, 0);
        pr(p, ";\n");
        break;
    case ND_DECL:
        {
            pr_indent(p);
            emit_type_prefix(p, n->left, 1);
            if (n->sval && *n->sval) {
                pr(p, " %s", n->sval);
                emit_type_suffix(p, n->left);
            }
            else {
                emit_type_suffix(p, n->left);
            }
            if (n->right) {
                pr(p, " = ");
                emit_expr(p, n->right, 2);
            }
            pr(p, ";\n");
            break;
        }
    case ND_COMPOUND:
        pr_indent(p);
        pr(p, "{\n");
        p->indent++;
        for (NodeList * c = n->children; c; c = c->next)
            emit_stmt(p, c->node);

        p->indent--;
        pr_indent(p);
        pr(p, "}\n");
        break;
    case ND_IF:
        pr_indent(p);
        pr(p, "if (");
        emit_expr(p, n->left, 0);
        pr(p, ")");
        emit_block(p, n->right);
        if (n->extra) {
            pr(p, "\n");
            pr_indent(p);
            pr(p, "else");
            if (n->extra->kind == ND_IF) {
                pr(p, " ");
                p->indent--;
                emit_stmt(p, n->extra);
                p->indent++;
            }
            else {
                emit_block(p, n->extra);
                pr(p, "\n");
            }
        }
        else {
            pr(p, "\n");
        }
        break;
    case ND_WHILE:
        pr_indent(p);
        pr(p, "while (");
        emit_expr(p, n->left, 0);
        pr(p, ")");
        emit_block(p, n->right);
        pr(p, "\n");
        break;
    case ND_DO_WHILE:
        pr_indent(p);
        pr(p, "do");
        emit_block(p, n->left);
        pr(p, " while (");
        emit_expr(p, n->right, 0);
        pr(p, ");\n");
        break;
    case ND_FOR:
        pr_indent(p);
        pr(p, "for (");
        if (n->left) {
            if (n->left->kind == ND_DECL) {
                emit_type_prefix(p, n->left->left, 1);
                if (n->left->sval && *n->left->sval) {
                    pr(p, " %s", n->left->sval);
                    emit_type_suffix(p, n->left->left);
                }
                else {
                    emit_type_suffix(p, n->left->left);
                }
                if (n->left->right) {
                    pr(p, " = ");
                    emit_expr(p, n->left->right, 2);
                }
            }
            else {
                emit_expr(p, n->left, 0);
            }
        }
        pr(p, "; ");
        if (n->right)
            emit_expr(p, n->right, 0);

        pr(p, "; ");
        if (n->extra)
            emit_expr(p, n->extra, 0);

        pr(p, ")");
        if (n->children)
            emit_block(p, n->children->node);

        pr(p, "\n");
        break;
    case ND_RETURN:
        pr_indent(p);
        pr(p, "return");
        if (n->left) {
            pr(p, " ");
            emit_expr(p, n->left, 0);
        }
        pr(p, ";\n");
        break;
    case ND_BREAK:
        pr_indent(p);
        pr(p, "break;\n");
        break;
    case ND_CONTINUE:
        pr_indent(p);
        pr(p, "continue;\n");
        break;
    case ND_GOTO:
        pr_indent(p);
        pr(p, "goto %s;\n", n->sval);
        break;
    case ND_LABEL:
        p->indent--;
        pr_indent(p);
        pr(p, "%s:\n", n->sval);
        p->indent++;
        emit_stmt(p, n->left);
        break;
    case ND_SWITCH:
        pr_indent(p);
        pr(p, "switch (");
        emit_expr(p, n->left, 0);
        pr(p, ")");
        emit_block(p, n->right);
        pr(p, "\n");
        break;
    case ND_CASE:
        p->indent--;
        pr_indent(p);
        pr(p, "case ");
        emit_expr(p, n->left, 0);
        pr(p, ":\n");
        p->indent++;
        emit_stmt(p, n->right);
        break;
    case ND_DEFAULT:
        p->indent--;
        pr_indent(p);
        pr(p, "default:\n");
        p->indent++;
        emit_stmt(p, n->left);
        break;
    default:
        pr_indent(p);
        pr(p, "/*?stmt%d*/;\n", n->kind);
    }
}

static void emit_func_def(Printer * p, Node * n)
{
    emit_type_prefix(p, n->left, 1);
    pr(p, " %s(", n->sval);
    int first = 1;
    for (NodeList * c = n->children; c; c = c->next) {
        if (!first)
            pr(p, ", ");

        first = 0;
        Node * par = c->node;
        if (par->is_ellipsis) {
            pr(p, "...");
            continue;
        }
        emit_decl(p, par->left, par->sval);
    }
    pr(p, ")");
    emit_type_suffix(p, n->left);
    pr(p, "\n");
    pr_indent(p);
    pr(p, "{\n");
    p->indent++;
    for (NodeList * c = n->right->children; c; c = c->next)
        emit_stmt(p, c->node);

    p->indent--;
    pr(p, "}\n");
}

static void emit_global_decl(Printer * p, Node * n)
{
    emit_type_prefix(p, n->left, 1);
    if (n->sval && *n->sval) {
        pr(p, " %s", n->sval);
        emit_type_suffix(p, n->left);
    }
    else {
        emit_type_suffix(p, n->left);
    }
    if (n->right) {
        pr(p, " = ");
        emit_expr(p, n->right, 2);
    }
    pr(p, ";\n");
}

void ast_to_c(Node * root, FILE * out)
{
    if (!root)
        return;

    Printer pr_s = {out, 0};
    Printer * p = &pr_s;
    (void)0;
    for (NodeList * c = root->children; c; c = c->next) {
        Node * ext = c->node;
        if (ext->kind == ND_FUNC_DEF)
            emit_func_def(p, ext);

        else     if (ext->kind == ND_DECL)
        emit_global_decl(p, ext);

    else {
        pr(p, "/*?external%d*/\n", ext->kind);
    }
        pr(p, "\n");
    }
}

