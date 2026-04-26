// expected: x=20 y=30
int main() {
    int a[3];
    a[0] = 10;
    a[1] = 20;
    a[2] = 30;
    int *p = &a[0];
    int x = *(p + 1);
    int y = *(p + 2);
    printf("x=%d y=%d\n", x, y);
    return 0;
}
