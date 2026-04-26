# 隨機數生成深入解析

## 1. 隨機數的重要性

密碼學中隨機數用於：
- 產生加密金鑰
- IV/Nonce 初始化向量
- Padding 常數
- Salt 值

## 2. 本實作分析

### rand_bytes

```c
void rand_bytes(uint8_t *buf, size_t len) {
    if (len == 0) return;
    arc4random_buf(buf, len);
}
```

### 使用 arc4random_buf

arc4random_buf 是 BSD/macOS 系統的密碼學安全隨機數生成器：

1. **來源**：/dev/urandom（熵池）
2. **線程安全**：內部狀態保護
3. **密碼學安全**：適合加密使用

### 底層原理

arc4random 實現通常：
1. 從作業系統熵池獲取隨機資料
2. 使用 RC4 演算法進行混合
3. 輸出密碼學安全的隨機位元組

## 3. 熵池概念

### 作業系統熵源

- 硬體時序（硬碟存取時間、滑鼠移動等）
- CPU 計時器 jitter
- 網路中斷
- 鍵盤時序

### /dev/random vs /dev/urandom

- /dev/random：阻塞直到有足夠熵（會耗盡）
- /dev/urandom：使用 CSPRNG（密碼學安全偽隨機數生成器），永不阻塞

### 推薦使用

密碼學用途：
- Linux：getrandom() 或 /dev/urandom
- macOS：arc4random_buf
- Windows：CryptGenRandom 或 BCryptGenRandom

## 4. 隨機數測試

### Dieharder 測試

測試隨機數序列的統計特性。

### NIST SP 800-22

美國國家標準與技術研究院的測試套件。

### 測試項目

- 頻率測試（Monobit）
- 塊內頻率測試
- 遊程測試
- 矩陣測試
- 離散傅立葉變換測試

## 5. 常見錯誤

### 使用 rand()

```c
int x = rand();  // 非密碼學安全！
```

rand() 是線性同餘發生器，輸出可預測。

### 時間作為種子

```c
srand(time(NULL));  // 可預測
```

攻擊者知道時間即可預測輸出。

### 可預測模式

- 使用遞增計數器
- 使用簡單 LCG

## 6. 正確使用隨機數

### C 標準

```c
// 不是密碼學安全
srand(time(NULL));
int x = rand();
```

### POSIX

```c
int fd = open("/dev/urandom", O_RDONLY);
read(fd, buf, len);
close(fd);
```

### Windows

```c
BCryptGenRandom(BCRYPT_RNG_ALGORITHM, buf, len, 0);
```

### 已淘汰的 /dev/random 使用

```
cat /dev/random  # 可能阻塞！
```

## 7. 安全性考量

### 熵估計

攻擊者可能嘗試：
- 猜测熵源
- 觀察系統行為
- 回歸攻擊

### 種子和狀態

- 初始種子必須真正隨機
- 狀態不應暴露

### 預測攻擊

- 狀態預測
- 回歸分析
- 側信道觀察

## 8. 本實作限制

- macOS 特定
- 若 arc4random 有漏洞則不安全
- 未實現使用者可自定義的 RNG

## 9. 實踐建議

### 建立開即用

```c
uint8_t key[32];
arc4random_buf(key, sizeof(key));
```

### 保持安全

- 不要實現自己的 RNG
- 使用作業系統提供的 CSPRNG
- 定期更新隨機種子（如果可能）

### 測試驗證

- 統計測試
- 輸入驗證
- 壓力測試