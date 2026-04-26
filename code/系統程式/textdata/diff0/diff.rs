use std::env;
use std::fs;
use std::process;

// 定義與 C 語言相同的結構體，使用生命週期 'a 來借用字串切片
struct DiffOp<'a> {
    op: char,
    line: &'a str,
}

// 動態規劃實作 Diff 演算法
fn diff_dp<'a>(a: &[&'a str], b: &[&'a str]) {
    let n = a.len();
    let m = b.len();

    // 建立二維的 DP 表格並初始化為 0
    // 相當於 C 的: int **dp = calloc(...)
    let mut dp = vec![vec![0; m + 1]; n + 1];

    // 填寫 DP 表格 (Longest Common Subsequence)
    for i in 1..=n {
        for j in 1..=m {
            if a[i - 1] == b[j - 1] {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = dp[i - 1][j].max(dp[i][j - 1]);
            }
        }
    }

    let mut i = n;
    let mut j = m;
    let mut ops = Vec::new();

    // 回溯尋找操作路徑
    while i > 0 || j > 0 {
        if i > 0 && j > 0 && a[i - 1] == b[j - 1] {
            ops.push(DiffOp { op: ' ', line: a[i - 1] });
            i -= 1;
            j -= 1;
        } else if i > 0 && (j == 0 || dp[i][j - 1] < dp[i - 1][j]) {
            ops.push(DiffOp { op: '-', line: a[i - 1] });
            i -= 1;
        } else {
            ops.push(DiffOp { op: '+', line: b[j - 1] });
            j -= 1;
        }
    }

    // 因為回溯是從後往前，所以我們需要反轉輸出
    for op in ops.iter().rev() {
        println!("{} {}", op.op, op.line);
    }
}

// 讀取檔案的輔助函式，整合了錯誤處理
fn read_file(filename: &str) -> String {
    match fs::read_to_string(filename) {
        Ok(content) => content,
        Err(err) => {
            eprintln!("Cannot open file: {} ({})", filename, err);
            process::exit(1);
        }
    }
}

fn main() {
    // 解析命令列參數
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!("Usage: {} <fileA> <fileB>", args[0]);
        process::exit(1);
    }

    // 讀取檔案內容
    // 相當於 C 中的 contentA 和 contentB
    let content_a = read_file(&args[1]);
    let content_b = read_file(&args[2]);

    // Rust 的 .lines() 會自動以換行符號分割字串，且不會產生額外的記憶體拷貝
    // 相當於 C 中的 split_lines 函式
    let lines_a: Vec<&str> = content_a.lines().collect();
    let lines_b: Vec<&str> = content_b.lines().collect();

    // 執行演算法
    diff_dp(&lines_a, &lines_b);

    // Rust 變數離開作用域時會自動釋放記憶體，不需要像 C 一樣呼叫 free_lines()
}