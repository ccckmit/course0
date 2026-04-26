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

int test_ptr_to_struct() {
    struct Point p;
    struct Point *pp = &p;
    pp->x = 50;
    pp->y = 60;
    return pp->x + pp->y;
}

int main() {
    return test_struct_basic() + test_ptr_to_struct();
}