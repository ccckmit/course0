/* 由 verilog0c 自動生成 */
#include <stdio.h>
#include <stdint.h>

// 變數宣告 (對應 wire, reg, input, output)
uint8_t d0;
uint8_t d1;
uint8_t sel;
uint8_t y;

// 組合邏輯運算 (Combinational Logic)
void eval_all() {
    y = ((!sel) & d0) | (sel & d1);
}

// --- 自動生成的 Testbench 主程式 ---
int main() {
    d0 = 0; d1 = 1; sel = 0; 
    eval_all();
    printf("d0=%d, d1=%d, sel=%d => y=%d\n", d0, d1, sel, y);
    d0 = 0; d1 = 1; sel = 1; 
    eval_all();
    printf("d0=%d, d1=%d, sel=%d => y=%d\n", d0, d1, sel, y);
    d0 = 1; d1 = 0; sel = 0; 
    eval_all();
    printf("d0=%d, d1=%d, sel=%d => y=%d\n", d0, d1, sel, y);
    d0 = 1; d1 = 0; sel = 1; 
    eval_all();
    printf("d0=%d, d1=%d, sel=%d => y=%d\n", d0, d1, sel, y);
    return 0;
}
