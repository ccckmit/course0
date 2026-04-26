// expected: sum=10 prod=24
#include <stdio.h>

int main() {
    int i = 0;
    int sum = 0;
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }

    int j;
    int prod = 1;
    for (j = 1; j <= 4; j = j + 1) {
        prod = prod * j;
    }

    printf("sum=%d prod=%d\n", sum, prod);
    return 0;
}
