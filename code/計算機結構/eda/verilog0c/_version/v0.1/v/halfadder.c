/* 由 verilog0c 自動生成 */
#include <stdio.h>
#include <stdint.h>

// 變數宣告 (對應 wire, reg, input, output)
uint8_t a;
uint8_t b;
uint8_t sum;
uint8_t cout;

// 組合邏輯運算 (Combinational Logic)
void eval_all() {
    sum = a ^ b;
    cout = a & b;
}

// --- 自動生成的 Testbench 主程式 ---
int main() {
    a = 0; b = 0; 
    eval_all();
    printf("a=%d, b=%d => sum=%d, cout=%d\n", a, b, sum, cout);
    a = 0; b = 1; 
    eval_all();
    printf("a=%d, b=%d => sum=%d, cout=%d\n", a, b, sum, cout);
    a = 1; b = 0; 
    eval_all();
    printf("a=%d, b=%d => sum=%d, cout=%d\n", a, b, sum, cout);
    a = 1; b = 1; 
    eval_all();
    printf("a=%d, b=%d => sum=%d, cout=%d\n", a, b, sum, cout);
    return 0;
}
