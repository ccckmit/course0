#include <stdio.h>
#include <stdlib.h>
#include "xml_parser.h"

int main() {
    const char *xml = 
        "<root>"
        "  <title>Hello</title>"
        "  <items>"
        "    <item>Item 1</item>"
        "    <item>Item 2</item>"
        "  </items>"
        "</root>";

    printf("Input XML:\n%s\n\n", xml);

    XmlNode *root = xml_parse(xml);
    printf("Parsed DOM Tree:\n");
    xml_print(root, 0);

    xml_free(root);
    return 0;
}