extern int counter;
extern int get_counter();
extern int inc_counter();
int main() {
  printf("counter=%d\n", get_counter());
  inc_counter();
  inc_counter();
  printf("counter=%d\n", get_counter());
  printf("direct=%d\n", counter);
  return 0;
}