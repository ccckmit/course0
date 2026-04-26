double myabs(double x) { if (x < 0.0) return -x; return x; }
double mypow(double b, int n) {
  double r; r = 1.0;
  while (n > 0) { r = r * b; n = n - 1; }
  return r;
}
int main()
{
  printf("myabs(-3.14)=%.6f\n", myabs(-3.14));
  printf("myabs(2.71)=%.6f\n", myabs(2.71));
  printf("mypow(2.0, 10)=%.2f\n", mypow(2.0, 10));
  printf("mypow(1.5, 4)=%.4f\n", mypow(1.5, 4));
  printf("(1.5 > 1.0)=%d\n", 1.5 > 1.0);
  printf("(1.0 == 1.0)=%d\n", 1.0 == 1.0);
  printf("(0.5 != 0.6)=%d\n", 0.5 != 0.6);
  exit(0);
}
