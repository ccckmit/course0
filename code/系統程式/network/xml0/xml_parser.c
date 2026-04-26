#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml_parser.h"

static char *skip_whitespace(char *p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
    return p;
}

static char *parse_tag_name(char *p, char *name, int len) {
    int i = 0;
    while (*p && *p != ' ' && *p != '>' && *p != '<' && i < len - 1) {
        name[i++] = *p++;
    }
    name[i] = '\0';
    return p;
}

static void free_node(XmlNode *node) {
    if (!node) return;
    free(node->tag);
    free(node->content);
    free(node->attr);
    XmlNode *child = node->children;
    while (child) {
        XmlNode *next = child->next;
        free_node(child);
        child = next;
    }
    free(node);
}

void xml_free(XmlNode *root) {
    free_node(root);
}

static XmlNode *new_node(char *tag, char *content, char *attr) {
    XmlNode *node = malloc(sizeof(XmlNode));
    node->tag = strdup(tag);
    node->content = content ? strdup(content) : NULL;
    node->attr = attr ? strdup(attr) : NULL;
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;
    return node;
}

static void add_child(XmlNode *parent, XmlNode *child) {
    child->parent = parent;
    if (!parent->children) {
        parent->children = child;
    } else {
        XmlNode *p = parent->children;
        while (p->next) p = p->next;
        p->next = child;
    }
}

XmlNode *xml_parse(const char *xml) {
    char *p = (char *)xml;
    XmlNode *root = NULL;
    XmlNode *stack[64];
    int sp = 0;
    char tag[256];

    while (*p) {
        p = skip_whitespace(p);
        if (*p != '<') break;

        p++;
        if (*p == '/') {
            p++;
            p = parse_tag_name(p, tag, 256);
            p = skip_whitespace(p);
            if (*p == '>') p++;
            if (sp > 0) sp--;
            continue;
        }

        p = parse_tag_name(p, tag, 256);
        p = skip_whitespace(p);

        char attr[512] = {0};
        int alen = 0;
        while (*p && *p != '>' && alen < 511) {
            attr[alen++] = *p++;
        }
        char *a = attr;
        while (*a == ' ' || *a == '\t') a++;
        if (*a) {
            char *end = a + strlen(a) - 1;
            while (end > a && (*end == ' ' || *end == '\t')) *end-- = '\0';
        }

        XmlNode *node = new_node(tag, NULL, *a ? a : NULL);

        if (sp == 0) {
            root = node;
        } else {
            add_child(stack[sp - 1], node);
        }

        if (*p == '>') {
            p++;
            if (sp < 64) stack[sp++] = node;
        }

        char content[1024];
        int clen = 0;
        while (*p && *p != '<') {
            if (clen < 1023) content[clen++] = *p;
            p++;
        }
        content[clen] = '\0';
        if (clen > 0) {
            char *trim = content;
            while (*trim == ' ' || *trim == '\t' || *trim == '\n') trim++;
            char *end = trim + strlen(trim) - 1;
            while (end > trim && (*end == ' ' || *end == '\t' || *end == '\n')) *end-- = '\0';
            if (*trim) node->content = strdup(trim);
        }
    }

    return root;
}

void xml_print(XmlNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    printf("<%s", node->tag);
    if (node->attr) printf(" %s", node->attr);
    printf(">");
    if (node->content) printf("%s", node->content);
    printf("\n");
    for (XmlNode *child = node->children; child; child = child->next) {
        xml_print(child, indent + 1);
    }
}