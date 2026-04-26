# Socket 網路介面

Socket（套接字）是網路程式設計的核心抽象，提供了不同主機上行程之間進行通訊的標準方法。1971 年，Berkeley UNIX 引入了 Socket API，此後成為網路程式設計的事實標準，被幾乎所有作業系統支援。

## Socket 的概念

Socket 本質上是一個檔案描述符（file descriptor），代表一個網路連接的端點。Unix 的「萬物皆檔案」哲學使得 Socket 可以使用標準的檔案 I/O 系統呼叫（read、write、close）進行操作。

### Socket 的類型

| 類型 | 描述 | 使用場景 |
|------|------|----------|
| SOCK_STREAM | 位元組流（TCP） | 可靠、順序、連接導向 |
| SOCK_DGRAM | 資料報（UDP） | 無連接、固定大小 |
| SOCK_RAW | 原始套接字 | 直接訪問網路層 |

### Socket 位址

不同的網域使用不同的位址格式：

```c
// IPv4
struct sockaddr_in {
    sa_family_t sin_family;  // AF_INET
    in_port_t sin_port;      // 連接埠 (如 80, 8080)
    struct in_addr sin_addr;  // IP 位址
};

// IPv6
struct sockaddr_in6 {
    sa_family_t sin6_family;   // AF_INET6
    in_port_t sin6_port;       // 連接埠
    struct in6_addr sin6_addr;  // IPv6 位址
};

// 通用位址（用於函式參數）
struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};
```

## TCP Socket 程式設計

TCP（傳輸控制協定）是連接導向、可靠的位元組流協定，適合需要保證資料送達的應用。

### TCP 伺服器

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int create_tcp_server(int port) {
    // 1. 建立 Socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return -1;

    // 2. 設定位址重用（快速重啟）
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. Bind - 綁定 IP 和連接埠
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY  // 監聽所有介面
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        return -1;
    }

    // 4. Listen - 開始監聽
    if (listen(server_fd, 128) < 0) {
        close(server_fd);
        return -1;
    }

    return server_fd;
}

void handle_client(int client_fd) {
    char buffer[1024];
    
    // 讀取客戶端資料
    ssize_t n = read(client_fd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        
        // 回覆客戶端
        const char *response = "HTTP/1.1 200 OK\r\n\r\n";
        write(client_fd, response, strlen(response));
    }
    
    close(client_fd);
}

int main() {
    int server = create_tcp_server(8080);
    printf("Server listening on port 8080\n");

    while (1) {
        // 5. Accept - 接受連接
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server, 
            (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd >= 0) {
            handle_client(client_fd);
        }
    }

    close(server);
    return 0;
}
```

### TCP 客戶端

```c
int create_tcp_client(const char *host, int port) {
    // 1. 建立 Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    // 2. 解析主機位址
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        close(sock);
        return -1;
    }

    // 3. Connect - 連接到伺服器
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = *(struct in_addr *)server->h_addr
    };
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

int main() {
    int sock = create_tcp_client("example.com", 80);
    if (sock < 0) {
        fprintf(stderr, "Connection failed\n");
        return 1;
    }

    // 發送 HTTP 請求
    const char *request = 
        "GET / HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Connection: close\r\n"
        "\r\n";
    write(sock, request, strlen(request));

    // 讀取回應
    char buffer[4096];
    ssize_t n;
    while ((n = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("%s", buffer);
    }

    close(sock);
    return 0;
}
```

## UDP Socket 程式設計

UDP（用戶資料報協定）是無連接的協定，適用於對實時性要求高但能容忍少量資料丟失的應用。

### UDP 伺服器

```c
int create_udp_server(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

void udp_server_loop(int sock) {
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        // recvfrom - 接收資料並獲取客戶端位址
        ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
            (struct sockaddr *)&client_addr, &addr_len);
        
        if (n > 0) {
            buffer[n] = '\0';
            printf("From %s:%d: %s\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port),
                buffer);

            // sendto - 回覆給客戶端
            const char *reply = "ACK";
            sendto(sock, reply, strlen(reply), 0,
                (struct sockaddr *)&client_addr, addr_len);
        }
    }
}
```

### UDP 客戶端

```c
int create_udp_client() {
    return socket(AF_INET, SOCK_DGRAM, 0);
}

