#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml_parser.h"

static void add_to_list(XmlNodeList *list, XmlNode *node) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity ? list->capacity * 2 : 4;
        list->nodes = realloc(list->nodes, sizeof(XmlNode *) * list->capacity);
    }
    list->nodes[list->count++] = node;
}

static XmlNodeList *new_list() {
    XmlNodeList *list = malloc(sizeof(XmlNodeList));
    list->nodes = NULL;
    list->count = 0;
    list->capacity = 0;
    return list;
}

void xml_query_free(XmlNodeList *list) {
    if (!list) return;
    free(list->nodes);
    free(list);
}

static int match_selector(XmlNode *node, const char *sel) {
    char tag[256] = {0};
    char class[256] = {0};
    int has_class = 0;

    for (int i = 0; sel[i] && i < 255; i++) {
        if (sel[i] == '.') {
            has_class = 1;
            int j = 0;
            i++;
            while (sel[i] && sel[i] != ' ' && j < 255) {
                class[j++] = sel[i++];
            }
            class[j] = '\0';
            i--;
        } else if (sel[i] != ' ') {
            int j = 0;
            while (sel[i] && sel[i] != ' ' && sel[i] != '.' && j < 255) {
                tag[j++] = sel[i++];
            }
            tag[j] = '\0';
            i--;
        }
    }

    if (has_class) {
        if (!node->attr || strstr(node->attr, class) == NULL) return 0;
    }
    if (tag[0] && strcmp(node->tag, tag) != 0) return 0;
    return 1;
}

static void query_descendant(XmlNode *node, const char *sel, XmlNodeList *list) {
    if (!node) return;
    if (match_selector(node, sel)) {
        add_to_list(list, node);
    }
    for (XmlNode *child = node->children; child; child = child->next) {
        query_descendant(child, sel, list);
    }
}

static int match_path(XmlNode *node, char **parts, int depth, int max) {
    if (depth >= max) return 1;
    XmlNode *p = node->parent;
    while (p) {
        if (match_selector(p, parts[depth])) {
            if (depth + 1 == max) return 1;
            if (match_path(p, parts, depth + 1, max)) return 1;
        }
        p = p->parent;
    }
    return 0;
}

XmlNodeList *xml_query(XmlNode *root, const char *path) {
    XmlNodeList *list = new_list();
    if (!root || !path || !*path) return list;

    char *path_copy = strdup(path);
    char *parts[16];
    int count = 0;

    char *tok = strtok(path_copy, " ");
    while (tok && count < 16) {
        parts[count++] = tok;
        tok = strtok(NULL, " ");
    }

    if (count == 1) {
        query_descendant(root, parts[0], list);
    } else {
        XmlNodeList *all = new_list();
        query_descendant(root, parts[count - 1], all);
        for (int i = 0; i < all->count; i++) {
            XmlNode *n = all->nodes[i];
            int ok = 1;
            for (int j = count - 2; j >= 0 && ok; j--) {
                XmlNode *p = n->parent;
                ok = 0;
                while (p) {
                    if (match_selector(p, parts[j])) {
                        ok = 1;
                        break;
                    }
                    p = p->parent;
                }
            }
            if (ok) add_to_list(list, n);
        }
        xml_query_free(all);
    }

    free(path_copy);
    return list;
}