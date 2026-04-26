# 程式碼技術文檔生成器

為 source code 檔案產生深度技術文檔，說明程式的原理（數學、密碼學、演算法等）。

## 觸發時機

當使用者說：
- 為某個程式加說明
- 解釋程式的原理
- 為程式加技術文檔
- 寫 *.c.md 或 *.md 說明檔

## 使用方式

1. 閱讀原始程式碼
2. 分析領域知識（密碼學、區塊鏈、網路等）
3. 撰寫技術文檔
4. 存為 `[原檔名].md`

## 領域知識對照

| 領域 | 需要說明 |
|------|---------|
| 密碼學 | AES/SHA/RSA 數學基礎、金鑰管理 |
| 區塊鏈 | PoW、共識機制、Merkle 樹 |
| 網路 | TLS/SSL、HTTP 協定 |
| 資料結構 | 時間/空間複雜度 |

## 參考範例

- `/Users/Shared/ccc/computer0/crpyto/ssl0/src/aes.c.md`
- `/Users/Shared/ccc/computer0/crpyto/blockchain0/blockchain.c.md`
- `/Users/Shared/ccc/computer0/crpyto/https0/httpd_ssl0.c.md`
- `/Users/Shared/ccc/computer0/crpyto/keygen0/keygen0.c.md`