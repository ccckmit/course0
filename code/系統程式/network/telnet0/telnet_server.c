// telnet_server.c — 簡易 Telnet 伺服器（支援 cd 指令）
// 編譯: gcc -Wall -o telnet_server telnet_server.c
// 執行: ./telnet_server [port]  (預設 port 2323)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define PROMPT_FMT  "\033[1;32m%s\033[0m$ "

// ------------------------------------------------------------------
// 清理子行程，避免殭屍行程
// ------------------------------------------------------------------
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// ------------------------------------------------------------------
// 移除字串尾端的 \r \n 空白
// ------------------------------------------------------------------
void trim(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == ' '))
        s[--len] = '\0';
}

// ------------------------------------------------------------------
// 傳送 prompt（含目前工作目錄）給 client
// ------------------------------------------------------------------
void send_prompt(int fd) {
    char cwd[1024];
    if (!getcwd(cwd, sizeof(cwd))) strcpy(cwd, "?");
    char prompt[1200];
    snprintf(prompt, sizeof(prompt), PROMPT_FMT, cwd);
    write(fd, prompt, strlen(prompt));
}

// ------------------------------------------------------------------
// 處理 cd 指令：回傳 1 表示是 cd，0 表示不是
// ------------------------------------------------------------------
int handle_cd(int client_fd, char *cmd) {
    // 跳過前面的空白
    while (*cmd == ' ') cmd++;

    // 判斷是否為 cd 指令
    if (strncmp(cmd, "cd", 2) != 0) return 0;
    if (cmd[2] != '\0' && cmd[2] != ' ') return 0;  // 例如 "cdd" 不算

    char *path = cmd + 2;
    while (*path == ' ') path++;

    // cd 沒有參數 → 回到 HOME
    if (*path == '\0') {
        const char *home = getenv("HOME");
        if (home) path = (char *)home;
        else path = "/";
    }

    // 執行 chdir
    if (chdir(path) != 0) {
        char errmsg[512];
        snprintf(errmsg, sizeof(errmsg), "cd: %s: %s\r\n", path, strerror(errno));
        write(client_fd, errmsg, strlen(errmsg));
    }

    return 1;
}

// ------------------------------------------------------------------
// 處理 client 連線（在 fork 的子行程中執行）
// ------------------------------------------------------------------
void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
    printf("[+] Client connected: %s:%d\n", client_ip, ntohs(client_addr->sin_port));

    // 歡迎訊息
    const char *welcome =
        "\r\n"
        "========================================\r\n"
        "  Welcome to Telnet Server\r\n"
        "  Type 'exit' or 'quit' to disconnect\r\n"
        "========================================\r\n\r\n";
    write(client_fd, welcome, strlen(welcome));

    char buffer[BUFFER_SIZE];

    while (1) {
        send_prompt(client_fd);

        // 讀取 client 輸入
        ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;  // client 斷線
        buffer[n] = '\0';
        trim(buffer);

        // 忽略空指令
        if (strlen(buffer) == 0) continue;

        // exit / quit → 結束連線
        if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "quit") == 0) {
            const char *bye = "Goodbye!\r\n";
            write(client_fd, bye, strlen(bye));
            break;
        }

        // 處理 cd 指令
        if (handle_cd(client_fd, buffer)) continue;

        // 其他指令：用 popen 執行（繼承當前 cwd）
        // 將 stderr 也導向 stdout，方便顯示錯誤訊息
        char full_cmd[BUFFER_SIZE + 16];
        snprintf(full_cmd, sizeof(full_cmd), "%s 2>&1", buffer);

        FILE *fp = popen(full_cmd, "r");
        if (!fp) {
            const char *err = "Error: cannot execute command\r\n";
            write(client_fd, err, strlen(err));
            continue;
        }

        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), fp)) {
            // 將 \n 轉換成 \r\n（telnet 協定需要）
            int len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
                strcat(line, "\r\n");
            }
            write(client_fd, line, strlen(line));
        }
        pclose(fp);
    }

    printf("[-] Client disconnected: %s:%d\n", client_ip, ntohs(client_addr->sin_port));
    close(client_fd);
}

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int port = 2323;
    if (argc >= 2) port = atoi(argv[1]);

    // 設定 SIGCHLD 處理器
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    signal(SIGPIPE, SIG_IGN);

    // 建立 socket
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
        perror("bind"); exit(1);
    }
    if (listen(server_fd, 5) < 0) { perror("listen"); exit(1); }

    printf("============================================\n");
    printf("  Telnet Server 啟動中...\n");
    printf("  Port: %d\n", port);
    printf("  連線方式: telnet localhost %d\n", port);
    printf("            或 ./telnet_client localhost %d\n", port);
    printf("  Ctrl+C 停止\n");
    printf("============================================\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
        } else if (pid == 0) {
            // 子行程處理 client
            close(server_fd);
            handle_client(client_fd, &client_addr);
            exit(0);
        } else {
            // 父行程繼續接受新連線
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
