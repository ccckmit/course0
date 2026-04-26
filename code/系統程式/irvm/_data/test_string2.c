// expected: 5
// Test simple char array
int main() {
    char s[10];
    s[0] = 'h';
    s[1] = 'e';
    s[2] = 'l';
    s[3] = 'l';
    s[4] = 'o';
    s[5] = 0;
    int len = 0;
    while (s[len] != 0) len++;
    return len;
}
