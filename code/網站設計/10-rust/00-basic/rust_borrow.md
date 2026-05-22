# Rust 所有權、借用與引用

Rust 最獨特的設計就是**所有權（Ownership）**系統，它在編譯期就保證記憶體安全，不需要垃圾回收器（GC），也不會有野指標或雙重釋放的問題。

---

## 1. 所有權（Ownership）三大原則

1. **每個值在 Rust 中都有一個擁有者（owner）變數。**
2. **同時間只能有一個擁有者。**
3. **當擁有者離開作用域時，值會被丟棄（drop）。**

```rust
{
    let s = String::from("hello");
    // s 是擁有者，在此作用域內有效
} // 作用域結束，s 被 drop，記憶體自動釋放
```

---

## 2. 移動（Move）— 所有權轉移

當一個變數賦值給另一個變數，或傳入函式時，所有權會**移動（move）**，原始變數不再有效。

```rust
let s1 = String::from("hello");
let s2 = s1; // s1 的所有權移動到 s2

// println!("{}", s1); // 編譯錯誤！s1 已無效
println!("{}", s2);   // 正確
```

**為什麼？** `String` 由三部分組成：指向堆積的指標、長度、容量。賦值時只複製這三個欄位（淺拷貝），而不複製堆積資料。為了避免雙重釋放，Rust 直接讓 `s1` 失效，這就是「移動」。

對**純棧型別**（如整數、布林、浮點數），情況不同：

```rust
let x = 5;
let y = x; // 整數實作 Copy trait，會自動複製
println!("{}", x); // 仍然有效
```

有實作 `Copy` trait 的型別不會被移動，而是複製。

---

## 3. 函式參數與返回值的所有權

### 傳入 = 移動

```rust
fn take_ownership(s: String) {
    println!("{}", s);
} // s 在此被 drop

fn main() {
    let s = String::from("hello");
    take_ownership(s); // 所有權移入函式
    // println!("{}", s); // 編譯錯誤！s 已無效
}
```

### 傳回 = 轉移回來

```rust
fn give_and_take(s: String) -> String {
    s // 所有權傳回給呼叫者
}

fn main() {
    let s1 = String::from("hello");
    let s2 = give_and_take(s1);
    println!("{}", s2); // 正確，s2 是新的擁有者
}
```

一直移來移去很麻煩，這正是**借用**登場的原因。

---

## 4. 借用（Borrowing）與引用（Reference）

### 不可變借用（`&T`）

```rust
fn calculate_length(s: &String) -> usize {
    s.len() // 借用，不能修改
} // s 是借用，離開作用域時不會 drop 原值

fn main() {
    let s1 = String::from("Hello");
    let len = calculate_length(&s1); // 傳入參考，不轉移所有權
    println!("'{}' 的長度是 {}", s1, len); // s1 仍然可用
}
```

`&s1` 就是**引用（reference）**，它指向 `s1` 但不擁有其值。函式 `calculate_length` 透過 `&String` 借用資料，用完後原值不受影響。

圖解：
```
s1 ───→ "Hello" (堆積)
        ↑
&s1 ───┘  (棧上的引用)
```

### 可變借用（`&mut T`）

若需要修改借用的值，使用 `&mut`：

```rust
fn change(s: &mut String) {
    s.push_str(" world");
}

fn main() {
    let mut s = String::from("Hello");
    change(&mut s);
    println!("{}", s); // "Hello world"
}
```

### 借用規則（兩者不可共存）

1. **任意時刻，只能有一個可變引用 (`&mut T`)，或任意數量的不可變引用 (`&T`)。**
2. **引用必須始終有效（不會懸空）。**

```rust
let mut s = String::from("hello");

let r1 = &s;     // OK
let r2 = &s;     // OK（多個不可變借用沒問題）
let r3 = &mut s; // 編譯錯誤！已有不可變借用，不能同時可變借用

println!("{}, {}", r1, r2);
```

```rust
let mut s = String::from("hello");

let r1 = &mut s; // OK
let r2 = &mut s; // 編譯錯誤！同一作用域不能有兩個可變借用
```

### 懸空引用（Dangling Reference）

Rust 編譯器保證引用永遠不會懸空：

```rust
fn dangle() -> &String {
    let s = String::from("hello");
    &s
} // s 被 drop，&s 變成懸空引用 → 編譯錯誤！
```

正確做法是回傳 `String` 本身（所有權轉移）：

```rust
fn no_dangle() -> String {
    let s = String::from("hello");
    s // 所有權移出函式
}
```

---

## 5. 切片（Slice）— 另一種引用

切片是對集合中連續一段的不可變引用：

```rust
let s = String::from("hello world");
let hello = &s[0..5];   // "hello"
let world = &s[6..11];  // "world"
```

切片本質上就是一個「指向某段資料的引用」，同樣受借用規則約束。

---

## 6. 完整範例對照

### 02-mut.rs — 所有權不轉移

```rust
fn main() {
    let s1 = String::from("Hello");
    let len = calculate_length(&s1); // 借用
    println!("'{}' 的長度是 {}.", s1, len); // s1 仍可用
}

fn calculate_length(s: &String) -> usize {
    s.len()
}
```

### 01-var.rs — 可變性與作用域

```rust
fn main() {
    let x = 5;      // 不可變
    let mut y = 10; // 可變
    y += x;
    println!("y 的結果是: {}", y);
}
```

---

## 7. 記憶體布局示意

```
棧 (Stack)                       堆積 (Heap)
───────                          ──────────
s1: [ptr, len, capacity] ─────→ "Hello"
&s1: [ptr] ──────────────────→     ↑
                                 同一個記憶體
calculate_length 的 s:
[ptr] ───────────────────────→     ↑
                                 也是同一個記憶體
```

所有權轉移時（`let s2 = s1`）：

```
s1: 被標記為無效
s2: [ptr, len, capacity] ─────→ "Hello" (同一塊堆積)
```

---

## 8. 核心心法

| 情境 | 使用方式 | 所有權 |
|------|----------|--------|
| 唯讀使用 | `&T`（不可變借用） | 不移轉 |
| 修改內容 | `&mut T`（可變借用） | 不移轉 |
| 需要完全擁有 | `T`（直接傳值） | 移轉 |
| 小資料複製 | 實作 `Copy` 的型別 | 自動拷貝 |

一句話記住所有權：
> **每個值只有一個擁有者，借了要還（不 drop），mutable borrow 只能借一個人，immutable borrow 可以借很多人，但不能同時有人 mutable borrow。**

---

## 9. 延伸閱讀

- `03-match.rs` — 模式匹配（常用於解構 Option/Result）
- `05-option.rs` — Option 與所有權的互動
- `10-spawn.rs` — `move` 關鍵字將所有權移入執行緒
