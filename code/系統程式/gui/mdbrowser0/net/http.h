#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

typedef struct {
    char* data;
    size_t size;
    int status_code;
    char error[256];
} HttpResponse;

HttpResponse http_get(const char* url);

void http_response_free(HttpResponse* resp);

int is_url(const char* str);

void url_parse(const char* url, char* host, int* port, char* path);

#endif
