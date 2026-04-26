# SSL Socket 深入解析

## 1. 概述

ssl_socket.c 實現了一個基本的 TLS 1.2 服務器 socket，包含：
- TCP socket 綁定和監聽
- TLS 握手處理
- 客戶端憑證解析
- 私密金鑰解析
- 加密資料讀寫

## 2. 網路基礎

### Socket 初始化

```c
int ssl_socket_bind(ssl_socket *sock, int port) {
    sock->fd = socket(AF_INET, SOCK_STREAM, 0);
    // ...
    bind(sock->fd, ...);
    listen(sock->fd, 128);
}
```

- 建立 IPv4 TCP socket
- 設置 SO_REUSEADDR
- 綁定指定連接埠
- 監聽連接

### Accept

```c
int ssl_socket_accept(ssl_socket *sock, ssl_socket *client, 
                    const char *cert_pem, const char *key_pem) {
    int client_fd = accept(sock->fd, ...);
    // 執行 TLS 握手
    // 返回加密連線
}
```

返回的 client socket 具有：
- 新的 client fd
- SSL 上下文

## 3. TLS 握手處理

### ServerHello

```c
static int build_server_hello(uint8_t *buf, size_t *len,
                           const uint8_t *server_random) {
    buf[pos++] = 0x03;
    buf[pos++] = 0x03;
    memcpy(buf + pos, server_random, 32);
    pos += 32;
    buf[pos++] = 0x00;  // session_id length
    buf[pos++] = 0x00;
    buf[pos++] = 0x2f;  // TLS_RSA_WITH_AES_128_CBC_SHA
    buf[pos++] = 0x00;
    *len = pos;
}
```

ServerHello 包含：
- TLS 版本（0x0303 = TLS 1.2）
- 服務端隨機數（32 位元組）
- 會話 ID
- 密碼套件

### 密碼套件

```
0x002F = TLS_RSA_WITH_AES_128_CBC_SHA
```

含义：
- TLS：密鑰交換使用 RSA
- AES_128_CBC：加密使用 AES-128-CBC
- SHA：MAC 使用 HMAC-SHA1

### 完整握手流程

```
1. ClientHello
2. ServerHello
3. Certificate
4. ServerHelloDone
5. ClientKeyExchange
6. CertificateVerify (可選)
7. ChangeCipherSpec
8. Finished
9. ChangeCipherSpec
10. Finished
11. 加密通信
```

## 4. 私密金鑰解析

### PKCS#1 格式

RSA 私密金鑰（PEM）結構：

```
-----BEGIN RSA PRIVATE KEY-----
[Base64 編碼]
-----END RSA PRIVATE KEY-----
```

ASN.1 結構：

```
RSAPrivateKey ::= SEQUENCE {
    version           Version,
    modulus           INTEGER,    -- n
    publicExponent    INTEGER,    -- e
    privateExponent   INTEGER,    -- d
    prime1            INTEGER,    -- p
    prime2            INTEGER,    -- q
    exponent1         INTEGER,    -- d mod (p-1)
    exponent2         INTEGER,    -- d mod (q-1)
    coefficient       INTEGER,    -- (inverse of q) mod p
}
```

本實作解析：
- n：模數（256 位元組）
- d：私密指數

### parse_private_key_pem

```c
static int parse_private_key_pem(const char *pem, uint8_t *n, size_t *n_len,
                                uint8_t *d, size_t *d_len) {
    // 1. PEM → DER
    // 2. DER 解析
    // 3. 提取 n 和 d
}
```

步驟：
1. Base64 解碼
2. 解析 ASN.1 SEQUENCE
3. 跳過 version
4. 檢查 PKCS#8 或 PKCS#1
5. 提取 n（第 0 個 INTEGER）
6. 提取 d（第 2 個 INTEGER）

## 5. RSA 解密

### 預主密鑰解密

```c
rsa_decrypt(private_n, private_n_len, private_d, private_d_len,
           cipher_data, cipher_len, decrypted, &decrypted_len);
```

解密得到 PKCS#1 格式的預主密鑰。

### PKCS#1 填充移除

```c
static int remove_pkcs1_padding(const uint8_t *input, size_t input_len,
                                uint8_t *output, size_t *output_len) {
    if (input_len < 2) return -1;
    if (input[0] != 0x00) return -1;
    
    if (input[1] == 0x02) {  // 塊類型
        // 找 0x00 分隔符
        size_t i = 2;
        while (i < input_len && input[i] != 0x00) i++;
        *output_len = input_len - i - 1;
        memcpy(output, input + i + 1, *output_len);
        return 0;
    }
    return -1;
}
```

