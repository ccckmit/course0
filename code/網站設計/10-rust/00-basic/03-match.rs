fn main() {
    let number = 13;
    match number {
        1 => println!("One!"),
        2 | 3 | 5 | 7 | 11 => println!("這是一個質數"),
        13..=19 => println!("青少年時期"), // 範圍匹配
        _ => println!("其他數字"), // 萬用字元
    }
}