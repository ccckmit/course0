# POSIX 標準

POSIX（Portable Operating System Interface）是 IEEE 制定的作業系統介面標準，旨在確保應用程式在不同 Unix-like 系統間的可攜性。POSIX 定義了作業系統應該提供的系統呼叫、工具和檔案格式。

## POSIX 的歷史

### 起源

1985 年，AT&T 的 Unix 商業化導致了「Unix 戰爭」，各廠商的 Unix 版本相互不相容。為了推動標準化，IEEE 成立了 POSIX 委員會。

1988 年，POSIX.1（IEEE 1003.1）正式發布，定義了 C 語言的作業系統介面。

### 標準家族

| 標準 | 內容 |
|------|------|
| POSIX.1 | C 語言系統介面 |
| POSIX.2 | Shell 和工具 |
| POSIX.3 | 測試方法 |
| POSIX.4 | 即時擴展 |
| POSIX.5 | 語言绑定 |
| POSIX.6 | 安全擴展 |
| POSIX.7 | 框架 |

## 核心概念

### 一切皆檔案

POSIX 的核心哲學：所有資源（檔案、裝置、管道、網路插座）都透過統一的檔案介面操作。

```
/dev/null    - 空裝置
/dev/zero   - 零位元組來源
/dev/random - 隨機數
/dev/tty    - 終端機
```

### 檔案描述符

非負整數，代表已開啟檔案的參照：

```c
#include <fcntl.h>
#include <unistd.h>

int fd = open("file.txt", O_RDONLY);
read(fd, buffer, 1024);
write(fd, buffer, 1024);
close(fd);

// 標準描述符
// 0: stdin
// 1: stdout
// 2: stderr
```

### 程序與身份

```c
#include <unistd.h>
#include <sys/types.h>

pid_t pid = getpid();      // 程序 ID
pid_t ppid = getppid();    // 父程序 ID
uid_t uid = getuid();      // 使用者 ID
gid_t gid = getgid();      // 群組 ID
```

## 檔案操作

### 基本 I/O

```c
#include <fcntl.h>
#include <unistd.h>

// 開啟檔案
int open(const char *pathname, int flags, ...);
// flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_APPEND, O_TRUNC...

int fd = open("file.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);

// 讀寫
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);

char buffer[1024];
ssize_t n = read(fd, buffer, sizeof(buffer));
write(STDOUT_FILENO, buffer, n);

// 檔案位置
off_t lseek(int fd, off_t offset, int whence);
// whence: SEEK_SET, SEEK_CUR, SEEK_END

// 關閉
int close(int fd);
```

### 檔案屬性

```c
#include <sys/stat.h>

struct stat st;
stat("file.txt", &st);  // 跟隨符號連結
lstat("file.txt", &st);  // 不跟隨符號連結

// stat 結構
st.st_mode   // 檔案類型和權限
st.st_size   // 檔案大小
st.st_mtime  // 修改時間
st.st_uid    // 擁有者
st.st_gid    // 擁有群組

// 檔案類型檢查
S_ISREG(st.st_mode)   // 普通檔案
S_ISDIR(st.st_mode)   // 目錄
S_ISLNK(st.st_mode)   // 符號連結
S_ISCHR(st.st_mode)   // 字元裝置
S_ISBLK(st.st_mode)   // 區塊裝置
S_ISFIFO(st.st_mode)  // FIFO/pipe
S_ISSOCK(st.st_mode)  // Socket
```

### 目錄操作

```c
#include <dirent.h>

DIR *dir = opendir("/path/to/dir");
struct dirent *entry;

while ((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
}
closedir(dir);

// 建立目錄
mkdir("/new/dir", 0755);

// 刪除
unlink("file");     // 刪除檔案
rmdir("/empty/dir"); // 刪除空目錄

// 遍歷目錄（遞迴）
int walk_dir(const char *path) {
    DIR *d = opendir(path);
    struct dirent *entry;
    char newpath[PATH_MAX];
    
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;
        
        snprintf(newpath, sizeof(newpath), "%s/%s", path, entry->d_name);
        
        if (entry->d_type == DT_DIR) {
            walk_dir(newpath);
        } else {
            printf("%s\n", newpath);
        }
    }
    closedir(d);
    return 0;
}
```

## 程序管理

