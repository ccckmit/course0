fn main() {
    let x = 5; // 不可變
    let mut y = 10; // 使用 mut 關鍵字變為可變
    y += x;
    println!("y 的結果是: {}", y);
}