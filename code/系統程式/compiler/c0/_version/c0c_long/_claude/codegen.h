#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

#define MAX_LOCALS 1024
#define MAX_GLOBALS 1024
#define MAX_STRINGS 256

typedef struct {
    char *name;       /* C name */
    char *llvm_name;  /* alloca register, e.g. %x */
    Type *type;
    int   is_param;   /* parameters: already a register, no alloca needed */
} Local;

typedef struct {
    char *name;
    Type *type;
    int   is_extern;
} Global;

struct Codegen {
    FILE   *out;
    const char *source_filename;

    /* counters */
    int    reg;      /* SSA register counter */
    int    label;    /* label counter */
    int    str_id;   /* string literal counter */

    /* scope */
    Local   locals[MAX_LOCALS];
    int     n_locals;
    int     scope_stack[64];  /* n_locals at scope entry */
    int     scope_depth;

    /* globals */
    Global  globals[MAX_GLOBALS];
    int     n_globals;

    /* string literals deferred to top-level */
    char   *str_literals[MAX_STRINGS];
    int     str_ids[MAX_STRINGS];
    int     n_strings;

    /* break/continue targets */
    char   break_label[64];
    char   cont_label[64];

    /* current function return type */
    Type  *cur_ret_type;
    char   cur_func[128];
};

typedef struct Codegen Codegen;

Codegen *codegen_new(FILE *out, const char *source_filename);
void     codegen_free(Codegen *cg);
void     codegen_emit(Codegen *cg, Node *tu);  /* emit whole translation unit */

#endif /* CODEGEN_H */