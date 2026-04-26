int isPrime(int n) {
  int i;
  if (n < 2) return 0;
  i = 2;
  while (i * i <= n) {
    if (n % i == 0) return 0;
    i = i + 1;
  }
  return 1;
}
int main()
{
  int i; int count;
  count = 0;
  i = 2;
  while (i < 200) {
    if (isPrime(i)) { count = count + 1; }
    i = i + 1;
  }
  printf("primes < 200: %d\n", count);
  exit(0);
}
