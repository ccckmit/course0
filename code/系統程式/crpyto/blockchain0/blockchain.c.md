# 區塊鏈深入解析

## 1. 區塊鏈概述

區塊鏈（Blockchain）是一種分散式帳本技術，透過密碼學確保資料的不可變性與完整性。本實作展示最基本的區塊鏈結構。

## 2. 區塊結構

### Block 結構

```c
typedef struct Block {
    int index;              // 區塊編號
    time_t timestamp;      // 時間戳記
    char data[DATA_SIZE];  // 資料（交易等）
    char previous_hash[];   // 前一個區塊的雜湎值
    char hash[];            // 本區塊的雜湎值
    int nonce;             // 工作量證明的計數器
    struct Block* next;     // 下一個區塊指標
} Block;
```

### 每個欄位的意義

| 欄位 | 說明 |
|------|------|
| index | 區塊在鏈中的位置 |
| timestamp | 區塊創建時間 |
| data | 實際資料（如交易） |
| previous_hash | 前一區塊的雜湎，確保鏈的連續性 |
| hash | 本區塊的雜湎（SHA-256） |
| nonce | 用於工作量證明（PoW） |
| next | 指向下一個區塊（鏈接串列） |

## 3. 密碼學基礎

### SHA-256 雜湎函數

本實作使用 SHA-256（參見 sha.c.md）將區塊內容雜湎成固定長度輸出：

```
SHA-256(input) → 256-bit (64 字元十六進制) output
```

### 雜湎計算

```c
char* calculate_hash(int index, time_t timestamp, const char* data, 
                 const char* previous_hash, int nonce) {
    char combined[512];
    sprintf(combined, "%d%ld%s%s%d", index, timestamp, data, previous_hash, nonce);
    
    uint8_t digest[SHA256_DIGEST_SIZE];
    sha256((uint8_t*)combined, strlen(combined), digest);
    
    // 轉換為十六進制字串
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        sprintf(hash_str + (i * 2), "%02x", digest[i]);
    }
}
```

### 為什麼使用雜湎

1. **單向性**：從輸出無法推導輸入
2. **確定性**：相同輸入永遠產生相同輸出
3. **雪崩效應**：輸入微小變化導致輸出巨大變化
4. **抗碰撞性**：難以找到兩個不同輸入產生相同輸出

### 雜湎的連續性

```
Block[i].previous_hash = Block[i-1].hash
```

這保證：
- 任何區塊資料改變都會使後續所有雜湎失效
- 需要重新挖掘（mine）該區塊後的所有區塊

## 4. 工作量證明（Proof of Work）

### 概念

工作量證明（PoW）要求找到一個 nonce 值，使得產生的雜湎值滿足特定條件（本實作為前 DIFFICULTY 位為 '0'）。

### 挖掘過程

```c
void mine_block(Block* block) {
    int nonce = 0;
    while (1) {
        char* hash = calculate_hash(..., nonce);
        if (is_hash_valid(hash)) {  // 前 DIFFICULTY 位為 '0'
            block->nonce = nonce;
            break;
        }
        nonce++;
    }
}
```

### 難度

本實作設定 DIFFICULTY = 4，意味著雜湎值前 4 個字元必須為 '0'：

```
有效雜湎：0000a1b2c3d4e5f6...
無效雜湎：1234abcd...
```

### 概率分析

SHA-256 輸出均勻分佈在 0x0...0xFF... 的 64 字元空間。

要满足前 n 位为 '0' 的概率：
- P(n 位皆為 '0') = (1/16)^n
- n = 4：P = 1/65536 ≈ 0.0015%

平均需要嘗試約 65,536 次才能找到有效 nonce。

### 比特幣難度

比特幣區塊的目標雜湎是一個 256 位元數值，目標閾值約為 2^224，難度動態調整（約 10 分鐘出一個區塊）。

## 5. 驗證演算法

### is_chain_valid()

```c
int is_chain_valid() {
    Block* current = genesis_block->next;
    Block* previous = genesis_block;
    
    while (current != NULL) {
        // 重新計算雜湎
        char* calculated_hash = calculate_hash(
            current->index, current->timestamp, current->data,
            current->previous_hash, current->nonce
        );
        
        // 1. 檢查雜湎是否匹配
        if (strcmp(calculated_hash, current->hash) != 0) return 0;
        
        // 2. 檢查雜湎是否有效（滿足難度）
        if (!is_hash_valid(current->hash)) return 0;
        
        // 3. 檢查鏈接是否正確
        if (strcmp(current->previous_hash, previous->hash) != 0) return 0;
        
        previous = current;
        current = current->next;
    }
    return 1;
}
```

### 驗證三個條件

1. **資料完整性**：重新計算的雜湎必須等於儲存的雜湎
2. **工作量**：雜湎必須滿足難度要求
3. **鏈的連續性**：previous_hash 必須指向前一區塊的实际雜湎

## 6. 攻擊向量分析

### 51% 攻擊

若攻擊者控制超過 50% 的算力，可以：
- 逆轉自己的交易（雙重支付）
- 阻止新交易確認
- 壟斷新區塊的挖掘

本實作無法防禦此攻擊（中央化設計）。

### 篡改攻擊

若攻擊者修改某區塊的 data：
1. 區塊的 hash 改變
2. 該區塊的 previous_hash 與前一區塊的 hash 不匹配
3. 區塊鏈斷裂

防禦：驗證時會檢測到此断链。

### 重放攻擊

本實作未實現，實際區塊鏈需要：
- 交易簽章
- Nonce 防重放
- 時間窗口

## 7. 資料結構

### 鏈接串列

本實作使用指標建構區塊鏈：

```
genesis_block → block1 → block2 → block3 → NULL
```

### 優點

- 新區塊可快速附加（O(1)）
- 記憶體效率高（依序配置）

### 缺點

- 無法快速隨機存取
- 搜尋需要 O(n)

### 改進方向

使用雜湎表（Hash Table）或Merkle 樹。

## 8. 實際區塊鏈系統

### 比特幣區塊

比特幣區塊包含：
- 版本號
- 前區塊雜湎（Merkle 根）
- 時間戳記
- 難度目標
- Nonce
- 交易計數
- 交易資料（Merkle 樹）

### 以太坊

- 使用帳戶模型（非 UTXO）
- EVM（以太坊虛擬機）
- 智慧合約

### Merkle 樹

```
        ROOT
       /    \
    [AB]    [CD]
   /  \      / \
  A    B    C    D
```

快速驗證交易是否存在區塊中（O(log n)）。

## 9. 共識機制

### 工作量證明（PoW）

- 比特幣使用
- 能源密集
- 安全但緩慢

### 權益證明（PoS）

- 以太坊 2.0 使用
- 根據持幣量選擇驗證者
- 能源效率高

### 實用拜占庭容錯（PBFT）

- 聯盟鏈使用
- 需要已知驗證者集合
- 快速確認

## 10. 安全性總結

### 本實作限制

- 中央化：單一節點
- 無交易驗證
- 無網路同步
- 輕量 PoW（DIFFICULTY = 4）

### 建議

僅用於學習區塊鏈基本概念，生產環境使用成熟的區塊鏈框架。