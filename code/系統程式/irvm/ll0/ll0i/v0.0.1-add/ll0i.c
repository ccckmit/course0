#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGS 256
#define MAX_LINE_LEN 256

// 模擬 LLVM 的虛擬暫存器 (例如 %1, %2...)
// 索引 1 代表 %1，儲存對應的整數值
int virtual_registers[MAX_REGS] = {0};

void execute_llvm_ir(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("錯誤：找不到檔案 %s\n", filename);
        return;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;

    printf("=== 開始直譯執行 ===\n");

    // 逐行讀取並解析
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        int dest_reg, src1, src2;
        int ret_reg, ret_val;

        // 1. 嘗試解析加法指令 (例如: %1 = add i32 10, 25)
        // 注意：為了簡化，這裡假設 src1 和 src2 都是常數數字
        if (sscanf(line, " %%%d = add i32 %d , %d", &dest_reg, &src1, &src2) == 3) {
            virtual_registers[dest_reg] = src1 + src2;
            printf("[執行] 暫存器 %%%d = %d + %d (結果: %d)\n", 
                   dest_reg, src1, src2, virtual_registers[dest_reg]);
            continue;
        }

        // 2. 嘗試解析返回指令 (回傳暫存器的值，例如: ret i32 %1)
        if (sscanf(line, " ret i32 %%%d", &ret_reg) == 1) {
            printf("[執行] 程式結束，返回值為: %d\n", virtual_registers[ret_reg]);
            break;
        }

        // 3. 嘗試解析返回指令 (回傳常數值，例如: ret i32 0)
        if (sscanf(line, " ret i32 %d", &ret_val) == 1) {
            printf("[執行] 程式結束，返回值為: %d\n", ret_val);
            break;
        }
    }

    printf("=== 執行完畢 ===\n");
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法: %s <filename.ll>\n", argv[0]);
        return 1;
    }

    execute_llvm_ir(argv[1]);
    return 0;
}