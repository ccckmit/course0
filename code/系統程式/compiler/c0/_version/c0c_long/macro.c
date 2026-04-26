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

/* ---------------------------------------------------------------- dynamic string buffer
   Buf is stored as a pointer to a 3-word heap block: [data_ptr, len, cap]
   accessed via helper functions to avoid struct field offset issues in c0c's IR.
   c0c treats ALL struct field accesses as offset 0, so we can't use a real struct. */

/* BUF_INIT = initial capacity */

/* A Buf is a char** pointing to a 3-element array:
   buf[0] = (char*) data pointer
   buf[1] = (char*) len (cast from size_t)
   buf[2] = (char*) cap (cast from size_t)
   
   We use char** so c0c indexes with ptr stride (8 bytes each). */
typedef char **Buf;

static Buf buf_new(void) {
    Buf b = calloc(3, sizeof(char*));
    b[0] = malloc(BUF_INIT);  /* data */
    b[0][0] = '\0';
    b[1] = (char*)0;          /* len = 0 */
    b[2] = (char*)BUF_INIT;   /* cap = BUF_INIT */
    return b;
}

static void buf_init(Buf b) {
    b[0] = malloc(BUF_INIT);
    if (!b[0]) { perror("malloc"); exit(1); }
    b[0][0] = '\0';
    b[1] = (char*)0;
    b[2] = (char*)BUF_INIT;
}

#define buf_data(b) ((b)[0])
#define buf_len(b)  ((size_t)(b)[1])
#define buf_cap(b)  ((size_t)(b)[2])
#define buf_set_len(b,v) ((b)[1] = (char*)(size_t)(v))
#define buf_set_cap(b,v) ((b)[2] = (char*)(size_t)(v))

static void buf_grow(Buf b, size_t need) {
    size_t len = buf_len(b);
    size_t cap = buf_cap(b);
    if (cap == 0) cap = BUF_INIT;  /* safety: prevent infinite loop if cap uninitialized */
    while (len + need + 1 > cap) cap *= 2;
    if (cap != buf_cap(b)) {
        b[0] = realloc(b[0], cap);
        if (!b[0]) { perror("realloc"); exit(1); }
        buf_set_cap(b, cap);
    }
}

static void buf_append(Buf b, const char *s, size_t n) {
    buf_grow(b, n);
    size_t len = buf_len(b);
    memcpy(b[0] + len, s, n);
    buf_set_len(b, len + n);
    b[0][len + n] = '\0';
}

static void buf_putc(Buf b, char c) {
    buf_grow(b, 1);
    size_t len = buf_len(b);
    b[0][len] = c;
    buf_set_len(b, len + 1);
    b[0][len + 1] = '\0';
}

/* ---------------------------------------------------------------- macro table */

/* Macro table: use parallel arrays instead of struct arrays.
   c0c's simplified IR generates all struct field accesses at offset 0,
   so structs with multiple fields get corrupted. Parallel arrays avoid this. */
#define MACRO_NAME     0
#define MACRO_BODY     1
/* params stored separately per macro index */

static char **macro_names    = NULL;  /* macro_names[i] = name string */
static char **macro_bodies   = NULL;  /* macro_bodies[i] = body string */
static char **macro_params_0 = NULL;  /* param 0 for each macro */
static char **macro_params_1 = NULL;
static char **macro_params_2 = NULL;
static char **macro_params_3 = NULL;
static char **macro_params_4 = NULL;
static char **macro_params_5 = NULL;
static char **macro_params_6 = NULL;
static char **macro_params_7 = NULL;
static int   *macro_nparams  = NULL;  /* n_params per macro */
static int   *macro_funclike = NULL;  /* is_func_like per macro */
static int   *macro_defined  = NULL;  /* is_defined per macro */
static int    n_macros       = 0;

