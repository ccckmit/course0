#include <stdio.h> // C4 會略過這行，但為了符合一般 C 習慣保留

// 測試：回傳與接收 double 的函式
double circle_area(double radius) {
    double pi;
    pi = 3.1415926535;
    return pi * radius * radius;
}

// 測試：包含 double 的 struct
struct Point {
    double x;
    double y;
};

int main() {
    // C4 規定所有局部變數必須宣告在函式開頭
    double a;
    double b;
    double c;
    int i;
    struct Point p;
    double arr[3];

    printf("=== 1. Basic Floating Point Arithmetic ===\n");
    a = 5.5;
    b = 2.0;
    
    c = a + b; printf("%f + %f = %f\n", a, b, c);
    c = a - b; printf("%f - %f = %f\n", a, b, c);
    c = a * b; printf("%f * %f = %f\n", a, b, c);
    c = a / b; printf("%f / %f = %f\n", a, b, c);

    printf("\n=== 2. Mixed Type (int and double) ===\n");
    i = 10;
    c = a + i; // 這裡 i 應該要被自動提升 (ITF) 為 double
    printf("%f + %d = %f\n", a, i, c);
    c = i - b;
    printf("%d - %f = %f\n", i, b, c);
    
    printf("\n=== 3. Floating Point Comparison ===\n");
    if (a > b) {
        printf("%f is greater than %f (Correct!)\n", a, b);
    } else {
        printf("Error in comparison!\n");
    }

    if (a == 5.5) {
        printf("a is exactly 5.5 (Correct!)\n");
    }

    if (b < a) {
        printf("%f is less than %f (Correct!)\n", b, a);
    }

    printf("\n=== 4. Function Call ===\n");
    a = 10.0;
    printf("Area of circle with radius %f is %f\n", a, circle_area(a));

    printf("\n=== 5. Structs and Pointers ===\n");
    p.x = 1.23;
    p.y = 4.56;
    printf("Point is at (%f, %f)\n", p.x, p.y);

    printf("\n=== 6. Arrays ===\n");
    arr[0] = 1.1;
    arr[1] = 2.2;
    arr[2] = arr[0] + arr[1];
    printf("arr[0] = %f, arr[1] = %f, arr[2] = arr[0] + arr[1] = %f\n", arr[0], arr[1], arr[2]);

    printf("\n=== 7. Scientific Notation ===\n");
    a = 1.25e3;
    b = 5.0e-1;
    printf("1.25e3 = %f\n", a);
    printf("5.0e-1 = %f\n", b);

    return 0;
}