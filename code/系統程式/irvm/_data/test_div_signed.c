// Test division with negative numbers
int main() {
    int a = 10;
    int b = -3;
    int c = 10 / 3;      // 3
    int d = -10 / 3;     // -3 (truncates toward zero)
    int e = 10 / -3;     // -3
    int f = -10 / -3;    // 3
    int g = 10 % 3;      // 1
    int h = -10 % 3;     // -1 (sign of dividend)
    int i = 10 % -3;     // 1
    int j = -10 % -3;    // -1
    return c + d + e + f + g + h + i + j;  // 3 + (-3) + (-3) + 3 + 1 + (-1) + 1 + (-1) = 0
}
