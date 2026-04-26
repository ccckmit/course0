# Unix Version 6 (Unix v6)

## 概述

Unix Version 6（簡稱 Unix v6 或 Sixth Edition Unix）於 1975 年發布，是首個廣泛被學術機構和企業採用的 Unix 版本。它是貝爾實驗室（Bell Labs）釋出的第六個版本，也是首個在 MIT 許可證下分發的版本，從而成為現代 Unix 系統的真正起點。Unix v6 的設計哲學和技術架構深刻影響了後續的 Unix 衍生系統，包括 BSD、System V，以及最終的 Linux。

## 歷史背景

### 發展歷程

```
Unix v6 發展時間線：

1969    Thompson 在 PDP-7 上開發 Unix 原型
        名稱"Unified"諧音 "UNICS"

1971    Unix v1 發布（PDP-11）
        主要為貝爾實驗室內部使用

1972    Unix v2 發布
        加入 pipe 機制

1973    Unix v3 發布
        Ritchie 用 C 重寫核心

1974    Unix v4 發布
        首次向外部機構分發

1975    Unix v6 發布 ★
        首次開源，MIT 許可證
        广泛传播至大学

1977    Unix v7 發布
        最後一個貝爾實驗室版本
        包含 Bourne Shell
```

### 為何如此重要

Unix v6 是第一個真正意義上的「開源」作業系統。在此之前，作業系統通常是商業專有軟體。Unix v6 的原始碼以極低的價格向大學分發，使得大學和研究機構能夠深入學習、研究和修改這個系統。這種開放策略催生了：

1. **學術研究**：無數作業系統研究基於 Unix v6
2. **BSD 誕生**：加州大學伯克利分校基於 v6 發展出 BSD
3. ** POSIX 標準**：POSIX 標準深受 Unix v6 設計影響
4. ** Linux 起源**：Linux 最初模擬 Unix v6 的 API

## 系統架構

### 核心组件

```
┌─────────────────────────────────────────────────────────────┐
│                      Unix v6 架構                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐     │
│   │   user      │   │   user      │   │   user      │     │
│   │  process    │   │  process    │   │  process    │     │
│   └──────┬──────┘   └──────┬──────┘   └──────┬──────┘     │
│          │                 │                 │             │
├──────────┴─────────────────┴─────────────────┴────────────┤
│                     系統呼叫介面                             │
│        (read, write, fork, exec, wait, etc.)              │
├─────────────────────────────────────────────────────────────┤
│                      核心空間                               │
│   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│   │  file    │  │   proc   │  │   pipe   │  │  inode  │  │
│   │ subsystem│  │ manager  │  │  buffer  │  │ cache   │  │
│   └──────────┘  └──────────┘  └──────────┘  └──────────┘  │
├─────────────────────────────────────────────────────────────┤
│                      設備驅動                               │
│    ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐              │
│    │  RK05│  │  tty │  │  LP  │  │  null │              │
│    └──────┘  └──────┘  └──────┘  └──────┘              │
├─────────────────────────────────────────────────────────────┤
│                     硬體抽象層                              │
│                    PDP-11 硬體                             │
└─────────────────────────────────────────────────────────────┘
```

### 核心特性

```c
// Unix v6 核心數據結構

// 1. inode 結構 - 文件系統核心
struct inode {
    ushort i_mode;      // 文件類型+權限
    short  i_uid;       // 用戶 ID
    off_t  i_size;     // 文件大小
    ushort i_addr[8];  // 數據塊地址（直接+間接）
    ushort i_dev;      // 設備號
    ushort i_num;      // inode 編號
    short  i_count;    // 引用計數
    time_t i_mtime;    // 修改時間
    // ... 其他欄位
};

// 2. proc 結構 - 行程結構
struct proc {
    char    p_stat;      // 行程狀態
    char    p_flag;     // 標誌
    char    p_pri;     // 優先級
    char    p_sig;     // 信號
    struct  proc *p_link;  // 運行隊列鏈接
    int     p_time;    // 運行時間
    int     p_cpu;     // CPU 使用量
    int     p_nice;    // Nice 值
    ushort  p_ttyp;    // 控制終端
    ushort  p_pid;     // 行程 ID
    ushort  p_ppid;    // 父行程 ID
    ushort  p_addr[4]; // 用戶空間映射
    ushort  p_size;    // 記憶體大小
    // ...
};

// 3. file 結構 - 打開的文件
struct file {
    char    f_flag;      // 標誌
    char    f_count;    // 引用計數
    struct  inode *f_inode;  // 關聯的 inode
    off_t   f_offset;   // 文件偏移
};
```

### 核心系統呼叫