PKCS#1 v1.5 加密填充：
```
EM = 0x00 || 0x02 || PS || 0x00 || M
PS = 非零隨機位元組（至少 8 位元組）
```

### 安全考量

PKCS#1 v1.5 已知問題：
- Bleichenbacher 攻擊（oracle）
- 需要 constant-time 處理

## 6. 讀寫流程

### ssl_socket_read

```c
int ssl_socket_read(ssl_socket *sock, uint8_t *buf, size_t len) {
    if (!sock->connected) {
        return recv(sock->fd, buf, len, 0);  // 非加密
    }
    
    // 讀取 TLS record header
    // 讀取加密資料
    // 解密
    // 返回明文
}
```

### ssl_socket_write

```c
int ssl_socket_write(ssl_socket *sock, const uint8_t *buf, size_t len) {
    if (!sock->connected) {
        return send(sock->fd, buf, len, 0);
    }
    
    // 加密明文
    // 發送 TLS record
}
```

## 7. 記錄處理

### TLS Record 格式

```
struct {
    uint8_t  content_type;     // 0x17 = application_data
    uint8_t  version[2];      // 0x03 0x03
    uint16_t length;
    uint8_t  data[length];
}
```

### 記錄處理循環

讀取多個記錄：

```c
while (record_offset < recv_pos) {
    content_type = record[offset];
    rec_len = record[offset+3] << 8 | record[offset+4];
    // 處理每個記錄
}
```

## 8. TLS Finished

### 握手雜湊

```c
sha256(handshake_log, hl_len, handshake_hash);
```

### Verify Data

```c
tls_prf(master_secret, 48, "server finished", 
        handshake_hash, 32, verify_data, 12);
```

### 結構

verify_data 用於驗證：
- 服務端：PRF(master_secret, "server finished", hash)
- 客戶端：PRF(master_secret, "client finished", hash)

### Finished 消息

```c
server_finished_msg[0] = 0x14;  // handshake
server_finished_msg[1] = 0x00;
server_finished_msg[2] = 0x00;
server_finished_msg[3] = 0x0c;  // length (12)
 memcpy(server_finished_msg + 4, verify_data, 12);
```

## 9. ChangeCipherSpec

### 消息格式

```
0x14 0x03 0x03 0x00 0x01 0x01
```

含義：
- 0x14：ChangeCipherSpec 內容類型
- 0x03 0x03：TLS 1.2
- 0x00 0x01：長度 1
- 0x01：有效

### 發送時機

- 服務端：ServerFinished 後
- 客戶端：ClientKeyExchange 後

## 10. 安全性分析

### 本實作限制

1. **未驗證客戶端憑證**
2. **未驗證伺服器的 Finished 訊息**
3. **固定密碼套件**
4. **無 session 恢復**
5. **簡化的握手**

### 建議使用

- 僅用於學習 TLS 握手
- 生產環境使用 OpenSSL 或 mbedTLS

## 11. 實踐

### 服務器示例

```c
ssl_socket server;
ssl_socket client;

ssl_socket_init(&server);
ssl_socket_bind(&server, 8443);

while (1) {
    if (ssl_socket_accept(&server, &client, 
                          "cert.pem", "key.pem") == 0) {
        // 處理客戶端連接
        ssl_socket_write(&client, "HTTP/1.0 200 OK\r\n\r\n", 21);
        ssl_socket_close(&client);
    }
}
```

### 清理

```c
ssl_socket_close(&client);
ssl_socket_close(&server);
```

## 12. 密碼學原理總結

### TLS 握手的設計目標

1. **身份驗證**：透過 RSA 簽章/加密
2. **金鑰衍生**：預主密鑰 → 主密鑰 → 金鑰
3. **完整性**：握手訊息 MAC
4. **前向安全性**：可選 DH（TLS 1.3 必須）

### 攻擊向量示例

- **PARITY**：TinyTexan
- **ROBOT**：Return of Bleichenbacher
- **Lucky Thirteen**：填充 oracle
- **POODLE**：SSL 3.0 填充

### 最佳實踐

- 使用 TLS 1.3
- 啟用前向安全性
- 驗證憑證