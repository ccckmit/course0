# HTTP

HTTP（HyperText Transfer Protocol）是網際網路的基礎應用層協定，用於在客戶端與伺服器之間傳輸超文本。HTTP 是無狀態的請求-回應協定，客戶端發送請求，伺服器回應結果。HTTP 的設計簡單而靈活，使其成為 Web 的支柱技術。從最初的單一版本發展到 HTTP/1.1、HTTP/2 再到 HTTP/3，HTTP 持續演進以滿足現代網路的需求。

## HTTP 的演進

### HTTP/0.9（1991）
- 僅支援 GET 方法
- 僅能傳輸 HTML
- 沒有 Header
- 單行請求格式

### HTTP/1.0（1996）
- 新增 POST 方法
- 新增 Header
- 支援多媒體類型
- 每個請求需要獨立連接

### HTTP/1.1（1997）
- 持久連接（Keep-Alive）
- 管道化（Pipelining）
- 分塊傳輸編碼
- 快取控制
- 主機頭（Virtual Hosting）

### HTTP/2（2015）
- 多路復用（Multiplexing）
- 頭部壓縮（HPACK）
- 伺服器推送（Server Push）
- 二進制分幀

### HTTP/3（2022）
- 基於 QUIC 協定
- 消除 HEAD-OF-LINE 阻塞
- 改善行動網路體驗
- 內建 TLS

## HTTP 請求格式

```
┌─────────────────────────────────────────────────────────────┐
│                    HTTP 請求格式                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   <method> <path> <protocol>                                │
│   Header-Name: Header-Value                                │
│   Header-Name: Header-Value                                │
│   ...                                                      │
│                                                             │
│   <request-body>                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 請求行（Request Line）

```
GET /index.html HTTP/1.1
↑      ↑            ↑
方法   路徑         版本
```

### 範例請求

```
GET /api/users/123 HTTP/1.1
Host: api.example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36
Accept: application/json
Accept-Language: zh-TW,zh;q=0.9,en-US;q=0.8,en;q=0.7
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
Cache-Control: no-cache
Accept-Encoding: gzip, deflate, br

{
  "fields": ["name", "email"]
}
```

## HTTP 回應格式

```
┌─────────────────────────────────────────────────────────────┐
│                    HTTP 回應格式                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   <protocol> <status> <status-message>                     │
│   Header-Name: Header-Value                                │
│   Header-Name: Header-value                                │
│   ...                                                      │
│                                                             │
│   <response-body>                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 狀態行（Status Line）

```
HTTP/1.1 200 OK
↑      ↑    ↑
版本   狀態  狀態訊息
```

### 範例回應

```
HTTP/1.1 200 OK
Date: Mon, 08 Apr 2024 12:34:56 GMT
Server: nginx/1.18.0
Content-Type: application/json
Content-Length: 256
Cache-Control: max-age=3600
ETag: "abc123"
X-Request-ID: req-456

{
  "id": 123,
  "name": "John Doe",
  "email": "john@example.com",
  "created_at": "2024-01-15T10:30:00Z"
}
```

## HTTP 方法

### 安全方法

安全方法不會修改資源，僅用於讀取：
- **GET**：請求資源的表示
- **HEAD**：只取得Header，不取得Body
- **OPTIONS**：取得伺服器支援的方法

### 不安全方法

這些方法會修改資源：
- **POST**：提交資料到伺服器
- **PUT**：完全替換資源
- **PATCH**：部分更新資源
- **DELETE**：刪除資源
- **CONNECT**：建立隧道連接

### 方法語意

```
┌─────────┬────────────────────────────────────────────────┐
│ 方法    │ 語意                                             │
├─────────┼────────────────────────────────────────────────┤
│ GET     │ 取得資源，應冪等（多次請求結果相同）             │
│ POST    │ 創建資源或執行動作                               │
│ PUT     │ 完全替換目標資源                                 │
│ PATCH   │ 部分更新資源                                    │
│ DELETE  │ 刪除資源                                        │
│ HEAD    │ 同 GET 但不返回 Body                            │
│ OPTIONS │ 返回伺服器支援的 HTTP 方法                       │
│ CONNECT │ 建立到目標的隧道                                │
│ TRACE   │ 回顯客戶端請求（除錯用）                         │
└─────────┴────────────────────────────────────────────────┘
```

### RESTful API 設計

```http
# 集合操作
GET    /api/users          # 取得用戶列表
POST   /api/users          # 創建新用戶

# 單一資源
GET    /api/users/123      # 取得特定用戶
PUT    /api/users/123      # 完全更新用戶
PATCH  /api/users/123      # 部分更新用戶
DELETE /api/users/123      # 刪除用戶

# 巢狀資源
GET    /api/users/123/posts          # 取得用戶的貼文
POST   /api/users/123/posts          # 創建用戶的貼文

# 動作
POST   /api/users/123/activate      # 啟用用戶
POST   /api/users/123/deactivate    # 停用用戶

# 過濾與分頁
GET /api/users?page=2&limit=20&sort=created_at&order=desc
GET /api/users?status=active&role=admin
GET /api/users?q=john&fields=name,email
```

