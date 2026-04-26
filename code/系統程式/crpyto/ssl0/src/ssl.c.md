# SSL/TLS 記錄層協議深入解析

## 1. 概述

ssl.c 實現 TLS 1.2 記錄層協議，包括：
- 主密鑰計算
- 金鑰衍生
- 記錄加密/解密
- MAC 計算

## 2. TLS 握手回顧

### ClientHello

客戶端發送：
- TLS 版本
- 隨機數（32 位元組）
- 會話 ID
- 密碼套件
- 擴展

### ServerHello

服務端發送：
- TLS 版本
- 隨機數
- 選擇的密碼套件

### Certificate

服務端發送 X.509 憑證。

### ServerKeyExchange

（可選）Diffie-Hellman 參數或 RSA 參數。

### CertificateRequest

（可選）請求客戶端憑證。

### ServerHelloDone

服務端完成。

### ClientKeyExchange

客戶端發送：
- 預主密鑰（PreMasterSecret）：使用服務端公鑰加密
- （可選）客戶端憑證
- CertificateVerify：使用客戶端私鑰簽章

### ChangeCipherSpec

切換到加密。

### Finished

驗證握手。

## 3. 密鑰計算

### 3.1 預主密鑰

客戶端生成 48 位元組隨機數：

```
PreMasterSecret[48] = 
    [0]  = 0x00 ( TLS major version )
    [1]  = 0x02 ( TLS 1.2 )
    [2..47] = random
```

使用 RSA 加密傳送。

### 3.2 主密鑰

```
MasterSecret = PRF(PreMasterSecret, "master secret",
                   ClientRandom + ServerRandom)
```

### 3.3 金鑰塊

```
KeyBlock = PRF(MasterSecret, "key expansion",
               ServerRandom + ClientRandom)
```

### KeyBlock 結構（TLS 1.2）

```
ClientWriteMACKey[20]  // HMAC-SHA1
ServerWriteMACKey[20]
ClientWriteKey[16]    // AES-128
ServerWriteKey[16]   // 72 bytes total
```

## 4. 記錄加密流程

### ssl_encrypt_record

```c
int ssl_encrypt_record(ssl_context *ctx,
                     uint8_t content_type,
                     const uint8_t *plaintext, size_t plain_len,
                     uint8_t *ciphertext, size_t *cipher_len)
```

### 步驟：

1. **計算 MAC**
   ```
   mac_data = seq_num || content_type || version || length || plaintext
   mac = HMAC-SHA1(mac_key, mac_data)
   ```

2. **填充**
   ```
   payload = plaintext || mac
   pkcs7_padding = 16 - (payload % 16)
   padded = payload || repeated(pkcs7_padding)
   ```

3. **生成 IV**
   ```
   IV = rand_bytes(16)
   ```

4. **加密**
   ```
   ciphertext = AES-CBC-Encrypt(key, IV, padded)
   ```

5. **構建記錄**
   ```
   record = content_type || version (0x0303) || length || IV || ciphertext
   ```

### 記錄格式

```
struct TLSRecord {
    uint8_t  content_type;    // 0x14=application, 0x16=handshake, 0x17=alert
    uint8_t  version_major;   // 0x03
    uint8_t  version_minor;  // 0x03 (TLS 1.2)
    uint16_t length;
    uint8_t  iv[16];
    uint8_t  encrypted[];
} TLSRecord;
```

## 5. 記錄解密流程

### ssl_decrypt_record

```c
int ssl_decrypt_record(ssl_context *ctx,
                    const uint8_t *ciphertext, size_t cipher_len,
                    uint8_t *content_type,
                    uint8_t *plaintext, size_t *plain_len)
```

### 步驟：

1. **提取 IV**
   ```
   IV = ciphertext[5:21]
   encrypted = ciphertext[21:]
   ```

2. **解密**
   ```
   padded = AES-CBC-Decrypt(key, IV, encrypted)
   ```

