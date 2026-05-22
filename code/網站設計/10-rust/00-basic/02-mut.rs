fn main() {
    let s1 = String::from("Hello");
    let len = calculate_length(&s1); // 借用 (Borrowing)，不轉移所有權
    println!("'{}' 的長度是 {}.", s1, len);
}

fn calculate_length(s: &String) -> usize {
    s.len()
}