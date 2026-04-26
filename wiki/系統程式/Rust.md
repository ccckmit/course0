# Rust

Rust 是一種注重安全性 並發性和效能的系統程式語言，由 Mozilla 工程的 Graydon Hoare 於 2006 年開發。Rust 的設計目標是提供 C 和 C++ 的效能，同時消除記憶體安全問題和資料競爭。透過所有權系統和借用法則，Rust 在編譯時就能預防大多數記憶體錯誤，無需垃圾回收器。這種「零成本抽象」的特性使得 Rust 特別適合開發高效能、低延遲的系統軟體，從作業系統核心到瀏覽器引擎、區塊鏈客戶端，Rust 都有廣泛應用。

## Rust 特色

### 核心特性

```
┌─────────────────────────────────────────────────────────────────┐
│                      Rust 核心特性                              │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  記憶體安全：                                                   │
│  • 沒有空指標、懸指標                                             │
│  • 沒有緩衝區溢位                                                │
│  • 沒有資料競爭                                                  │
│  • 編譯時所有權檢查                                              │
│                                                              │
│  並發安全：                                                     │
│  • 無資料競爭的並發                                              │
│  • Send 和 Sync 特性                                             │
│  • 編譯器強制執行                                                │
│                                                              │
│  零成本抽象：                                                   │
│  • 高階抽象不增加運行期開銷                                      │
│  • 內聯優化                                                      │
│  • 編譯期多型                                                    │
│                                                              │
│  實用性：                                                       │
│  • 現代工具鏈 (cargo)                                            │
│  • 豐富標準庫                                                    │
│  • 學習曲線適中                                                  │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

## 基本語法

### Hello World

```rust
fn main() {
    println!("Hello, world!");
}
```

### 變數與可變性

```rust
// 不可變變數
let x = 5;
// x = 6; // 錯誤！

// 可變變數
let mut y = 5;
y = 6;

// 常量
const MAX_SIZE: usize = 100;

// 類型推斷
let z = 10;  // 推斷為 i32

// 明確類型
let a: i32 = 10;
```

### 資料類型

```rust
// 標量類型
let int: i32 = 42;
let uint: u32 = 42;
let float: f64 = 3.14;
let bool: bool = true;
let char: char = 'A';

// 複合類型
let tuple: (i32, f64, u8) = (500, 6.4, 1);
let (a, b, c) = tuple;

let array: [i32; 5] = [1, 2, 3, 4, 5];
let slice: &[i32] = &array[1..4];
```

### 流程控制

```rust
// if 表達式
let x = 5;
if x > 0 {
    println!("positive");
} else if x < 0 {
    println!("negative");
} else {
    println!("zero");
}

// match 表達式
let n = 2;
match n {
    1 => println!("one"),
    2 => println!("two"),
    3 | 4 => println!("three or four"),
    5..=10 => println!("five to ten"),
    _ => println!("other"),
}

// 迴圈
loop {
    // 無限迴圈，需要 break
    break;
}

while n > 0 {
    n -= 1;
}

for i in 0..5 {
    println!("{}", i);
}
```

## 所有權系統

### 核心概念

Rust 的所有權系統是其記憶體安全的基石：

```rust
// 所有權規則
// 1. 每個值有一個所有者
// 2. 同一時間只有一個所有者
// 3. 當所有者離開作用域，值被drop

// 移動語意
let s1 = String::from("hello");
let s2 = s1;  // s1 移動到 s2
// println!("{}", s1); // 錯誤！s1 已無效

// 複製語意 (Copy types)
let x = 5;
let y = x;  // 複製，兩者都有效

// 借用
let s1 = String::from("hello");
let len = calculate_length(&s1);  // 借用
println!("{} {}", s1, len);  // s1 仍然有效

fn calculate_length(s: &String) -> usize {
    s.len()
}
```

### 借用法則

```
┌─────────────────────────────────────────────────────────────────┐
│                      借用法則                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                              │
│  • 任何時間只能有一個可變引用                                   │
│  • 或者任意數量的不可變引用                                     │
│  • 引用必須總是有效                                            │
│                                                              │
└─────────────────────────────────────────────────────────────────┘
```

```rust
// 不可變借用
let s = String::from("hello");
let r1 = &s;
let r2 = &s;
// println!("{} and {}", r1, r2);

// 可變借用
let mut s = String::from("hello");
let r1 = &mut s;
// println!("{}", r1); // 不能同時有不可變
r1.push_str(", world");
println!("{}", r1);
```

## 結構與列舉

### 結構體

```rust
struct User {
    username: String,
    email: String,
    active: bool,
}

let user = User {
    username: String::from("alice"),
    email: String::from("alice@example.com"),
    active: true,
};

println!("{}", user.username);

// 結構體更新語法
let user2 = User {
    email: String::from("bob@example.com"),
    ..user.clone()  // 其餘欄位從 user 複製
};
```

### 實作方法

```rust
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    // 關聯函式
    fn new(width: u32, height: u32) -> Self {
        Rectangle { width, height }
    }

    // 方法
    fn area(&self) -> u32 {
        self.width * self.height
    }

    // 可變方法
    fn scale(&mut self, factor: u32) {
        self.width *= factor;
        self.height *= factor;
    }
}
```

### 列舉

```rust
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

