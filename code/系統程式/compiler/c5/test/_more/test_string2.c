char buf[64];
int main()
{
  int i;
  i = 0;
  while (i < 26) {
    buf[i] = 'a' + i;
    i = i + 1;
  }
  buf[26] = 0;
  printf("%s\n", buf);
  printf("len=%d\n", 26);
  exit(0);
}