static void macros_init(void) {
    if (macro_names) return;
    macro_names    = calloc(MAX_MACROS, sizeof(char*));
    macro_bodies   = calloc(MAX_MACROS, sizeof(char*));
    macro_params_0 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_1 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_2 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_3 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_4 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_5 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_6 = calloc(MAX_MACROS, sizeof(char*));
    macro_params_7 = calloc(MAX_MACROS, sizeof(char*));
    macro_nparams  = calloc(MAX_MACROS, sizeof(int));
    macro_funclike = calloc(MAX_MACROS, sizeof(int));
    macro_defined  = calloc(MAX_MACROS, sizeof(int));
}

/* Helper to set/get params by index — avoids params[MAX_PARAMS] struct array */
static char *macro_get_param(int mi, int pi) {
    if (pi == 0) return macro_params_0[mi];
    if (pi == 1) return macro_params_1[mi];
    if (pi == 2) return macro_params_2[mi];
    if (pi == 3) return macro_params_3[mi];
    if (pi == 4) return macro_params_4[mi];
    if (pi == 5) return macro_params_5[mi];
    if (pi == 6) return macro_params_6[mi];
    if (pi == 7) return macro_params_7[mi];
    return NULL;
}
static void macro_set_param(int mi, int pi, char *v) {
    if (pi == 0) macro_params_0[mi] = v;
    else if (pi == 1) macro_params_1[mi] = v;
    else if (pi == 2) macro_params_2[mi] = v;
    else if (pi == 3) macro_params_3[mi] = v;
    else if (pi == 4) macro_params_4[mi] = v;
    else if (pi == 5) macro_params_5[mi] = v;
    else if (pi == 6) macro_params_6[mi] = v;
    else if (pi == 7) macro_params_7[mi] = v;
}

/* Find macro index by name, returns -1 if not found */
static int macro_find_idx(const char *name) {
    if (!macro_names) return -1;
    for (int i = 0; i < n_macros; i++)
        if (macro_defined[i] && macro_names[i] && strcmp(macro_names[i], name) == 0)
            return i;
    return -1;
}

/* Return body of named macro, or NULL if not defined */
static const char *macro_find_body(const char *name) {
    int i = macro_find_idx(name);
    return (i >= 0) ? macro_bodies[i] : NULL;
}

static void macro_define(const char *name, const char *body,
                          char **params, int n_params, int is_func) {
    if (!macro_names) macros_init();
    /* replace existing */
    int idx = macro_find_idx(name);
    if (idx < 0) {
        /* also check undefined slots */
        for (int i = 0; i < n_macros; i++) {
            if (macro_names[i] && strcmp(macro_names[i], name) == 0) {
                idx = i; break;
            }
        }
    }
    if (idx >= 0) {
        free(macro_bodies[idx]);
        macro_bodies[idx] = strdup(body);
        macro_defined[idx] = 1;
        return;
    }
    if (n_macros >= MAX_MACROS) {
        fprintf(stderr, "macro table full\n");
        return;
    }
    idx = n_macros++;
    macro_names[idx]    = strdup(name);
    macro_bodies[idx]   = strdup(body);
    macro_nparams[idx]  = n_params;
    macro_funclike[idx] = is_func;
    macro_defined[idx]  = 1;
    for (int i = 0; i < n_params && i < MAX_PARAMS; i++)
        macro_set_param(idx, i, params[i] ? strdup(params[i]) : NULL);
}

static void macro_undef(const char *name) {
    if (!macro_names) return;
    int idx = macro_find_idx(name);
    if (idx >= 0) macro_defined[idx] = 0;
}

/* Compatibility shim — old code used Macro* pointers */
typedef struct {
    const char *name;
    const char *body;
    int n_params;
    int is_func_like;
    int is_defined;
    int idx;  /* index into parallel arrays */
} MacroRef;

