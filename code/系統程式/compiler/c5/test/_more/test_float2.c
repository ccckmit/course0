double pi() { return 3.14159265358979; }
double sqr(double x) { return x * x; }
int main()
{
  double r; double area;
  r = 5.0;
  area = pi() * sqr(r);
  printf("area=%.4f\n", area);
  exit(0);
}
