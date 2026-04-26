// expected: sum=3
int main() {
    int i = 0;
    int sum = 0;
    do {
        sum = sum + i;
        i = i + 1;
    } while (i < 3);
    printf("sum=%d\n", sum);
    return 0;
}
