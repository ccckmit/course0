# 大整數運算（Bignum）深入解析

## 1. 數據結構與表示方式

### bignum 結構

```c
typedef struct {
    uint32_t words[MAX_BIGNUM_WORDS];
    size_t len;
} bignum;
```

### 特點

1. **Little-Endian 表示**：words[0] 存放最低有效 32 位元
2. **Trimmed 表示**：len 實際表示有效字數，高位的零字組會被移除
3. **最多 MAX_BIGNUM_WORDS 個 32 位元字**（通常可達 2048 位元以上）

這種表示方式便於：
- 加法/減法進位的傳遞
- 乘法 carry 的處理

## 2. 基本運算原理

### 加法（bn_add）

使用小端表示法，從最低位開始逐字相加：

```c
for (size_t i = 0; i < n || carry; i++) {
    uint64_t sum = carry;
    if (i < a->len) sum += a->words[i];
    if (i < b->len) sum += b->words[i];
    tmp.words[i] = (uint32_t)(sum & 0xFFFFFFFF);
    carry = sum >> 32;
}
```

關鍵點：使用 64 位元暫存器避免溢位，利用 carry 傳遞進位。

### 減法（bn_sub）

使用「加負的補數」方式處理借位：

```c
int64_t diff = (int64_t)a->words[i] - borrow;
if (i < b->len) diff -= b->words[i];
if (diff < 0) { 
    tmp.words[i] = (uint32_t)(diff + 0x100000000LL); 
    borrow = 1; 
}
```

當不夠減時，加上 2^32（即 0x100000000LL）並設置借位。

### 乘法（bn_mul）

使用傳統的豎式乘法：

```c
for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < m; j++) {
        tmp.words[i+j] += a->words[i] * b->words[j];
    }
}
```

但本實作分兩層處理：
1. 計算完整的 64 位元乘積
2. 處理進位

### 左移一位元（bn_shl1）

用於除法的位移-相減演算法：

```c
uint32_t carry = 0;
for (size_t i = 0; i < a->len; i++) {
    uint32_t next = a->words[i] >> 31;
    a->words[i] = (a->words[i] << 1) | carry;
    carry = next;
}
```

保留移出的位元作為回傳值。

## 3. 除法與模運算（bn_div_mod）

### 二進制除法（Binary Division）

使用位移-相減演算法，類似十進制長除法：

```
for (i = bit_length - 1; i >= 0; i--) {
    remainder = remainder << 1 | bit(i)
    if (remainder >= divisor) {
        remainder = remainder - divisor
        quotient = quotient | (1 << i)
    }
}
```

### 複雜度

- 時間複雜度：O(n²)，n 為位元數
- 空間複雜度：O(n)

### 實際應用

在 RSA 中，這個演算法用於：
- 計算模反元素（使用擴展歐幾里得）
- 模指數運算（square-and-multiply）

## 4. 擴展歐幾里得演算法（bn_ext_gcd）

### 數學基礎

對於整數 a, b，存在 x, y 使得：
```
a*x + b*y = gcd(a, b)
```

這稱為貝祖等式（Bezout's identity），x 和 y 可為負數。

### 演算法

使用表格追蹤係數變化：
- (old_r, r) = (a, b)
- (old_s, s) = (1, 0)
- (old_t, t) = (0, 1)

每次迭代：
```
q = old_r / r
old_r = r
r = old_r - q * r
（同時更新 s, t）
```

### 用於 RSA 的模反元素

要計算 d = e^(-1) mod φ(n)，其中 e 與 φ(n) 互質：

```c
bn_ext_gcd(&e, &phi_n, &gcd, &x, NULL);
if (gcd == 1) {
    // x 即為 e 的模反元素（可能為負）
    if (x < 0) d = phi_n + x;
}
```

### 帶號大整數（sbignum）

為了處理負數係數，內部使用帶號大整數：

```c
typedef struct {
    bignum mag;  // 絕對值
    int neg;      // 正負號
} sbignum;
```

## 5. 模指數運算（bn_mod_exp）

### Square-and-Multiply（冪示運算）

RSA 加密/解密核心運算 c = m^e mod n

### 演算法

使用二進制冪示法，從低位到高位掃描指數：

```
result = 1
b = base mod mod

for each bit i in exponent:
    if bit i == 1:
        result = result * b mod mod
    b = b * b mod mod
```

### 優勢

將 O(e) 次乘法減少為 O(log e) 次乘法。

例如：
- e = 2^10 = 1024：只需 10 次乘法而非 1024 次
- 實際 RSA 使用 65537（0x10001）：只需 1 次平方 + 16 次乘法

## 6. 位的處理

### 獲取位元（bn_bit）

```c
size_t word = k / 32, bit = k % 32;
return (a->words[word] >> bit) & 1;
```

### 位數計算（bn_bits）

從最高有效字開始，計算實際使用的位數：

```c
uint32_t top = a->words[a->len - 1];
size_t b = (a->len - 1) * 32;
while (top) { b++; top >>= 1; }
return b;
```

## 7. 位元組轉換

### bn_from_bytes

大端序位元組轉換為小端 words：

```c
for (size_t i = 0; i < len; i++) {
    size_t byte_pos = len - 1 - i;
    size_t word_idx = byte_pos / 4;
    size_t shift = (byte_pos % 4) * 8;
    a->words[word_idx] |= (uint32_t)bytes[i] << shift;
}
```

### bn_to_bytes

小端 words 轉換為大端序位元組輸出。

## 8. 安全性考量（RSA 場景）

### 選擇明文攻擊（ CPA）

本實作未實作 OAEP/PKCS#1 v1.5 填充，存在選擇明文攻擊風險。

### 定時攻擊（Timing Attack）

.constant-time 實作需注意：
- 跳過不必要的迴圈
- 使用固定時間比較函數

### 旁路攻擊

本實作未防護 power analysis, fault injection 等攻擊。

### 建議用途

僅用於：
- 學習大整數運算原理
- 理解 RSA 機制
- 不建議用於實際產品的加密部署