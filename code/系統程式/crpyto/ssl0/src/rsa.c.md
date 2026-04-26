# RSA 加密演算法密碼學深入解析

## 1. 演算法概述

RSA 是目前最廣泛使用的非對稱加密演算法，由 Ron Rivest、Adi Shamir、Leonard Adleman 於 1977 年發明。安全性基於大整數分解的困難性。

## 2. 數學基礎

### 模運算環

設 n = p × q，其中 p, q 為質數，則：
- Z_n = {0, 1, 2, ..., n-1}
- Z_n 在加法和乘法下形成有限環

### 歐拉函數 φ(n)

φ(n) = (p-1)(q-1)

含義：與 n 互質的小於 n 的整數個數。

### 模反元素

若 gcd(a, n) = 1，則存在唯一的 b 使得：
```
a × b ≡ 1 (mod n)
```

b 稱為 a 在模 n 下的反元素，記作 a^(-1) mod n。

### 費馬-歐拉定理

若 gcd(a, n) = 1，則：
```
a^φ(n) ≡ 1 (mod n)
```

### e 和 d 的關係

選擇 e 使得 gcd(e, φ(n)) = 1，計算 d 使得：
```
e × d ≡ 1 (mod φ(n))
```

則：
```
m = m^(e×d) ≡ m^1 ≡ m (mod n)
```

## 3. 金鑰生成

### 步驟

1. **選擇兩個大質數** p, q
   - 建議：2048 位元以上使用 1024 位元質數
   - 保密：p, q 不應公開

2. **計算 n = p × q**
   - n 公開，用於加密
   - n 的位元數表示金鑰強度

3. **計算 φ(n) = (p-1)(q-1)**
   - 需要 p, q，不公開則無法計算

4. **選擇公開指數 e**
   - 常用值：65537 (= 0x10001)
   - 特點：65537 是質數，且二進制只有兩個 1
   - 需 gcd(e, φ(n)) = 1

5. **計算私有指數 d**
   - d = e^(-1) mod φ(n)
   - 使用擴展歐幾里得演算法

### 金鑰

- 公開金鑰：(n, e)
- 私有金鑰：(n, d)

## 4. 加密與解密

### 加密

給定明文 m，計算密文 c：

```
c = m^e mod n
```

限制：0 ≤ m < n

### 解密

給定密文 c，計算明文 m：

```
m = c^d mod n
```

### 數學證明

```
c^d = (m^e)^d = m^(e×d) = m^(1+k×φ(n)) = m × (m^φ(n))^k ≡ m × 1^k ≡ m (mod n)
```

## 5. 簽章與驗證

### 簽章

```
s = m^d mod n
```

### 驗證

```
m' = s^e mod n
若 m' = m，簽章有效
```

### 訊息雜湊

實際使用中不直接對訊息簽章，而是：

```
s = H(m)^d mod n
```

其中 H 為雜湊函數（如 SHA-256）。

## 6. 本實作分析

### rsa_encrypt

```c
int rsa_encrypt(const uint8_t *n, size_t n_len,
              const uint8_t *e, size_t e_len,
              const uint8_t *plaintext, size_t plain_len,
              uint8_t *ciphertext, size_t *cipher_len) {
    bignum N, E, M, C;
    bn_from_bytes(&N, n, n_len);
    bn_from_bytes(&E, e, e_len);
    bn_from_bytes(&M, plaintext, plain_len);
    bn_mod_exp(&C, &M, &E, &N);
    bn_to_bytes(&C, ciphertext, cipher_len);
    return 0;
}
```

- 直接使用 bignum 模指數運算
- 明文長度受限於 n 的長度
- 未實現填充

### rsa_decrypt

```c
int rsa_decrypt(const uint8_t *n, size_t n_len,
              const uint8_t *d, size_t d_len,
              const uint8_t *ciphertext, size_t cipher_len,
              uint8_t *plaintext, size_t *plain_len) {
    // ...
    bn_mod_exp(&M, &C, &D, &N);
    // ...
}
```

## 7. 安全性問題

### 未實現填充

本實作使用「原生 RSA」：

```
c = m^e mod n
```

這導致：

1. **確定性加密**：相同明文產生相同密文
2. **選擇明文攻擊**：可透過試探加密Oracle
3. **小指數攻擊**：e太小可能遭受攻擊

### 標準填充

#### PKCS#1 v1.5（RSAES-PKCS1-v1_5）

```
EM = 0x00 || 0x02 || PS || 0x00 || M
PS = non-zero random bytes（至少 8 位元組）
```

#### OAEP（RSAES-OAEP）

使用 optimal asymmetric encryption padding，有更好的安全性證明。

### 選擇密文攻擊（CCA��

無填充的 RSA 對 CCA 攻擊脆弱：
- 不應直接對訊息加密
- 應使用混合加密

### 側信道攻擊

本實作未防止：
- 定時攻擊
- 快取攻擊
- 電力分析

## 8. 實際使用建議

### 金鑰長度

- 2048 位元：可安全使用至 2030 年
- 4096 位元：長期安全

### 混合加密

1. 使用 RSA 加密隨機對稱金鑰
2. 使用對稱金鑰加密實際資料
3. AES-256-GCM 推薦

### 推薦流程

```
1. 生成隨機 session key（256 位元）
2. 使用 RSA 加密 session key
3. 使用 AES-256-GCM 加密資料
```

## 9. 數學原理深究

### 為什麼選擇 65537

- 65537 = 2^16 + 1
- 二進制：10000000000000001（只有兩位 1）
- 計算優勢：快速平方 + 16 次乘法
- 安全性證明充分

### 質數選擇的複雜性

- 需要大質數測試
- p-1 和 p+1 不應太光滑
- p 和 q 應保持距離

### 計算複雜度

- 金鑰生成：O(n^3) 或更優（大數乘法）
- 加密：O(log e × n^2)
- 解密：O(log d × n^2)

## 10. 歷史背景

### 1977：發明

Ron Rivest、Adi Shamir、Leonard Adleman 在 MIT 發明。

### 1991：PKCS#1 v1.5

成為事實標準。

### 2000-至今

發現多種理論攻擊，但實用攻擊仍需大數分解。

### 未來

- 量子電腦威脅：Shor 演算法可在多項式時間分解
- 後量子密碼學：NIST 後量子標準化進行中