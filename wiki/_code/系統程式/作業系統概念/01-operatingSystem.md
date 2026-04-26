# 作業系統概念範例測試結果

## 執行方式
```bash
cd _code/系統程式/作業系統概念
rustc 01-operatingSystem.rs -o 01-operatingSystem
./01-operatingSystem
```

## 執行結果

```
=== Rust 作業系統概念範例 ===

=== 1. 程序創建 ===
就緒佇列:
  PID 1: init (priority=0)
  PID 2: bash (priority=1)
  PID 3: vim (priority=2)

=== 2. 排程演算法 ===
初始順序: ["P1", "P2", "P3"]
FCFS 後: ["P1", "P2", "P3"]
Priority 後: ["P1", "P3", "P2"]

=== 3. 記憶體管理 ===
配置 256 bytes: Some(0)
配置 512 bytes: Some(256)
記憶體使用: 768/1024 bytes
空閒區塊數: 1
釋放後:
記憶體使用: 512/1024 bytes
空閒區塊數: 2

=== 4. 訊號處理 ===
收到 Ctrl+C，準備退出...
收到終止訊號
被強制終止
子程序已結束

=== 5. 系統呼叫 ===
[SYSCALL] fork()
[SYSCALL] exec(/bin/ls)
[SYSCALL] wait()
[SYSCALL] read(fd=0)
[SYSCALL] write(fd=1, size=11)

=== 6. 上下文切換 ===
執行序 1: registers[0] = 200
恢復後: registers[0] = 100

=== 7. 排程器模擬 ===
Tick 0: Running PID 1 (P1)
Tick 1: Running PID 1 (P1)
Tick 2: Running PID 1 (P1)
Tick 3: Running PID 2 (P2)
Tick 4: Running PID 2 (P2)
Tick 5: Running PID 2 (P2)
Tick 6: Running PID 3 (P3)
Tick 7: Running PID 3 (P3)
Tick 8: Running PID 3 (P3)
Tick 9: Running PID 1 (P1)
最終狀態:
  Terminated: 0

作業系統範例完成!
```

## 相關頁面
- [作業系統概念](../系統程式/作業系統.md)
- [行程與執行緒](../系統程式/行程與執行緒.md)
