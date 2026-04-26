// expected: x=15 y=2
int main() {
    int x = 10;
    int *p = &x;
    *p = *p + 5;

    int a[3];
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    int y = a[1];
    printf("x=%d y=%d\n", x, y);
    return 0;
}
