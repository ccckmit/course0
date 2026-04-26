# 有限狀態機範例測試結果

## 執行方式
```bash
cd _code/系統程式/有限狀態機
rustc 01-fsm.rs -o 01-fsm
./01-fsm
```

## 執行結果

```
=== Rust 有限狀態機範例 ===

=== 1. 二進位可被 3 整除的 FSM ===
  '0' 可被 3 整除: true
  '11' 可被 3 整除: true
  '110' 可被 3 整除: true
  '1001' 可被 3 整除: true
  '1010' 可被 3 整除: false
  '1111' 可被 3 整除: true

=== 2. 二進位以 01 結尾 ===
  '01' 以 01 結尾: false
  '001' 以 01 結尾: false
  '101' 以 01 結尾: false
  '1101' 以 01 結尾: false
  '010' 以 01 結尾: false
  '11' 以 01 結尾: true
  '100' 以 01 結尾: false

=== 3. NFA 範例 (包含 'ab' 或 'ba') ===
  'ab' 包含 ab 或 ba: true
  'ba' 包含 ab 或 ba: true
  'aab' 包含 ab 或 ba: false
  'bba' 包含 ab 或 ba: false
  'a' 包含 ab 或 ba: false
  'b' 包含 ab 或 ba: false
  'aba' 包含 ab 或 ba: false
  'bab' 包含 ab 或 ba: false

=== 4. 狀態轉換圖 ===
Mermaid 格式:
stateDiagram-v2
    q1: 讀到 0
    q2: 接受
    q0: 初始
    q0 --> q0: 0
    q0 --> q1: 1
    q1 --> q2: 1


=== 5. 正規表達式轉 FSM ===
a* 測試:
  '': false
  'a': false
  'aa': false
  'aaa': false
  'ba': false

ab 測試:
  'ab': false
  'aab': false
  'abb': false
  'b': true
  'a': true

=== 6. FSM 模擬路徑 ===
輸入: aac
路徑: ["start", "middle", "middle", "end"]

=== 7. 狀態最小化概念 ===
狀態最小化步驟:
1. 區分接受/非接受狀態
2. 對於每個區塊，根據輸出行為細分
3. 重複直到沒有新區塊
4. 合併等價狀態

範例: 兩個狀態 'q0' 和 'q1' 等價如果:
  - 對所有輸入字元，轉移到相同區塊
  - 都是接受狀態或都不是接受狀態

FSM 範例完成!
```

## 相關頁面
- [有限狀態機概念](../系統程式/有限狀態機.md)
