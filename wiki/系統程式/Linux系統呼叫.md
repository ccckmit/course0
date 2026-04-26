# Linux系統呼叫

Linux 系統呼叫是使用者空間應用程式與 Linux 核心溝通的介面，提供了作業系統提供給使用者程式的最基本服務。透過系統呼叫，程式可以請求核心執行特權操作，如檔案存取、行程管理、網路通訊、記憶體配置等。了解系統呼叫是理解作業系統運作原理的關鍵，也是系統程式設計的基礎。

## 系統呼叫的運作原理

### 使用者空間與核心空間

Linux 系統採用保護模式，將記憶體分為使用者空間和核心空間：

```
┌─────────────────────────────────────────────────────────────────┐
│                      記憶體空間                                │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  使用者空間 (User Space)                                      │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                                                         │  │
│  │  使用者程式         程式碼、資料、堆疊                   │  │
│  │                                                         │  │
│  └─────────────────────────────────────────────────────────┘  │
│                           │                                    │
│                     系統呼叫介面                              │
│                           │                                    │
│  ────────────────────────┼────────────────────────────────────│
│                           │                                    │
│  核心空間 (Kernel Space)                                        │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                                                         │  │
│  │  Linux Kernel    行程排程、檔案系統、網路、驅動程式       │  │
│  │                                                         │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

### 系統呼叫的執行流程

```c
// 系統呼叫的執行過程：
// 1. 使用者程式呼叫包裝函式（如 read()）
// 2. 包裝函式執行 syscall 指令，觸發軟體中斷
// 3. CPU 切換到核心模式，執行中斷處理常式
// 4. 核心根據系統呼叫號碼分派到對應的處理函式
// 5. 處理函式執行對應的核心功能
// 6. 執行完畢後返回使用者空間
// 7. 包裝函式傳回結果給呼叫者
```

### 系統呼叫號碼

每個系統呼叫都有唯一的編號：

```c
// 在 x86_64 Linux 中
#define __NR_read          0
#define __NR_write         1
#define __NR_open          2
#define __NR_close         3
#define __NR_fork          57
#define __NR_clone         56
#define __NR_execve        59
#define __NR_wait4         61
#define __NR_socket        41
#define __NR_bind         49
#define __NR_listen        50
#define __NR_accept       43

// 系統呼叫表
// arch/x86/entry/syscalls/syscall_64.tbl
```

### 系統呼叫包裝函式

C 標準庫提供了系統呼叫的包裝函式：

```c
// 標準庫包裝
#include <unistd.h>

// 這些函式內部呼叫 syscall
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int open(const char *pathname, int flags, ...);
int close(int fd);

// 直接呼叫 syscall
#include <sys/syscall.h>
syscall(__NR_read, fd, buf, count);
```

## 常見系統呼叫

### 行程管理

行程管理系統呼叫用於建立、管理和終止程序：

```c
#include <unistd.h>
#include <sys/wait.h>

// fork() - 建立新行程
pid_t pid = fork();
if (pid == 0) {
    // 子行程
    printf("我是子行程，PID = %d\n", getpid());
} else if (pid > 0) {
    // 父行程
    printf("我是父行程，子行程 PID = %d\n", pid);
    wait(NULL);  // 等待子行程
} else {
    // fork 失敗
    perror("fork");
}

// exec() - 執行新程式
// 有多個變體：execl, execv, execle, execve, execlp, execvp
char *args[] = {"ls", "-l", NULL};
execve("/bin/ls", args, NULL);  // 直接使用 sys_execve

// _exit() - 終止行程（系統呼叫）
_exit(0);  // 立即終止，不清理

// exit() - 終止程序（標準庫函式）
exit(0);   // 會呼叫 atexit 處理常式、刷新緩衝區

// getpid() - 取得目前行程 ID
pid_t my_pid = getpid();

// getppid() - 取得父行程 ID
pid_t parent_pid = getppid();

// wait() / waitpid() - 等待子行程
int status;
pid_t child = wait(&status);         // 阻塞等待任何子行程
pid_t child = waitpid(pid, &status, 0);  // 等待特定子行程
if (WIFEXITED(status)) {
    printf("子行程正常結束，返回值 = %d\n", WEXITSTATUS(status));
}
```

### 檔案操作

檔案操作系統呼叫提供了基本的檔案存取能力：

```c
#include <fcntl.h>
#include <unistd.h>

