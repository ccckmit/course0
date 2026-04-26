// #include <stdio.h>

// 遞回計算階乘
long long factorial(int n) {
    // 終止條件：0! = 1, 1! = 1
    if (n <= 1) {
        return 1;
    }
    // 遞回呼叫：n! = n * (n-1)!
    return n * factorial(n - 1);
}

int main() {
    int number = 5;
    long long result = factorial(number);
    
    // 注意：在裸機 (Bare-metal) 環境下可能無法使用 printf
    // 若是在 Linux 環境則正常運作
    // printf("Factorial of %d is %lld\n", number, result);
    
    return result;
}