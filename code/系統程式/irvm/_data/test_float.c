// Test floating point operations
int main() {
    float a = 1.5;
    float b = 2.0;
    float c = a + b;    // 3.5
    float d = a * b;    // 3.0
    float e = b - a;    // 0.5
    int result = (c > 0) ? 1 : 0;  // Use comparison to get integer result
    return result;  // Should return 1
}
