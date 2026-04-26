#include "ast.h"
#include <stdlib.h>

StructDef g_struct_defs[64];
int g_struct_def_cnt = 0;

ASTNode* new_node(ASTNodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;
    return n;
}
