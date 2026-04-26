int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
int clamp(int v, int lo, int hi) { return min(max(v, lo), hi); }
int main()
{
  printf("%d\n", min(3,7));
  printf("%d\n", max(3,7));
  printf("%d\n", clamp(5, 0, 10));
  printf("%d\n", clamp(-5, 0, 10));
  printf("%d\n", clamp(15, 0, 10));
  exit(0);
}
