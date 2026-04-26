// expected: x=7 y=9
struct Point {
    int x;
    char y;
};

typedef struct Point Point;

int main() {
    Point p;
    Point *pp = &p;
    p.x = 7;
    p.y = 9;
    printf("x=%d y=%d\n", pp->x, p.y);
    return 0;
}
