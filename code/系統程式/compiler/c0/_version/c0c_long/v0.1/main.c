#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int mode_expand_only = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-E") == 0) {
            mode_expand_only = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i++;
            }
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }
    
    if (!input_file) {
        fprintf(stderr, "用法: ./c0c <input.c> [-o <output.ll>]   (編譯)\n");
        fprintf(stderr, "       ./c0c <input.c> -E [-o <output.i>] (僅展開巨集)\n");
        return 1;
    }
    
    if (mode_expand_only) {
        char *expanded = macro_expand_file(input_file);
        if (!expanded) {
            fprintf(stderr, "無法展開巨集: %s\n", input_file);
            return 1;
        }
        
        if (output_file) {
            FILE *f = fopen(output_file, "w");
            if (!f) {
                perror("fopen output");
                free(expanded);
                return 1;
            }
            fputs(expanded, f);
            fclose(f);
            printf("[成功] 已展開巨集 %s 至 %s\n", input_file, output_file);
        } else {
            printf("%s", expanded);
        }
        
        free(expanded);
        return 0;
    }
    
    FILE *f = fopen(input_file, "rb");
    if (!f) error("無法開啟輸入檔案");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    src = malloc(fsize + 1);
    fread(src, 1, fsize, f);
    src[fsize] = 0;
    fclose(f);
    
    char *expanded = macro_expand(src);
    free(src);
    src = expanded;
    
    if (!output_file) {
        output_file = "a.ll";
    }
    
    FILE *out = fopen(output_file, "w");
    if (!out) error("無法開啟輸出檔案");
    
    p = src;
    next_token();
    ASTNode *ast = parse_program();
    gen_llvm_ir(ast, out);
    
    printf("[成功] 已將 %s 編譯至 %s\n", input_file, output_file);
    fclose(out);
    free(src);
    return 0;
}
