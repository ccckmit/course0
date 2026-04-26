#include <stdio.h>

// 測試：全域變數宣告時初始化
int global_x = 42;
double global_pi = 3.14159;
char *msg = "Hello from Global String!";

int main() {
    // 測試：區域變數宣告時初始化
    int a = 10;
    int b = a + 20;  // 支援使用前面定義的變數做運算
    double c = 2.5;

    printf("Global variables:\n");
    printf("global_x = %d\n", global_x);
    printf("global_pi = %f\n", global_pi);
    printf("msg = %s\n\n", msg);

    printf("Local variables:\n");
    printf("a = %d\n", a);
    printf("b = %d\n", b);
    printf("c = %f\n", c);
    
    // 故意寫錯語法來測試 error() 提示
    // b = c + ;
    
    return 0;
}