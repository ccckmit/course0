// Rust I/O 範例
// 展示檔案、網路、命令列輸出

use std::env;
use std::fs::{self, File, OpenOptions};
use std::io::{self, BufRead, BufReader, Read, Write};
use std::path::Path;

// 1. 檔案讀取
fn file_read() {
    println!("=== 檔案讀取 ===");

    // 讀取整個檔案
    let contents = fs::read_to_string("test.txt").unwrap_or_else(|_| {
        println!("檔案不存在，使用預設內容");
        String::from("預設內容\nHello, Rust!")
    });
    println!("檔案內容: {}", contents);
    println!();
}

// 2. 檔案寫入
fn file_write() {
    println!("=== 檔案寫入 ===");

    let text = "Hello, Rust!\n這是第二行";

    // 寫入檔案
    fs::write("output.txt", text).expect("無法寫入檔案");
    println!("已寫入 output.txt");

    // 讀回來驗證
    let contents = fs::read_to_string("output.txt").unwrap();
    println!("驗證內容: {}", contents);
    println!();
}

// 3. 逐行讀取
fn line_read() {
    println!("=== 逐行讀取 ===");

    let file = File::open("input.txt").unwrap_or_else(|_| File::create("input.txt").unwrap());

    let reader = BufReader::new(file);

    for (index, line) in reader.lines().enumerate() {
        if let Ok(line) = line {
            println!("第 {} 行: {}", index + 1, line);
        }
    }
    println!();
}

// 4. 標準輸入
fn stdin_read() {
    println!("=== 標準輸入 ===");
    println!("請輸入文字（或按 Enter 略過）:");

    let mut input = String::new();
    if io::stdin().read_line(&mut input).is_ok() {
        println!("你輸入的是: {}", input.trim());
    }
    println!();
}

// 5. 命令列參數
fn command_args() {
    println!("=== 命令列參數 ===");

    let args: Vec<String> = env::args().collect();

    println!("程式名稱: {}", args[0]);
    println!("參數數量: {}", args.len() - 1);

    for (i, arg) in args.iter().enumerate().skip(1) {
        println!("參數 {}: {}", i, arg);
    }
    println!();
}

// 6. 環境變數
fn env_vars() {
    println!("=== 環境變數 ===");

    // 取得特定環境變數
    let path = env::var("PATH").unwrap_or_else(|_| String::from(""));
    println!("PATH 長度: {}", path.len());

    // 列出所有環境變數（只顯示前5個）
    println!("前 5 個環境變數:");
    for (i, (key, value)) in env::vars().enumerate() {
        if i >= 5 {
            break;
        }
        println!("  {} = {}", key, value);
    }
    println!();
}

// 7. 網路請求（使用標準庫）
fn http_request() {
    println!("=== 網路請求 ===");
    println!("注意: 標準庫沒有 HTTP 客戶端");
    println!("建議使用 reqwest 或 ureq crate");
    println!();

    // 展示概念程式碼（需要額外依賴）
    println!("// 範例程式碼（需要 reqwest）:");
    println!("let response = reqwest::blocking::get(\"https://httpbin.org/json\")");
    println!("    .unwrap()");
    println!("    .text()");
    println!("    .unwrap();");
    println!();
}

// 8. JSON 處理（展示概念）
fn json_handling() {
    println!("=== JSON 處理 ===");
    println!("注意: 需要 serde_json crate");
    println!();

    println!("// 範例:");
    println!("#[derive(Serialize, Deserialize)]");
    println!("struct Person {{");
    println!("    name: String,");
    println!("    age: u32,");
    println!("}}");
    println!();

    println!("let person = Person {{");
    println!("    name: String::from(\"Alice\"),");
    println!("    age: 30,");
    println!("}};");
    println!();

    println!("let json = serde_json::to_string(&person).unwrap();");
    println!("let parsed: Person = serde_json::from_str(&json).unwrap();");
}

// 9. 錯誤處理
fn error_handling() {
    println!("=== 錯誤處理 ===");

    fn may_fail() -> Result<i32, String> {
        if true {
            Ok(42)
        } else {
            Err(String::from("發生錯誤"))
        }
    }

    match may_fail() {
        Ok(value) => println!("成功: {}", value),
        Err(e) => println!("錯誤: {}", e),
    }

    // 使用 ? 運算符
    fn read_and_double(path: &str) -> Result<i32, io::Error> {
        let contents = fs::read_to_string(path)?;
        let num: i32 = contents
            .trim()
            .parse()
            .map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))?;
        Ok(num * 2)
    }

    // 範例
    fs::write("number.txt", "21").ok();
    if let Ok(result) = read_and_double("number.txt") {
        println!("讀取並加倍: {}", result);
    }
    println!();
}

// 10. 臨時檔案
fn temp_file() {
    println!("=== 臨時檔案 ===");

    use std::env::temp_dir;

    let temp_path = temp_dir().join("rust_temp.txt");

    // 寫入臨時檔案
    fs::write(&temp_path, "臨時內容").ok();
    println!("臨時檔案: {:?}", temp_path);

    // 讀取
    if let Ok(content) = fs::read_to_string(&temp_path) {
        println!("內容: {}", content);
    }

    // 清理
    fs::remove_file(&temp_path).ok();
    println!("已刪除臨時檔案");
    println!();
}

fn main() {
    println!("=== Rust I/O 範例 ===\n");

    // 建立測試用的檔案
    fs::write("input.txt", "第一行\n第二行\n第三行").ok();
    fs::write("number.txt", "21").ok();

    file_read();
    file_write();
    line_read();
    stdin_read();
    command_args();
    env_vars();
    http_request();
    json_handling();
    error_handling();
    temp_file();

    // 清理測試檔案
    fs::remove_file("input.txt").ok();
    fs::remove_file("output.txt").ok();
    fs::remove_file("number.txt").ok();

    println!("I/O 範例完成!");
}
