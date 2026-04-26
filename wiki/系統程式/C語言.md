# C語言

C 語言是一種高效、低階的程式語言，由 Dennis Ritchie 於 1972 年在貝爾實驗室開發。C 語言最初是為了開發 Unix 作業系統而設計，如今已成為系統程式設計、嵌入式開發、編譯器設計等領域的標準語言。C 語言結合了高階語言的可讀性與低階語言的硬體控制能力，是少數能夠直接操作記憶體的語言之一。

## C 語言特性

### 核心優勢

```
┌─────────────────────────────────────────────────────────────────┐
│                      C 語言核心特性                            │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  • 低階存取                                                   │
│    - 直接記憶體操作指標                                      │
│    - 位元運算                                                │
│    - 記憶體位址操作                                          │
│                                                              │
│  • 高效                                                       │
│    - 接近硬體的執行效率                                      │
│    - 最小的執行階段                                          │
│    - 指標操作零開銷                                          │
│                                                              │
│  • 可移植                                                     │
│    - 標準庫確保可移植性                                      │
│    - 幾乎所有平台都有 C 編譯器                              │
│    - 硬體抽象層                                              │
│                                                              │
│  • 簡潔                                                       │
│    - 語法簡單，關鍵字少                                      │
│    - 允許低層級操作                                          │
│    - 給程式設計師充分控制權                                  │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

### 設計哲學

C 語言的設計遵循「信任程式設計師」的原則：

```c
// C 語言不阻止你做錯誤的事
// 這是特點，不是缺點

// 危險但有效的操作
int *p = (int *)0x1000;  // 直接指標
*p = 42;                  // 寫入任意記憶體

// 未定義行為
int arr[10];
arr[10] = 0;             // 陣列越界 - 不會被攔截

// 類型別名
void *ptr;               // 泛型指標
```

## 基本語法

### Hello World

```c
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}
```

### 資料類型

```c
// 基本類型
char c = 'A';           // 字元 (1 位元組)
int i = 42;             // 整數 (通常 4 位元組)
float f = 3.14f;       // 單精度浮點數
double d = 3.14159;    // 雙精度浮點數

// 修飾符
short s = 32767;       // 短整數
long l = 2147483647L;  // 長整數
long long ll = 1LL;    // 長長整數
unsigned int u = 42U; // 無號整數
signed int si = -10;  // 有號整數

// 陣列
int arr[10];           // 10 個整數的陣列
char str[] = "Hello";  // 字串

// 指標
int *ptr;              // 整數指標
int **pptr;           // 指標的指標
int (*func)();        // 函式指標

// 結構
struct Point {
    int x;
    int y;
};

// 聯合
union Data {
    int i;
    float f;
    char c;
};

// 列舉
enum Color { RED, GREEN, BLUE };
```

### 控制流程

```c
// 條件
if (x > 0) {
    printf("正數\n");
} else if (x < 0) {
    printf("負數\n");
} else {
    printf("零\n");
}

// 三元運算子
int max = (a > b) ? a : b;

// 迴圈
for (int i = 0; i < 10; i++) {
    printf("%d\n", i);
}

while (condition) {
    // 循環
}

do {
    // 至少執行一次
} while (condition);

// 選擇
switch (value) {
    case 1:
        printf("一\n");
        break;
    case 2:
        printf("二\n");
        break;
    default:
        printf("其他\n");
}
```

### 函式

```c
// 基本函式
int add(int a, int b) {
    return a + b;
}

// 指標參數
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 可變參數
#include <stdarg.h>
int sum(int count, ...) {
    va_list args;
    va_start(args, count);
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);
    }
    va_end(args);
    return total;
}

// 遞迴
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
```

## 指標

指標是 C 語言的核心概念：

```c
int x = 10;
int *p = &x;   // p 指向 x
*p = 20;       // 修改 x 的值

// 指標運算
int arr[] = {1, 2, 3, 4, 5};
int *ptr = arr;
ptr++;         // 指標移動
*ptr = 20;     // arr[1] = 20

// 指標與陣列
int *p = arr;
p[0] == arr[0];  // 等價
*(p + 2) == arr[2];

// 函式指標
int (*func)(int, int) = add;
int result = func(1, 2);

// 回呼函式
void process(int (*callback)(int)) {
    int r = callback(10);
}

