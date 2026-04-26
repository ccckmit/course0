// expected: c=65 d=66 x=131
char inc(char c) {
    return c + 1;
}

int main() {
    char c = 65;
    char d = inc(c);
    int x = c + d;
    printf("c=%d d=%d x=%d\n", c, d, x);
    return 0;
}
