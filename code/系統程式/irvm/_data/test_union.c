union Data {
    int i;
    char c;
};

int test_union_basic() {
    union Data d;
    d.i = 0x41424344;
    return d.c;
}

int test_union_cast() {
    union Data d;
    d.i = 65;
    return d.i;
}

int main() {
    return test_union_basic() + test_union_cast();
}