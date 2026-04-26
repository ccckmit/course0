struct Point {
    int x;
    int y;
};

int test_struct_array() {
    struct Point arr[3];
    arr[0].x = 1;
    arr[1].y = 2;
    arr[2].x = 3;
    return arr[0].x + arr[1].y + arr[2].x;
}

int main() {
    return test_struct_array();
}