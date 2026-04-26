# Ken Thompson

Kenneth Lane Thompson（1943 年 2 月 4 日－），美國電腦科學家，Unix 作業系統與 B 語言的共同發明者，正則表達式的發明者之一。他被譽為「Unix 之父」之一，與 Dennis Ritchie 共同創造了現代運算的基礎。

## 早年生活

### 求學時期

Thompson 出生於美國紐奧良，父親是空軍電子技師。他在年就展現出對電子與機械的天興趣，1965 年從加州大學柏克萊分校取得電機工程與電腦科學雙學士學位，1966 年取得碩士學位。

### 貝爾實驗室時期

碩士畢業後，Thompson 加入貝爾實驗室（Bell Labs），在此與 Ritchie 共同開創了 Unix 時代：

```
貝爾實驗室時期：
- 1966-1979 Bell Labs
- 1979-1983 Passport Systems
- 1983-1994貝爾實驗室 (再度加入)
- 1994-現在 Google
```

## Unix 作業系統

### 早期開發

1969 年，Thompson 在 PDP-7 迷你電腦上開發了 Unix 的前身：

```
發展歷程：
- 1969 PDP-7 上開發
- 1970 PDP-11 上移植
- 1973 用 C 重寫 (與 Ritchie)
- 1974 發布 Unix 第4版
- 1979 發布 Unix 第7版 (最後的 Bell Labs 版本)
```

### 技術創新

```c
// Unix 的核心創新
//
// 1. 階層檔案系統
//    - 樹狀目錄結構
//    - 設備檔案抽象
//    - stdin/stdout/stderr
//
// 2. 程序管理
//    - fork/exec 模型
//    - 訊號機制
//    - 管道
//
// 3. 文字界面
//    - 終端設備
//    - 標準工具
//    - Shell
//
// 4. 設計哲學
//    - 小型工具
//    - 組合使用
```

### 經典 Unix 工具

```bash
# Thompson 參與開發的工具
ls - list              # 目錄列表
cd - change directory # 改變目錄
pwd - print working dir# 顯示目前目錄
cat - concatenate     # 串接檔案
mv - move            # 移動/改名
cp - copy           # 複製
rm - remove         # 刪除
mkdir - make dir     # 建立目錄

# 文字處理
ed - editor         # 行編輯器
grep - pattern      # 模式匹配
sed - stream edit  # 流編輯器
awk - (Aho, Weinberger, Kernighan)
// 模式掃描與處理語言
```

### Unix 哲學

```
Unix 哲學（Mike Gancarz）：
1. 小就是美
2. 讓每個程式做好一件事
3. 盡快建立原型
4. 選擇便利性而非效率
5. 所有程式都是篩選器

原則擴展：
- 文字資料最有威力
- 可組合性
- 開放延展
- 命令列優先
```

## B 語言

### 語言發展

B 語言是 Thompson 基於 BCPL（Basic Combined Programming Language）簡化而來的：

```
時間線：
- 1969 B 語言開發 (PDP-7)
- 1972 C 語言開發 (Ritchie)
- 1978 K&R C 出版

設計目標：
- 簡潔
- 直譯執行
- 適合系統程式
```

### 語法特點

```c
// B 語言語法
// 
// main() {
//     printf("Hello");
// }

// 特徵：
// - 無類型 (所有值為整數)
// - 無函式原型
// - 指標運算受限
// - 直譯執行
```

## 正則表達式

### 發明正則表達式

1970 年代，Thompson 與同僚發明瞭正則表達式，並實現於 Unix 編輯器 ed 中：

```regex
// 正則表達式基礎
.       任意字元
*       零或多個
+       一或多個 (擴展)
?       零或一 (擴展)
^       行首
$       行尾
[abc]   字元類
[^abc]  否定
[a-z]   範圍
|       或者 (擴展)
()      分組 (擴展)

// 範例
^[A-Z][a-z]+$      # 大寫開頭的英文單字
\d{3}-\d{4}       # 電話號碼
[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}  # Email
```

### 實現影響

```
正則表達式的 Unix 實現：
- ed (編��器)
- grep (模式匹配)
- sed (流編輯器)
- awk (語言)
- vi/vim (編輯器)
- perl (語言)
- JavaScript, Python, ...
```

## Go 語言

### 晚年在 Google

2006 年，Thompson 加入 Google，與 Robert Griesemer、Rob Pike 共同開發 Go 語言：

```
Go 語言設計目標：
- 簡潔
- 安全
- 並行
- 快速編譯
- 網路導向
```

### Go 特色

```go
// Go 語言範例
package main

import "fmt"

func main() {
    ch := make(chan string)
    go func() {
        ch <- "Hello"
    }()
    msg := <-ch
    fmt.Println(msg)
}
```

```go
// Go 核心特性：
// - goroutine (並行)
// - channel (通訊)
// - interface (介面)
// - 垃圾收集
// - 快速編譯
// - 型別推斷
```

## 獲獎與榮譽

| 年份 | 獎項 |
|------|------|
| 1983 | IEEE Emanuel R. Piore Medal |
| 1983 | ACM 圖靈獎 (與 Dennis Ritchie) |
| 1992 | ACM Fellow |
| 1994 | 全國技術獎章 (與 Dennis Ritchie) |
| 1999 | 電話先驅獎 |
| 2011 | 甘迺迪獎 |

```
圖靈獎頒發理由：
"for their development of generic operating system
 concepts underlying UNIX"
```

## 個人特質

### 工作風格

```
 Thompson 的特點：
- 注重實用
- 小型化
- 直接
- 深思熟慮
- 動手實作
```

### 興趣

```
興趣愛好：
- 航海
- 木工
- 園藝
- 業餘電臺
```

## 遺產

### 計算機科學影響

```
 Thompson 的遺產：
- Unix 作業系統標準
- 正則表達式
- B/C 語言系統
- Go 語言
- 文字處理原則
```

### 設計原則

```c
// Thompson 的設計哲學
//
// 1. 簡潔性
//    最少驚訝原則
//    一致性
//
// 2. 可組合性
//    小型構建區塊
//    標準介面
//
// 3. 效率
//    快速啟動
//    低記憶體
//
// 4. 延展性
//    可編程
//    可擴展
```

## 相關概念

- [ 作業系統](作業系統.md) - 作業系統基礎
- [ C語言](C語言.md) - C 語言
- [ 編譯器](編譯器.md) - 編譯器
- [ Dennis_Ritchie](Dennis_Ritchie.md) - C 語言發明者
- [ Linux系統呼叫](Linux系統呼叫.md) - Linux 系統呼叫
- [ Linus_Torvalds](Linus_Torvalds.md) - Linux 之父