## HTTP 狀態碼

### 1xx 資訊性

| 狀態碼 | 說明 |
|--------|------|
| 100 Continue | 客戶端可繼續發送請求 |
| 101 Switching Protocols | 切換協定（如 WebSocket） |
| 102 Processing | 處理中（WebDAV） |

### 2xx 成功

| 狀態碼 | 說明 |
|--------|------|
| 200 OK | 請求成功 |
| 201 Created | 資源已創建 |
| 202 Accepted | 請求已接受，待處理 |
| 204 No Content | 成功但無回應體 |
| 206 Partial Content | 部分內容（斷點下載） |

### 3xx 重定向

| 狀態碼 | 說明 |
|--------|------|
| 301 Moved Permanently | 永久重定向 |
| 302 Found | 暫時重定向 |
| 304 Not Modified | 使用快取（需要重新驗證） |
| 307 Temporary Redirect | 暫時重定向，保持方法 |
| 308 Permanent Redirect | 永久重定向，保持方法 |

### 4xx 客戶端錯誤

| 狀態碼 | 說明 |
|--------|------|
| 400 Bad Request | 請求格式錯誤 |
| 401 Unauthorized | 需要認證 |
| 403 Forbidden | 權限不足 |
| 404 Not Found | 資源不存在 |
| 405 Method Not Allowed | 方法不允許 |
| 409 Conflict | 資源衝突 |
| 422 Unprocessable Entity | 請求格式正確但無法處理 |
| 429 Too Many Requests | 請求過多（限流） |

### 5xx 伺服器錯誤

| 狀態碼 | 說明 |
|--------|------|
| 500 Internal Server Error | 伺服器內部錯誤 |
| 501 Not Implemented | 功能未實現 |
| 502 Bad Gateway | 閘道錯誤 |
| 503 Service Unavailable | 服務不可用 |
| 504 Gateway Timeout | 閘道超時 |

## HTTP Header

### 請求Header

```
Accept                    # 可接受的回應類型
Accept-Encoding          # 可接受的編碼
Accept-Language          # 可接受的語言
Authorization             # 認證資訊
Cache-Control            # 快取控制
Cookie                   # Cookie
Host                     # 目標主機
If-Modified-Since        # 條件式請求
If-None-Match            # ETag 條件
Origin                   # 跨域請求來源
Referer                  # 請求來源頁面
User-Agent               # 客戶端識別
```

### 回應Header

```
Allow                    # 允許的方法
Cache-Control            # 快取控制
Content-Encoding         # 內容編碼
Content-Language         # 內容語言
Content-Length           # 內容長度
Content-Type             # 內容類型
ETag                     # 資源版本標識
Expires                  # 過期時間
Last-Modified            # 最後修改時間
Location                 # 重定向目標
Server                   # 伺服器資訊
Set-Cookie               # 設定 Cookie
WWW-Authenticate        # 認證挑戰
```

### 自訂Header

```http
X-Request-ID: req-abc123
X-Rate-Limit-Limit: 1000
X-Rate-Limit-Remaining: 999
X-Custom-Header: custom-value
```

## HTTP 快取

### 快取控制

```http
# Cache-Control 指令
Cache-Control: no-cache              # 每次都要驗證
Cache-Control: no-store               # 不儲存快取
Cache-Control: private                # 只在瀏覽器快取
Cache-Control: public                 # 可被 CDN 快取
Cache-Control: max-age=3600           # 快取有效期（秒）
Cache-Control: s-maxage=86400         # 共享快取有效期

# 條件式請求
If-Modified-Since: Mon, 08 Apr 2024 12:00:00 GMT
If-None-Match: "abc123"
```

### ETag

```http
# 首次請求
GET /api/resource HTTP/1.1
HTTP/1.1 200 OK
ETag: "abc123"
Content-Length: 1234

# 客戶端快取，之後請求
GET /api/resource HTTP/1.1
If-None-Match: "abc123"

# 如果未變更
HTTP/1.1 304 Not Modified
（不返回 Body，節省頻寬）
```

## HTTP 認證

### Basic 認證

```http
# 客戶端
Authorization: Basic dXNlcm5hbWU6cGFzc3dvcmQ=
# Base64(user:password)

# 伺服器回應
HTTP/1.1 401 Unauthorized
WWW-Authenticate: Basic realm="Protected Area"
```

### Bearer Token（OAuth 2.0）

```http
# 客戶端
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...

# 伺服器回應
HTTP/1.1 401 Unauthorized
WWW-Authenticate: Bearer realm="api", error="invalid_token"
```

### API Key

```http
# 作為 Header
X-API-Key: your-api-key

# 作為 Query 參數
GET /api/users?api_key=your-api-key
```

## Cookie 與 Session

### Set-Cookie

```http
Set-Cookie: session_id=abc123; Path=/; HttpOnly; Secure; SameSite=Strict; Max-Age=3600
```

### Cookie 屬性

