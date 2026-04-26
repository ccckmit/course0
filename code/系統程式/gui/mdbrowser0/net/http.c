#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "http.h"

#define BUFFER_SIZE 65536
#define MAX_REDIRECTS 3

int is_url(const char* str) {
    if (!str) return 0;
    return (strncmp(str, "http://", 7) == 0) || (strncmp(str, "https://", 8) == 0);
}

void url_parse(const char* url, char* host, int* port, char* path) {
    const char* ptr = url;

    if (strncmp(ptr, "https://", 8) == 0) {
        ptr += 8;
        *port = 443;
    } else if (strncmp(ptr, "http://", 7) == 0) {
        ptr += 7;
        *port = 80;
    }

    const char* colon = strchr(ptr, ':');
    const char* slash = strchr(ptr, '/');
    const char* end = slash ? slash : (colon ? colon : ptr + strlen(ptr));

    size_t host_len = end - ptr;
    if (host_len > 255) host_len = 255;
    strncpy(host, ptr, host_len);
    host[host_len] = '\0';

    if (colon && (!slash || colon < slash)) {
        *port = atoi(colon + 1);
    }

    if (slash) {
        strncpy(path, slash, 1023);
        path[1023] = '\0';
    } else {
        strcpy(path, "/");
    }
}

static int socket_connect(const char* host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    struct hostent* server = gethostbyname(host);
    if (server == NULL) {
        close(sock);
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

static SSL* ssl_connect(const char* host, int port) {
    static int ssl_initialized = 0;
    if (!ssl_initialized) {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        ssl_initialized = 1;
    }

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        return NULL;
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        SSL_CTX_free(ctx);
        return NULL;
    }

    int sock = socket_connect(host, port);
    if (sock < 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }

    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, host);

    if (SSL_connect(ssl) != 1) {
        int err = SSL_get_error(ssl, -1);
        (void)err;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return NULL;
    }

    SSL_CTX_free(ctx);
    return ssl;
}

static int send_http_request(int sock, SSL* ssl, const char* host, const char* path, int use_ssl) {
    char request[2048];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: md0r/1.0\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host);

    if (use_ssl) {
        return SSL_write(ssl, request, strlen(request));
    } else {
        return send(sock, request, strlen(request), 0);
    }
}

static char* receive_response(int sock, SSL* ssl, int use_ssl) {
    char* response = malloc(BUFFER_SIZE);
    if (!response) return NULL;
    response[0] = '\0';

    size_t total = 0;
    char buffer[4096];

    while (1) {
        ssize_t n;
        if (use_ssl) {
            n = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        } else {
            n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        }
        
        if (n <= 0) break;

        buffer[n] = '\0';
        size_t new_total = total + n + 1;
        if (new_total > BUFFER_SIZE) {
            char* new_resp = realloc(response, new_total * 2);
            if (!new_resp) {
                free(response);
                return NULL;
            }
            response = new_resp;
        }

        strcat(response, buffer);
        total += n;
    }

    return response;
}

static void close_ssl(SSL* ssl) {
    if (ssl) {
        SSL_shutdown(ssl);
        int fd = SSL_get_fd(ssl);
        SSL_free(ssl);
        if (fd >= 0) close(fd);
    }
}

static int parse_status_line(const char* response, int* status_code) {
    int major, minor;
    if (sscanf(response, "HTTP/%d.%d %d", &major, &minor, status_code) >= 1) {
        return 1;
    }
    return 0;
}

static char* extract_body(const char* response) {
    char* header_end = strstr(response, "\r\n\r\n");
    if (!header_end) {
        header_end = strstr(response, "\n\n");
        if (!header_end) return NULL;
        return (char*)(header_end + 2);
    }
    return (char*)(header_end + 4);
}

HttpResponse http_get(const char* url) {
    HttpResponse resp = {0};

    char host[256] = {0};
    char path[1024] = {0};
    int port = 80;
    int use_ssl = 0;

    url_parse(url, host, &port, path);
    use_ssl = (port == 443);

    int redirects = 0;
    char current_url[2048];
    strncpy(current_url, url, sizeof(current_url) - 1);

    while (redirects < MAX_REDIRECTS) {
        url_parse(current_url, host, &port, path);
        use_ssl = (port == 443);

        SSL* ssl = NULL;
        int sock = -1;

        if (use_ssl) {
            ssl = ssl_connect(host, port);
            if (!ssl) {
                snprintf(resp.error, sizeof(resp.error), "Cannot connect to %s:%d (SSL)", host, port);
                return resp;
            }
            sock = SSL_get_fd(ssl);
        } else {
            sock = socket_connect(host, port);
            if (sock < 0) {
                snprintf(resp.error, sizeof(resp.error), "Cannot connect to %s:%d", host, port);
                return resp;
            }
        }

        send_http_request(sock, ssl, host, path, use_ssl);

        char* response = receive_response(sock, ssl, use_ssl);
        
        if (use_ssl) {
            close_ssl(ssl);
        } else {
            close(sock);
        }

        if (!response) {
            snprintf(resp.error, sizeof(resp.error), "No response from server");
            return resp;
        }

        parse_status_line(response, &resp.status_code);

        if (resp.status_code >= 300 && resp.status_code < 400) {
            char* location = strstr(response, "Location:");
            if (!location) location = strstr(response, "location:");
            if (location) {
                location += 9;
                while (*location == ' ' || *location == '\t') location++;
                char* end = strchr(location, '\r');
                if (!end) end = strchr(location, '\n');
                if (end) {
                    size_t len = end - location;
                    if (len > sizeof(current_url) - 1) len = sizeof(current_url) - 1;
                    strncpy(current_url, location, len);
                    current_url[len] = '\0';
                    free(response);
                    redirects++;
                    continue;
                }
            }
        }

        char* body = extract_body(response);
        if (body) {
            size_t body_len = strlen(body);
            resp.data = malloc(body_len + 1);
            if (resp.data) {
                strncpy(resp.data, body, body_len);
                resp.data[body_len] = '\0';
                resp.size = body_len;
            }
        }
        free(response);
        break;
    }

    if (redirects >= MAX_REDIRECTS) {
        snprintf(resp.error, sizeof(resp.error), "Too many redirects");
    }

    return resp;
}

void http_response_free(HttpResponse* resp) {
    if (resp->data) {
        free(resp->data);
        resp->data = NULL;
    }
    resp->size = 0;
}
