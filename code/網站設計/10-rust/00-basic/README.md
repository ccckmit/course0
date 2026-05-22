# Rust 基礎語法入門 — 11 個範例循序導覽

本系列從 `00-hello.rs` 到 `10-spawn.rs`，共 11 個範例，循序介紹 Rust 的核心概念。


---

## 00-hello.rs — Hello World

```rust
fn main() {
    println!("Hello, world! 你好！");
}
```

- `fn main()` 是程式的進入點。
- `println!` 是**巨集（macro）**，以 `!` 結尾區別於一般函式。
- 支援 Unicode，可直接印出中文。

---

## 01-var.rs — 變數綁定與不可變性

```rust
let x = 5;          // 不可變
let mut y = 10;     // 使用 mut 變為可變
y += x;
```

- Rust 預設變數**不可變（immutable）**。
- 加上 `mut` 關鍵字讓變數可被修改。
- 這是 Rust 安全性設計的核心之一：預設唯讀，明確宣告才可寫。

---

## 02-mut.rs — 所有權與借用

```rust
let s1 = String::from("Hello");
let len = calculate_length(&s1); // 借用，不轉移所有權
println!("'{}' 的長度是 {}.", s1, len);
```

- `&s1` 是**借用（borrowing）**，不轉移所有權，呼叫後 `s1` 仍可使用。
- 若直接傳遞 `s1`（不使用 `&`），所有權會移動（move），之後 `s1` 將無法存取。
- Rust 的所有權系統在編譯期保證記憶體安全，無需 GC。

---

## 03-match.rs — 模式匹配

```rust
match number {
    1 => println!("One!"),
    2 | 3 | 5 | 7 | 11 => println!("這是一個質數"),
    13..=19 => println!("青少年時期"),
    _ => println!("其他數字"),
}
```

- `match` 是 Rust 最強大的控制流運算子，強制窮舉所有可能性。
- 支援多重條件 `|`、範圍 `..=`、萬用字元 `_`。
- 編譯器會檢查所有分支是否涵蓋，避免遺漏。

---

## 04-struct.rs — 結構體與方法

```rust
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn area(&self) -> u32 {
        self.width * self.height
    }
}
```

- `struct` 定義結構體，類似其他語言的 class 但僅有資料。
- `impl` 區塊為結構體綁定方法（method），第一個參數 `&self` 代表實例的參考。
- Rust 區分**方法（method）**與**關聯函式（associated function）**（如 `::new()`）。

---

## 05-option.rs — Option 枚舉（無 nil 設計）

```rust
fn divide(numerator: f64, denominator: f64) -> Option<f64> {
    if denominator == 0.0 {
        None
    } else {
        Some(numerator / denominator)
    }
}
```

- Rust **沒有 `null` / `nil`**，改用 `Option<T>` 表達可能為空的值。
- `Some(value)` 表示有值，`None` 表示無值。
- 必須用 `match` 解開才能使用，編譯器強制處理空值情況，根除空指標異常。

---

## 06-trait.rs — Trait（介面）

```rust
trait Summary {
    fn summarize(&self) -> String;
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String { ... }
}
```

- `trait` 定義共享行為，類似其他語言的 interface。
- `impl Trait for Type` 為型別實作 trait。
- 此例中 `.chars().take(10)` 按**字元**取前 10 個，而非按位元組，正確處理中文字串。

---

## 07-result.rs — Result 錯誤處理

```rust
fn read_username() -> Result<String, io::Error> {
    let mut f = File::open("hello.txt")?;
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    Ok(s)
}
```

- `Result<T, E>` 是 Rust 的錯誤處理機制，`Ok(T)` 表示成功，`Err(E)` 表示失敗。
- `?` 運算子簡化錯誤傳播：若 `Err` 則提早回傳，否則取出 `Ok` 內的值。
- 強制開發者處理錯誤，不像例外可能被忽略。

---

## 08-generic.rs — 泛型

```rust
fn largest<T: PartialOrd>(list: &[T]) -> &T { ... }
```

- `<T: PartialOrd>` 表示泛型型別 `T` 必須實作 `PartialOrd` trait（即可比較大小）。
- `&[T]` 是切片（slice）參考，可接受任何型別的陣列或 Vec。
- Rust 的泛型採用**單態化（monomorphization）**，編譯期為每種具體型別生成專屬程式碼，零執行時期開銷。

---

## 09-closure.rs — 閉包

```rust
let v2: Vec<_> = v1.iter().map(|x| x + 1).collect();
```

- 閉包（closure）是匿名函式，用 `|參數|` 語法定義。
- `iter().map().collect()` 是函數式風格的迭代器鏈。
- `Vec<_>` 讓編譯器自動推斷型別。

---

## 10-spawn.rs — 執行緒與 move

```rust
let handle = thread::spawn(move || {
    println!("來自執行緒的向量: {:?}", v);
});
handle.join().unwrap();
```

- `thread::spawn` 建立新執行緒。
- `move` 關鍵字強制將捕獲的變數所有權移入閉包，避免所有權競爭。
- `handle.join()` 等待執行緒結束。
- Rust 的所有權與型別系統在**編譯期**防止資料競爭（data race）。

---

## 總結

| 範例 | 核心概念 |
|------|----------|
| 00 | 程式進入點、println 巨集 |
| 01 | 不可變 vs 可變變數 |
| 02 | 借用與所有權 |
| 03 | 模式匹配 |
| 04 | 結構體與 impl 方法 |
| 05 | Option 與無 nil |
| 06 | Trait 與字串處理 |
| 07 | Result 與 ? 運算子 |
| 08 | 泛型與 trait bound |
| 09 | 閉包與迭代器 |
| 10 | 執行緒與 move |

這 11 個範例涵蓋了 Rust 最核心的語言特性，從變數、所有權、模式匹配、泛型、錯誤處理到並行，循序漸進地建立對 Rust 的理解。建議逐一編譯執行，親身體驗 Rust 編譯器的嚴格與安全性保證。