// open() - 開啟或建立檔案
int fd = open("file.txt", O_RDONLY);        // 唯讀
int fd = open("file.txt", O_WRONLY|O_CREAT, 0644);  // 寫入，若不存在則建立
int fd = open("file.txt", O_RDWR);          // 讀寫

// open() 旗標
// O_RDONLY, O_WRONLY, O_RDWR
// O_CREAT, O_EXCL, O_TRUNC, O_APPEND
// O_SYNC, O_NONBLOCK, O_DIRECTORY

// read() - 讀取資料
char buffer[1024];
ssize_t n = read(fd, buffer, sizeof(buffer));
if (n > 0) {
    buffer[n] = '\0';
    printf("讀取到 %zd 位元組: %s\n", n, buffer);
}

// write() - 寫入資料
const char *msg = "Hello, World!\n";
ssize_t n = write(fd, msg, strlen(msg));

// lseek() - 移動檔案指標
off_t pos = lseek(fd, 0, SEEK_SET);    // 移到開頭
off_t pos = lseek(fd, 0, SEEK_END);   // 移到結尾
off_t pos = lseek(fd, 10, SEEK_CUR);  // 從目前位置前進 10 位元組

// close() - 關閉檔案
close(fd);

// fstat() - 取得檔案狀態
struct stat st;
fstat(fd, &st);
printf("大小: %ld 位元組\n", st.st_size);

// fsync() - 同步檔案到磁碟
fsync(fd);
```

### 目錄操作

```c
#include <dirent.h>

// opendir() - 開啟目錄
DIR *dir = opendir("/path/to/dir");
if (dir == NULL) {
    perror("opendir");
}

// readdir() - 讀取目錄項目
struct dirent *entry;
while ((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
    // entry->d_type: DT_DIR, DT_REG, DT_LNK 等
}

// closedir() - 關閉目錄
closedir(dir);

// mkdir() - 建立目錄
mkdir("/path/to/newdir", 0755);

// rmdir() - 刪除空目錄
rmdir("/path/to/emptydir");

// getcwd() - 取得目前工作目錄
char cwd[1024];
if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("目前目錄: %s\n", cwd);
}

// chdir() - 改變目前目錄
chdir("/new/path");
```

### 記憶體管理

```c
#include <sys/mman.h>

// brk() / sbrk() - 改變資料段大小（早期方式）
void *old_break = sbrk(1024);  // 增加 1KB

// mmap() - 映射檔案或設備到記憶體
void *addr = mmap(NULL,           // 位址（NULL 表示自動選擇）
                 4096,           // 長度
                 PROT_READ|PROT_WRITE,  // 權限
                 MAP_PRIVATE,   // 映射類型
                 fd,            // 檔案描述符
                 0);            // 偏移量

// munmap() - 解除映射
munmap(addr, 4096);

// mprotect() - 改變記憶體保護
mprotect(addr, 4096, PROT_READ);  // 唯讀

// mlock() / munlock() - 鎖定記憶體（防止換頁）
mlock(addr, 4096);
munlock(addr, 4096);

// madvise() - 記憶體使用提示
madvise(addr, size, MADV_WILLNEED);  // 預期將被使用
madvise(addr, size, MADV_DONTNEED); // 不需要了
```

### 程序間通訊

```c
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>

// 共用記憶體
int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT|0644);
void *shmaddr = shmat(shmid, NULL, 0);
// 使用共用記憶體
shmdt(shmaddr);
shmctl(shmid, IPC_RMID, NULL);

// 訊號佇列
int msgid = msgget(IPC_PRIVATE, IPC_CREAT|0644);
struct msgbuf {
    long mtype;
    char mtext[100];
} msg;
msgsnd(msgid, &msg, sizeof(msg.mtext), 0);
msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
msgctl(msgid, IPC_RMID, NULL);

// 訊號量
int semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0644);
struct sembuf op = {0, -1, 0};  // P 操作
semop(semid, &op, 1);
op.sem_op = 1;                   // V 操作
semop(semid, &op, 1);
semctl(semid, 0, IPC_RMID);
```

### 時間管理

```c
#include <time.h>

// time() - 取得目前時間（秒）
time_t now = time(NULL);
printf("目前時間: %ld\n", now);

// gettimeofday() - 取得高精度時間
struct timeval tv;
gettimeofday(&tv, NULL);
printf("秒: %ld, 微秒: %ld\n", tv.tv_sec, tv.tv_usec);

