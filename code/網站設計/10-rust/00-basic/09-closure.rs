fn main() {
    let v1 = vec![1, 2, 3];
    // 使用 map 閉包將元素加一，再 collect 成新的向量
    let v2: Vec<_> = v1.iter().map(|x| x + 1).collect();
    println!("v2: {:?}", v2);
}