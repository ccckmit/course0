// https_server.c — 簡易 HTTPS 靜態檔案伺服器
// 編譯: gcc -o https_server https_server.c -lssl -lcrypto
// 執行: ./https_server [port]  (預設 port 8443)
// 會回傳 ./public 資料夾下的檔案給瀏覽器
// 注意: 需先產生 SSL 憑證 (可使用 openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE  8192
#define PUBLIC_DIR   "./public"
#define DEFAULT_CERT "cert.pem"
#define DEFAULT_KEY  "key.pem"

// ------------------------------------------------------------------
// 根據副檔名回傳對應的 Content-Type
// ------------------------------------------------------------------
const char *get_content_type(const char *path) {
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

// ------------------------------------------------------------------
// 傳送 HTTPS 錯誤回應
// ------------------------------------------------------------------
void send_error(SSL *ssl, int status_code, const char *status_text) {
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

    SSL_write(ssl, header, strlen(header));
    SSL_write(ssl, body, strlen(body));
}

// ------------------------------------------------------------------
// 處理單一 HTTPS 請求
// ------------------------------------------------------------------
void handle_request(SSL *ssl, int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (n <= 0) {
        SSL_free(ssl);
        close(client_fd);
        return;
    }
    buffer[n] = '\0';

    if (strncmp(buffer, "GET ", 4) != 0) {
        send_error(ssl, 405, "Method Not Allowed");
        SSL_free(ssl);
        close(client_fd);
        return;
    }

    char *path_start = buffer + 4;
    char *path_end   = strchr(path_start, ' ');
    if (!path_end) {
        SSL_free(ssl);
        close(client_fd);
        return;
    }
    *path_end = '\0';

    char *query = strchr(path_start, '?');
    if (query) *query = '\0';

    if (strcmp(path_start, "/") == 0) {
        path_start = "/index.html";
    }

    if (strstr(path_start, "..")) {
        send_error(ssl, 403, "Forbidden");
        SSL_free(ssl);
        close(client_fd);
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
        send_error(ssl, 404, "Not Found");
        SSL_free(ssl);
        close(client_fd);
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
    SSL_write(ssl, header, strlen(header));

    char file_buf[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
        SSL_write(ssl, file_buf, bytes);
    }

    fclose(fp);
    printf("  200 %s (%ld bytes, %s)\n", filepath, file_size, content_type);
    SSL_free(ssl);
    close(client_fd);
}

// ------------------------------------------------------------------
// 初始化 SSL
// ------------------------------------------------------------------
SSL_CTX *init_ssl(const char *cert_file, const char *key_file) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        fprintf(stderr, "無法建立 SSL context\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_certificate_file(ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "無法載入憑證: %s\n", cert_file);
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "無法載入金鑰: %s\n", key_file);
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "憑證與金鑰不匹配\n");
        exit(1);
    }

    return ctx;
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int port = 8443;
    const char *cert_file = DEFAULT_CERT;
    const char *key_file = DEFAULT_KEY;

    if (argc >= 2) port = atoi(argv[1]);
    if (argc >= 3) cert_file = argv[2];
    if (argc >= 4) key_file = argv[3];

    signal(SIGPIPE, SIG_IGN);

    SSL_CTX *ctx = init_ssl(cert_file, key_file);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 128) < 0) { perror("listen"); exit(1); }

    printf("============================================\n");
    printf("  HTTPS Server 啟動中...\n");
    printf("  https://localhost:%d\n", port);
    printf("  靜態檔案目錄: %s\n", PUBLIC_DIR);
    printf("  憑證: %s\n", cert_file);
    printf("  Ctrl+C 停止\n");
    printf("============================================\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%s:%d] ", client_ip, ntohs(client_addr.sin_port));

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_fd);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stdout);
            close(client_fd);
            continue;
        }

        handle_request(ssl, client_fd);
    }

    close(server_fd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}