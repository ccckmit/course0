#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc != 4 || strcmp(argv[2], "-o") != 0) {
        printf("用法: ./verilog0c <input.v> -o <output.c>\n");
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        printf("錯誤: 無法開啟輸入檔案 %s\n", argv[1]);
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    long fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    
    char *source = (char *)malloc(fsize + 1);
    fread(source, 1, fsize, fin);
    source[fsize] = 0;
    fclose(fin);

    init_lexer(source);
    
    // Parse into AST
    Node *ast = parse_verilog();

    // Code generation
    FILE *fout = fopen(argv[3], "w");
    if (!fout) {
        printf("錯誤: 無法開啟輸出檔案 %s\n", argv[3]);
        return 1;
    }
    
    generate_c_code(ast, fout);
    
    fclose(fout);
    free(source);

    printf("成功從 AST 轉換: %s -> %s\n", argv[1], argv[3]);
    return 0;
}