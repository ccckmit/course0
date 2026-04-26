/* Test that will be compiled as multi-ELF then linked */
int helper(int x) { return x * x + 1; }
int main()
{
  int i;
  i = 1;
  while (i <= 5) {
    printf("helper(%d)=%d\n", i, helper(i));
    i = i + 1;
  }
  exit(0);
}
