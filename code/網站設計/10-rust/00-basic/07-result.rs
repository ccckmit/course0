use std::fs::File;
use std::io::{self, Read};

fn read_username() -> Result<String, io::Error> {
    // 建立一個測試用的檔案（實際執行前需確保檔案存在，或這裡改用存在的檔案名）
    let mut f = File::open("hello.txt")?; 
    let mut s = String::new();
    f.read_to_string(&mut s)?;
    Ok(s)
}

fn main() {
    // 呼叫會回傳 Result 的函式
    match read_username() {
        Ok(name) => println!("讀取到的內容是: {}", name),
        Err(e) => println!("讀取檔案失敗: {:?}", e.kind()),
    }
}