int max3(int a, int b, int c) {
  if (a >= b && a >= c) return a;
  if (b >= c) return b;
  return c;
}
int main()
{
  printf("%d\n", max3(1,2,3));
  printf("%d\n", max3(5,3,4));
  printf("%d\n", max3(0,0,0));
  exit(0);
}
