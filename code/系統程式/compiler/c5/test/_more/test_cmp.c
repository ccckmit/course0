int main()
{
  int x; int y;
  x = 5; y = 3;
  if (x > y) printf("gt ok\n");
  if (x >= y) printf("ge ok\n");
  if (y < x) printf("lt ok\n");
  if (y <= x) printf("le ok\n");
  if (x == 5) printf("eq ok\n");
  if (x != y) printf("ne ok\n");
  exit(0);
}
