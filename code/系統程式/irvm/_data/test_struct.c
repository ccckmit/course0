struct Point {
    int x;
    int y;
};

struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

int test_struct_basic() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    return p.x + p.y;
}

int test_struct_nested() {
    struct Rect r;
    r.top_left.x = 1;
    r.top_left.y = 2;
    r.bottom_right.x = 100;
    r.bottom_right.y = 200;
    return r.top_left.x + r.bottom_right.y;
}

int test_struct_array() {
    struct Point arr[3];
    arr[0].x = 1;
    arr[1].y = 2;
    arr[2].x = 3;
    return arr[0].x + arr[1].y + arr[2].x;
}

int test_ptr_to_struct() {
    struct Point p;
    struct Point *pp = &p;
    pp->x = 50;
    pp->y = 60;
    return pp->x + pp->y;
}

int main() {
    int sum = 0;
    sum += test_struct_basic();
    sum += test_struct_nested();
    sum += test_struct_array();
    sum += test_ptr_to_struct();
    return sum;
}