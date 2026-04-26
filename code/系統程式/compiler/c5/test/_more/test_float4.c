double arr[5];
int main()
{
  int i;
  i = 0;
  while (i < 5) { arr[i] = (double)i * 1.5; i = i + 1; }
  i = 0;
  while (i < 5) { printf("%.1f\n", arr[i]); i = i + 1; }
  exit(0);
}
