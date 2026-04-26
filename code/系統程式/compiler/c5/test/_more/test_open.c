int main()
{
  int fd; char buf[128]; int n;
  fd = open("hello.c", 0, 0);
  if (fd < 0) { printf("open failed\n"); exit(1); }
  n = read(fd, buf, 127);
  buf[n] = 0;
  close(fd);
  write(1, buf, n);
  exit(0);
}
