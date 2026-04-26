// expected: 0
int main() {
    int i = 0;
    int sum = 0;
    while (i < 5 && sum < 10) {
        sum += i;
        i++;
    }

    int j = 3;
    for (j = 3; j > 0; j--) {
        sum = sum + j;
    }

    if (!(sum % 2 == 0) || sum == 0) {
        printf("sum=%d\n", sum);
    }
    return 0;
}
