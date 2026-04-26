#ifndef XML_PARSER_H
#define XML_PARSER_H

typedef struct XmlNode {
    char *tag;
    char *content;
    char *attr;
    struct XmlNode *parent;
    struct XmlNode *children;
    struct XmlNode *next;
} XmlNode;

typedef struct XmlNodeList {
    XmlNode **nodes;
    int count;
    int capacity;
} XmlNodeList;

XmlNode *xml_parse(const char *xml);
void xml_free(XmlNode *root);
void xml_print(XmlNode *node, int indent);
XmlNodeList *xml_query(XmlNode *root, const char *path);
void xml_query_free(XmlNodeList *list);

#endif