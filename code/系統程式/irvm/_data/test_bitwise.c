// expected: 60
int main() {
    int a = 12;   // 1100
    int b = 10;   // 1010
    int c = a & b; // 1000 = 8
    int d = a | b; // 1110 = 14
    int e = a ^ b; // 0110 = 6
    int f = (d << 2); // 56
    return c + e + (f - 10); // 8 + 6 + 46 = 60
}
