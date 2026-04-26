int main()
{
  int a; int b;
  a = 0xFF; b = 0x0F;
  printf("%d\n", a & b);
  printf("%d\n", a | b);
  printf("%d\n", a ^ b);
  printf("%d\n", a >> 4);
  printf("%d\n", b << 4);
  printf("%d\n", ~a & 0xFF);
  exit(0);
}