let m = Message::Move { x: 10, y: 20 };

match m {
    Message::Quit => println!("Quit"),
    Message::Move { x, y } => println!("Move to ({}, {})", x, y),
    Message::Write(s) => println!("Write: {}", s),
    Message::ChangeColor(r, g, b) => println!("Color: {}, {}, {}", r, g, b),
}

// Option 類型
fn find_user(id: u32) -> Option<String> {
    if id == 1 {
        Some(String::from("admin"))
    } else {
        None
    }
}

if let Some(name) = find_user(1) {
    println!("Found: {}", name);
}
```

## 錯誤處理

### Result 類型

```rust
use std::fs::File;
use std::io::Read;

fn read_file(path: &str) -> Result<String, std::io::Error> {
    let mut file = File::open(path)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents)
}

// 使用
match read_file("test.txt") {
    Ok(contents) => println!("{}", contents),
    Err(e) => eprintln!("Error: {}", e),
}

// ? 運算子
fn read_file2(path: &str) -> Result<String, std::io::Error> {
    let mut file = File::open(path)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents)
}
```

### Panic

```rust
// 不可恢復錯誤
panic!("Something went wrong");

// 斷言
assert!(condition, "message");
assert_eq!(a, b);
assert_ne!(a, b);
```

## 泛型與特徵

### 泛型

```rust
fn largest<T: PartialOrd>(list: &[T]) -> &T {
    let mut largest = &list[0];
    for item in list {
        if item > largest {
            largest = item;
        }
    }
    largest
}

// 泛型結構
struct Point<T> {
    x: T,
    y: T,
}

let p1 = Point { x: 5, y: 10 };
let p2 = Point { x: 1.0, y: 4.0 };
```

### 特徵

```rust
trait Summary {
    fn summarize(&self) -> String;
    
    // 預設實作
    fn summarize_author(&self) -> String {
        String::from("(Unknown)")
    }
}

struct Article {
    title: String,
    author: String,
    content: String,
}

impl Summary for Article {
    fn summarize(&self) -> String {
        format!("{} by {}", self.title, self.author)
    }
}

// 特徵邊界
fn notify<T: Summary>(item: &T) {
    println!("Breaking: {}", item.summarize());
}
```

## 並發

### 執行緒

```rust
use std::thread;
use std::sync::Mutex;

let handle = thread::spawn(|| {
    for i in 1..10 {
        println!("thread: {}", i);
    }
});

handle.join().unwrap();

// 共享狀態
let counter = Mutex::new(0);

let mut handles = vec![];
for _ in 0..10 {
    let handle = thread::spawn(|| {
        let mut num = counter.lock().unwrap();
        *num += 1;
    });
    handles.push(handle);
}

for handle in handles {
    handle.join().unwrap();
}
```

### Send 和 Sync

```rust
// Send: 可以跨執行緒傳遞
// Sync: 可以跨執行緒共享引用

// 大多數 Rust 類型預設實作這些
// 需手動實現的情況很少
```

### 訊息傳遞

```rust
use std::sync::mpsc;

let (tx, rx) = mpsc::channel();

thread::spawn(move || {
    tx.send("Hello from thread").unwrap();
});

let received = rx.recv().unwrap();
println!("Got: {}", received);
```

## 專案管理

### Cargo

```bash
# 建立新專案
cargo new my_project

# 構建
cargo build
cargo build --release

# 執行
cargo run

# 測試
cargo test

# 文件
cargo doc --open

# 依賴管理
cargo add <crate>
```

### Cargo.toml

```toml
[package]
name = "my_project"
version = "0.1.0"
edition = "2021"

[dependencies]
serde = "1.0"
tokio = { version = "1.0", features = ["full"] }

[dev-dependencies]
criterion = "0.5"

[profile.release]
opt-level = 3
lto = true
```

## 實際應用

### Web 開發

```rust
// 使用 actix-web
use actix_web::{web, App, HttpServer, Responder};

async fn hello() -> impl Responder {
    "Hello, world!"
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    HttpServer::new(|| {
        App::new()
            .route("/", web::get().to(hello))
    })
    .bind("127.0.0.1:8080")?
    .run()
    .await
}
```

### 系統程式

```rust
// Rust 核心功能用於：
// • 作業系統 (RustOS)
// • 檔案系統 (Bolt, Stratis)
// • 驅動程式
// • 嵌入式系統
// • WASM
```

### 工具開發

```rust
// 命令列工具
// • ripgrep (高效能搜尋)
// • fd (檔案搜尋)
// • starship (終端機提示)
// • etc.
```

## 學習資源

### 相關概念

- [C語言](C語言.md) - 系統程式基礎
- [組合語言](組合語言.md) - 低階程式設計
- [編譯器](編譯器.md) - Rust 編譯器 (rustc)
- [Docker](Docker.md) - 容器化 (Rust 專案)
- [LLVM](LLVM.md) - Rust 編譯後端