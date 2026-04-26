struct Point {
    int x;
    int y;
};

struct Rect {
    struct Point top_left;
    struct Point bottom_right;
};

int test_struct_nested() {
    struct Rect r;
    r.top_left.x = 1;
    r.top_left.y = 2;
    r.bottom_right.x = 100;
    r.bottom_right.y = 200;
    return r.top_left.x + r.bottom_right.y;
}

int main() {
    return test_struct_nested();
}