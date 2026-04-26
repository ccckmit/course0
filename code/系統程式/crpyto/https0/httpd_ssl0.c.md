# HTTPS 靜態檔案伺服器深入解析

## 1. 概述

httpd_ssl0.c 是一個使用 ssl0 庫的 HTTPS 靜態檔案伺服器，提供：
- TLS 加密連線
- 靜態檔案服務
- 自動 Content-Type 檢測

## 2. HTTP 協議基礎

### HTTP 請求

瀏覽器發送的 HTTP 請求格式：

```
GET /path HTTP/1.1
Host: localhost:8443
User-Agent: Mozilla/5.0...
Accept: */*
...
[空行]
```

### 請求行

```
[方法] [路徑] [版本]
```

支援的方法：
- GET：請求資料
- POST：提交資料（本實作未支援）
- HEAD：僅檢查檔案（本實作未支援）

### HTTP 回應

本實作發送的回應格式：

```
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Content-Length: 1234
Connection: close

[檔案內容]
```

### 狀態碼

| 狀態碼 | 意義 |
|--------|------|
| 200 | 成功 |
| 403 | 禁止（路徑遍历攻擊） |
| 404 | 找不到 |
| 405 | 不允許的方法 |

## 3. 檔案路徑處理

### 路徑檢查

```c
if (strstr(path_start, "..")) {
    send_error(client, 403, "Forbidden");
    return;
}
```

防止目錄遍历攻擊（Directory Traversal）：
- ../etc/passwd
- ../../secrets/key.pem

### 預設檔案

```c
if (strcmp(path_start, "/") == 0) {
    path_start = "/index.html";
}
```

目錄請求自動轉為 index.html。

### 目錄處理

```c
if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
    snprintf(filepath, ..., "%s/index.html", ...);
}
```

請求 /docs → /docs/index.html。

## 4. Content-Type 檢測

### MIME 類型

伺服器透過副檔名判斷內容類型：

```c
static const char *get_content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html; charset=utf-8";
    // ...
}
```

### 常見類型對照

| 副檔名 | Content-Type |
|--------|-----------|
| .html | text/html; charset=utf-8 |
| .css | text/css; charset=utf-8 |
| .js | application/javascript |
| .json | application/json |
| .png | image/png |
| .jpg | image/jpeg |
| .gif | image/gif |
| .pdf | application/pdf |
| .wasm | application/wasm |

### 為什麼需要 Content-Type

瀏覽器根據此欄位決定：
- 如何解釋內容
- 是否需要額外處理
- 是否需要轉譯

## 5. TLS/SSL 加密

### SSL Socket 初始化

```c
ssl_socket server;
ssl_socket_init(&server);
```

初始化 SSL 上下文（與 ssl_socket.c 相同）。

### 綁定端口

```c
if (ssl_socket_bind(&server, port) != 0) {
    exit(1);
}
```

創建 TCP socket 並監聽。

### TLS 握手

```c
if (ssl_socket_accept(&server, &client, cert_file, key_file) != 0) {
    // 握手失敗
}
```

完整 TLS 1.2 握手（參見 ssl_socket.c.md）：

1. 客戶端發送 ClientHello
2. 伺服器回應 ServerHello + Certificate
3. 客戶端驗證憑證
4. 客戶端發送 PreMasterSecret（RSA 加密）
5. 雙方計算金鑰
6. 切換到加密模式
7.  Finished 訊息驗證

### 加密讀寫

```c
ssl_socket_read(client, buffer, ...);   // 解密讀取
ssl_socket_write(client, data, ...);  // 加密寫入
```

## 6. 憑證與金鑰

### 憑證格式（PEM）

```
-----BEGIN CERTIFICATE-----
MIIBgjCCASegAwIBAgIB...
-----END CERTIFICATE-----
```

X.509 憑證，包含：
- 公鑰
- 主體資訊（CN=localhost）
- 頒發者
- 有效期
- 簽章

### 金鑰格式（PEM）

```
-----BEGIN RSA PRIVATE KEY-----
MIIEpgIBAAKCAQEA...
-----END RSA PRIVATE KEY-----
```

PKCS#1 格式的 RSA 私密金鑰���

### 自簽憑證

本實作使用自簽發憑證：
- 瀏覽器會警告
- 可接受繼續訪問

## 7. 安全性分析

### 本實作限制

1. **僅支援 GET**
   - 未處理 POST
   - 無法處理表單提交

2. **無會話管理**
   - 每個請求獨立連線
   - Connection: close

3. **固定目錄**
   - 僅提供 ./public
   - 無虛擬主機

4. **輕量驗證**
   - 未驗證客戶端憑證
   - 未實現 CRL/OCSP

### 安全漏洞

1. **路徑遍历**
   - 本實作已禁止 ..  
   - 但仍需加強路徑規範化

2. **SSL strip 攻擊**
   - 應啟用 HSTS

3. **CRIME/BREACH**
   - TLS 壓縮相關（本實作未使用）

### 改進建議

```c
// 添加：
- HSTS header
- Content-Security-Policy
- X-Frame-Options
- 請求大小限制
- 速率限制
- 日誌記錄
```

## 8. 效能考量

### 讀取模式

本實作使用簡單的阻塞讀取：

```c
while ((bytes = fread(file_buf, 1, sizeof(file_buf), fp)) > 0) {
    ssl_socket_write(client, (uint8_t*)file_buf, bytes);
}
```

### 改進方向

1. **零拷貝**
   - 使用 sendfile() 直接從檔案描述符傳輸
   
2. **緩衝**
   - 預先讀取到記憶體
   
3. **多程序/執行緒**
   - fork() 或 pthread

4. **非同步 I/O**
   - epoll/kqueue

## 9. 使用方式

### 啟動伺服器

```bash
./httpd_ssl0 8443 cert.pem key.pem
```

### 訪問

```
https://localhost:8443/
https://localhost:8443/index.html
```

### 測試

```bash
curl -k https://localhost:8443/
```

-k 忽略憑證驗證。

## 10. 密碼學原理總結

### 加密傳輸

```
HTTP Request (明文)
    ↓ RSA 加密
PreMasterSecret
    ↓ 金鑰衍生
MasterSecret → AES-256-CBC
    ↓ 加密
TLS Record (密文) → 網路
```

### 為什麼需要 HTTPS

1. **機密性**：傳輸內容加密
2. **完整性**：MAC 防止篡改
3. **身份驗證**：憑證確認伺服器身份

### 建議

- 正式環境使用 Let's Encrypt
- 啟用 TLS 1.3
- 使用強密碼套件