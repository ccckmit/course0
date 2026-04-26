#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

typedef struct Codegen Codegen;

Codegen *codegen_new(FILE *out, const char *source_filename);
void     codegen_free(Codegen *cg);
void     codegen_emit(Codegen *cg, Node *tu);  /* emit whole translation unit */

#endif /* CODEGEN_H */