```c
// Unix v6 主要系統呼叫

// 1. fork - 創建新行程
int fork() {
    register struct proc *p;
    
    // 分配新 proc 結構
    for (p = &proc[0]; p < &proc[NPROC]; p++)
        if (p->p_stat == 0)
            goto found;
    return -1;  // 無可用行程
    
found:
    // 複製當前行程
    p->p_stat = SRUN;
    p->p_pid = ++pid;
    p->p_ppid = u.u_procp->p_pid;
    p->p_pri = u.u_procp->p_pri;
    p->p_cpu = 0;
    p->p_nice = NZERO;
    
    // 複製用戶空間
    copyseg(0, p->p_addr, u.u_dsize);
    
    // 複製 kernel 棧
    // ... 省略
    
    // 設置返回值
    u.u_rval1 = p->p_pid;
    u.u_rval0 = 0;
    
    return 0;
}

// 2. read - 讀取數據
int read(dev, buf, count)
int dev, count;
char *buf;
{
    register struct file *fp;
    register struct inode *ip;
    register int off;
    
    fp = getf(fd);
    ip = fp->f_inode;
    
    // 檢查是否可讀
    if ((fp->f_flag & FREAD) == 0)
        return -1;
    
    // 根據 inode 類型處理
    if ((ip->i_mode & IFMT) == IFDIR)
        return readi(ip, buf, count);
    
    // 讀取數據
    return readi(ip, buf, count);
}

// 3. exec - 執行程序
int exec(name, args)
char *name;
char **args;
{
    register struct inode *ip;
    register char *cp;
    struct exec ex;
    
    // 讀取可執行文件頭
    ip = namei(0);
    if (ip == 0)
        return -1;
    
    // 讀取 header
    readi(ip, (char *)&ex, sizeof(ex));
    
    // 檢查魔數
    if (ex.a_magic != 0410 && ex.a_magic != 0411)
        return -1;
    
    // 分配內存
    // ... 省略
    
    // 載入程序
    // ... 省略
    
    // 設置新的 u 結構
    // ...
}
```

## 檔案系統

### v6 檔案系統結構

```
┌─────────────────────────────────────────────────────────────────┐
│                   Unix v6 檔案系統布局                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  超級塊 (superblock)    存放於 block 1                         │
│  ┌─────────────────────┐                                        │
│  │ s_isize        : i節點表大小                                │
│  │ s_fsize       : 數據塊總數                                  │
│  │ s_nfree       : 空閒塊數量                                  │
│  │ s_free[100]   : 空閒塊號                                    │
│  │ s_fmod        : 修改標誌                                    │
│  └─────────────────────┘                                        │
│                                                                 │
│  i節點區 (inode area)    從 block 2 開始                       │
│  ┌──────┬──────┬──────┬──────┬──────┬──────┐                │
│  │ inode│ inode│ inode│ inode│ inode│ ...  │                │
│  │  0   │  1   │  2   │  3   │  4   │      │                │
│  │ root │ dev  │ /etc │ /bin │ /usr│      │                │
│  └──────┴──────┴──────┴──────┴──────┴──────┘                │
│                                                                 │
│  數據區 (data area)                                             │
│  ┌──────────────────────────────────────────────────┐        │
│  │  目錄塊    │    目錄塊     │   數據塊   │  數據塊   │        │
│  │ /dev/tty0 │   /etc/passwd │   file1.c │  file2.c │        │
│  └──────────────────────────────────────────────────┘        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 目錄結構

```c
// v6 目錄項結構（16 bytes）
struct direct {
    ushort d_ino;        // inode 編號
    char d_name[14];     // 文件名（14字符）
};

// 目錄範例：
// .  -> inode 2 (自身)
// .. -> inode 2 (父目錄)
// bin -> inode 3 (可執行文件)
// etc -> inode 4 (配置)
// dev -> inode 5 (設備)
// usr -> inode 6 (用戶目錄)

// 系統目錄
// /              - root inode 2
// /dev/         - 設備文件
// /etc/         - 系統配置
// /bin/         - 可執行文件
// /usr/         - 用戶目錄
// /usr/bin/     - 用戶程序
// /tmp/         - 臨時文件
```

### 檔案系統 API

```c
// 文件操作示例

// 打開文件
int fd = open("/etc/passwd", O_RDONLY);

// 讀取
char buf[256];
int n = read(fd, buf, sizeof(buf));

// 寫入
int fd = open("/tmp/test", O_WRONLY | O_CREAT);
write(fd, "hello", 5);

// 創建目錄
mkdir("/newdir", 0755);

// 創建文件
creat("/newfile", 0644);

// 文件偏移
lseek(fd, 100, SEEK_SET);
```

## 行程管理

### 行程狀態

```c
// v6 行程狀態定義
enum {
    SSLEEP = 1,   // 睡眠（高優先級）
    SWAIT = 2,    // 睡眠（可中斷）
    SRUN = 3,     // 運行
    SIDL = 4,     // 過渡狀態
    SSTOP = 5,    // 停止
    SZOMB = 6     // 僵屍
};

// 調度演算法
// 簡單的輪轉調度
// 每個 time slice 為 1 tick (約 1/60 秒)
// 優先級動態調整
```

### Shell 命令

```c
// v6 Shell (sh) 支持的命令
// 內建命令：
// cd, echo, login, pwd, set, shift, wait
//
// 外部程序：
// ls, cp, mv, rm, cat, ed, grep, sed
// awk, make, ar, nm, ld
// cc, as
//
// 使用示例：
// $ ls -l
// $ cat file | grep pattern
// $ make
```

## 設備驅動

### 設備類型

```
字符設備：
- /dev/tty    : 控制台終端
- /dev/tty[0-7]: 虛擬終端
- /dev/lp     : 行式打印機
- /dev/null   : 空設備
- /dev/pty*   : 偽終端

