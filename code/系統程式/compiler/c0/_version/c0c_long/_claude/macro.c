#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ================================================================
   Preprocessor
   ================================================================ */

#define MAX_MACROS   512
#define MAX_PARAMS   16
#define MAX_INCLUDE  32
#define BUF_INIT     65536

/* ---------------------------------------------------------------- dynamic string buffer */

typedef struct {
    char  *data;
    size_t len;
    size_t cap;
} Buf;

static void buf_init(Buf *b) {
    b->data = malloc(BUF_INIT);
    if (!b->data) { perror("malloc"); exit(1); }
    b->data[0] = '\0';
    b->len = 0;
    b->cap = BUF_INIT;
}

static void buf_grow(Buf *b, size_t need) {
    while (b->len + need + 1 > b->cap) {
        b->cap *= 2;
        b->data = realloc(b->data, b->cap);
        if (!b->data) { perror("realloc"); exit(1); }
    }
}

static void buf_append(Buf *b, const char *s, size_t n) {
    buf_grow(b, n);
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void buf_putc(Buf *b, char c) {
    buf_grow(b, 1);
    b->data[b->len++] = c;
    b->data[b->len]   = '\0';
}

/* ---------------------------------------------------------------- macro table */

typedef struct {
    char  *name;
    char  *body;           /* replacement body (after expansion of ## etc.) */
    char  *params[MAX_PARAMS];
    int    n_params;
    int    is_func_like;
    int    is_defined;
} Macro;

static Macro macros[MAX_MACROS];
static int   n_macros = 0;

static Macro *macro_find(const char *name) {
    for (int i = 0; i < n_macros; i++)
        if (macros[i].is_defined && strcmp(macros[i].name, name) == 0)
            return &macros[i];
    return NULL;
}

static void macro_define(const char *name, const char *body,
                          char **params, int n_params, int is_func) {
    /* replace existing */
    for (int i = 0; i < n_macros; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            free(macros[i].body);
            macros[i].body = strdup(body);
            macros[i].is_defined = 1;
            return;
        }
    }
    if (n_macros >= MAX_MACROS) {
        fprintf(stderr, "macro table full\n");
        return;
    }
    Macro *m   = &macros[n_macros++];
    m->name    = strdup(name);
    m->body    = strdup(body);
    m->n_params = n_params;
    m->is_func_like = is_func;
    m->is_defined   = 1;
    for (int i = 0; i < n_params && i < MAX_PARAMS; i++)
        m->params[i] = params[i] ? strdup(params[i]) : NULL;
}

static void macro_undef(const char *name) {
    for (int i = 0; i < n_macros; i++)
        if (strcmp(macros[i].name, name) == 0) {
            macros[i].is_defined = 0;
            return;
        }
}

/* ---------------------------------------------------------------- skip utilities */

static const char *skip_ws(const char *p) {
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

static const char *skip_to_eol(const char *p) {
    while (*p && *p != '\n') p++;
    return p;
}

/* read an identifier starting at p; returns end pointer, writes into buf */
static const char *read_ident(const char *p, char *buf, size_t bufsz) {
    size_t i = 0;
    while (*p && (isalnum((unsigned char)*p) || *p == '_') && i < bufsz - 1)
        buf[i++] = *p++;
    buf[i] = '\0';
    return p;
}

/* ---------------------------------------------------------------- simple macro expansion */
/* Expand macros in 'src' string, write to 'out'.
   We do a single pass – good enough for self-compilation. */
static void expand_text(const char *src, Buf *out, int depth);

static void expand_func_macro(Macro *m, const char **src_ptr, Buf *out, int depth) {
    const char *p = *src_ptr;
    p = skip_ws(p);
    if (*p != '(') {
        /* not a call, emit name as-is */
        buf_append(out, m->name, strlen(m->name));
        return;
    }
    p++; /* skip '(' */

    /* collect arguments */
    char *args[MAX_PARAMS] = {NULL};
    int   n_args = 0;
    int   paren_depth = 0;
    Buf   arg_buf; buf_init(&arg_buf);

    while (*p && !(paren_depth == 0 && *p == ')')) {
        if (*p == ',' && paren_depth == 0) {
            if (n_args < MAX_PARAMS) args[n_args++] = strdup(arg_buf.data);
            arg_buf.len = 0; arg_buf.data[0] = '\0';
            p++;
        } else {
            if (*p == '(') paren_depth++;
            if (*p == ')') paren_depth--;
            buf_putc(&arg_buf, *p++);
        }
    }
    /* last arg */
    if (arg_buf.len > 0 || n_args > 0)
        if (n_args < MAX_PARAMS) args[n_args++] = strdup(arg_buf.data);
    free(arg_buf.data);
    if (*p == ')') p++;
    *src_ptr = p;

    /* substitute params in body */
    const char *body = m->body;
    Buf expanded; buf_init(&expanded);
    while (*body) {
        if (isalpha((unsigned char)*body) || *body == '_') {
            char ident[256];
            const char *end = read_ident(body, ident, sizeof ident);
            int found = 0;
            for (int i = 0; i < m->n_params && i < MAX_PARAMS; i++) {
                if (m->params[i] && strcmp(m->params[i], ident) == 0) {
                    if (i < n_args && args[i])
                        buf_append(&expanded, args[i], strlen(args[i]));
                    found = 1;
                    break;
                }
            }
            if (!found) buf_append(&expanded, ident, strlen(ident));
            body = end;
        } else {
            buf_putc(&expanded, *body++);
        }
    }
    /* recursively expand result */
    expand_text(expanded.data, out, depth + 1);
    free(expanded.data);

    for (int i = 0; i < n_args; i++) free(args[i]);
}

static void expand_text(const char *src, Buf *out, int depth) {
    if (depth > 64) { buf_append(out, src, strlen(src)); return; }
    const char *p = src;
    while (*p) {
        /* string literal – pass through without expansion */
        if (*p == '"') {
            buf_putc(out, *p++);
            while (*p && *p != '"') {
                if (*p == '\\') buf_putc(out, *p++);
                if (*p) buf_putc(out, *p++);
            }
            if (*p) buf_putc(out, *p++);
            continue;
        }
        /* char literal */
        if (*p == '\'') {
            buf_putc(out, *p++);
            while (*p && *p != '\'') {
                if (*p == '\\') buf_putc(out, *p++);
                if (*p) buf_putc(out, *p++);
            }
            if (*p) buf_putc(out, *p++);
            continue;
        }
        /* identifier - check for macro */
        if (isalpha((unsigned char)*p) || *p == '_') {
            char ident[256];
            const char *end = read_ident(p, ident, sizeof ident);
            Macro *m = macro_find(ident);
            if (m && m->is_func_like) {
                const char *q = end;
                q = skip_ws(q);
                if (*q == '(') {
                    p = end;
                    expand_func_macro(m, &p, out, depth + 1);
                    continue;
                }
            }
            if (m && !m->is_func_like) {
                expand_text(m->body, out, depth + 1);
                p = end;
                continue;
            }
            buf_append(out, ident, strlen(ident));
            p = end;
            continue;
        }
        buf_putc(out, *p++);
    }
}

/* ---------------------------------------------------------------- file reading */

static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t nr = fread(buf, 1, sz, f);
    buf[nr] = '\0';
    fclose(f);
    return buf;
}

