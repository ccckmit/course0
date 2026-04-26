// expected: hi\nv=7
void hello();

void hello() {
    printf("hi\n");
}

int add(int a, int b) {
    return a + b;
}

int main() {
    hello();
    int v = add(3, 4);
    printf("v=%d\n", v);
    return 0;
}