- **Path**：Cookie 有效的路徑
- **Domain**：Cookie 有效的域名
- **Expires**：過期時間
- **Max-Age**：有效期（秒）
- **Secure**：僅 HTTPS 傳輸
- **HttpOnly**：無法被 JavaScript 存取
- **SameSite**：CSRF 防護（Strict/Lax/None）

## HTTPS

### HTTPS 的運作

```
┌─────────────────────────────────────────────────────────────┐
│                     HTTPS 加密流程                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   客戶端                              伺服器                 │
│     │                                    │                   │
│     │─────── ClientHello ──────────────→│                   │
│     │                                    │                   │
│     │←────── ServerHello ───────────────│                   │
│     │     + 伺服器憑證                   │                   │
│     │                                    │                   │
│     │─────── 驗證憑證 + Pre-master ─────→│                   │
│     │                                    │                   │
│     │←─────── Finished ─────────────────│                   │
│     │      （加密通道建立）               │                   │
│     │                                    │                   │
│     │═════════ 加密的 HTTP ════════════│                   │
│     │                                    │                   │
└─────────────────────────────────────────────────────────────┘
```

### TLS 1.3 握手

```http
# 1. 客戶端 Hello（包含支援的Cipher Suites）
ClientHello

# 2. 伺服器 Hello（選擇 Cipher Suite + 憑證）
ServerHello, Certificate, ServerKeyExchange

# 3. 客戶端驗證並完成
ClientKeyExchange, Finished

# 4. 伺服器完成
Finished

# 開始加密通信
```

### 憑證

```http
# 憑證類型
DV (Domain Validation)     # 域名驗證
OV (Organization Validation)# 組織驗證
EV (Extended Validation)   # 擴展驗證

# 憑證格式
PEM (.pem, .crt)          # Base64 編碼
DER (.der)                 # 二進制
PKCS#12 (.p12)            # 含私鑰
```

## HTTP/2 新特性

### 多路復用

```http
# HTTP/1.1：需要多個 TCP 連接
# HTTP/2：一個連接上可並行多個請求

┌─────────────────────────────────────────────────────────────┐
│                    HTTP/2 幀                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   Stream 1: ─────── GET /index.html ───────→              │
│   Stream 3: ── GET /style.css ──→                           │
│   Stream 5: ── GET /script.js ──→                          │
│                                                             │
│   全部在同一個 TCP 連接上                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 頭部壓縮

```http
# HTTP/2 使用 HPACK
# 動態表 + 靜態編碼

# 請求1
:method: GET
:path: /index.html
:scheme: https
host: example.com

# 請求2（使用動態表索引）
:method: GET
:path: /about.html
:scheme: https
[索引]
```

### 伺服器推送

```http
# 伺服器主動推送資源
HTTP/2 200 OK
Content-Type: text/html
Link: </style.css>; rel=preload; as=style
Link: </script.js>; rel=preload; as=script

<html>
  <link rel="stylesheet" href="/style.css">
  ...
</html>
```

## HTTP/3 新特性

### QUIC 協定

- 基於 UDP
- 消除 Head-of-Line 阻塞
- 連接遷移（切換網路時保持連接）
- 內建 TLS 1.3
- 0-RTT 連接建立

```
┌─────────────────────────────────────────────────────────────┐
│                    協定堆疊                                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   HTTP/1.1      HTTP/2        HTTP/3                       │
│      ↓            ↓             ↓                          │
│    TCP          TCP           QUIC (UDP)                  │
│      ↓            ↓             ↓                          │
│     IP          IP              IP                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## cURL 範例

```bash
# 基本 GET
curl https://api.example.com/users

# POST 請求
curl -X POST https://api.example.com/users \
  -H "Content-Type: application/json" \
  -d '{"name": "John", "email": "john@example.com"}'

# 認證
curl -H "Authorization: Bearer token" https://api.example.com/protected

# 上傳檔案
curl -F "file=@/path/to/file.txt" https://api.example.com/upload

# 下載並保存
curl -O https://example.com/file.zip
curl -o output.zip https://example.com/file.zip

# 顯示詳細資訊
curl -v https://example.com
curl -i https://example.com  # 包含 Header
curl -I https://example.com  # 僅 Header

# 設定 Header
curl -H "Accept: application/json" \
     -H "Authorization: Bearer token" \
     https://api.example.com/data

# 遵循重定向
curl -L https://example.com/redirect

# 超時設定
curl --max-time 10 --connect-timeout 5 https://example.com
```

## 常見 Web 框架

### 伺服器實作

| 語言 | 框架 |
|------|------|
| Node.js | Express, Fastify, Koa |
| Python | Flask, Django, FastAPI |
| Go | Gin, Echo, Fiber |
| Java | Spring Boot, Jakarta EE |
| Ruby | Rails, Sinatra |
| PHP | Laravel, Symfony |

## 相關主題

- [TCP-IP協定](TCP-IP協定.md) - 傳輸層協定
- [網路](網路.md) - 網路基礎
- [雲端技術](雲端技術.md) - Web 服務託管