/* search paths for includes */
static const char *INCLUDE_PATHS[] = {
    "/usr/include",
    "/usr/local/include",
    ".",
    NULL
};

static char *find_include(const char *name, int is_system) {
    if (!is_system) {
        /* relative to CWD first */
        char *content = read_file(name);
        if (content) return content;
    }
    for (int i = 0; INCLUDE_PATHS[i]; i++) {
        char path[1024];
        snprintf(path, sizeof path, "%s/%s", INCLUDE_PATHS[i], name);
        char *content = read_file(path);
        if (content) return content;
    }
    return NULL;
}

/* ---------------------------------------------------------------- main preprocessor */

static void preprocess_into(const char *src, const char *filename,
                              Buf *out, int depth,
                              /* if-stack */
                              int *if_stack, int *if_depth);

static void process_directive(const char *line, const char *filename,
                               Buf *out, int include_depth,
                               int *if_stack, int *if_depth) {
    const char *p = skip_ws(line + 1); /* skip '#' */

    char directive[64];
    p = read_ident(p, directive, sizeof directive);
    p = skip_ws(p);

    /* ------ if / ifdef / ifndef / elif / else / endif ------ */
    if (strcmp(directive, "ifdef") == 0) {
        char name[256]; read_ident(p, name, sizeof name);
        int val = (macro_find(name) != NULL);
        if (*if_depth < 32) if_stack[(*if_depth)++] = val;
        return;
    }
    if (strcmp(directive, "ifndef") == 0) {
        char name[256]; read_ident(p, name, sizeof name);
        int val = (macro_find(name) == NULL);
        if (*if_depth < 32) if_stack[(*if_depth)++] = val;
        return;
    }
    if (strcmp(directive, "if") == 0) {
        /* very simplified: just check if non-zero literal or defined() */
        int val = 0;
        if (strncmp(p, "defined", 7) == 0) {
            p += 7; p = skip_ws(p);
            if (*p == '(') p++;
            p = skip_ws(p);
            char name[256]; const char *e = read_ident(p, name, sizeof name);
            (void)e;
            val = (macro_find(name) != NULL);
        } else {
            val = atoi(p) != 0;
        }
        if (*if_depth < 32) if_stack[(*if_depth)++] = val;
        return;
    }
    if (strcmp(directive, "elif") == 0) {
        if (*if_depth > 0) {
            /* flip: only activate if we haven't been active yet
               (simplified: toggle) */
            int val = atoi(p) != 0;
            if_stack[*if_depth - 1] = val;
        }
        return;
    }
    if (strcmp(directive, "else") == 0) {
        if (*if_depth > 0) if_stack[*if_depth - 1] ^= 1;
        return;
    }
    if (strcmp(directive, "endif") == 0) {
        if (*if_depth > 0) (*if_depth)--;
        return;
    }

    /* ------ check if currently active ------ */
    int active = 1;
    for (int i = 0; i < *if_depth; i++) if (!if_stack[i]) { active = 0; break; }
    if (!active) return;

    /* ------ define / undef ------ */
    if (strcmp(directive, "define") == 0) {
        char name[256];
        p = read_ident(p, name, sizeof name);
        /* function-like? */
        if (*p == '(') {
            p++;
            char *params[MAX_PARAMS] = {NULL};
            int n_params = 0;
            while (*p && *p != ')') {
                p = skip_ws(p);
                if (*p == ')') break;
                char pn[64]; p = read_ident(p, pn, sizeof pn);
                params[n_params++] = strdup(pn);
                p = skip_ws(p);
                if (*p == ',') p++;
            }
            if (*p == ')') p++;
            p = skip_ws(p);
            /* body until EOL */
            const char *body_start = p;
            const char *body_end   = skip_to_eol(p);
            char *body = malloc(body_end - body_start + 1);
            memcpy(body, body_start, body_end - body_start);
            body[body_end - body_start] = '\0';
            macro_define(name, body, params, n_params, 1);
            free(body);
            for (int i = 0; i < n_params; i++) free(params[i]);
        } else {
            /* object-like */
            if (*p == ' ' || *p == '\t') p++;
            const char *body_start = p;
            const char *body_end   = skip_to_eol(p);
            char *body = malloc(body_end - body_start + 1);
            memcpy(body, body_start, body_end - body_start);
            body[body_end - body_start] = '\0';
            macro_define(name, body, NULL, 0, 0);
            free(body);
        }
        return;
    }
    if (strcmp(directive, "undef") == 0) {
        char name[256]; read_ident(p, name, sizeof name);
        macro_undef(name);
        return;
    }

    /* ------ include ------ */
    if (strcmp(directive, "include") == 0) {
        if (include_depth > MAX_INCLUDE) {
            fprintf(stderr, "warning: max include depth reached\n");
            return;
        }
        int is_system = 0;
        char inc_name[1024];
        if (*p == '"') {
            p++;
            const char *e = strchr(p, '"');
            if (!e) return;
            size_t n = (size_t)(e - p);
            memcpy(inc_name, p, n); inc_name[n] = '\0';
        } else if (*p == '<') {
            p++;
            const char *e = strchr(p, '>');
            if (!e) return;
            size_t n = (size_t)(e - p);
            memcpy(inc_name, p, n); inc_name[n] = '\0';
            is_system = 1;
        } else {
            return;
        }

        char *content = find_include(inc_name, is_system);
        if (!content) {
            /* silently skip missing includes (common for system headers) */
            buf_append(out, "\n", 1);
            return;
        }
        /* preprocess included file */
        char *expanded = macro_preprocess(content, inc_name, include_depth + 1);
        free(content);
        buf_append(out, expanded, strlen(expanded));
        buf_putc(out, '\n');
        free(expanded);
        return;
    }

    /* ------ pragma / line / error / warning – silently ignore ------ */
    /* nothing */
}