### 建立程序

```c
#include <unistd.h>

// fork - 建立新程序
pid_t pid = fork();

if (pid == -1) {
    perror("fork");
    exit(1);
} else if (pid == 0) {
    // 子程序
    execlp("ls", "ls", "-la", NULL);
    perror("execlp");  // 只有 execlp 失敗才執行
    exit(1);
} else {
    // 父程序
    int status;
    waitpid(pid, &status, 0);
}

// vfork - 輕量 fork（已過時）
pid_t pid = vfork();
if (pid == 0) {
    // 子程序在此執行，共享父程序記憶體
    _exit(0);  // 必須用 _exit
}
```

### 執行新程式

```c
#include <unistd.h>

// execl - 傳遞可變參數
execl("/bin/ls", "ls", "-l", NULL);

// execv - 傳遞參數陣列
char *args[] = {"ls", "-l", NULL};
execv("/bin/ls", args);

// execp - 在 PATH 中搜尋
execlp("ls", "ls", "-l", NULL);

// execve - 傳遞環境變數
char *envp[] = {"HOME=/home/user", NULL};
execve("/bin/sh", args, envp);

// 組合：fork + exec
if (fork() == 0) {
    dup2(fd, STDOUT_FILENO);  // 重定向輸出
    close(fd);
    execvp("ls", args);
    perror("execvp");
    _exit(127);
}
```

### 等待程序

```c
#include <sys/wait.h>

// 等待任一子程序結束
int status;
pid_t pid = wait(&status);

// 等待特定程序
pid_t pid = waitpid(child_pid, &status, 0);

// 檢查退出狀態
if (WIFEXITED(status)) {
    printf("Exited with status: %d\n", WEXITSTATUS(status));
}
if (WIFSIGNALED(status)) {
    printf("Killed by signal: %d\n", WTERMSIG(status));
}
if (WIFSTOPPED(status)) {
    printf("Stopped by signal: %d\n", WSTOPSIG(status));
}
```

### 程序終止

```c
#include <stdlib.h>
#include <unistd.h>

exit(0);    // 正常退出，刷新緩衝區，呼叫 atexit
_exit(0);   // 快速退出，不刷新緩衝區
abort();     // 產生 SIGABRT 訊號
raise(SIGTERM);  // 發送訊號給自己
```

## 訊號處理

```c
#include <signal.h>
#include <unistd.h>

// 簡單訊號處理
signal(SIGINT, SIG_IGN);  // 忽略 Ctrl+C

void handler(int sig) {
    // 不建議在此使用 printf，訊號處理中安全的函式有限
    write(STDOUT_FILENO, "Caught SIGINT!\n", 15);
}
signal(SIGINT, handler);

// 可移植的訊號處理（sigaction）
struct sigaction sa;
sa.sa_handler = handler;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;  // 或 SA_RESTART 自動重啟中斷的系統呼叫

sigaction(SIGINT, &sa, NULL);

// 常用訊號
// SIGINT  (2)  - Ctrl+C
// SIGTERM (15) - 終止請求
// SIGKILL (9)  - 強制終止（無法忽略）
// SIGSEGV (11) - 段錯誤
// SIGALRM (14) - 計時器
// SIGCHLD (17) - 子程序狀態變化

// 發送訊號
kill(pid, SIGTERM);

// 計時器
alarm(5);  // 5 秒後產生 SIGALRM
pause();   // 等待訊號
```

## 管道與重定向

### 匿名管道

```c
#include <unistd.h>

int pipefd[2];
pipe(pipefd);  // pipefd[0] 讀, pipefd[1] 寫

if (fork() == 0) {
    close(pipefd[0]);           // 關閉讀端
    dup2(pipefd[1], STDOUT_FILENO);  // 標準輸出重定向到管道
    close(pipefd[1]);
    execlp("ls", "ls", NULL);
    _exit(1);
} else {
    close(pipefd[1]);           // 關閉寫端
    // 從 pipefd[0] 讀取子程序的輸出
    char buffer[1024];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer));
    close(pipefd[0]);
    wait(NULL);
}
```

### FIFO（命名管道）