3. **移除填充**
   ```
   pad_len = padded[len-1]
   payload = padded[:len-pad_len-1]
   mac = payload[-20:]
   plaintext = payload[:-20]
   ```

4. **驗證 MAC**
   ```
   seq_num = ctx->server_seq_num
   mac_data = seq_num || content_type || version || length || plaintext
   expected_mac = HMAC(mac_key, mac_data)
   
   if (mac != expected_mac) error
   else seq_num++
   ```

## 6. 上下文管理

### ssl_context_init

```c
void ssl_context_init(ssl_context *ctx) {
    memset(ctx, 0, sizeof(ssl_context));
    ctx->has_keys = 0;
    ctx->is_server = 0;
    ctx->client_seq_num = 0;
    ctx->server_seq_num = 0;
}
```

保存：
- master_secret
- write/read MAC keys
- write/read encryption keys
- 序列號

### ssl_handshake_client

```c
int ssl_handshake_client(ssl_context *ctx,
                        const uint8_t *server_cert, size_t server_cert_len,
                        const uint8_t *client_random, size_t client_random_len,
                        const uint8_t *server_random, size_t server_random_len,
                        const uint8_t *pre_master_secret, size_t pms_len)
```

計算 master_secret 和衍生金鑰。

## 7. 安全性分析

### 本實作限制

#### 1. MAC 忽略

```c
// We intentionally ignore MAC verification
(*seq_ptr)++;
```

這導致：
- 無法檢測篡改
- 不完整性保護不足

#### 2. 無抗重放

序列號未完全保護。

#### 3. 固定填充模式

填充值可預測。

#### 4. CBC 填充 oracle

可能的 padding oracle 攻擊。

### 正確實現需要

1. 完整 MAC 驗證
2. constant-time 比較
3. 抗側信道保護

## 8. 實踐

### 初始化

```c
ssl_context ctx;
ssl_context_init(&ctx);
```

### 加密

```c
uint8_t ciphertext[1024];
size_t cipher_len;
int ret = ssl_encrypt_record(&ctx, 0x17, plaintext, plain_len,
                            ciphertext, &cipher_len);
```

### 解密

```c
uint8_t plaintext[1024];
size_t plain_len;
int ret = ssl_decrypt_record(&ctx, ciphertext, cipher_len,
                           &content_type, plaintext, &plain_len);
```

### 清理

```c
ssl_free(&ctx);
```

## 9. 密碼學原理

### 為什麼需要 MAC 先於加密

1. **認證**：確認發送者
2. **完整性**：檢測篡改
3. **順序**：序列號

### MAC-then-Encrypt 的風險

MAC-then-Encrypt：
```
E(K, M || MAC(K, M))
```

問題：
1. 修改密文可能破壞 MAC
2. 可利用 oracle

### Encrypt-then-MAC

Encrypt-then-MAC 更安全：
```
C = E(K, M)
T = MAC(K, C)
send(C || T)
```

驗證：
1. 驗證 MAC
2. 解密（若 MAC 有效）

本實作類似 Encrypt-then-MAC（先加密再傳輸 MAC）。

## 10. TLS 版本演進

| 版本 | 年份 | 狀態 |
|------|------|------|
| SSL 2.0 | 1994 | 不安全 |
| SSL 3.0 | 1995 | 不安全（POODLE） |
| TLS 1.0 | 1999 | 不安全（BEAST） |
| TLS 1.1 | 2006 | 修復 |
| TLS 1.2 | 2008 | 當前 |
| TLS 1.3 | 2018 | 更好（0-RTT） |

### TLS 1.3 改進

- 移除 MD5/SHA-1
- AEAD 加密（AES-GCM）
- 0-RTT 支持
- 簡化握手

## 11. 結論

ssl.c 提供了理解 TLS 記錄層的基礎框架，但出於安全性考慮，切勿用於生產環境。