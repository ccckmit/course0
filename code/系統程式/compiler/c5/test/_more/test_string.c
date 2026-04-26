int strlen2(char *s) {
  int n; n = 0;
  while (s[n]) n = n + 1;
  return n;
}
int strcmp2(char *a, char *b) {
  while (*a && *a == *b) { a = a + 1; b = b + 1; }
  return *a - *b;
}
int main()
{
  printf("%d\n", strlen2("hello"));
  printf("%d\n", strlen2(""));
  printf("%d\n", strcmp2("abc","abc"));
  printf("%d\n", strcmp2("abc","abd"));
  exit(0);
}