區塊設備：
- /dev/rk0    : RK05 磁碟
- /dev/tm0    : 磁帶機
- /dev/hp0    : 硬碟
```

### 設備驅動結構

```c
// 設備驅動示例 - 終端驅動
struct cdevsw {
    int (*d_open)();
    int (*d_close)();
    int (*d_read)();
    int (*d_write)();
    int (*d_sgtty)();
};

// 主要設備表
struct cdevsw cdevsw[NCHRDEV];

// 設備打開
int ttyopen(dev, flag) {
    register struct tty *tp;
    tp = &crtty[minor(dev)];
    tp->t_stat = 0;
    return 0;
}

// 設備讀取
int ttyread(dev) {
    register struct tty *tp;
    tp = &crtty[minor(dev)];
    // 讀取字符...
    passc(tp->t_delct ? tp->t_char : getc(tp->t_inq));
}
```

## 傳承與影響

### 對後續系統的影響

| 系統 | 繼承特點 |
|------|----------|
| BSD | 源自 Unix v6, v7 |
| System V | 貝爾實驗室商業版本 |
| Linux | API 模擬 Unix v6 |
| Minix | 教學用途的 Unix-like |
| macOS | 基於 BSD + Mach |

### 技術特點

1. **可移植性**：用 C 編寫，硬體抽象好
2. **簡潔性**：核心僅約 10,000 行
3. **管道**：進程間通信的革命性創新
4. **層次目錄**：統一的文件系統視角
5. **設備即文件**：統一的 I/O 介面

## 原始碼結構

```
Unix v6 原始碼組織：

/usr/source/
    s1/         - 設備驅動
        tty.c   - 終端驅動
        rk.c    - 磁碟驅動
        lp.c    - 打印機驅動
        
    s2/         - 核心代碼
        main.c  - 啟動
        pipe.c  - 管道
        plock.c - 鎖
        misc.c  - 雜項
        
    s3/         - 文件系統
        alloc.c - 空間分配
        ialloc.c - inode 分配
        fsize.c - 空間計算
        
    s4/         - 調度與行程
        clock.c - 時鐘中斷
        sleep.c - 睡眠/喚醒
        proc.c  - 行程管理
        
    s5/         - 系統呼叫
        sys1.c  - 讀寫
        sys2.c  - fork, exec
        sys3.c  - 錯誤處理
        
    s6/         - 實用程序
        init/   - 初始化
        login/  - 登入
        sh/     - Shell
        cat/    - 顯示
        ls/     - 列目錄
```

## 學習價值

### 為何學習 Unix v6

1. **理解核心概念**：檔案系統、行程管理、設備驅動
2. **簡潔的代碼**：核心只有 10K 行，易於閱讀理解
3. **歷史價值**：了解現代作業系統的起源
4. **啟發設計**：Unix 哲學源於此處

### 實踐資源

```bash
# 模擬 Unix v6 環境
# 1. 使用 PDP-11 模擬器
git clone https://github.com/DoctorWkt/pdp7-unix.git

# 2. 在 SIMH 中運行
sim> set cpu 11/20
sim> set rk0 enabled
sim> boot rk0

# 3. 或者使用 v6 相容層
# https://github.com/rswier/c4 - C 編譯器
# https://github.com/plan9-from-bell-labs/unix - 原始碼
```

### 關鍵源文件

```c
// 核心文件學習路徑
// 1. init/main.c       - 系統啟動
// 2. fs/ alloc.c       - 檔案系統空間管理
// 3. fs/ ialloc.c      - inode 分配
// 4. proc/ proc.c     - 行程排程
// 5. conf.c           - 配置表
// 6. pipe.c           - 管道實現
```

## 與現代 Unix 的比較

| 特性 | Unix v6 | 現代 Unix |
|------|---------|-----------|
| 核心大小 | ~10K 行 | 數百萬行 |
| 支援 CPU | PDP-11 | 多種架構 |
| 文件系統 | 簡單 Unix FS | UFS/ZFS/ext4 |
| 網路 | 無 | TCP/IP |
| 多工 | 協作式 | 搶佔式 |
| 內存管理 | 簡單分頁 | 虛擬記憶體 |
| 用戶數 | 單用戶 | 多用戶 |

## 相關概念

- [xv6](xv6.md) - Unix v6 的現代 RISC-V 重實現
- [Ken_Thompson](Ken_Thompson.md) - Unix v6 發明者
- [Dennis_Ritchie](Dennis_Ritchie.md) - C 語言與 Unix 發明者
- [作業系統](作業系統.md) - 作業系統基礎

## Tags

#Unix #Unixv6 #PDP-11 #BellLabs #作業系統 #經典系統