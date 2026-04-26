int g;
void setg(int v) { g = v; }
int getg() { return g; }
int swap(int *a, int *b) {
  int t; t = *a; *a = *b; *b = t;
  return 0;
}
int main()
{
  int x; int y;
  setg(42); printf("%d\n", getg());
  x = 10; y = 20;
  swap(&x, &y);
  printf("x=%d y=%d\n", x, y);
  exit(0);
}
