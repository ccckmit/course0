#ifndef C0C_CODEGEN_H
#define C0C_CODEGEN_H

#include "ast.h"
#include <stdio.h>

void gen_llvm_ir(ASTNode *funcs, FILE *out);

#endif
