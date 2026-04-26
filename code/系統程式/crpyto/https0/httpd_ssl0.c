// httpd_ssl0.c — 使用 ssl0 的 HTTPS 靜態檔案伺服器
// 編譯: cd ssl0 && gcc -o https/httpd_ssl0 https/httpd_ssl0.c src/ssl_socket.c src/ssl.c src/crypto.c src/sha.c src/aes.c src/bignum.c src/rsa.c src/certificate.c src/rand.c -I include
// 執行: cd https && ./httpd_ssl0 [port] [cert] [key]  (預設 port 8443)
// 會回傳 ./public 資料夾下的檔案給瀏覽器

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include "../include/ssl_socket.h"

#define BUFFER_SIZE  16384
#define PUBLIC_DIR   "./public"
#define DEFAULT_CERT "cert.pem"
#define DEFAULT_KEY  "key.pem"

static const char *get_content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html; charset=utf-8";
    if (strcmp(ext, ".css")  == 0) return "text/css; charset=utf-8";
    if (strcmp(ext, ".js")   == 0) return "application/javascript; charset=utf-8";
    if (strcmp(ext, ".json") == 0) return "application/json; charset=utf-8";
    if (strcmp(ext, ".png")  == 0) return "image/png";
    if (strcmp(ext, ".jpg")  == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif")  == 0) return "image/gif";
    if (strcmp(ext, ".svg")  == 0) return "image/svg+xml";
    if (strcmp(ext, ".ico")  == 0) return "image/x-icon";
    if (strcmp(ext, ".txt")  == 0) return "text/plain; charset=utf-8";
    if (strcmp(ext, ".pdf")  == 0) return "application/pdf";
    if (strcmp(ext, ".xml")  == 0) return "application/xml; charset=utf-8";
    if (strcmp(ext, ".mp3")  == 0) return "audio/mpeg";
    if (strcmp(ext, ".mp4")  == 0) return "video/mp4";
    if (strcmp(ext, ".wasm") == 0) return "application/wasm";
    return "application/octet-stream";
}

static void send_error(ssl_socket *client, int status_code, const char *status_text) {
    char body[512];
    snprintf(body, sizeof(body),
        "<!DOCTYPE html><html><head><title>%d %s</title></head>"
        "<body><h1>%d %s</h1></body></html>\r\n",
        status_code, status_text, status_code, status_text);

    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %lu\r\n"
        "Connection: close\r\n\r\n",
        status_code, status_text, (unsigned long)strlen(body));

    ssl_socket_write(client, (uint8_t*)header, strlen(header));
    ssl_socket_write(client, (uint8_t*)body, strlen(body));
}

static void handle_request(ssl_socket *client) {
    uint8_t buffer[BUFFER_SIZE];
    
    ssize_t n = ssl_socket_read(client, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        return;
    }
    buffer[n] = '\0';
    
    if (strncmp((char*)buffer, "GET ", 4) != 0) {
        send_error(client, 405, "Method Not Allowed");
        return;
    }

    char *path_start = (char*)buffer + 4;
    char *path_end = strchr(path_start, ' ');
    if (!path_end) {
        return;
    }
    *path_end = '\0';

    if (strcmp(path_start, "/") == 0) {
        path_start = "/index.html";
    }

    if (strstr(path_start, "..")) {
        send_error(client, 403, "Forbidden");
        return;
    }

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%s", PUBLIC_DIR, path_start);

    struct stat st;
    if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
        snprintf(filepath, sizeof(filepath), "%s%s/index.html", PUBLIC_DIR, path_start);
    }

    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        printf("  404 %s\n", filepath);
        send_error(client, 404, "Not Found");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    const char *content_type = get_content_type(filepath);
    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n",
        content_type, file_size);

    ssl_socket_write(client, (uint8_t*)header, strlen(header));

    char file_buf[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
        ssl_socket_write(client, (uint8_t*)file_buf, bytes);
    }

    fclose(fp);
    printf("  200 %s (%ld bytes, %s)\n", filepath, file_size, content_type);
}

int main(int argc, char *argv[]) {
    int port = 8443;
    const char *cert_file = DEFAULT_CERT;
    const char *key_file = DEFAULT_KEY;

    if (argc >= 2) port = atoi(argv[1]);
    if (argc >= 3) cert_file = argv[2];
    if (argc >= 4) key_file = argv[3];

    signal(SIGPIPE, SIG_IGN);

    ssl_socket server;
    ssl_socket_init(&server);
    
    if (ssl_socket_bind(&server, port) != 0) {
        fprintf(stderr, "無法綁定端口 %d\n", port);
        exit(1);
    }

    printf("============================================\n");
    printf("  HTTPS Server (ssl0) 啟動中...\n");
    printf("  https://localhost:%d\n", port);
    printf("  靜態檔案目錄: %s\n", PUBLIC_DIR);
    printf("  憑證: %s\n", cert_file);
    printf("  金鑰: %s\n", key_file);
    printf("  Ctrl+C 停止\n");
    printf("============================================\n");
    fflush(stdout);

    while (1) {
        ssl_socket client;
        ssl_socket_init(&client);
        
        if (ssl_socket_accept(&server, &client, cert_file, key_file) != 0) {
            printf("  Handshake 失敗\n");
            continue;
        }

        printf("[客戶端已連線]\n");
        fflush(stdout);
        
        handle_request(&client);
        
        ssl_socket_close(&client);
    }

    close(server.fd);
    return 0;
}
