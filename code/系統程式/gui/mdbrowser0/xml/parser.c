#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/* =========================================
 * 2. 輔助字串處理函數
 * ========================================= */

// 複製指定長度的字串 (類似 POSIX strndup)
char *copy_string(const char *start, int length) {
    char *str = (char *)malloc(length + 1);
    if (str) {
        strncpy(str, start, length);
        str[length] = '\0';
    }
    return str;
}

// 複製一般字串 (類似 POSIX strdup)
char *custom_strdup(const char *s) {
    char *d = (char *)malloc(strlen(s) + 1);
    if (d) strcpy(d, s);
    return d;
}

/* =========================================
 * 3. 節點操作函數
 * ========================================= */

XMLNode* create_element_node(const char *tag) {
    XMLNode *node = (XMLNode *)malloc(sizeof(XMLNode));
    node->type = NODE_ELEMENT;
    node->tag = custom_strdup(tag);
    node->text = NULL;
    node->attributes = NULL;
    node->parent = NULL;
    node->children = NULL;
    node->children_count = 0;
    node->children_capacity = 0;
    return node;
}

XMLNode* create_text_node(const char *text) {
    XMLNode *node = (XMLNode *)malloc(sizeof(XMLNode));
    node->type = NODE_TEXT;
    node->tag = NULL;
    node->text = custom_strdup(text);
    node->attributes = NULL;
    node->parent = NULL;
    node->children = NULL;
    node->children_count = 0;
    node->children_capacity = 0;
    return node;
}

void add_attribute(XMLNode *node, const char *name, const char *value) {
    XMLAttribute *attr = (XMLAttribute *)malloc(sizeof(XMLAttribute));
    attr->name = custom_strdup(name);
    attr->value = custom_strdup(value);
    attr->next = NULL;

    if (!node->attributes) {
        node->attributes = attr;
    } else {
        XMLAttribute *curr = node->attributes;
        while (curr->next) curr = curr->next;
        curr->next = attr;
    }
}

void add_child(XMLNode *parent, XMLNode *child) {
    if (parent->children_count >= parent->children_capacity) {
        parent->children_capacity = parent->children_capacity == 0 ? 4 : parent->children_capacity * 2;
        parent->children = (XMLNode **)realloc(parent->children, parent->children_capacity * sizeof(XMLNode*));
    }
    parent->children[parent->children_count++] = child;
    child->parent = parent;
}

void free_xml(XMLNode *node) {
    if (!node) return;
    if (node->tag) free(node->tag);
    if (node->text) free(node->text);

    XMLAttribute *attr = node->attributes;
    while (attr) {
        XMLAttribute *next = attr->next;
        free(attr->name);
        free(attr->value);
        free(attr);
        attr = next;
    }

    for (int i = 0; i < node->children_count; i++) {
        free_xml(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node);
}

/* =========================================
 * 4. 核心 Parser 函數
 * ========================================= */

XMLNode* parse_xml(const char *xml) {
    // 建立一個假的 Root 節點來容納所有的頂層元素
    XMLNode *root = create_element_node("DOCUMENT_ROOT");
    XMLNode *current = root;
    const char *p = xml;

    while (*p) {
        if (*p == '<') {
            if (strncmp(p, "<!--", 4) == 0) {
                // 跳過註解
                p = strstr(p, "-->");
                if (p) p += 3; else break;
            } else if (strncmp(p, "<?", 2) == 0) {
                // 跳過 XML 宣告或處理指令 (Processing Instruction)
                p = strstr(p, "?>");
                if (p) p += 2; else break;
            } else if (p[1] == '/') {
                // 遇到結束標籤，例如 </book>
                p += 2;
                while (*p && *p != '>') p++; // 尋找結尾的 '>'
                if (*p == '>') p++;
                if (current->parent) {
                    current = current->parent; // 回退到父節點
                }
            } else {
                // 遇到開始標籤，例如 <book ...>
                p++;
                const char *tag_start = p;
                while (*p && !isspace((unsigned char)*p) && *p != '>' && *p != '/') p++;
                char *tag = copy_string(tag_start, p - tag_start);
                
                XMLNode *node = create_element_node(tag);
                free(tag);
                add_child(current, node);

                // 解析屬性
                while (*p && *p != '>' && *p != '/') {
                    while (isspace((unsigned char)*p)) p++;
                    if (*p == '>' || *p == '/') break;

                    const char *attr_start = p;
                    while (*p && *p != '=' && !isspace((unsigned char)*p) && *p != '>' && *p != '/') p++;
                    char *attr_name = copy_string(attr_start, p - attr_start);

                    while (isspace((unsigned char)*p)) p++;
                    if (*p == '=') {
                        p++;
                        while (isspace((unsigned char)*p)) p++;
                        char quote = *p;
                        if (quote == '"' || quote == '\'') {
                            p++;
                            const char *val_start = p;
                            while (*p && *p != quote) p++;
                            char *attr_val = copy_string(val_start, p - val_start);
                            if (*p == quote) p++;
                            add_attribute(node, attr_name, attr_val);
                            free(attr_val);
                        }
                    }
                    free(attr_name);
                }

                // 判斷是否為自閉合標籤 (例如 <available/>) 或是普通標籤 (例如 <title>)
                if (*p == '/') {
                    p++;
                    if (*p == '>') p++;
                } else if (*p == '>') {
                    p++;
                    current = node; // 切換當前作用範圍到新標籤
                }
            }
        } else {
            // 解析文字內容
            const char *text_start = p;
            while (*p && *p != '<') p++;
            
            // 檢查是否全為空白字元 (過濾縮排等無用節點)
            int is_ws = 1;
            for (const char *t = text_start; t < p; t++) {
                if (!isspace((unsigned char)*t)) { is_ws = 0; break; }
            }

            if (!is_ws) {
                char *text = copy_string(text_start, p - text_start);
                XMLNode *t_node = create_text_node(text);
                add_child(current, t_node);
                free(text);
            }
        }
    }
    return root;
}

/* =========================================
 * 5. 印出 DOM Tree 以驗證
 * ========================================= */
void print_xml_tree(XMLNode *node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++) printf("  ");

    if (node->type == NODE_TEXT) {
        printf("TEXT: [%s]\n", node->text);
    } else {
        printf("TAG: <%s>", node->tag);
        XMLAttribute *attr = node->attributes;
        while (attr) {
            printf(" %s=\"%s\"", attr->name, attr->value);
            attr = attr->next;
        }
        printf("\n");
        for (int i = 0; i < node->children_count; i++) {
            print_xml_tree(node->children[i], depth + 1);
        }
    }
}
