/* Emulate struct using manual field layout */
int point_x(int *p) { return p[0]; }
int point_y(int *p) { return p[1]; }
int dist2(int *a, int *b) {
  int dx; int dy;
  dx = point_x(a) - point_x(b);
  dy = point_y(a) - point_y(b);
  return dx*dx + dy*dy;
}
int main()
{
  int p1[2]; int p2[2];
  p1[0] = 0; p1[1] = 0;
  p2[0] = 3; p2[1] = 4;
  printf("dist2=%d\n", dist2(p1, p2));
  exit(0);
}