```c
#include <sys/stat.h>

// 建立 FIFO
mkfifo("/tmp/myfifo", 0666);

// 寫入
int fd = open("/tmp/myfifo", O_WRONLY);
write(fd, "Hello", 5);
close(fd);

// 讀取
int fd = open("/tmp/myfifo", O_RDONLY);
char buffer[1024];
read(fd, buffer, sizeof(buffer));
close(fd);
```

### 重定向

```c
#include <fcntl.h>

// 開啟檔案
int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

// 重定向標準輸出
int saved_stdout = dup(STDOUT_FILENO);
dup2(fd, STDOUT_FILENO);

// 此時所有 stdout 輸出都寫入檔案
printf("This goes to file\n");

// 恢復標準輸出
dup2(saved_stdout, STDOUT_FILENO);
close(saved_stdout);
close(fd);
```

## 記憶體映射

```c
#include <sys/mman.h>
#include <fcntl.h>

// 映射檔案到記憶體
int fd = open("file.dat", O_RDWR);
size_t size = 4096;

void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                  MAP_SHARED, fd, 0);

if (addr == MAP_FAILED) {
    perror("mmap");
    exit(1);
}

// 使用記憶體
memcpy(addr, "Hello", 5);
msync(addr, size, MS_SYNC);  // 同步到磁碟

// 解除映射
munmap(addr, size);
close(fd);
```

## IPC（行程間通訊）

### 訊號量

```c
#include <sys/sem.h>

// 建立訊號量集合
int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);

// 初始化
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} arg;
arg.val = 1;
semctl(semid, 0, SETVAL, arg);

// P 操作（等待）
struct sembuf p = {0, -1, 0};
semop(semid, &p, 1);

// V 操作（信號）
struct sembuf v = {0, 1, 0};
semop(semid, &v, 1);

// 刪除
semctl(semid, 0, IPC_RMID);
```

### 共享記憶體

```c
#include <sys/shm.h>

// 建立共享記憶體
int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);

// 附加到行程
void *shmaddr = shmat(shmid, NULL, 0);

// 使用
strcpy(shmaddr, "Hello, shared memory!");

// 脫離
shmdt(shmaddr);

// 控制
struct shmid_ds buf;
shmctl(shmid, IPC_STAT, &buf);
shmctl(shmid, IPC_RMID, NULL);  // 刪除
```

### 訊息佇列

```c
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[1024];
};

// 建立
int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

// 發送
struct msgbuf msg;
msg.mtype = 1;
strcpy(msg.mtext, "Hello");
msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0);

// 接收
struct msgbuf rcv;
msgrcv(msqid, &rcv, sizeof(rcv.mtext), 1, 0);

// 刪除
msgctl(msqid, IPC_RMID, NULL);
```

## 時間

```c
#include <time.h>

// 取得目前時間
time_t now = time(NULL);
printf("%s", ctime(&now));

// 時間結構
struct tm *tm = localtime(&now);
printf("%04d-%02d-%02d %02d:%02d:%02d\n",
       tm->tm_year + 1900,
       tm->tm_mon + 1,
       tm->tm_mday,
       tm->tm_hour,
       tm->tm_min,
       tm->tm_sec);

// 計時
struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC, &start);

// 工作
// ...

clock_gettime(CLOCK_MONOTONIC, &end);
double elapsed = (end.tv_sec - start.tv_sec) +
                 (end.tv_nsec - start.tv_nsec) / 1e9;
```

## POSIX 相容性

### Linux

```bash
# 檢查 POSIX 相容性
getconf POSIX_VERSION
# 輸出: 200112

# GNU Coreutils 支援 POSIX
# 但擴展了 GNU 特有選項（如 --help）
```

### macOS

```bash
# macOS 基本 POSIX 相容
# 但部分特性不同：
# - BSD 特有的指令選項
# - 較舊的 BSD 特性
```

### 撰寫可攜腳本

```bash
#!/bin/sh  # 使用 sh 而非 bash
# 避免使用 Bash 特有語法

# 避免
# - [[ ]]  (使用 [ ] )
# - $( )   (可使用 ` `)
# - arrays (Bash 陣列)
# - declare (Bash 內建)
# - source  (使用 . )
```

## 相關主題

- [Unix](Unix.md) - POSIX 的作業系統基礎
- [Linux系統呼叫](Linux系統呼叫.md) - 系統呼叫介面
- [行程與執行緒](行程與執行緒.md) - 程序管理
