int counter;
int inc() { counter = counter + 1; return counter; }
int main()
{
  counter = 0;
  printf("%d\n", inc());
  printf("%d\n", inc());
  printf("%d\n", inc());
  exit(0);
}
