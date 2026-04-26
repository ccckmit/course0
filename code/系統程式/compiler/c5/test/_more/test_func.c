int add(int a, int b) { return a + b; }
int fib(int n) {
  if (n <= 1) return n;
  return fib(n-1) + fib(n-2);
}
int main()
{
  printf("add=%d\n", add(3,4));
  printf("fib(10)=%d\n", fib(10));
  exit(0);
}
