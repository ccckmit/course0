int main()
{
  int a; int b; int c;
  a = 2; b = 3; c = 4;
  printf("%d\n", a + b * c);
  printf("%d\n", (a + b) * c);
  printf("%d\n", a * b + c * a);
  printf("%d\n", c / a);
  printf("%d\n", c % b);
  printf("%d\n", a << 3);
  printf("%d\n", 16 >> 2);
  exit(0);
}
