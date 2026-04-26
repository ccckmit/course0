# Go 語言

Go 是 Google 於 2009 年推出的系統程式語言，由 Robert Griesemer、Rob Pike 和 Ken Thompson（UNIX 和 C 語言的共同發明者）設計。Go 的設計目標是簡潔、安全、並發，特別適合雲端服務和網路應用開發。

## Go 的設計哲學

### 簡潔至上

Go 的語法極度精簡，刻意限制語言的複雜度：

```go
package main

import "fmt"

func main() {
    fmt.Println("Hello, World!")
}
```

與 C++ 的數十個關鍵字相比，Go 只有 25 個關鍵字：
```
break      default       func     interface   select
case       defer         go       map         struct
chan       else          goto     package     switch
const      fallthrough   if       range       type
continue   for           import   return      var
```

### 內建並發

Goroutine 和 Channel 使得並發程式設計優雅而強大：

```go
// Goroutine：輕量級執行緒
go func() {
    fmt.Println("Running in goroutine")
}()

// Channel：執行緒安全的消息傳遞
ch := make(chan int, 10)
go func() {
    ch <- 42  // 發送
}()
value := <-ch  // 接收
```

### 快速編譯

Go 的編譯速度極快，適合大型專案。即使數十萬行程式碼，編譯時間也在秒級。

## Go 語法基礎

### 變數與型別

```go
// 基本型別
var i int = 42
var f float64 = 3.14
var b bool = true
var s string = "hello"

// 短變數宣告
x := 10
y := "world"

// 多個變數
var (
    name    = "Alice"
    age     = 30
    height  = 165.5
)

// 指標
p := &i       // p 是 *int，指向 i
fmt.Println(*p)  // 印出 i 的值
```

### 控制結構

```go
// if 語句
if x > 0 {
    fmt.Println("positive")
} else if x < 0 {
    fmt.Println("negative")
} else {
    fmt.Println("zero")
}

// for 迴圈（Go 沒有 while）
for i := 0; i < 10; i++ {
    sum += i
}

// 無限迴圈
for {
    // ...
    if done {
        break
    }
}

// for-range 迴圈
for i, v := range slice {
    fmt.Printf("Index: %d, Value: %v\n", i, v)
}

// switch
switch day := time.Now().Weekday(); day {
case time.Saturday, time.Sunday:
    fmt.Println("Weekend")
default:
    fmt.Println("Weekday")
}
```

### 函數

```go
// 基本函數
func add(a, b int) int {
    return a + b
}

// 多返回值的函數
func divide(a, b int) (int, error) {
    if b == 0 {
        return 0, errors.New("division by zero")
    }
    return a / b, nil
}

// 命名返回值
func split(sum int) (x, y int) {
    x = sum * 4 / 9
    y = sum - x
    return  // 自動返回 x, y
}

// 可變參數
func sum(nums ...int) int {
    total := 0
    for _, n := range nums {
        total += n
    }
    return total
}
```

## 複合型別

### 陣列與切片

```go
// 陣列（固定長度）
var arr [5]int = [5]int{1, 2, 3, 4, 5}

// 切片（動態大小）
slice := []int{1, 2, 3}
slice = append(slice, 4, 5)

// 切片操作
slice = append(slice[:2], slice[3:]...)  // 刪除元素
sub := slice[1:3]                        // 子切片

// make 建立切片/映射/通道
s := make([]int, 0, 10)  // 長度 0，容量 10
m := make(map[string]int)
ch := make(chan int, 5)
```

### 映射（字典）

```go
// 建立映射
ages := make(map[string]int)

// 初始化
ages = map[string]int{
    "Alice": 30,
    "Bob": 25,
}

// 操作
ages["Charlie"] = 35   // 插入
age, exists := ages["Alice"]  // 查詢
delete(ages, "Bob")    // 刪除

// 迭代
for name, age := range ages {
    fmt.Printf("%s: %d\n", name, age)
}
```

### 結構體

```go
type Person struct {
    Name string
    Age  int
    Email string
}

// 建立結構體
p := Person{
    Name: "Alice",
    Age: 30,
}

// 指標
pp := &Person{Name: "Bob", Age: 25}

// 欄位指標
fmt.Println(pp.Name)  // Go 自動解引用

// 嵌入結構體
type Employee struct {
    Person      // 嵌入
    Company string
}

e := Employee{
    Person: Person{Name: "Charlie", Age: 28},
    Company: "Tech Corp",
}
fmt.Println(e.Name)  // 直接存取嵌入的欄位
```

## Goroutine 與 Channel

### Goroutine

```go
// 啟動 goroutine
go f()      // f() 非同步執行

// 使用匿名函數
go func() {
    for {
        // ...
    }
}()

// 等待 goroutine 完成
done := make(chan bool)
go func() {
    // 工作
    done <- true
}()
<-done  // 阻塞直到收到訊號
```

### Channel

