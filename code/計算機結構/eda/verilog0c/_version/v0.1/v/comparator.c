/* 由 verilog0c 自動生成 */
#include <stdio.h>
#include <stdint.h>

// 變數宣告 (對應 wire, reg, input, output)
uint8_t a;
uint8_t b;
uint8_t eq;
uint8_t neq;

// 組合邏輯運算 (Combinational Logic)
void eval_all() {
    eq = !(a ^ b);
    neq = a ^ b;
}

// --- 自動生成的 Testbench 主程式 ---
int main() {
    a = 0; b = 0; 
    eval_all();
    printf("a=%d, b=%d => eq=%d, neq=%d\n", a, b, eq, neq);
    a = 0; b = 1; 
    eval_all();
    printf("a=%d, b=%d => eq=%d, neq=%d\n", a, b, eq, neq);
    a = 1; b = 0; 
    eval_all();
    printf("a=%d, b=%d => eq=%d, neq=%d\n", a, b, eq, neq);
    a = 1; b = 1; 
    eval_all();
    printf("a=%d, b=%d => eq=%d, neq=%d\n", a, b, eq, neq);
    return 0;
}
