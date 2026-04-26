# AES 加密演算法深入解析

## 1. 演算法概述

AES (Advanced Encryption Standard) 是一個對稱式區塊加密演算法，曾經過 NIST 競爭淘汰 DES 後成為新一代的加密標準。本實作支援 AES-128（128 位元金鑰，10 回合）、AES-192（192 位元金鑰，12 回合）、AES-256（256 位元金鑰，14 回合）。

## 2. 數學基礎

### 有限體運算 GF(2^8)

AES 的核心數學基礎是建立在有限體 GF(2^8) 上的多項式體運算：

- 每個位元組（8 位元）視為有限體的元素，可表示為 GF(2)[x] 中的多項式，最高次數為 7
- 不可約多項式（Irreducible Polynomial）：m(x) = x^8 + x^4 + x^3 + x + 1（即 0x11B）
- 加法：就是位元層級的 XOR
- 乘法：多項式乘法後取模 m(x)

### xtime() 函數

xtime(x) = x ⊗ x，其實作方式是在 GF(2^8) 中進行平方：

```c
static uint8_t xtime(uint8_t x) {
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}
```

當 x 的最高位為 1 時（x & 0x80 != 0），左移後需與 0x1b（即 m(x) 的係數）進行 XOR，這正是取模 x^8 + x^4 + x^3 + x + 1 的運算。

### multiply() 函數

一般的乘法透過「附加乘法」（Russian Peasant Multiplication）實現：

```c
static uint8_t multiply(uint8_t x, uint8_t y) {
    return ((y & 1) * x) ^ ((y & 2) ? xtime(x) : 0) 
         ^ ((y & 4) ? xtime(xtime(x)) : 0) 
         ^ ((y & 8) ? xtime(xtime(xtime(x))) : 0)
         ^ ((y & 16) ? xtime(xtime(xtime(xtime(x)))) : 0);
}
```

這相當於：x ⊗ y = Σ (y 的每一位 i) × (2^i ⊗ x)

## 3. S-Box 與 Inverse S-Box

### S-Box 的建構

S-Box（元逆代換）透過以下步驟建構：

1. 初始化為 Identity：S[i] = i
2. 對每個元素計算其在 GF(2^8) 中的乘法反元素（在 m(x) 下）
3. 進行仿射變換：A(x) = a(x) ⊕ (1 ⊕ x ⊕ x^2 ⊕ x^3 ⊕ x^4 ⊕ x^6)

本實作直接使用預先計算好的查表（0x63 起始）。

### S-Box 的 криптографische 意義

S-Box 是 AES 中唯一的非線性元件，提供：
- 非線性性：輸出與輸入之間的關係無法透過簡單的線性函數描述
- 雪崩效應：輸入 1 位元變化導致輸出多位元變化
- 差分均勻性：差分傳播特性良好

## 4. 金鑰擴展（Key Expansion）

金鑰擴展將原始金鑰擴展為 11 個回合金鑰（共 176 位元組）：

### 過程說明

- w[0..3]：原始金鑰的 4 個 32 位元字
- 對於 i ≥ 4：
  - 若 i ≡ 0 (mod 4)：w[i] = w[i-4] ⊕ g(w[i-1])
  - 否則：w[i] = w[i-4] ⊕ w[i-1]

### g() 函數

g() 函數包含三個操作：
1. RotWord：32 位元組循環左移 8 位元
2. SubWord：对每個位元組進行 S-Box 代換
3. RCON：與回合常數進行 XOR

RCON[i] = [x^(i-1), 0, 0, 0]（在 GF(2^8) 中），可用於抵抗增量攻擊。

## 5. 加密回合

### State 矩陣

State 是 4×4 的位元組矩陣，以Column為主序（column-major）：

```
State[0]  State[4]  State[8]  State[12]
State[1]  State[5]  State[9]  State[13]
State[2]  State[6]  State[10] State[14]
State[3]  State[7]  State[11] State[15]
```

### 四個回合轉換

#### SubBytes（位元組代換）

對 State 中的每個位元組進行 S-Box 代換：

```c
for (int i = 0; i < 16; i++) state[i] = AES_SBOX[state[i]];
```

這是非線性轉換，提供混淆（Confusion）。

#### ShiftRows（列移位）

對每列進行不同位移：
- 第 0 列：不移位
- 第 1 列：循環左移 1 位元組
- 第 2 列：循環左移 2 位元組
- 第 3 列：循環左移 3 位元組

提供擴散（Diffusion）。

#### MixColumns（列混合）

對每個 Column（4 個位元組）進行矩陣乘法：

```c
state[i*4] = multiply(c0, 2) ^ multiply(c1, 3) ^ c2 ^ c3;
state[i*4+1] = c0 ^ multiply(c1, 2) ^ multiply(c2, 3) ^ c3;
...
```

這相當於在 GF(2^8) 中與固定矩陣相乘，提供更強的擴散。

#### AddRoundKey

簡單的 XOR 操作，將回合金鑰與 State 混合。

### 完整加密流程

```
Round 0: AddRoundKey
Round 1-9: SubBytes → ShiftRows → MixColumns → AddRoundKey
Round 10:  SubBytes → ShiftRows → AddRoundKey
```

## 6. 解密回合

解密是加密的逆過程，使用對應的反函數：

### InvSubBytes

使用 Inverse S-Box（INV_SBOX）進行代換。

### InvShiftRows

反向的列移位：
- 第 1 列：循環右移 1 位元組
- 第 2 列：循環右移 2 位元組
- 第 3 列：循環右移 3 位元組

### InvMixColumns

使用不同的 Mix 矩陣（在 GF(2^8) 中 14, 11, 13, 9 的對應乘法）：

```c
state[i*4] = multiply(c0, 14) ^ multiply(c1, 11) ^ multiply(c2, 13) ^ multiply(c3, 9);
...
```

注意：在第一回合和最後一回合，解密不需要 InvMixColumns。

## 7. CBC 模式

### 加密流程

```
C_0 = IV
C_i = AES_CBC_Encrypt(K, C_{i-1} ⊕ P_i)
```

每個區塊的加密輸入是前一個密文與當前明文的 XOR，確保相同的明文區塊會產生不同的密文。

### 解密流程

```
P_i = C_{i-1} ⊕ AES_CBC_Decrypt(K, C_i)
```

本實作的 AES-CBC 使用 PKCS#7 填充（但不完整）。

## 8. 安全性考量

本實作的簡化：
- 固定使用 128 位元金鑰（10 回合）
- 固定使用 16 位元組 IV
- 未實現完整的 PKCS#7 填充驗證

建議用於：
- 學習和理解 AES 結構
- 安全的本地加密（在金鑰管理完善的情況下）
- 不建議用於正式產品的加密需求