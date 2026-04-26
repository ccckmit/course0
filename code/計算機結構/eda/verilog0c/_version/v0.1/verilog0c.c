#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int main(int argc, char *argv[]) {
    // 1. 解析命令列參數
    if (argc != 4 || strcmp(argv[2], "-o") != 0) {
        printf("用法: ./verilog0c <input.v> -o <output.c>\n");
        return 1;
    }

    const char *in_filename = argv[1];
    const char *out_filename = argv[3];

    FILE *fin = fopen(in_filename, "r");
    if (!fin) {
        printf("錯誤: 無法開啟輸入檔案 %s\n", in_filename);
        return 1;
    }

    FILE *fout = fopen(out_filename, "w");
    if (!fout) {
        printf("錯誤: 無法開啟輸出檔案 %s\n", out_filename);
        fclose(fin);
        return 1;
    }

    char line[512];
    char declarations[4096] = ""; 
    char assignments[4096] = "";  
    char testbench_body[4096] = ""; 
    char declared_vars[4096] = " "; 
    
    int in_initial = 0;

    // 2. 逐行解析 Verilog
    while (fgets(line, sizeof(line), fin)) {
        char *p = trim(line);
        if (strlen(p) == 0 || strncmp(p, "//", 2) == 0) continue; // 略過空行與註解

        if (strncmp(p, "module ", 7) == 0) {
            // 目前略過處理 module name，所有變數將成全域， assignments 也混合
        } 
        else if (strncmp(p, "input ", 6) == 0 || 
                 strncmp(p, "output ", 7) == 0 || 
                 strncmp(p, "wire ", 5) == 0 ||
                 strncmp(p, "reg ", 4) == 0) {
            // 找出變數名稱起點
            char *var_start = strchr(p, ' ');
            if (var_start) {
                var_start = trim(var_start);
                // 剔除結尾的分號
                char vname[64];
                sscanf(var_start, "%[^;]", vname);
                char *v = trim(vname);
                
                // 避免重複宣告同名變數
                char search[128];
                sprintf(search, " %s ", v);
                if (!strstr(declared_vars, search)) {
                    sprintf(declared_vars + strlen(declared_vars), "%s ", v);
                    sprintf(declarations + strlen(declarations), "uint8_t %s;\n", v);
                }
            }
        } 
        else if (strncmp(p, "assign ", 7) == 0) {
            // 提取等式
            char *eq_start = trim(p + 7);
            sprintf(assignments + strlen(assignments), "    %s\n", eq_start);
        }
        else if (strncmp(p, "initial", 7) == 0) {
            in_initial = 1;
        }
        else if (strncmp(p, "endmodule", 9) == 0) {
            // ignore
        }
        else if (strcmp(p, "end") == 0) {
            in_initial = 0;
        }
        else if (in_initial) {
            // 處理 testbench initial block 內的指令
            if (p[0] == '#') {
                // 將 delay 當作運算觸發
                sprintf(testbench_body + strlen(testbench_body), "    eval_all();\n");
            }
            else if (strncmp(p, "$display", 8) == 0) {
                // 將 $display 轉成 printf
                char *lparen = strchr(p, '(');
                char *rparen = strrchr(p, ')');
                if (lparen && rparen) {
                    *rparen = '\0';
                    char *args = lparen + 1;
                    char new_args[512] = "";
                    int len = strlen(args);
                    int in_fmt = 0;
                    int i, j = 0;
                    for (i = 0; i < len; i++) {
                        if (args[i] == '"') {
                            if (!in_fmt) {
                                in_fmt = 1;
                                new_args[j++] = args[i];
                            } else {
                                // 準備離開字串時加上換行
                                new_args[j++] = '\\';
                                new_args[j++] = 'n';
                                new_args[j++] = '"';
                                in_fmt = 0;
                            }
                        } else {
                            if (args[i] == '%') {
                                if (i + 1 < len && args[i+1] == 'b') {
                                    new_args[j++] = '%';
                                    new_args[j++] = 'd'; // 將 %b 轉為 %d
                                    i++; // skip 'b'
                                } else {
                                    new_args[j++] = args[i];
                                }
                            } else {
                                new_args[j++] = args[i];
                            }
                        }
                    }
                    new_args[j] = '\0';
                    sprintf(testbench_body + strlen(testbench_body), "    printf(%s);\n", new_args);
                } else {
                    sprintf(testbench_body + strlen(testbench_body), "    printf(\"%%s\\n\", \"%s\");\n", p);
                }
            }
            else {
                // 其他陳述句，例如變數賦值 a = 0; b = 1;
                sprintf(testbench_body + strlen(testbench_body), "    %s %s\n", p, strchr(p, ';') ? "" : ";");
            }
        }
    }

    // 3. 輸出 C 語言程式碼
    fprintf(fout, "/* 由 verilog0c 自動生成 */\n");
    fprintf(fout, "#include <stdio.h>\n");
    fprintf(fout, "#include <stdint.h>\n\n");
    
    fprintf(fout, "// 變數宣告 (對應 wire, reg, input, output)\n");
    fprintf(fout, "%s\n", declarations);
    
    fprintf(fout, "// 組合邏輯運算 (Combinational Logic)\n");
    fprintf(fout, "void eval_all() {\n");
    fprintf(fout, "%s", assignments);
    fprintf(fout, "}\n\n");

    fprintf(fout, "// --- 自動生成的 Testbench 主程式 ---\n");
    fprintf(fout, "int main() {\n");
    if (strlen(testbench_body) > 0) {
        fprintf(fout, "%s", testbench_body);
    } else {
        fprintf(fout, "    eval_all();\n");
    }
    fprintf(fout, "    return 0;\n");
    fprintf(fout, "}\n");

    fclose(fin);
    fclose(fout);

    printf("成功轉換: %s -> %s\n", in_filename, out_filename);
    return 0;
}