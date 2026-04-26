# Dennis Ritchie

Dennis MacAlistair Ritchie（1941 年 9 月 9 日－2011 年 10 月 12 日），美國電腦科學家，C 語言與 Unix 作業系統的共同發明者，被譽為「C 語言之父」與「Unix 之父」之一。他的貢獻奠定了現代軟體開發與系統程式設計的基礎，影響了數十年的電腦科學發展。

## 早年生活

### 求學時期

Ritchie 出生於紐約 Bronxville，父親是系統分析師。他從小就對數學與科學產生興趣，1963 年從哈佛大學物理系畢業，1968 年在哈佛大學獲得應用數學博士學位。

### 貝爾實驗室時期

博士畢業後，Ritchie 加入貝爾實驗室（Bell Labs），在此度過了大部分職業生涯。他的父親也在貝爾實驗室的子公司工作。

```
貝爾實驗室時期：
- 1968-1996 貝爾實驗室
- 1996-2007 Lucent Technologies
- 2007-2011 萊斯大學
```

## C 語言

### 發展歷程

C 語言由 Dennis Ritchie 在 1972 年開發，源自 Ken Thompson 的 B 語言：

```
時間線：
- 1969 B 語言 (Ken Thompson)
- 1972 C 語言 (Dennis Ritchie)
- 1978 K&R C (Kernighan & Ritchie)
- 1989 ANSI C (C89)
- 1999 ISO C (C99)
- 2011 ISO C (C11)
- 2018 ISO C (C17)
```

### 設計哲學

```c
// C 語言核心設計原則
// 
// 1. 低階訪問 + 高階抽象
//    -指標操作記憶體
//    -結構化控制流程
//
// 2. 親硬體
//    - char, int, float
//    - 直接記憶體操作
//
// 3. 表達力
//    - 簡潔語法
//    - 豐富運算子
//
// 4. 可移植性
//    - 標準庫抽象
//    - 編譯器產出
```

### 經典範例

```c
// Hello World (K&R C)
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Hello, world!\n");
    return 0;
}

// 指標操作
int arr[] = {1, 2, 3, 4, 5};
int *p = arr;
for (int i = 0; i < 5; i++) {
    printf("%d\n", *(p + i));
}

// 結構
struct point {
    int x;
    int y;
};

struct point p = {10, 20};
printf("%d, %d\n", p.x, p.y);
```

### 影響力

```
C 語言的影響：
- 作業系統：Unix, Linux, Windows core
- 語言：C++, Java, C#, Go, Rust
- 系統程式：編譯器, 資料庫, 網路
- 嵌入式：MCU, RTOS
```

## Unix 作業系統

### 與 Ken Thompson 的合作

Unix 是 Ritchie 與 Ken Thompson 共同開發的作業系統：

```
發展歷程：
- 1969 Thompson 在 PDP-7 上開發 Unix 原型
- 1973 Ritchie 用 C 重寫 Unix
- 1974 Unix 第4版發布
- 1975 Unix 第6版發布 (開放授權)
- 1979 Unix 第7版發布 (最後的Bell Labs版本)
```

### 可移植性突破

```c
// Unix 可移植性的關鍵
// 
// 1. C 語言
//    - 硬體抽象
//    - 編譯器產出
//
// 2. 標準化介面
//    - 系統呼叫
//    - 檔案系統
//    - 標準 IO
//
// 3. UNIX Philosophy
//    - 小型工具
//    - 組合使用
//    - 文字界面
```

### 核心貢獻

```c
// Ritchie 在 Unix 的貢獻
//
// 1. C 語言實現
//    - 用 C 重寫核心
//    - 可移植性
//
// 2. 標準庫
//    - stdio
//    - string
//    - memory
//
// 3. 工具開發
//    - cat, ls, echo
//    - grep, sed, awk
```

## 其他貢獻

### 程式設計語言

Ritchie 還參與了其他語言的開發：

| 語言 | 年份 | 貢獻 |
|------|------|------|
| B | 1969 | 協助 Thompson |
| C | 1972 | 原創 |
| Limbo | 1995 | 協助開發 |
| Go | 2009 | 顧問 |

### 程式設計工具

```
 Ritchie 開發的經典工具：
- C 編譯器 (cc)
- 標準 I/O 庫 (stdio)
- make 工具
- Yacc (Parser Generator)
```

## 獲獎與榮譽

| 年份 | 獎項 |
|------|------|
| 1983 | IEEE Emmanuel R. Piore Medal |
| 1983 | ACM 圖靈獎 (與 Ken Thompson) |
| 1992 | ACM Fellow |
| 1994 | 全國技術獎章 (與 Ken Thompson) |
| 1999 | 電話先驅獎 (Thomson Prize) |
| 2011 | 貝爾實驗室院士 |

```
圖靈獎頒發理由：
"for their development of generic operating system
 concepts underlying UNIX"
```

## 遺產

### C 語言標準

```c
// C 語言標準化過程
// ANSI X3J11 -> ISO/IEC 9899
// C89, C99, C11, C17, C23

// 核心概念：
// - 靜態型別
// - 指標
// - 記憶體管理
// - 組合性
```

### Unix 哲學

```
Unix 哲學原則：
1. 小型工具
2. 每個工具做一件事
3. 輸出可作為輸入
4. 文字界面
5. 組合使用
```

### 遺傳程式

```
 Ritchie 的精神遺產：
- C 語言教學
- Unix 哲學
- 開放原始碼
- 簡潔設計
```

## 個人特質

### 工作風格

```
 Ritchie 的特點：
- 謙遜低調
- 注重實用
- 深思熟慮
- 幽默感
- 遠離媒體
```

### 生活

```
興趣愛好：
- 古典音樂
- 閱讀
- 帆船運動
- 烹飪
```

## 逝世與紀念

2011 年 10 月 12 日，Ritchie 在新澤西州 Berkeley Heights 的家中逝世，享年 70 歲。死因是前列腺癌引發的心衰竭。

```
紀念：
- C 語言標準化組織以他命名獎項
- 貝爾實驗室設立 Ritchie Prize
- 眾多編譯器紀念他
```

## 相關概念

- [ C語言](C語言.md) - C 語言
- [ 編譯器](編譯器.md) - 編譯器
- [ 作業系統](作業系統.md) - 作業系統
- [ Ken_Thompson](Ken_Thompson.md) - Unix 共同發明者
- [ Linux系統呼叫](Linux系統呼叫.md) - Linux 系統呼叫介面