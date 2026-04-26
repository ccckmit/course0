int main()
{
  char *a; char *b; int r;
  a = malloc(16);
  b = malloc(16);
  memset(a, 0x41, 15); a[15] = 0;
  memset(b, 0x41, 15); b[15] = 0;
  r = memcmp(a, b, 15);
  printf("eq=%d\n", r == 0);
  b[7] = 0x42;
  r = memcmp(a, b, 15);
  printf("lt=%d\n", r < 0);
  printf("%s\n", a);
  free(a); free(b);
  exit(0);
}
