#ifndef AST_H
#define AST_H

typedef struct Node {
    char *type;
    char *val;
    struct Node *child;
    struct Node *next;
} Node;

Node* new_node(char *type, char *val);
void add_child(Node *parent, Node *child);
void add_next(Node *node, Node *next);
void print_ast(Node *node, int level);

#endif
