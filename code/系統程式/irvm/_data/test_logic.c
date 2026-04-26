// expected: 3
int main() {
    int a = 1;
    int b = 0;
    int r = 0;
    if (a && !b) r++;      // 1
    if (a || b) r++;       // 2
    if (!(a && b)) r++;    // 3
    return r;
}
