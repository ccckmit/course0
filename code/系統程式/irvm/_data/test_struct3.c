struct Point {
    int x;
    int y;
};

int test_struct_basic() {
    struct Point p;
    p.x = 10;
    p.y = 20;
    return p.x + p.y;
}

int main() {
    return test_struct_basic();
}