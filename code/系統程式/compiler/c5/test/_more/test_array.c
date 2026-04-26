int main()
{
  int arr[10];
  int i;
  i = 0;
  while (i < 10) { arr[i] = i * i; i = i + 1; }
  i = 0;
  while (i < 10) { printf("%d ", arr[i]); i = i + 1; }
  printf("\n");
  exit(0);
}
