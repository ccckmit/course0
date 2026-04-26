# TLS PRF 與密碼學原語深入解析

## 1. 概述

本模組實現 TLS 1.2 使用的 PRF（ Pseudo-Random Function）以及密碼學原語。

## 2. TLS PRF

### TLS 1.0-1.1 PRF

早期版本使用 MD5+SHA-1 的組合：

```
PRF(secret, label, seed) =
    MD5(HL(secret, label, seed)) XOR
    SHA-1(HL(secret, label, seed))
```

其中 HL 按長度分割秘密。

### TLS 1.2 PRF

TLS 1.2 統一使用 HMAC：

```
PRF(secret, label, seed) = HMAC-SHA256(secret, label || seed)
```

若需要更長輸出，迭代：

```
A(0) = label || seed
A(i) = HMAC(secret, A(i-1))
PRF = HMAC(secret, A(1)) ⊕ HMAC(secret, A(2)) ⊕ ...
```

### 本實作

```c
void tls_prf(const uint8_t *secret, size_t secret_len,
            const char *label,
            const uint8_t *seed, size_t seed_len,
            uint8_t *output, size_t out_len) {
    // 構建 label || seed
    // A_0 = HMAC(secret, label || seed)
    // 迭代生成輸出
}
```

### 實現詳解

```c
void tls_prf(...) {
    // 步驟 1：構建 seed + label
    uint8_t *label_seed = malloc(label_len + seed_len);
    memcpy(label_seed, label, label_len);
    memcpy(label_seed + label_len, seed, seed_len);
    
    // 步驟 2：A_0 = HMAC(secret, label_seed)
    uint8_t A[SHA256_DIGEST_SIZE];
    hmac_sha256(secret, secret_len, label_seed, A_len, A);
    
    // 步驟 3：迭代生成
    while (generated < out_len) {
        uint8_t *block = malloc(SHA256_DIGEST_SIZE + A_len);
        memcpy(block, A, SHA256_DIGEST_SIZE);
        memcpy(block + SHA256_DIGEST_SIZE, label_seed, A_len);
        
        hmac_sha256(secret, secret_len, block, ..., result);
        
        memcpy(output + generated, result, copy_len);
        generated += copy_len;
        
        // A = HMAC(secret, A)
        hmac_sha256(secret, secret_len, A, SHA256_DIGEST_SIZE, A);
    }
}
```

### 安全性

- 每輪都混入金鑰
- 非確定性輸出（若秘密未知）
- 需要 2^256 嘗試猜測

## 3. 密碼學金鑰衍生

### 用途

TLS 握手中的金鑰衍生：

```
master_secret = PRF(pre_master_secret, "master secret", 
                   client_random + server_random)

key_block = PRF(master_secret, "key expansion",
                server_random + client_random)
```

### Key Block 結構

TLS 1.2 的 key_block：

```
struct {
    mac_key_client[20]
    mac_key_server[20]
    enc_key_client[16]
    enc_key_server[16]
} key_block = 72 位元組
```

- MAC：金鑰用於 HMAC-SHA1（20 位元組）
- Encryption：AES-128-CBC（16 位元組）
- IV：16 位元組（在加密記錄中傳遞）

## 4. 訊息認證（MAC）

### MAC 的重要性

MAC 提供：
1. 完整性：檢測篡改
2. 認證：確認發送者身份
3. 順序：防重放攻擊

### HMAC 結構

```
HMAC(K, M) = H( (K ⊕ opad) || H( (K ⊕ ipad) || M ) )
```

### TLS MAC 計算

TLS 1.2 使用：

```
Mac(Key, seq_num || type || version || length || data)
```

包含：
- 序列號：防重放
- 內容類型
- TLS 版本
- 長度
- 資料

## 5. 填充

### PKCS#7 填充

AES-CBC 需要輸入為區塊大小倍數：

```
padded[i] = byte(n - i)  // 當 i >= n
```

其中 n = 區塊大小（16）。

### 填充驗證

解密後檢查最後位元組：
- 若 ≥ 16 或 = 16 且不符：錯誤
- 移除填充取得實際長度

## 6. 與其他模組的關係

### 依賴圖

```
crypto.c
    └── sha.c (hmac_sha256)
```

### ssl.c 使用 crypto.c

- tls_prf 用於 master_secret 計算
- tls_prf 用於 key_expansion

### SSL 記錄加密

```
ssl.c
    ├── aes.c (aes_cbc_encrypt/decrypt)
    ├── sha1.c (hmac_sha1 用於 MAC)
    ├── rand.c (rand_bytes 用於 IV)
    └── crypto.c (tls_prf)
```

## 7. 安全性考量

### 本實作限制

1. 未實現完整的 TLS 握手
2. 簡化的 PRF 實現
3. 無抗重放保護

### 攻擊向量

1. **Oracle 填充攻擊**： Lucky Thirteen
2. **-timing attack **：側信道
3. **POODLE**：SSL 3.0 填充

### 建議

- 僅用於學習
- 生產環境使用已知 TLS 庫
- 禁用 SSL 3.0
- 啟用 TLS 1.3

## 8. 實踐

### 使用 tls_prf

```c
uint8_t master_secret[48];
tls_prf(pre_master_secret, pms_len,
        "master secret",
        seed, seed_len,
        master_secret, 48);
```

### 金鑰衍生模式

```c
uint8_t key_block[72];
tls_prf(master_secret, 48,
        "key expansion",
        seed, seed_len,
        key_block, 72);
```

提取各部分：
```c
memcpy(client_write_mac_key, key_block, 20);
memcpy(server_write_mac_key, key_block + 20, 20);
memcpy(client_write_key, key_block + 40, 16);
memcpy(server_write_key, key_block + 56, 16);
```