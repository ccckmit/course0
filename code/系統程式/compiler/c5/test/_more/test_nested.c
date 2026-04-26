int fact(int n) {
  if (n <= 1) return 1;
  return n * fact(n - 1);
}
int sum(int n) {
  int s; s = 0;
  while (n > 0) { s = s + n; n = n - 1; }
  return s;
}
int main()
{
  int i;
  i = 1;
  while (i <= 10) {
    printf("%d! = %d, sum(%d) = %d\n", i, fact(i), i, sum(i));
    i = i + 1;
  }
  exit(0);
}
