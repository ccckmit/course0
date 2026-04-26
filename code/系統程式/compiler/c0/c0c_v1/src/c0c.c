/*
 * c0c.c – main driver for the c0c C→LLVM IR compiler
 *
 * Usage: c0c <input.c> -o <output.ll>
 *
 * The input is assumed to be preprocessed (or self-contained without
 * system headers).  For real use, pipe through gcc -E first.
 */

#include "parser.h"
#include "ast.h"
#include "codegen_ll.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *input  = NULL;
    const char *output = NULL;
    const char *target = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
        } else if (strcmp(argv[i], "-target") == 0 && i + 1 < argc) {
            target = argv[++i];
        } else if (argv[i][0] != '-') {
            input = argv[i];
        }
    }

    if (!input) {
        fprintf(stderr, "usage: c0c <input.c> [-o output.ll] [-target <triple>]\n");
        return 1;
    }

    /* Parse */
    Parser *p = parser_new_file(input);
    if (!p) return 1;

    Node *root = parser_parse(p);
    if (!root || parser_errors(p)) {
        fprintf(stderr, "c0c: parse failed with %d error(s)\n", parser_errors(p));
        parser_free(p);
        return 1;
    }

    /* Open output */
    FILE *out = stdout;
    if (output && strcmp(output, "-") != 0) {
        out = fopen(output, "w");
        if (!out) { perror(output); parser_free(p); return 1; }
    }

    /* Generate */
    Codegen *cg = codegen_new(out, input, target);
    codegen_emit(cg, root);
    codegen_free(cg);

    if (out != stdout) fclose(out);
    parser_free(p);
    return 0;
}
