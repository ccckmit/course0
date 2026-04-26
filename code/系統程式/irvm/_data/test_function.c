// expected: 42
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }
int main() {
    int x = add(10, 11); // 21
    int y = mul(x, 2);   // 42
    return y;
}
