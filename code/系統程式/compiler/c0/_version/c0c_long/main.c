#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

/* ---------------------------------------------------------------- file utils */

static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "c0c: cannot open '%s'\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz + 2);
    if (!buf) { perror("malloc"); exit(1); }
    size_t nr = fread(buf, 1, sz, f);
    buf[nr] = '\0';
    fclose(f);
    return buf;
}

/* ---------------------------------------------------------------- usage */

static void usage(const char *prog) {
    fprintf(stderr,
        "c0c - a self-hosting C to LLVM IR compiler\n"
        "\n"
        "Usage:\n"
        "  %s -c <input.c> -o <output.ll>\n"
        "  %s -c <input.c>          (writes to stdout)\n"
        "\n"
        "Options:\n"
        "  -c <file>   Compile <file> (required)\n"
        "  -o <file>   Write LLVM IR to <file>\n"
        "  -h          Show this help\n"
        "  -v          Show version\n"
        "\n"
        "c0c is a subset-C compiler that produces LLVM IR (.ll) files.\n"
        "The output can be linked with clang:\n"
        "  clang output.ll -o binary\n",
        prog, prog);
}

/* ---------------------------------------------------------------- compile pipeline */

static void compile(const char *input_path, FILE *out) {
    /* 1. Read source */
    char *src = read_file(input_path);

    /* 2. Preprocess */
    char *preprocessed = macro_preprocess(src, input_path, 0);
    free(src);

    /* 3. Lex + Parse */
    Lexer  *lexer  = lexer_new(preprocessed, input_path);
    Parser *parser = parser_new(lexer);
    Node   *ast    = parser_parse(parser);

    /* 4. Code generation */
    Codegen *cg = codegen_new(out, input_path);
    codegen_emit(cg, ast);

    /* 5. Cleanup */
    codegen_free(cg);
    node_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(preprocessed);
}

/* ---------------------------------------------------------------- main */

int main(int argc, char *argv[]) {
    const char *input_file  = NULL;
    const char *output_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("c0c version 0.1.0\n");
            return 0;
        }
        if (strcmp(argv[i], "-c") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "c0c: -c requires an argument\n");
                return 1;
            }
            input_file = argv[i];
            continue;
        }
        if (strcmp(argv[i], "-o") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "c0c: -o requires an argument\n");
                return 1;
            }
            output_file = argv[i];
            continue;
        }
        /* positional – treat as input file if -c not given */
        if (!input_file) {
            input_file = argv[i];
        } else {
            fprintf(stderr, "c0c: unexpected argument '%s'\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (!input_file) {
        fprintf(stderr, "c0c: no input file\n");
        usage(argv[0]);
        return 1;
    }

    FILE *out = stdout;
    if (output_file) {
        out = fopen(output_file, "w");
        if (!out) {
            fprintf(stderr, "c0c: cannot open output file '%s'\n", output_file);
            return 1;
        }
    }

    compile(input_file, out);

    if (output_file) fclose(out);

    return 0;
}
