// expected: inner=2\nouter=1
int main() {
    int x = 1;
    {
        int x = 2;
        printf("inner=%d\n", x);
    }
    printf("outer=%d\n", x);
    return 0;
}
