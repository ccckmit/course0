int test_array_1d() {
    int arr[5];
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    return arr[0] + arr[1] + arr[2];
}

int main() {
    return test_array_1d();
}