// Rust 基礎語法範例
// 展示 Rust 的所有權、借用、生命周期等核心概念

// 1. 變數與可變性
fn variables() {
    // 不可變變數
    let x = 5;
    println!("x = {}", x);

    // 可變變數
    let mut y = 10;
    y += 5;
    println!("y = {}", y);

    // 常量
    const MAX_SIZE: i32 = 100;
    println!("MAX_SIZE = {}", MAX_SIZE);
}

// 2. 資料類型
fn data_types() {
    // 標量類型
    let _integer: i32 = 42;
    let _float: f64 = 3.14;
    let _boolean: bool = true;
    let _character: char = 'A';

    // 字元串
    let _string: &str = "Hello";
    let _string_owned: String = String::from("World");

    // 元組
    let tuple: (i32, f64, bool) = (1, 2.0, true);
    let (a, b, c) = tuple;
    println!("tuple: {}, {}, {}", a, b, c);

    // 陣列
    let arr: [i32; 5] = [1, 2, 3, 4, 5];
    println!("arr[0] = {}", arr[0]);
}

// 3. 所有權與移動
fn ownership() {
    // String 擁有堆上的資料
    let s1 = String::from("hello");
    let s2 = s1; // s1 的所有權移動到 s2
                 // println!("{}", s1); // 錯誤：s1 不再有效
    println!("{}", s2); // 正確

    // 複製（Copy 特徵）
    let x = 5;
    let y = x; // 複製而不是移動
    println!("x = {}, y = {}", x, y);
}

// 4. 借用與引用
fn borrowing() {
    let s = String::from("hello");

    // 不可變引用
    let len = calculate_length(&s);
    println!("'{}' 的長度是 {}", s, len);

    // 可變引用
    let mut s = String::from("hello");
    change(&mut s);
    println!("修改後: {}", s);
}

fn calculate_length(s: &String) -> usize {
    s.len()
}

fn change(s: &mut String) {
    s.push_str(", world");
}

// 5. 結構體
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn new(width: u32, height: u32) -> Self {
        Rectangle { width, height }
    }

    fn area(&self) -> u32 {
        self.width * self.height
    }

    fn can_hold(&self, other: &Rectangle) -> bool {
        self.width > other.width && self.height > other.height
    }
}

// 6. 列舉與模式匹配
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

impl Message {
    fn call(&self) {
        match self {
            Message::Quit => println!("Quit"),
            Message::Move { x, y } => println!("Move to ({}, {})", x, y),
            Message::Write(text) => println!("Write: {}", text),
            Message::ChangeColor(r, g, b) => println!("Change color to RGB({}, {}, {})", r, g, b),
        }
    }
}

// 7. 錯誤處理
fn read_file_contents(path: &str) -> Result<String, std::io::Error> {
    use std::fs;

    let contents = fs::read_to_string(path)?;
    Ok(contents)
}

// 8. 泛型
fn largest<T: PartialOrd>(list: &[T]) -> &T {
    let mut largest = &list[0];
    for item in list {
        if item > largest {
            largest = item;
        }
    }
    largest
}

// 9. trait 定義
trait Summary {
    fn summarize(&self) -> String;
    fn summarize_author(&self) -> String {
        String::from("(Unknown)")
    }
}

impl Summary for Rectangle {
    fn summarize(&self) -> String {
        format!("Rectangle: {}x{}", self.width, self.height)
    }
}

// 10. 生命週期
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

fn main() {
    println!("=== Rust 基礎範例 ===\n");

    variables();
    println!();

    data_types();
    println!();

    ownership();
    println!();

    borrowing();
    println!();

    // 結構體
    let rect = Rectangle::new(30, 50);
    println!("矩形: {}", rect.summarize());
    println!("面積: {}", rect.area());

    let rect2 = Rectangle::new(10, 40);
    println!("rect 可以容納 rect2: {}", rect.can_hold(&rect2));

    // 列舉
    let msgs = vec![
        Message::Quit,
        Message::Move { x: 10, y: 20 },
        Message::Write(String::from("hello")),
        Message::ChangeColor(255, 0, 0),
    ];
    for msg in msgs {
        msg.call();
    }

    // 泛型
    let numbers = vec![34, 50, 25, 100, 65];
    let result = largest(&numbers);
    println!("最大的數: {}", result);

    let chars = vec!['y', 'm', 'a', 'q'];
    let result = largest(&chars);
    println!("最大的字元: {}", result);

    // 生命週期
    let string1 = String::from("hello");
    let string2 = String::from("world!");
    let result = longest(&string1, &string2);
    println!("較長的字串: {}", result);
}