static void preprocess_into(const char *src, const char *filename,
                              Buf *out, int include_depth,
                              int *if_stack, int *if_depth) {
    const char *p = src;

    while (*p) {
        /* line continuation */
        if (*p == '\\' && *(p+1) == '\n') { p += 2; continue; }

        /* find end of logical line */
        const char *line_start = p;
        while (*p && *p != '\n') {
            if (*p == '\\' && *(p+1) == '\n') { p += 2; continue; }
            /* skip strings */
            if (*p == '"') {
                p++;
                while (*p && *p != '"') { if (*p == '\\' && *(p+1)) p++; p++; }
                if (*p) p++;
                continue;
            }
            p++;
        }
        size_t line_len = (size_t)(p - line_start);
        char *line = malloc(line_len + 1);
        memcpy(line, line_start, line_len); line[line_len] = '\0';
        if (*p == '\n') p++;

        const char *lp = skip_ws(line);

        if (*lp == '#') {
            process_directive(lp, filename, out, include_depth, if_stack, if_depth);
        } else {
            /* check if currently active */
            int active = 1;
            for (int i = 0; i < *if_depth; i++) if (!if_stack[i]) { active = 0; break; }
            if (active) {
                /* expand macros in line */
                expand_text(line, out, 0);
                buf_putc(out, '\n');
            } else {
                buf_putc(out, '\n'); /* preserve line numbers */
            }
        }
        free(line);
    }
}

/* ---------------------------------------------------------------- public API */

char *macro_preprocess(const char *src, const char *filename, int include_depth) {
    Buf out; buf_init(&out);
    int if_stack[64] = {0};
    int if_depth = 0;

    /* predefine common macros */
    if (include_depth == 0) {
        macro_define("__C0C__",   "1",    NULL, 0, 0);
        macro_define("__STDC__",  "1",    NULL, 0, 0);
        macro_define("NULL",      "((void*)0)", NULL, 0, 0);
        macro_define("__LP64__",  "1",    NULL, 0, 0);
    }

    preprocess_into(src, filename, &out, include_depth, if_stack, &if_depth);
    return out.data;  /* caller must free */
}