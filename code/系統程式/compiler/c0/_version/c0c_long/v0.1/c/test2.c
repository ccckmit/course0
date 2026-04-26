// expected: The value of a is: 10\nResult of calculation: 30
int main() {
    int a;
    int b;
    int c;
    a = 10;
    b = 5 * (a + 2);
    c = b / 2;
    
    // 呼叫 printf！支援字串與變數
    printf("The value of a is: %d\n", a);
    printf("Result of calculation: %d\n", c);
    
    return 0;
}