# RSA 金鑰生成器深入解析

## 1. 概述

keygen0.c 是一個 RSA 金鑰生成器，產生用於加密和數位簽章的 RSA 金鑰對。主要功能：
- 隨機質數生成
- RSA 模數計算
- 私密指數計算（模反元素）

## 2. RSA 金鑰生成流程

### 步驟 1：選擇兩個大質數 p, q

```
p = random prime (bits/2)
q = random prime (bits/2)
```

### 步驟 2：計算模數 n

```
n = p × q
```

n 的位元長度 = bits（例如 2048 位元 = 2048/8 = 256 位元組）

### 步驟 3：計算 φ(n)

歐拉函數：
```
φ(n) = (p-1)(q-1)
```

### 步驟 4：選擇公開指數 e

通常選擇 65537（二進制 10000000000000001）：
```
e = 65537 = 0x10001
```

選擇原因：
- 65537 是質數
- 二進制只有兩個 1（計算高效）
- 已通過安全性驗證

### 步驟 5：計算私密指數 d

使用擴展歐幾里得演算法：
```
e × d ≡ 1 (mod φ(n))
```

即求 e 在模 φ(n) 下的模反元素。

### 輸出

```
公開金鑰：(n, e)
私密金鑰：(n, d)
```

## 3. 質數生成

### 生成隨機數

```c
void generate_prime(bignum *p, int bits) {
    p->len = (bits + 31) / 32;
    
    for (size_t i = 0; i < p->len; i++) {
        p->words[i] = rand() ^ (rand() << 15);
    }
    
    // 確保最高位為 1（維持位元長度）
    p->words[0] |= 1;
    
    // 確保最高位為 1
    p->words[last_idx] |= (1ULL << (bits % 32));
}
```

### 質數檢驗

```c
int is_prime(const bignum *n) {
    // 簡單的試除法
    for (uint32_t i = 3; i < 100; i += 2) {
        // 檢查 n 是否被 i 整除
        if (n % i == 0) return 0;
    }
    return 1;
}
```

### 質數檢驗方法

#### 費馬小定理

若 a^(p-1) ≡ 1 (mod p)，且 p 為質數，則 p 大概率為質數。

#### Miller-Rabin

更準確的質數檢驗：
- 不機會產生偽質數（False Positive）
- 使用機率閾值

#### AKS

確定性質數檢驗，但運行緩慢。

## 4. 模反元素計算

### 擴展歐幾里得演算法

```c
bignum gcd, x, y;
bn_ext_gcd(&e, &phi, &gcd, &x, &y);
```

計算：
```
e × x + φ(n) × y = gcd(e, φ(n)) = 1
```

所以：
```
e × x ≡ 1 (mod φ(n))
x 即為 d
```

### 處理負數

擴展歐幾里得可能返回負數：

```c
if (x.words[0] & 0x80000000) {  // 負數檢查
    // d = x + φ(n)
    d = x + φ(n);
} else {
    d = x;
}
```

## 5. 數學原理

### 為什麼 RSA 安全

RSA 安全性基於：
```
n = p × q
```

已知 n，求 p 和 q 需要因式分解：
- 質數乘積的因式分解是困難的
- 目前無已知多項式時間演算法
- 4096 位元 RSA 需要超級電腦數千年

### 明文加密

```
c = m^e mod n
```

### 密文解密

```
m = c^d mod n
```

### 數學證明

```
c^d = (m^e)^d = m^(e×d) = m^(1+k×φ(n))
    = m × (m^φ(n))^k ≡ m × 1^k ≡ m (mod n)
```

（最後一步使用費馬-歐拉定理）

## 6. 輸出格式

### 私密金鑰格式

```
----BEGIN RSA PRIVATE KEY----
N: [n 的十六進制]
D: [d 的十六進制]
E:010001
----END RSA PRIVATE KEY----
```

### 公鑰格式

```
ssh-rsa [n 的十六進制] user@localhost
```

### SSH 公鑰格式

SSH 公鑰格式（RFC 4716）：
```
ssh-rsa AAAAB3NzaC1...
```

本簡化格式僅作為演示。

## 7. 安全性分析

### 本實作限制

1. **弱質數檢驗**
   - 只試除到 100
   - 可能接受偽質數
   
2. **可預測隨機數**
   - 使用 rand() 非密碼學安全
   - 應使用 arc4random_buf

3. **無填充**
   - 直接加密原始數據
   - 應使用 OAEP/PKCS#1 v1.5

### 攻擊向量

1. **因式分解攻擊**
   - 如果 p, q 太接近
   - 如果 p, q 有規律

2. **時序攻擊**
   - 如果解密時間可測量

3. **側信道攻擊**
   - 電力分析
   - 电磁分析

### 建議

- 生產環境使用 OpenSSL 或 libsodium
- 使用夠大的金鑰（2048+ 位元）
- 使用真正的隨機數生成

## 8. 使用方式

### 產生金鑰

```bash
./keygen0              # 產生 2048-bit 金鑰
./keygen0 -b 4096     # 產生 4096-bit 金鑰
./keygen0 -b 512 -f test   # 產生到 test, test.pub
```

### 輸出

```
Generating 2048-bit RSA keypair...
Generating prime p...
Generating prime q...
Computing modulus...
Computing private exponent...
Keypair generated!
Private key written to id_rsa
Public key written to id_rsa.pub
```

## 9. 密碼學原理總結

### 金鑰長度建議

| 位元 | 安全等級 | 建議 |
|------|--------|------|
| 1024 | 不安全 | 不使用 |
| 2048 | 中等 | 可用至 2030 |
| 4096 | 安全 | 長期安全 |

### 公用場合

1024 位元 RSA：
- 受信任的 CCA2 攻擊
- 需要數百萬美元算力

4096 位元 RSA：
- 量子電腦也難以破解
- 但運行緩慢

### 未來

量子電腦威脅：
- Shor 演算法可在多項式時間分解

後量子密碼學：
- RLWE（Ring Learning with Errors）
- Lattice-based
- Hash-based（如 SPHINCS+）