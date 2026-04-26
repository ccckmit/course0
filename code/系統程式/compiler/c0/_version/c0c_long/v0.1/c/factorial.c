// expected: fact(5)=120
#include <stdio.h>

int fact(int n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

int main() {
    int r = fact(5);
    printf("fact(5)=%d\n", r);
    return 0;
}