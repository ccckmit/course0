// telnet_client.c — 簡易 Telnet 客戶端
// 編譯: gcc -Wall -o telnet_client telnet_client.c
// 執行: ./telnet_client [host] [port]  (預設 localhost 2323)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>

#define BUFFER_SIZE 4096

static volatile int running = 1;

void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char *argv[]) {
    const char *host = "localhost";
    int port = 2323;

    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = atoi(argv[2]);

    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    // 解析主機名稱
    struct addrinfo hints = { .ai_family = AF_INET, .ai_socktype = SOCK_STREAM };
    struct addrinfo *res;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);

    int err = getaddrinfo(host, port_str, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }

    // 建立連線
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) { perror("socket"); exit(1); }

    printf("Connecting to %s:%d ...\n", host, port);
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(1);
    }
    freeaddrinfo(res);
    printf("Connected! (Ctrl+C to quit)\n");

    // 主迴圈：用 poll 同時監聽 stdin 和 socket
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;   // 使用者輸入
    fds[0].events = POLLIN;
    fds[1].fd = sock;           // 伺服器回應
    fds[1].events = POLLIN;

    char buffer[BUFFER_SIZE];

    while (running) {
        int ret = poll(fds, 2, 1000);  // 1 秒 timeout
        if (ret < 0) {
            if (running) perror("poll");
            break;
        }

        // 伺服器有資料 → 印出來
        if (fds[1].revents & POLLIN) {
            ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
            if (n <= 0) {
                printf("\nConnection closed by server.\n");
                break;
            }
            buffer[n] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }

        // 伺服器 hangup
        if (fds[1].revents & (POLLHUP | POLLERR)) {
            printf("\nConnection lost.\n");
            break;
        }

        // 使用者有輸入 → 傳給伺服器
        if (fds[0].revents & POLLIN) {
            if (!fgets(buffer, sizeof(buffer), stdin)) break;
            // 傳送時保留 \n
            ssize_t len = strlen(buffer);
            if (write(sock, buffer, len) < 0) {
                perror("write");
                break;
            }
        }
    }

    close(sock);
    printf("Disconnected.\n");
    return 0;
}
