#!/bin/bash

echo "=== Building Query Test ==="
gcc -o query_test main.c xml_parser.c xml_query.c

echo ""
echo "=== Running Query Tests ==="
echo ""

cat > test_query.c << 'EOF'
#include <stdio.h>
#include "xml_parser.h"

int main() {
    const char *xml = 
        "<root>"
        "  <items>"
        "    <item class=\"item1\">Item 1</item>"
        "    <item class=\"item2\">Item 2</item>"
        "  </items>"
        "  <title>Hello</title>"
        "</root>";

    XmlNode *root = xml_parse(xml);
    printf("XML:\n%s\n\n", xml);
    xml_print(root, 0);
    printf("\n--- Queries ---\n");

    printf("Query 'item':\n");
    XmlNodeList *list = xml_query(root, "item");
    for (int i = 0; i < list->count; i++) {
        printf("  - <%s>%s</%s>\n", list->nodes[i]->tag, list->nodes[i]->content, list->nodes[i]->tag);
    }
    xml_query_free(list);

    printf("\nQuery 'items item':\n");
    list = xml_query(root, "items item");
    for (int i = 0; i < list->count; i++) {
        printf("  - <%s>%s</%s>\n", list->nodes[i]->tag, list->nodes[i]->content, list->nodes[i]->tag);
    }
    xml_query_free(list);

    printf("\nQuery '.item1':\n");
    list = xml_query(root, ".item1");
    for (int i = 0; i < list->count; i++) {
        printf("  - <%s>%s</%s>\n", list->nodes[i]->tag, list->nodes[i]->content, list->nodes[i]->tag);
    }
    xml_query_free(list);

    printf("\nQuery 'items .item2':\n");
    list = xml_query(root, "items .item2");
    for (int i = 0; i < list->count; i++) {
        printf("  - <%s>%s</%s>\n", list->nodes[i]->tag, list->nodes[i]->content, list->nodes[i]->tag);
    }
    xml_query_free(list);

    xml_free(root);
    return 0;
}
EOF

gcc -o query_test test_query.c xml_parser.c xml_query.c
./query_test

echo ""
echo "=== Test Complete ==="