void udp_client_example() {
    int sock = create_udp_client();

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(8080),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    // 發送資料（不需要 connect）
    const char *msg = "Hello, UDP Server!";
    sendto(sock, msg, strlen(msg), 0,
        (struct sockaddr *)&server, sizeof(server));

    // 接收回覆
    char buffer[1024];
    struct sockaddr_in from;
    socklen_t from_len = sizeof(from);
    ssize_t n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
        (struct sockaddr *)&from, &from_len);
    
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server replied: %s\n", buffer);
    }

    close(sock);
}
```

## 網路位元組序

不同的 CPU 架構使用不同的位元組序（Endianness）：
- **大端序（Big Endian）**：高位元組在前（網路序）
- **小端序（Little Endian）**：低位元組在前（x86、ARM）

作業系統提供位元組序轉換函數：

```c
uint32_t htonl(uint32_t hostlong);   // host to network long (32-bit)
uint16_t htons(uint16_t hostshort);  // host to network short (16-bit)
uint32_t ntohl(uint32_t netlong);    // network to host long
uint16_t ntohs(uint16_t netshort);    // network to host short
```

## 多路複用：select、poll、epoll

單執行緒處理多個 Socket 連接，需要 I/O 多路複用：

### select

```c
fd_set read_fds;
FD_ZERO(&read_fds);
FD_SET(server_fd, &read_fds);
int max_fd = server_fd;

// 添加其他 fd...

struct timeval timeout = { .tv_sec = 5, .tv_usec = 0 };

int ready = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

if (ready > 0) {
    if (FD_ISSET(server_fd, &read_fds)) {
        // 新連接到達
        int client = accept(server_fd, NULL, NULL);
        // ...
    }
    // 檢查其他 fd...
}
```

### epoll（Linux 特有）

```c
#include <sys/epoll.h>

int epoll_fd = epoll_create1(0);

struct epoll_event event = {
    .events = EPOLLIN,
    .data.fd = server_fd
};
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

struct epoll_event events[1024];

while (1) {
    int n = epoll_wait(epoll_fd, events, 1024, -1);
    
    for (int i = 0; i < n; i++) {
        if (events[i].events & EPOLLIN) {
            int fd = events[i].data.fd;
            // 處理 fd 上的讀事件
        }
    }
}
```

epoll 相比 select 的優勢：
- 無需每次重建 fd_set
- 無限制的 fd 數量
- 高效的 O(1) 事件通知

## 阻塞與非阻塞 I/O

預設情況下，Socket 操作是阻塞的：

```c
// 阻塞 read - 會一直等待直到有資料
ssize_t n = read(sock, buffer, 1024);  // 可能永遠不返回

// 設定為非阻塞
int flags = fcntl(sock, F_GETFL, 0);
fcntl(sock, F_SETFL, flags | O_NONBLOCK);

ssize_t n = read(sock, buffer, 1024);
if (n < 0 && errno == EAGAIN) {
    // 沒有資料，稍後重試
}
```

## Socket 選項

常見的 Socket 選項：

```c
// 設定超時
struct timeval tv = { .tv_sec = 30, .tv_usec = 0 };
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

// 設定緩衝區大小
int buf_size = 64 * 1024;  // 64KB
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));

// 取得對方位址
struct sockaddr_in peer;
socklen_t peer_len = sizeof(peer);
getpeername(sock, (struct sockaddr *)&peer, &peer_len);
printf("Connected to %s:%d\n", 
    inet_ntoa(peer.sin_addr), 
    ntohs(peer.sin_port));
```

## 現代網路程式設計

### 非同步 I/O

```c
// libuv 風格的非同步模式
uv_tcp_t client;
uv_tcp_init(uv_default_loop(), &client);

uv_connect_t req;
uv_tcp_connect(&req, &client, 
    (struct sockaddr *)&server_addr,
    [](uv_connect_t *req, int status) {
        if (status == 0) {
            // 連接成功，開始讀寫
        }
    });
```

### WebSocket

WebSocket 是 HTML5 引入的全雙工通訊協定，基於 HTTP upgrade：

```javascript
// 客戶端 JavaScript
const ws = new WebSocket('ws://example.com/ws');

ws.onopen = () => {
    ws.send('Hello Server!');
};

ws.onmessage = (event) => {
    console.log('Received:', event.data);
};

// 伺服器（使用 websocket 庫）
const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => {
    ws.on('message', (message) => {
        console.log('Received:', message);
        ws.send('Hello Client!');
    });
});
```

## 效能優化

### Zero-Copy

使用 `sendfile()` 系統呼叫，避免使用者空間和核心空間之間的資料複製：

```c
#include <sys/sendfile.h>

int file_fd = open("large_file", O_RDONLY);
off_t offset = 0;
size_t count = file_size;

// 直接從檔案傳送到 Socket
sendfile(client_fd, file_fd, &offset, count);
```

### TCP 選項優化

```c
// 禁用 Nagle 演算法（低延遲場景）
int nodelay = 1;
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

// 保持連接活躍
int keepalive = 1;
setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

// 快速關閉（SO_LINGER）
struct linger linger = { .l_onoff = 1, .l_linger = 0 };
setsockopt(sock, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
```

## 相關主題

- [TCP-IP協定](TCP-IP協定.md) - TCP/IP 協定棧
- [HTTP](HTTP.md) - HTTP 應用層協定
- [網路](網路.md) - 網路技術總覽
- [作業系統](作業系統.md) - 系統呼叫介面
