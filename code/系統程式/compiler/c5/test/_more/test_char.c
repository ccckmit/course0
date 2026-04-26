int main()
{
  char c;
  int i;
  c = 'A';
  while (c <= 'Z') {
    printf("%c", c);
    c = c + 1;
  }
  printf("\n");
  i = 0;
  while (i < 256) {
    if (i >= 'a' && i <= 'z') printf("%c", i - 32);
    i = i + 1;
  }
  printf("\n");
  exit(0);
}
