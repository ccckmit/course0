// expected: d=3 c1=1 c2=1 c3=1
int main() {
    int a[10];
    int *p = &a[5];
    int *q = &a[2];
    int d = p - q;
    int c1 = p > q;
    int c2 = p >= q;
    int c3 = q < p;
    printf("d=%d c1=%d c2=%d c3=%d\n", d, c1, c2, c3);
    return 0;
}
