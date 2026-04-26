// expected: x=64990 y=100005 z=18446744073414584325
int main() {
    const unsigned short us = 65000;
    short s = -10;
    long l = 100000;
    unsigned long ul = 4000000000;
    int x = us + s;
    long y = l + 5;
    unsigned long z = ul + 5;
    printf("x=%d y=%ld z=%lu\n", x, y, z);
    return 0;
}