// clock_gettime() - 取得各種時鐘
struct timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);   // 牆上時鐘
clock_gettime(CLOCK_MONOTONIC, &ts);  // 單調時鐘
clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);  // 程序 CPU 時間

// 睡眠
sleep(1);           // 秒
usleep(100000);     // 微秒（已棄用）
nanosleep(&(struct timespec){0, 100000000}, NULL);  // 奈秒
```

### 使用者與群組

```c
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

// getuid() / getgid() - 取得 ID
uid_t uid = getuid();
gid_t gid = getgid();

// geteuid() / getegid() - 取得有效 ID
uid_t euid = geteuid();
gid_t egid = getegid();

// setuid() / setgid() - 設定 ID（需要權限）
setuid(1000);
setgid(1000);

// 取得使用者資訊
struct passwd *pw = getpwuid(uid);
printf("使用者名稱: %s\n", pw->pw_name);
printf("主目錄: %s\n", pw->pw_dir);

// 取得群組資訊
struct group *gr = getgrgid(gid);
printf("群組名稱: %s\n", gr->gr_name);
```

### 系統資訊

```c
#include <sys/utsname.h>

// uname() - 取得系統資訊
struct utsname name;
uname(&name);
printf("系統名稱: %s\n", name.sysname);
printf("節點名稱: %s\n", name.nodename);
printf("發行版本: %s\n", name.release);
printf("版本號: %s\n", name.version);
printf("硬體: %s\n", name.machine);

// gethostname() - 取得主機名
char hostname[256];
gethostname(hostname, sizeof(hostname));
printf("主機名: %s\n", hostname);
```

## 進階系統呼叫

### 檔案描述符操作

```c
#include <fcntl.h>

// dup() / dup2() - 複製檔案描述符
int new_fd = dup(old_fd);   // 複製到最小可用描述符
dup2(old_fd, new_fd);      // 複製到指定描述符

// fcntl() - 檔案描述符控制
int flags = fcntl(fd, F_GETFL);
fcntl(fd, F_SETFL, flags | O_NONBLOCK);

// select() / poll() - I/O 多工
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd, &readfds);
struct timeval timeout = {1, 0};  // 1 秒
select(fd + 1, &readfds, NULL, NULL, &timeout);

// epoll - 高效能 I/O 多工（Linux 特有）
int epfd = epoll_create1(0);
struct epoll_event ev;
ev.events = EPOLLIN;
ev.data.fd = fd;
epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
epoll_wait(epfd, &ev, 1, -1);  // 阻塞等待
```

### 程序屬性

```c
#include <sched.h>

// sched_setaffinity() - 設定 CPU 亲和性
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(0, &cpuset);    // 只在 CPU 0 執行
sched_setaffinity(0, sizeof(cpuset), &cpuset);

// nice() - 設定程序優先權
nice(10);  // 增加優先權值（降低優先權）

// getpriority() / setpriority() - 取得/設定優先權
int prio = getpriority(PRIO_PROCESS, 0);
setpriority(PRIO_PROCESS, 0, -10);
```

### 核心控制

```c
#include <sys/utsname.h>

// sysinfo() - 取得系統資訊
struct sysinfo si;
sysinfo(&si);
printf("記憶體總量: %ld MB\n", si.totalram / 1024 / 1024);
printf("可用記憶體: %ld MB\n", si.freeram / 1024 / 1024);

// getrlimit() / setrlimit() - 資源限制
struct rlimit limit;
getrlimit(RLIMIT_NOFILE, &limit);
printf("檔案描述符上限: %lu\n", limit.rlim_max);
```

## 錯誤處理

### 錯誤碼

系統呼叫失敗時會設定 errno：

```c
#include <errno.h>
#include <perror.h>

int fd = open("nonexistent", O_RDONLY);
if (fd == -1) {
    perror("open failed");  // 自動輸出錯誤訊息
    // 或
    printf("錯誤: %s\n", strerror(errno));
}

// 常見錯誤碼
// EACCES: 權限不足
// ENOENT: 檔案不存在
// EMFILE: 檔案描述符太多
// ENOMEM: 記憶體不足
// EIO: I/O 錯誤
// EINTR: 系統呼叫被訊號中斷
```

## 相關概念

- [作業系統](作業系統.md) - 作業系統核心概念
- [行程與執行緒](行程與執行緒.md) - 程序與執行緒
- [檔案系統](檔案系統.md) - 檔案系統運作
- [排程](排程.md) - 程序排程
- [網路](網路.md) - 網路通訊