```go
// 無緩衝 channel
unbuffered := make(chan int)

// 有緩衝 channel
buffered := make(chan int, 10)

// 發送和接收
ch <- value    // 發送到 channel
value := <-ch  // 從 channel 接收

// 關閉 channel
close(ch)

// select 多路複用
select {
case msg := <-ch1:
    fmt.Println("Received from ch1:", msg)
case ch2 <- data:
    fmt.Println("Sent to ch2")
case <-time.After(time.Second):
    fmt.Println("Timeout")
default:
    fmt.Println("No activity")
}
```

### 並發模式

```go
// Pipeline 模式
func pipeline(nums []int) <-chan int {
    out := make(chan int)
    go func() {
        for _, n := range nums {
            out <- n * 2
        }
        close(out)
    }()
    return out
}

// Fan-out / Fan-in
func merge(cs ...<-chan int) <-chan int {
    var wg sync.WaitGroup
    out := make(chan int)
    
    output := func(c <-chan int) {
        for n := range c {
            out <- n
        }
        wg.Done()
    }
    
    wg.Add(len(cs))
    for _, c := range cs {
        go output(c)
    }
    
    go func() {
        wg.Wait()
        close(out)
    }()
    return out
}

// Context 取消
func withCancel(ctx context.Context) {
    for {
        select {
        case <-ctx.Done():
            return
        default:
            // 工作
        }
    }
}
```

## 錯誤處理

### 錯誤作為值

```go
// 錯誤慣例
func readFile(path string) ([]byte, error) {
    data, err := os.ReadFile(path)
    if err != nil {
        return nil, fmt.Errorf("readFile: %w", err)
    }
    return data, nil
}

// 處理錯誤
data, err := readFile("test.txt")
if err != nil {
    log.Fatal(err)
}

// 延後清理
func doSomething() error {
    file, err := os.Open("data.txt")
    if err != nil {
        return err
    }
    defer file.Close()  // 確保關閉
    
    // ... 使用檔案
    return nil
}
```

### 自訂義錯誤

```go
type ValidationError struct {
    Field   string
    Message string
}

func (e *ValidationError) Error() string {
    return fmt.Sprintf("%s: %s", e.Field, e.Message)
}

// 使用 errors.Is / errors.As
err := &ValidationError{"email", "invalid format"}
if errors.As(err, &ValidationError{}) {
    fmt.Println(err.Error())
}
```

## 介面

```go
// 定義介面
type Reader interface {
    Read(p []byte) (n int, err error)
}

type Writer interface {
    Write(p []byte) (n int, err error)
}

// 組合介面
type ReadWriter interface {
    Reader
    Writer
}

// 空介面（類似泛型）
type Any interface{}

// 介面檢查（在編譯時）
var _ io.Writer = (*FileWriter)(nil)
```

## 套件管理

### 模組

```go
// 初始化模組
go mod init github.com/user/project

// go.mod 檔案
module github.com/user/project

go 1.21

require (
    github.com/pkg/errors v0.9.1
    golang.org/x/net v0.17.0
)

require (
    golang.org/x/text v0.13.0 // indirect
)

// 下載依賴
go mod download

// 整理
go mod tidy
```

### 測試

```go
// xxx_test.go
package main

func Add(a, b int) int {
    return a + b
}

// 表格式測試
func TestAdd(t *testing.T) {
    tests := []struct {
        name string
        a, b int
        want int
    }{
        {"positive", 1, 2, 3},
        {"negative", -1, -2, -3},
        {"mixed", -1, 2, 1},
    }
    
    for _, tt := range tests {
        t.Run(tt.name, func(t *testing.T) {
            got := Add(tt.a, tt.b)
            if got != tt.want {
                t.Errorf("Add(%d, %d) = %d, want %d", tt.a, tt.b, got, tt.want)
            }
        })
    }
}

// 執行測試
// go test ./...
// go test -v -run TestAdd
// go test -cover
```

## 標準庫常用功能

### HTTP 伺服器

```go
import "net/http"

func handler(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Hello, %s!", r.URL.Path[1:])
}

func main() {
    http.HandleFunc("/", handler)
    log.Fatal(http.ListenAndServe(":8080", nil))
}
```

### JSON 處理

```go
import "encoding/json"

type Person struct {
    Name  string `json:"name"`
    Age   int    `json:"age"`
}

// 序列化
p := Person{Name: "Alice", Age: 30}
data, _ := json.Marshal(p)

// 反序列化
var p2 Person
json.Unmarshal(data, &p2)
```

## 效能特性

### Goroutine 的優勢

- 初始棧空間僅 2KB（執行緒為 MB 級）
- 切換成本極低（使用者空間切換）
- 支援數十萬並發 goroutine

### 垃圾回收

Go 使用並發標記-清除 GC，設計目標是低延遲。雖然不是零 GC 開銷，但 pause time 通常在亞毫秒級。

## 應用場景

- **雲端服務**：Kubernetes、Docker、Terraform
- **網路工具**：Caddy、Prometheus
- **命令列工具**：Hug、kubectl
- **分散式系統**：etcd、CockroachDB
- **Web 開發**：Gin、Echo 框架

## 相關主題

- [C語言](C語言.md) - Go 的設計深受 C 影響
- [Rust](Rust.md) - 另一種系統程式語言
- [Docker](Docker.md) - Go 開發的容器化工具
- [雲端技術](雲端技術.md) - Go 的主要應用場景
