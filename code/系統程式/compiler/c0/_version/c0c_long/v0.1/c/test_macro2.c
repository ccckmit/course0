#define MAX 100
#define MIN 10
#define SUM(a, b) ((a) + (b))
#define MULTI(a, b) ((a) * (b))
#define STR "Hello World"
#define CHAR 'A'
#define IFDEF_TEST 1

int main() {
    int x = MAX;
    int y = MIN;
    int z = SUM(x, y);
    int m = MULTI(x, y);
    char *s = STR;
    char c = CHAR;
    
    return z;
}
