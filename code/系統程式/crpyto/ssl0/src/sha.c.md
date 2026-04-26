# SHA-256 密碼學深入解析

## 1. 演算法概述

SHA-256（Secure Hash Algorithm 256-bit）是 SHA-2 家族的一員，輸出 256 位元（32 位元組）的雜湊值。2001 年由 NSA 設計，2002 年被 NIST 選定為新一代標準雜湎演算法。

## 2. 數學基礎

### 模運算

所有運算均在模 2^32 的整數環上進行，使用 32 位元無號整數的自然溢位。

### 位元運算

#### ROTR(x, n)

右旋轉（Rotate Right）：

```c
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
```

#### SHR(x, n)

右移（Shift Right）：

```c
#define SHR(x, n) ((x) >> (n))
```

### 三個邏輯函數

#### CH(x, y, z) — 選擇函數

```
CH(x, y, z) = (x AND y) XOR (NOT x AND z)
```

若 x 為 1 則選擇 y，否則選擇 z。

#### MAJ(x, y, z) — 多數函數

```
MAJ(x, y, z) = (x AND y) XOR (x AND z) XOR (y AND z)
```

三個輸入中多数為 1 時結果為 1。

#### Parity

XOR 用於產生奇偶校驗。

## 3. 常數與初始化

### K 常數

64 個回合常數 K[i]，來自前 32 個質數的立方根小數部分：

```c
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    // ... 繼續 64 個常數
};
```

### 初始雜湊值

8 個初始值 H[0..7]，來自前 8 個質數的平方根小數部分：

```c
uint32_t h[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
```

這些常數的設計確保：
- 無法透過逆向初始化值來製造碰撞
- 常數本身經過嚴格審查

## 4. 雜湊計算流程

### 步驟 1：填充（Padding）

將訊息填充至 512 位元的倍數：

```
原始訊息：M
填充後：M || 1 || 0...0 || (64-bit length)
```

填充規則：
1. 在訊息末尾添加 bit '1'（0x80）
2. 添加 '0'，直到長度 ≡ 448 (mod 512)
3. 在最後 64 位元添加原始訊息的位元長度（big-endian）

### 步驟 2：分割區塊

將填充後的訊息分割為 512 位元（64 位元組）的區塊。

### 步驟 3：區塊處理

對每個 512 位元區塊進行處理：

```
For each chunk M[i]:
    1. 準備訊息排程 W[0..63]
    2. 初始化 a, b, c, d, e, f, g, h
    3. 執行 64 回合compression
    4. 更新雜湊值
```

## 5. 訊息排程（Message Schedule）

### 擴展為 64 個字

```c
for (int i = 0; i < 16; i++) {
    w[i] = chunk[i*4..i*4+3];
}
for (int i = 16; i < 64; i++) {
    w[i] = SIG1(w[i-2]) + w[i-7] + SIG0(w[i-15]) + w[i-16];
}
```

其中：

```
SIG0(x) = ROTR(x,7) XOR ROTR(x,18) XOR SHR(x,3)
SIG1(x) = ROTR(x,17) XOR ROTR(x,19) XOR SHR(x,10)
```

### 設計原理

- 使用前 16 個字的原始值
- 第 16-63 字透過較早的字非線性組合產生
- 每一輪的輸入與前多輪相關，防止快速計算攻擊

## 6. Compression 函數

### 回合運算

```c
for (int i = 0; i < 64; i++) {
    T1 = h + EP1(e) + CH(e,f,g) + K[i] + W[i];
    T2 = EP0(a) + MAJ(a,b,c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;
}
```

其中：

```
EP0(x) = ROTR(x,2) XOR ROTR(x,13) XOR ROTR(x,22)
EP1(x) = ROTR(x,6) XOR ROTR(x,11) XOR ROTR(x,25)
```

### 運算意義

- T1 = h + Σ₁(e) + Ch(e,f,g) + K[i] + W[i]
  - Σ₁(e)：e 的「大力」擴散
  - Ch：選擇函數（非線性）
  - K[i]+W[i]：訊息混合
- T2 = Σ₀(a) + Maj(a,b,c)
  - Σ₀(a)：a 的「大力」擴散
  - Maj：多数函數

### 為什麼這樣設計

1. **雪崩效應**：每一位元變化會影響多個位元的最��輸出
2. **非線性性**：CH 和 MAJ 提供混合
3. **可逆性**：Compression 函數本身的逆向困難
4. **擴散**：使用旋轉和移位讓每一位元影響整個狀態

## 7. 雜湊更新

每個區塊處理後，更新累積的雜湊值：

```c
h[0] += a; h[1] += b; h[2] += c; h[3] += d;
h[4] += e; h[5] += f; h[6] += g; h[7] += h;
```

這確保最終雜湊是所有區塊的函數。

## 8. 輸出

將 8 個 32 位元狀態連接為 256 位元（32 位元組）輸出：

```c
for (int i = 0; i < 8; i++) {
    digest[i*4] = (h[i] >> 24) & 0xff;
    digest[i*4+1] = (h[i] >> 16) & 0xff;
    digest[i*4+2] = (h[i] >> 8) & 0xff;
    digest[i*4+3] = h[i] & 0xff;
}
```

## 9. 安全性

### 抗碰撞性

SHA-256 的設計使得：
1. 找到碰撞需要約 2^128 次操作（生日攻擊的最佳可能）
2. 目前無已知的實用攻擊

### 原像抗性

找到具有特定雜湎的前像需要約 2^256 次操作。

### 第二原像抗性

給定訊息 M₁，找到 M₂ 使 H(M₁) = H(M₂) 需要約 2^256 次操作。

## 10. HMAC-SHA256

### 設計原理

HMAC（Keyed-Hashing Message Authentication Code）：

```
HMAC(K, m) = H( (K ⊕ opad) || H( (K ⊕ ipad) || m ) )
```

其中：
- ipad = 0x36 重複 64 次
- opad = 0x5C 重複 64 次
- K：金鑰
- m：訊息

### 為什麼有效

1. 內層雜湎：K ⊕ ipad 與訊息混淆
2. 外層雜湎：金鑰再次摻入，防止內層輸出暴露
3. 認證金鑰長度的話，金鑰不應超過 block size

### 本實作說明

```c
void hmac_sha256(...) {
    // 步驟 1：金鑰處理（若太長則先雜湎）
    // 步驟 2：構建 inner 和 outer padding
    // 步驟 3：計算 inner hash
    // 步驟 4：計算 outer hash
}
```

## 11. 與 TLS PRF

TLS 1.0-1.2 使用 HMAC 結構的 PRF（PRF with HMAC）。

## 12. 使用場景

- 訊息完整性驗證
- 數位簽章的輔助（如 RSA 對 SHA-256 雜湎簽章）
- 金鑰衍生（TLS master secret）
- 密碼儲存（Argon2 更好）