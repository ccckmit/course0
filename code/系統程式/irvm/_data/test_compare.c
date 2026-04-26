// expected: 6
int main() {
    int a = 5;
    int b = 10;
    int r = 0;
    if (a < b) r++;      // 1
    if (a <= b) r++;     // 2
    if (b > a) r++;      // 3
    if (b >= a) r++;     // 4
    if (a != b) r++;     // 5
    if (a == 5) r++;     // 6
    return r;
}
