#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* new_node(char *type, char *val) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->type = strdup(type);
    node->val = val ? strdup(val) : NULL;
    node->child = NULL;
    node->next = NULL;
    return node;
}

void add_child(Node *parent, Node *child) {
    if (!parent || !child) return;
    if (!parent->child) {
        parent->child = child;
    } else {
        Node *n = parent->child;
        while (n->next) n = n->next;
        n->next = child;
    }
}

void add_next(Node *node, Node *next_node) {
    if (!node || !next_node) return;
    Node *n = node;
    while (n->next) n = n->next;
    n->next = next_node;
}

void print_ast(Node *node, int level) {
    while (node) {
        for (int i=0; i<level; i++) printf("  ");
        if (node->val) printf("%s: %s\n", node->type, node->val);
        else printf("%s\n", node->type);
        
        if (node->child) print_ast(node->child, level + 1);
        node = node->next;
    }
}
