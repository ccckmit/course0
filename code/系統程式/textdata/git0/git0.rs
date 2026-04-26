use std::env;
use std::fs::{self, OpenOptions};
use std::io::{self, Write};
use std::path::Path;
use std::process;
use std::time::{SystemTime, UNIX_EPOCH};

const GIT0_DIR: &str = ".git0";
const OBJECTS_DIR: &str = ".git0/objects";
const INDEX_FILE: &str = ".git0/index";
const COMMITS_FILE: &str = ".git0/commits";
const FNV1A_OFFSET: u32 = 0x811C9DC5;
const FNV1A_PRIME: u32 = 0x01000193;

// FNV-1a 雜湊函數
fn fnv1a_hash(data: &[u8]) -> u32 {
    let mut hash = FNV1A_OFFSET;
    for &byte in data {
        hash ^= byte as u32;
        // 使用 wrapping_mul 防止溢位報錯，等同於 C 語言預設的溢位行為
        hash = hash.wrapping_mul(FNV1A_PRIME);
    }
    hash
}

// 初始化倉庫
fn init_git0() -> io::Result<()> {
    if Path::new(GIT0_DIR).exists() {
        println!("git0 repository already exists.");
        return Ok(());
    }
    
    fs::create_dir(GIT0_DIR)?;
    fs::create_dir(OBJECTS_DIR)?;
    fs::File::create(INDEX_FILE)?;
    fs::File::create(COMMITS_FILE)?;
    
    println!("Initialized empty git0 repository in {}/", GIT0_DIR);
    Ok(())
}

// 添加文件到暫存區
fn add_file(filename: &str) -> io::Result<()> {
    if !Path::new(filename).exists() {
        println!("File not found: {}", filename);
        return Ok(());
    }
    if !Path::new(GIT0_DIR).is_dir() {
        println!("Not a git0 repo. Run 'init' first.");
        return Ok(());
    }

    // 讀取檔案並計算雜湊
    let content = fs::read(filename)?;
    let hash = fnv1a_hash(&content);
    let hash_hex = format!("{:08x}", hash);

    // 寫入 objects
    let obj_path = format!("{}/{}", OBJECTS_DIR, hash_hex);
    if !Path::new(&obj_path).exists() {
        fs::write(obj_path, &content)?;
    }

    // 更新 index 檔案
    let index_content = fs::read_to_string(INDEX_FILE).unwrap_or_default();
    let mut new_index = String::new();
    let mut found = false;

    for line in index_content.lines() {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 && parts[0] == filename {
            new_index.push_str(&format!("{} {}\n", filename, hash_hex));
            found = true;
        } else {
            new_index.push_str(&format!("{}\n", line));
        }
    }

    if !found {
        new_index.push_str(&format!("{} {}\n", filename, hash_hex));
    }

    fs::write(INDEX_FILE, new_index)?;
    println!("Added {} to index.", filename);
    
    Ok(())
}

// 提交變更
fn commit_changes(message: &str) -> io::Result<()> {
    if !Path::new(GIT0_DIR).is_dir() {
        println!("Not a git0 repo. Run 'init' first.");
        return Ok(());
    }

    let index_content = fs::read_to_string(INDEX_FILE).unwrap_or_default();
    if index_content.trim().is_empty() {
        println!("No changes to commit. Use 'add' first.");
        return Ok(());
    }

    let commits_content = fs::read_to_string(COMMITS_FILE).unwrap_or_default();
    let mut parent_id = String::from("none");

    // 尋找最後一次 commit (等同於 C 語言中的 strrstr)
    if let Some(last_idx) = commits_content.rfind("commit ") {
        let substring = &commits_content[last_idx..];
        let parts: Vec<&str> = substring.split_whitespace().collect();
        if parts.len() >= 2 {
            parent_id = parts[1].to_string();
        }
    }

    // 產生 Commit Hash (為保持無第三方依賴，使用系統時間戳 ^ 進程 ID 來模擬亂數)
    let sys_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap();
    let commit_hash = (sys_time.as_secs() as u32) ^ (process::id() << 16);
    let commit_id = format!("{:08x}", commit_hash);

    // 時間戳格式 (為保持無第三方依賴，純使用 UNIX 時間戳)
    let timestamp = format!("Epoch {}", sys_time.as_secs());

    let commit_content = format!(
        "commit {}\nparent {}\ndate {}\nmessage {}\nfiles\n{}\n",
        commit_id, parent_id, timestamp, message, index_content
    );

    // 附加模式寫入
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(COMMITS_FILE)?;
    
    file.write_all(commit_content.as_bytes())?;

    println!("Committed as {}", commit_id);
    Ok(())
}

fn main() {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        println!("Usage: git0 <command> [args]");
        println!("  init          Initialize repo");
        println!("  add <file>    Add file to index");
        println!("  commit <msg>  Commit changes");
        process::exit(0);
    }

    let command = args[1].as_str();
    
    let result = match command {
        "init" => init_git0(),
        "add" => {
            if args.len() >= 3 {
                add_file(&args[2])
            } else {
                println!("Usage: add <file>");
                process::exit(1);
            }
        }
        "commit" => {
            if args.len() >= 3 {
                commit_changes(&args[2])
            } else {
                println!("Usage: commit <msg>");
                process::exit(1);
            }
        }
        _ => {
            println!("Unknown command: {}", command);
            process::exit(1);
        }
    };

    if let Err(e) = result {
        eprintln!("Error executed command: {}", e);
        process::exit(1);
    }
}