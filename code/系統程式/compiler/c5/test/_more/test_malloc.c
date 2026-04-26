int main()
{
  char *buf;
  buf = malloc(64);
  memset(buf, 65, 10);
  buf[10] = 0;
  printf("%s\n", buf);
  free(buf);
  exit(0);
}
