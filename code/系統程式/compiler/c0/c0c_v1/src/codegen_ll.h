#ifndef CODEGEN_LL_H
#define CODEGEN_LL_H

#include "ast.h"
#include <stdio.h>

/*
 * codegen_ll – LLVM IR code generator for c0c
 *
 * Translates an AST (ND_TRANSLATION_UNIT) into LLVM IR (.ll) format.
 * The output follows clang -O0 conventions:
 *   • alloca for every local variable (mem2reg not applied)
 *   • SSA registers numbered from %1 continuously per function
 *   • ptr type for all pointers (opaque pointer model, LLVM 15+)
 *   • i32 for int, i8 for char, i64 for long, double for double, etc.
 */

typedef struct Codegen Codegen;

/* Create a codegen that writes to 'out'. */
Codegen *codegen_new(FILE *out, const char *source_filename, const char *target);

/* Generate IR for the entire translation unit. */
void codegen_emit(Codegen *cg, Node *root);

/* Free resources. */
void codegen_free(Codegen *cg);

#endif /* CODEGEN_LL_H */
