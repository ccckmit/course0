// expected: i=3 f=3.500000
int main() {
    double d = 3.7;
    int i = (int)d;
    float f = (float)i + 0.5;
    printf("i=%d f=%f\n", i, f);
    return 0;
}
