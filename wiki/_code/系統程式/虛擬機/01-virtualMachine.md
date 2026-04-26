# 虛擬機範例測試結果

## 執行方式
```bash
cd _code/系統程式/虛擬機
rustc 01-virtualMachine.rs -o 01-virtualMachine
./01-virtualMachine
```

## 執行結果

```
=== Rust 虛擬機範例 ===

=== 1. Bytecode VM ===
輸出: ["8"]

=== 2. Stack VM ===
VM 輸出: ["11"]

=== 3. Register VM ===
初始化: R0 = 10, R1 = 20
執行 ADD 後: R0 = 30
Zero flag: false

=== 4. JIT 編譯 ===
IR: add r0, r1, r2
編譯為: [195] ...
執行 JIT 編譯的程式碼: [195] ...

=== 5. 指令編碼 ===
指令 -> 位元組編碼:
  Add -> [1]
  Sub -> [2]
  Mul -> [3]
  Div -> [4]
  Jump -> [96]
  Call -> [98]
  Halt -> [255]

=== 6. 堆疊操作 ===
Push 10, 20, 30
堆疊深度: 3
Top: Some(30)
Pop: 30
Pop 後堆疊深度: 2

=== 7. 記憶體訪問 ===
寫入記憶體位址 100: 42
記憶體大小: 1024 bytes

虛擬機範例完成!
```

## 相關頁面
- [虛擬機概念](../系統程式/虛擬機.md)
- [直譯器概念](../系統程式/直譯器.md)
