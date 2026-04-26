#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =========================================
 * 1. DOM Tree 資料結構
 * ========================================= */

// 節點類型
typedef enum {
    NODE_ELEMENT, // 元素節點 (例如 <book>)
    NODE_TEXT     // 文字節點 (例如 "Harry Potter" 這串純文字)
} NodeType;

// 屬性結構 (Linked List)
typedef struct XMLAttribute {
    char *name;                 // 屬性名稱
    char *value;                // 屬性數值
    struct XMLAttribute *next;  // 指向下一個屬性
} XMLAttribute;

// 節點結構
typedef struct XMLNode {
    NodeType type;              // 節點類型 (ELEMENT 或 TEXT)
    char *tag;                  // 若為 ELEMENT，此為標籤名稱；否則為 NULL
    char *text;                 // 若為 TEXT，此為文字內容；否則為 NULL
    XMLAttribute *attributes;   // 屬性的 Linked List
    struct XMLNode *parent;     // 父節點指標
    struct XMLNode **children;  // 子節點陣列
    int children_count;         // 目前子節點的數量
    int children_capacity;      // 子節點陣列的容量
} XMLNode;

/* =========================================
 * 2. 核心 Parser 函數宣告
 * ========================================= */

/**
 * @brief 解析 XML 字串並建立 DOM Tree
 * @param xml 包含 XML 內容的原始字串
 * @return XMLNode* 回傳虛擬的根節點 (標籤為 "DOCUMENT_ROOT")
 */
XMLNode* parse_xml(const char *xml);

/**
 * @brief 釋放整棵 DOM Tree 的記憶體
 * @param node 要釋放的根節點
 */
void free_xml(XMLNode *node);

/**
 * @brief 印出 DOM Tree (用於除錯與驗證)
 * @param node 要印出的根節點
 * @param depth 縮排深度 (初始呼叫通常帶入 0)
 */
void print_xml_tree(XMLNode *node, int depth);

/* =========================================
 * 3. 節點操作與建立函數宣告 (供手動操作 DOM Tree 使用)
 * ========================================= */

XMLNode* create_element_node(const char *tag);
XMLNode* create_text_node(const char *text);
void add_attribute(XMLNode *node, const char *name, const char *value);
void add_child(XMLNode *parent, XMLNode *child);

#endif // XML_PARSER_H