int sign(int x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}
int abs2(int x) {
  if (x < 0) return -x;
  return x;
}
int main()
{
  printf("%d %d %d\n", sign(5), sign(-3), sign(0));
  printf("%d %d %d\n", abs2(5), abs2(-3), abs2(0));
  exit(0);
}
