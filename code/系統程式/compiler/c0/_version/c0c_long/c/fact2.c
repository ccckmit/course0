#include <stdio.h>

// 定義階層函數
long long factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int num = 5;
    long long result = factorial(num);
    
    printf("factorial(%d)=%lld\n", num, result);
    
    return 0;
}