// void 指標
void *memcpy(void *dest, const void *src, size_t n);
```

### 指標與記憶體

```
┌─────────────────────────────────────────────────────────────────┐
│                      指標類型                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  int *           整數指標                                    │
│  char *          字元指標 (字串)                              │
│  void *          泛型指標 (任意類型)                          │
│  int **          指標的指標                                  │
│  int (*)[10]     指標 (10 元素陣列)                          │
│  int *(*)()      指標 (返回 int 的函式)                      │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

## 記憶體配置

```c
// 靜態配置
int arr[100];              // 堆疊上的陣列
static int static_arr[100]; // 靜態記憶體

// 動態配置
int *p = malloc(100 * sizeof(int));  // 堆積配置
free(p);                               // 釋放記憶體

// 配置記憶體失敗檢查
int *p = malloc(size);
if (p == NULL) {
    fprintf(stderr, "記憶體配置失敗\n");
    exit(1);
}

// 常見模式
// 配置並初始化
int *p = calloc(n, sizeof(int));  // 配置並歸零

// 重新配置
int *new_p = realloc(p, new_size);

// 配置結構
struct Node *node = malloc(sizeof(struct Node));
node->data = 0;
node->next = NULL;
```

### 記憶體配置函式

```c
#include <stdlib.h>

// 配置記憶體
void *malloc(size_t size);          // 配置未初始化記憶體
void *calloc(size_t nmemb, size_t size);  // 配置並歸零
void *realloc(void *ptr, size_t size);    // 重新配置

// 釋放記憶體
void free(void *ptr);

// 記憶體配置失敗處理
void *malloc(size_t size);  // 若失敗返回 NULL
```

## 前置處理器

```c
// 巨集
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define DEBUG

// 條件編譯
#ifdef DEBUG
    printf("Debug: x = %d\n", x);
#endif

#ifndef CONFIG_H
#define CONFIG_H
#endif

#if defined(UNIX)
    // Unix 特定程式碼
#elif defined(WINDOWS)
    // Windows 特定程式碼
#endif

// 包含防護
#ifndef HEADER_H
#define HEADER_H
// 內容
#endif
```

## 標準函式庫

```c
#include <stdio.h>      // 輸入/輸出
#include <stdlib.h>     // 記憶體配置、程序控制
#include <string.h>     // 字串操作
#include <math.h>       // 數學函式
#include <time.h>       // 時間日期
#include <ctype.h>      // 字元分類
#include <errno.h>      // 錯誤處理
#include <limits.h>     // 類型限制
#include <stdarg.h>     // 可變參數
#include <stddef.h>     // 標準定義
```

### 常見函式

```c
// printf/scanf
printf("Format: %d, %s, %.2f\n", i, str, f);
scanf("%d", &i);

// 字串
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);

// 檔案
FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

## 結構與聯合

```c
// 結構
struct Point {
    int x;
    int y;
};

struct Point p1 = {10, 20};
struct Point *pp = &p1;
pp->x = 30;

// 位元欄位
struct Flags {
    unsigned int enabled : 1;
    unsigned int visible : 1;
    unsigned int mode : 2;
};

// 聯合
union Data {
    int i;
    float f;
    char c;
};

union Data d;
d.i = 42;   // 現在是整數
d.f = 3.14; // 現在是浮點數
```

## 常見模式

### 鏈結串列

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *create_node(int data) {
    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

void insert(Node **head, int data) {
    Node *new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}
```

### 雜湊表

```c
typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

typedef struct HashTable {
    Entry **buckets;
    size_t size;
} HashTable;

unsigned int hash(const char *key, size_t size) {
    unsigned int h = 0;
    while (*key) {
        h = h * 31 + *key++;
    }
    return h % size;
}
```

## 與其他語言的比較

| 特性 | C | C++ | Java | Python |
|------|-----|-----|------|--------|
| 記憶體管理 | 手動 | 手動/智慧指標 | GC | GC |
| 指標 | 有 | 有 | 無 | 無 |
| 類別 | 無 | 有 | 有 | 有 |
| 標準範本 | 無 | STL | JDK | 標準庫 |
| 執行階段 | 無 | 無 | JVM | 直譯器 |

## 相關概念

- [編譯器](編譯器.md) - C 語言編譯過程
- [組合語言](組合語言.md) - 低階語言
- [Dennis_Ritchie](Dennis_Ritchie.md) - C 語言發明者
- [作業系統](作業系統.md) - Unix 作業系統