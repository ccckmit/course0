#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdio.h>

void generate_c_code(Node* root, FILE* fout);

#endif
