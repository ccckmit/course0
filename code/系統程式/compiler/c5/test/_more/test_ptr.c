int main()
{
  char *s;
  int i;
  s = "hello";
  i = 0;
  while (s[i]) {
    printf("%c", s[i]);
    i = i + 1;
  }
  printf("\n");
  exit(0);
}