static MacroRef macro_find_ref(const char *name) {
    MacroRef r = {NULL, NULL, 0, 0, 0, -1};
    int i = macro_find_idx(name);
    if (i >= 0) {
        r.name = macro_names[i];
        r.body = macro_bodies[i];
        r.n_params = macro_nparams[i];
        r.is_func_like = macro_funclike[i];
        r.is_defined = macro_defined[i];
        r.idx = i;
    }
    return r;
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
static void expand_text(const char *src, Buf out, int depth);

static void expand_func_macro(int mi, const char **src_ptr, Buf out, int depth) {
    const char *m_name      = macro_names[mi];
    const char *m_body      = macro_bodies[mi];
    int         m_n_params  = macro_nparams[mi];
    int         m_is_vadic  = (m_n_params > 0 && macro_get_param(mi, m_n_params-1) &&
                               strcmp(macro_get_param(mi, m_n_params-1), "...") == 0);
    const char *p = *src_ptr;
    p = skip_ws(p);
    if (*p != '(') {
        /* not a call, emit name as-is */
        buf_append(out, m_name, strlen(m_name));
        return;
    }
    p++; /* skip '(' */

    /* collect arguments */
    char *args[MAX_PARAMS] = {NULL};
    int   n_args = 0;
    int   paren_depth = 0;
    Buf arg_buf = buf_new();

    while (*p && !(paren_depth == 0 && *p == ')')) {
        if (*p == ',' && paren_depth == 0) {
            if (n_args < MAX_PARAMS) args[n_args++] = strdup(buf_data(arg_buf));
            buf_set_len(arg_buf, 0); buf_data(arg_buf)[0] = '\0';
            p++;
        } else if (*p == '"') {
            /* skip string literal without counting parens inside */
            buf_putc(arg_buf, *p++);
            while (*p && *p != '"') {
                if (*p == '\\' && *(p+1)) buf_putc(arg_buf, *p++);
                buf_putc(arg_buf, *p++);
            }
            if (*p) buf_putc(arg_buf, *p++);
        } else if (*p == '\'') {
            /* skip char literal */
            buf_putc(arg_buf, *p++);
            if (*p == '\\' && *(p+1)) buf_putc(arg_buf, *p++);
            if (*p) buf_putc(arg_buf, *p++);
            if (*p == '\'') buf_putc(arg_buf, *p++);
        } else {
            if (*p == '(') paren_depth++;
            if (*p == ')') paren_depth--;
            if (paren_depth >= 0) buf_putc(arg_buf, *p++);
            else break;
        }
    }
    /* last arg */
    if (buf_len(arg_buf) > 0 || n_args > 0)
        if (n_args < MAX_PARAMS) args[n_args++] = strdup(buf_data(arg_buf));
    free(buf_data(arg_buf)); free(arg_buf);
    if (*p == ')') p++;
    *src_ptr = p;

    /* substitute params in body */
    const char *body = m_body;
    Buf expanded = buf_new();
    while (*body) {
        /* Handle # stringification operator */
        if (*body == '#' && *(body+1) != '#') {
            body++;
            while (*body == ' ' || *body == '\t') body++;
            char param_name[256];
            const char *end = read_ident(body, param_name, sizeof param_name);
            int found = 0;
            for (int i = 0; i < m_n_params && i < MAX_PARAMS; i++) {
                char *pn = macro_get_param(mi, i);
                if (pn && strcmp(pn, param_name) == 0) {
                    buf_putc(expanded, '"');
                    if (i < n_args && args[i]) {
                        const char *av = args[i];
                        while (*av) {
                            if (*av == '"' || *av == '\\') buf_putc(expanded, '\\');
                            buf_putc(expanded, *av++);
                        }
                    }
                    buf_putc(expanded, '"');
                    found = 1;
                    break;
                }
            }
            if (!found) { buf_putc(expanded, '"'); buf_putc(expanded, '"'); }
            body = end;
            continue;
        }
        /* Handle ## token-pasting operator */
        if (*body == '#' && *(body+1) == '#') {
            body += 2;
            while (*body == ' ' || *body == '\t') body++;
            continue;
        }
        if (isalpha((unsigned char)*body) || *body == '_') {
            char ident[256];
            const char *end = read_ident(body, ident, sizeof ident);
            int found = 0;
            /* handle __VA_ARGS__ */
            if (strcmp(ident, "__VA_ARGS__") == 0) {
                for (int i = m_n_params; i < n_args; i++) {
                    if (i > m_n_params) buf_putc(expanded, ',');
                    if (args[i]) buf_append(expanded, args[i], strlen(args[i]));
                }
                found = 1;
            }
            if (!found) {
                for (int i = 0; i < m_n_params && i < MAX_PARAMS; i++) {
                    char *pn = macro_get_param(mi, i);
                    if (pn && strcmp(pn, ident) == 0) {
                        if (i < n_args && args[i])
                            buf_append(expanded, args[i], strlen(args[i]));
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) buf_append(expanded, ident, strlen(ident));
            body = end;
        } else {
            buf_putc(expanded, *body++);
        }
    }
    /* recursively expand result */
    expand_text(buf_data(expanded), out, depth + 1);
    free(buf_data(expanded)); free(expanded);

    for (int i = 0; i < n_args; i++) free(args[i]);
}

static void expand_text(const char *src, Buf out, int depth) {
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
            int mi = macro_find_idx(ident);
            if (mi >= 0 && macro_funclike[mi]) {
                const char *q = end;
                q = skip_ws(q);
                if (*q == '(') {
                    p = end;
                    expand_func_macro(mi, &p, out, depth + 1);
                    continue;
                }
            }
            if (mi >= 0 && !macro_funclike[mi]) {
                expand_text(macro_bodies[mi], out, depth + 1);
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

/* search paths for includes — returned by a function to avoid global array issues */
static const char **get_include_paths(void) {
    static const char *paths[4];
    static int init = 0;
    if (!init) {
        paths[0] = "/usr/include";
        paths[1] = "/usr/local/include";
        paths[2] = ".";
        paths[3] = NULL;
        init = 1;
    }
    return paths;
}

static char *find_include(const char *name, int is_system) {
    if (!is_system) {
        /* relative to CWD first */
        char *content = read_file(name);
        if (content) return content;
    }
    const char **INCLUDE_PATHS = get_include_paths();
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
                              Buf out, int depth,
                              /* if-stack */
                              int *if_stack, int *if_depth);

static void process_directive(const char *line, const char *filename,
                               Buf out, int include_depth,
                               int *if_stack, int *if_depth) {
    const char *p = skip_ws(line + 1); /* skip '#' */

    char directive[64];
    p = read_ident(p, directive, sizeof directive);
    p = skip_ws(p);

    /* ------ if / ifdef / ifndef / elif / else / endif ------ */
    if (strcmp(directive, "ifdef") == 0) {
        char name[256]; read_ident(p, name, sizeof name);
        int val = (macro_find_idx(name) >= 0);
        if (*if_depth < 32) if_stack[(*if_depth)++] = val;
        return;
    }
    if (strcmp(directive, "ifndef") == 0) {
        char name[256]; read_ident(p, name, sizeof name);
        int val = (macro_find_idx(name) < 0);
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
            val = (macro_find_idx(name) >= 0);
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
            int variadic_macro = 0;
            while (*p && *p != ')') {
                p = skip_ws(p);
                if (*p == ')') break;
                if (*p == '.' && *(p+1) == '.' && *(p+2) == '.') {
                    variadic_macro = 1;
                    p += 3;
                    p = skip_ws(p);
                    break;
                }
                char pn[64]; p = read_ident(p, pn, sizeof pn);
                if (*pn && n_params < MAX_PARAMS)
                    params[n_params++] = strdup(pn);
                p = skip_ws(p);
                if (*p == ',') p++;
            }
            (void)variadic_macro;
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
            /* silently skip unresolvable includes */
            buf_append(out, "\n", 1);
            return;
        }
        /* skip system headers during self-compilation - they contain
           GCC-specific extensions our parser doesn't support */
        if (is_system) {
            free(content);
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
                              Buf out, int include_depth,
                              int *if_stack, int *if_depth) {
    const char *p = src;

    while (*p) {
        /* line continuation */
        if (*p == '\\' && *(p+1) == '\n') { p += 2; continue; }

        /* Extract logical line, joining backslash-newline continuations */
        Buf line_buf = buf_new();
        while (*p && *p != '\n') {
            if (*p == '\\' && *(p+1) == '\n') {
                /* line continuation: skip backslash+newline, continue on next line */
                p += 2;
                /* skip leading whitespace on continuation line */
                /* (don't skip — preserve for macro bodies) */
                continue;
            }
            if (*p == '\'') {
                /* char literal — copy verbatim without interpreting contents */
                buf_putc(line_buf, *p++);
                if (*p == '\\' && *(p+1) && *(p+1) != '\n') {
                    buf_putc(line_buf, *p++);
                }
                if (*p && *p != '\n') buf_putc(line_buf, *p++);
                if (*p == '\'') buf_putc(line_buf, *p++);
                continue;
            }
            if (*p == '"') {
                buf_putc(line_buf, *p++);
                while (*p && *p != '"' && *p != '\n') {
                    if (*p == '\\' && *(p+1)) buf_putc(line_buf, *p++);
                    buf_putc(line_buf, *p++);
                }
                if (*p == '"') buf_putc(line_buf, *p++);
                continue;
            }
            if (*p == '/' && *(p+1) == '/') {
                /* line comment: stop here */
                while (*p && *p != '\n') p++;
                break;
            }
            if (*p == '/' && *(p+1) == '*') {
                /* block comment: may span lines */
                p += 2;
                while (*p && !(*p == '*' && *(p+1) == '/')) {
                    if (*p == '\n') buf_putc(line_buf, ' ');
                    p++;
                }
                if (*p) p += 2;
                continue;
            }
            buf_putc(line_buf, *p++);
        }
        char *line = buf_data(line_buf);
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
    macros_init();
    Buf out = buf_new();
    int if_stack[64] = {0};
    int if_depth = 0;

    /* predefine common macros */
    if (include_depth == 0) {
        macro_define("__C0C__",   "1",    NULL, 0, 0);
        macro_define("__STDC__",  "1",    NULL, 0, 0);
        macro_define("NULL",      "((void*)0)", NULL, 0, 0);
        macro_define("__LP64__",  "1",    NULL, 0, 0);
        /* stdio SEEK constants */
        macro_define("SEEK_SET",  "0",    NULL, 0, 0);
        macro_define("SEEK_CUR",  "1",    NULL, 0, 0);
        macro_define("SEEK_END",  "2",    NULL, 0, 0);
        /* common size macros */
        macro_define("EOF",       "(-1)", NULL, 0, 0);
        macro_define("EXIT_SUCCESS", "0", NULL, 0, 0);
        macro_define("EXIT_FAILURE", "1", NULL, 0, 0);
        /* assert — already handled by replacing calls with if/exit */
        macro_define("assert",    "((void)0)", NULL, 0, 0);
        /* va_list macros — simple object-like replacements.
           va_start(ap,last) becomes __c0c_va_start(ap,last)
           We do this by making va_start expand to the wrapper name token only,
           relying on the call site to provide the parenthesized args. */
        macro_define("va_start",  "__c0c_va_start", NULL, 0, 0);
        macro_define("va_end",    "__c0c_va_end",   NULL, 0, 0);
        macro_define("va_copy",   "__c0c_va_copy",  NULL, 0, 0);
        /* stdio FILE* handles — use getter functions for portability */
        macro_define("stderr",    "__c0c_stderr()", NULL, 0, 0);
        macro_define("stdout",    "__c0c_stdout()", NULL, 0, 0);
        macro_define("stdin",     "__c0c_stdin()",  NULL, 0, 0);
    }

    preprocess_into(src, filename, out, include_depth, if_stack, &if_depth);
    return buf_data(out);  /* caller must free */
}
