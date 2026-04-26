int x;
int f(int a) {
  int x; x = a * 2;
  return x;
}
int g(int a) {
  int y; y = f(a) + a;
  return y;
}
int main()
{
  x = 10;
  printf("%d\n", x);
  printf("%d\n", f(3));
  printf("%d\n", g(4));
  printf("%d\n", x);
  exit(0);
}
