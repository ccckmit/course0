struct Node {
    int val;
    struct Node *next;
};
int sum(struct Node *n) {
    int s = 0;
    while (n) {
        s = s + n->val;
        n = n->next;
    }
    return s;
}
int main(void) {
    struct Node a;
    struct Node b;
    struct Node c;
    a.val = 1; a.next = &b;
    b.val = 2; b.next = &c;
    c.val = 3; c.next = 0;
    return sum(&a);
}
