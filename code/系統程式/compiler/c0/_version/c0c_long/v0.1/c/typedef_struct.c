// expected: v=3
typedef struct {
    int v;
} Box;

int main() {
    Box b;
    b.v = 3;
    printf("v=%d\n", b.v);
    return 0;
}
