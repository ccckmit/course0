#include "parser.h"
/* =========================================
 * 6. 測試主程式
 * ========================================= */
int main() {
    const char *xml_string = 
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<bookstore>\n"
        "  <!-- 這是一個註解 -->\n"
        "  <book category=\"fiction\">\n"
        "    <title lang=\"en\">Harry Potter</title>\n"
        "    <author>J.K. Rowling</author>\n"
        "    <year>2005</year>\n"
        "    <price>29.99</price>\n"
        "    <available/>\n"
        "  </book>\n"
        "</bookstore>";

    printf("原始 XML 字串：\n%s\n\n", xml_string);
    printf("解析出的 DOM Tree：\n");

    XMLNode *dom_root = parse_xml(xml_string);
    
    // 略過我們自己建立的 DOCUMENT_ROOT，直接印出真實節點
    for (int i = 0; i < dom_root->children_count; i++) {
        print_xml_tree(dom_root->children[i], 0);
    }

    // 釋放記憶體
    free_xml(dom_root);

    return 0;
}