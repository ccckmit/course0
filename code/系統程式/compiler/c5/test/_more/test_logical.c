int main()
{
  int x; int y;
  x = 1; y = 0;
  if (x && y) printf("both\n"); else printf("not both\n");
  if (x || y) printf("either\n"); else printf("neither\n");
  if (!x) printf("not x\n"); else printf("x is true\n");
  exit(0);
}
