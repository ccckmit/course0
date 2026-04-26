// expected: 4
int main() {
    int v = 3;
    int r = 0;
    switch (v) {
        case 1: r = 1; break;
        case 2: r = 2; break;
        case 3: r = 4; break;
        default: r = 9; break;
    }
    return r;
}
