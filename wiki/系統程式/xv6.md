# xv6

## 概述

xv6 是 MIT 對 Unix Version 6 的重新實作，使用 ANSI C 編寫，支援現代 RISC-V 多核心處理器。是作業系統教學的經典教材。

## 基本資訊

| 項目 | 內容 |
|------|------|
| 維護者 | MIT PDOS Group |
| GitHub | https://github.com/mit-pdos/xv6-riscv |
| 語言 | C, RISC-V Assembly |
| 星標 | 9.5k |
| 分支 | 3.9k |
| 課程 | MIT 6.1810 |

## 架構總覽

```
xv6 系統架構
┌─────────────────────────────────────────────────────────────┐
│                        使用者空間                            │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │
│  │  init   │  │   sh    │  │ cat     │  │  ls     │       │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘       │
├───────┴────────────┴────────────┴────────────┴────────────┤
│                     系統呼叫介面                             │
│              (user/user.h, usys.S)                        │
├─────────────────────────────────────────────────────────────┤
│                        核心空間                              │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │
│  │ proc.c  │  │  vm.c   │  │  bio.c  │  │  file.c │       │
│  │ 行程管理 │  │ 虛擬記憶體│  │ 區塊層  │  │ 檔案系統  │       │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘       │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                    │
│  │  trap.c │  │  pipe.c │  │  spinlock│                   │
│  │ 陷阱處理 │  │  管線    │  │  自旋鎖  │                    │
│  └─────────┘  └─────────┘  └─────────┘                    │
├─────────────────────────────────────────────────────────────┤
│                      硬體抽象層                             │
│              (plic.c, virtio_disk.c)                      │
└─────────────────────────────────────────────────────────────┘
```

## 目錄結構

```
xv6-riscv/
├── kernel/                 # 核心程式碼
│   ├── main.c             # 入口點
│   ├── proc.c             # 行程管理
│   ├── vm.c               # 虛擬記憶體
│   ├── trampoline.S       # 使用者/核心切換
│   ├── trap.c             # 中斷和陷阱處理
│   ├── bio.c              # 區塊層
│   ├── fs.c               # 檔案系統
│   ├── file.c             # 檔案抽象層
│   ├── pipe.c             # 管線
│   ├── spinlock.c         # 鎖機制
│   ├── sleeplock.c        # 睡眠鎖
│   ├── string.c           # 字串處理
│   └── printf.c           # 格式化輸出
├── user/                  # 使用者程式
│   ├── init.c             # 第一個行程
│   ├── sh.c               # 殼層
│   ├── cat.c              # 檔案串接
│   ├── ls.c               # 目錄列表
│   ├── mkdir.c            # 建立目錄
│   ├── rm.c               # 刪除檔案
│   ├── echo.c             # 輸出文字
│   ├── grep.c             # 模式匹配
│   ├── wc.c               # 字數統計
│   └── _*.c               # 測試程式
├── mkfs/                  # 檔案系統映像
└── Makefile              # 建置系統
```

## 核心元件

### 行程管理 (proc.c)

```c
// 行程結構
struct proc {
    struct spinlock lock;
    enum procstate state;      // 行程狀態
    int pid;                   // 行程 ID
    uint64_t kstack;           // 核心堆疊
    pagetable_t pagetable;     // 頁表
    struct trapframe *trapframe;
    struct context context;    // 上下文
    struct file *ofile[NOFILE]; // 開啟的檔案
    char name[16];             // 行程名稱
};

// 行程狀態
enum procstate {
    UNUSED,    // 未使用
    USED,      // 已分配
    SLEEPING,  // 睡眠中
    RUNNABLE,  // 可執行
    RUNNING,   // 執行中
    ZOMBIE     // 殭屍狀態
};
```

### 虛擬記憶體 (vm.c)

```c
// 虛擬位址空間佈局
/*
 * xv6 虛擬位址空間 (RISC-V Sv39)
 * 
 * TRAMPOLINE   - trampoline.S 程式碼 (兩處映射)
 * TRAPFRAME    - 陷阱框架頁
 * KERNEL       -核心程式碼和資料
 * user data    - 使用者資料
 * user text    - 使用者程式碼
 * guard page   - 防止堆疊溢位
 * heap (grows up)
 * stack        - 初始堆疊
 * guard page   - 防止堆疊溢位
 */
```

### 系統呼叫

```c
// 常見系統呼叫
int fork(void);              // 建立新行程
int exit(int status);         // 終止行程
int wait(int *status);        // 等待子行程
int kill(int pid);            // 終止行程
int getpid(void);             // 取得行程 ID
int sleep(int ticks);         // 睡眠

// 檔案相關
int open(char *path, int mode);
int read(int fd, void *buf, int n);
int write(int fd, void *buf, int n);
int close(int fd);
int dup(int fd);

// 行程控制
int exec(char *path, char **argv);
int pipe(int fd[2]);
int mknod(char *path);
int unlink(char *path);
```

## 建置與執行

### 環境需求
```bash
# 安裝 RISC-V 工具鏈
# Ubuntu/Debian
sudo apt install riscv64-unknown-elf-gcc

# 或使用 qemu 開發環境
sudo apt install qemu-system-riscv64
```

### 編譯執行
```bash
# 完整編譯
make qemu

# 單獨編譯
make

# 清除
make clean

# 執行除錯模式
make qemu-gdb
```

### GDB 除錯
```bash
# 終端 1：啟動 QEMU (等待 GDB)
make qemu-gdb

# 終端 2：連接 GDB
riscv64-unknown-elf-gdb kernel kernel
(gdb) target remote localhost:26000
(gdb) break main
(gdb) continue
```

## MIT 6.1810 作業

MIT 的作業通常包括：

| 作業 | 內容 |
|------|------|
| Lab 1 | 孵化 (uthread) - 用戶態執行緒切換 |
| Lab 2 | 系統呼叫 (syscall) |
| Lab 3 | 頁表 (pgtbl) - 虛擬記憶體 |
| Lab 4 | 陷阱 (traps) - 中斷處理 |
| Lab 5 | 鎖 (locks) - 並行控制 |
| Lab 6 | 檔案系統 (fs) |
| Lab 7 | 網路驅動 (networking) |

## 設計特點

### 簡潔性
- 程式碼總量約 10,000 行
- 清晰的目錄結構
- 詳細的程式碼註解

### 教育價值
- 完整作業系統的最小實現
- 涵蓋核心概念
- 可逐步擴展

### 現代特性
- RISC-V 架構支援
- 對稱多處理 (SMP)
- 虛擬記憶體保護
- 檔案系統日誌（部分版本）

## 與原始 Unix v6 的差異

| 特性 | Unix v6 | xv6 |
|------|---------|-----|
| 架構 | PDP-11 | RISC-V |
| C 標準 | K&R C | ANSI C |
| 記憶體保護 | 無 | 頁表隔離 |
| 多核心 | 無 | SMP |
| 程式碼規模 | ~10K 行 | ~10K 行 |

## 相關資源

- 官方網站：https://pdos.csail.mit.edu/6.1810/
- GitHub：https://github.com/mit-pdos/xv6-riscv
- 書籍：《xv6: a simple, Unix-like teaching operating system》
- 相關概念：[作業系統](作業系統.md)
- 相關概念：[記憶體管理](記憶體管理.md)

## Tags

#作業系統 #Unix #MIT #教學 #RISC-V
