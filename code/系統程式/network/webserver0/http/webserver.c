// webserver.c — 簡易 HTTP 靜態檔案伺服器
// 編譯: gcc -o webserver webserver.c
// 執行: ./webserver [port]  (預設 port 8080)
// 會回傳 ./public 資料夾下的檔案給瀏覽器

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE  8192
#define PUBLIC_DIR   "./public"

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
// 傳送 HTTP 錯誤回應
// ------------------------------------------------------------------
void send_error(int client_fd, int status_code, const char *status_text) {
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

    write(client_fd, header, strlen(header));
    write(client_fd, body, strlen(body));
}

// ------------------------------------------------------------------
// 處理單一 HTTP 請求
// ------------------------------------------------------------------
void handle_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n <= 0) { close(client_fd); return; }
    buffer[n] = '\0';

    // 只處理 GET 請求
    if (strncmp(buffer, "GET ", 4) != 0) {
        send_error(client_fd, 405, "Method Not Allowed");
        close(client_fd);
        return;
    }

    // 取出請求路徑
    char *path_start = buffer + 4;
    char *path_end   = strchr(path_start, ' ');
    if (!path_end) { close(client_fd); return; }
    *path_end = '\0';

    // 去掉 query string
    char *query = strchr(path_start, '?');
    if (query) *query = '\0';

    // 預設首頁
    if (strcmp(path_start, "/") == 0) {
        path_start = "/index.html";
    }

    // 安全檢查：阻擋 ".." 路徑穿越攻擊
    if (strstr(path_start, "..")) {
        send_error(client_fd, 403, "Forbidden");
        close(client_fd);
        return;
    }

    // 組合完整檔案路徑
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%s", PUBLIC_DIR, path_start);

    // 檢查是否為目錄，若是則嘗試 index.html
    struct stat st;
    if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
        snprintf(filepath, sizeof(filepath), "%s%s/index.html", PUBLIC_DIR, path_start);
    }

    // 開啟檔案
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        printf("  404 %s\n", filepath);
        send_error(client_fd, 404, "Not Found");
        close(client_fd);
        return;
    }

    // 取得檔案大小
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 傳送 HTTP 回應標頭
    const char *content_type = get_content_type(filepath);
    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n\r\n",
        content_type, file_size);
    write(client_fd, header, strlen(header));

    // 傳送檔案內容
    char file_buf[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
        write(client_fd, file_buf, bytes);
    }

    fclose(fp);
    printf("  200 %s (%ld bytes, %s)\n", filepath, file_size, content_type);
    close(client_fd);
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int port = 8080;
    if (argc >= 2) port = atoi(argv[1]);

    // 忽略 SIGPIPE，避免 client 提前斷線導致程式結束
    signal(SIGPIPE, SIG_IGN);

    // 建立 socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    // 允許快速重用 port
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 綁定位址
    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 開始監聽
    if (listen(server_fd, 128) < 0) { perror("listen"); exit(1); }

    printf("============================================\n");
    printf("  Web Server 啟動中...\n");
    printf("  http://localhost:%d\n", port);
    printf("  靜態檔案目錄: %s\n", PUBLIC_DIR);
    printf("  Ctrl+C 停止\n");
    printf("============================================\n");

    // 主迴圈：接受連線並處理
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%s:%d] ", client_ip, ntohs(client_addr.sin_port));

        handle_request(client_fd);
    }

    close(server_fd);
    return 0;
}
