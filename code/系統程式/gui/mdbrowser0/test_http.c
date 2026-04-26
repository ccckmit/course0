#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"

int main() {
    printf("Testing HTTP/HTTPS client...\n");
    
    printf("\n1. Testing URL parsing:\n");
    char host[256], path[1024];
    int port;
    
    url_parse("https://raw.githubusercontent.com/ccc-c/c0computer/refs/heads/main/README.md", host, &port, path);
    printf("   URL: https://raw.githubusercontent.com/ccc-c/c0computer/refs/heads/main/README.md\n");
    printf("   Host: %s, Port: %d, Path: %s\n", host, port, path);
    
    url_parse("http://example.com/test/file.md", host, &port, path);
    printf("   URL: http://example.com/test/file.md\n");
    printf("   Host: %s, Port: %d, Path: %s\n", host, port, path);
    
    printf("\n2. Testing is_url:\n");
    printf("   'https://...' is_url: %d\n", is_url("https://example.com"));
    printf("   'http://...' is_url: %d\n", is_url("http://example.com"));
    printf("   '/path/to/file' is_url: %d\n", is_url("/path/to/file"));
    printf("   'file.md' is_url: %d\n", is_url("file.md"));
    
    printf("\n3. Testing HTTPS GET...\n");
    HttpResponse resp = http_get("https://raw.githubusercontent.com/ccc-c/c0computer/refs/heads/main/README.md");
    
    if (resp.data && resp.size > 0) {
        printf("   Status: %d\n", resp.status_code);
        printf("   Size: %zu bytes\n", resp.size);
        printf("   Content (first 500 chars):\n");
        printf("   ---\n");
        if (resp.size > 500) {
            resp.data[500] = '\0';
        }
        printf("%s\n", resp.data);
        printf("   ---\n");
    } else {
        printf("   Error: %s\n", resp.error);
    }
    
    http_response_free(&resp);
    
    printf("\nAll tests completed!\n");
    return 0;
}
