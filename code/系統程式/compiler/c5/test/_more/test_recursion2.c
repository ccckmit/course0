int ack(int m, int n) {
  if (m == 0) return n + 1;
  if (n == 0) return ack(m - 1, 1);
  return ack(m - 1, ack(m, n - 1));
}
int main()
{
  printf("%d\n", ack(0,0));
  printf("%d\n", ack(1,1));
  printf("%d\n", ack(2,2));
  printf("%d\n", ack(3,3));
  exit(0);
}
