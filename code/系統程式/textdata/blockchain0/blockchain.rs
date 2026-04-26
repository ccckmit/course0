use std::io::{self, Write};
use std::time::{SystemTime, UNIX_EPOCH};

const DIFFICULTY: usize = 4;

// 使用結構體定義區塊，去除了 C 版本中繁雜的 linked list pointer (next)
#[derive(Debug, Clone)]
struct Block {
    index: u32,
    timestamp: u64,
    data: String,
    previous_hash: String,
    hash: String,
    nonce: u32,
}

// 檢查雜湊值的前綴是否有滿足 DIFFICULTY 數量的 '0'
fn is_hash_valid(hash: &str) -> bool {
    let prefix = "0".repeat(DIFFICULTY);
    hash.starts_with(&prefix)
}

// DJB2 字串雜湊演算法 (對應 C 的 simple_hash)
fn simple_hash(s: &str) -> u64 {
    let mut hash: u64 = 5381;
    for byte in s.bytes() {
        // 等同於 C 的: hash = ((hash << 5) + hash) + c;
        // 使用 wrapping 防止 Rust 在 Debug 模式下因整數溢位而 Panic
        hash = (hash << 5).wrapping_add(hash).wrapping_add(byte as u64);
    }
    hash
}

// 計算區塊的 Hash
fn calculate_hash(
    index: u32,
    timestamp: u64,
    data: &str,
    previous_hash: &str,
    nonce: u32,
) -> String {
    let combined = format!("{}{}{}{}{}", index, timestamp, data, previous_hash, nonce);
    let h = simple_hash(&combined);
    // 輸出成 64 字元的十六進位字串，不足補 0 (等同於 C 的 "%064lx")
    format!("{:064x}", h)
}

impl Block {
    // 建立新區塊並進行挖礦
    fn new(index: u32, data: &str, previous_hash: &str) -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let mut block = Block {
            index,
            timestamp,
            data: data.to_string(),
            previous_hash: previous_hash.to_string(),
            hash: String::new(),
            nonce: 0,
        };

        block.mine();
        block
    }

    // 挖礦 (Proof of Work)
    fn mine(&mut self) {
        print!("Mining block #{}...", self.index);
        io::stdout().flush().unwrap(); // 確保文字立即印出

        let mut nonce = 0;
        loop {
            let hash = calculate_hash(
                self.index,
                self.timestamp,
                &self.data,
                &self.previous_hash,
                nonce,
            );

            if is_hash_valid(&hash) {
                self.hash = hash;
                self.nonce = nonce;
                println!(" Done! Nonce: {}", nonce);
                break;
            }
            nonce += 1;
        }
    }

    // 印出區塊資訊
    fn print(&self) {
        println!("Block #{}", self.index);
        println!("  Timestamp: {}", self.timestamp);
        println!("  Data: {}", self.data);
        println!("  Previous Hash: {}", self.previous_hash);
        println!("  Hash: {}", self.hash);
        println!("  Nonce: {}\n", self.nonce);
    }
}

// 驗證整條區塊鏈是否合法
fn is_chain_valid(chain: &[Block]) -> bool {
    // 從第 1 個區塊開始遍歷（跳過 Genesis 區塊）
    for i in 1..chain.len() {
        let current = &chain[i];
        let previous = &chain[i - 1];

        // 1. 重新計算 Hash，確認資料沒有被篡改
        let calculated_hash = calculate_hash(
            current.index,
            current.timestamp,
            &current.data,
            &current.previous_hash,
            current.nonce,
        );

        if calculated_hash != current.hash {
            return false;
        }

        // 2. 檢查 Hash 是否符合難度要求 (PoW)
        if !is_hash_valid(&current.hash) {
            return false;
        }

        // 3. 檢查當前區塊的 previous_hash 是否指向前一個區塊的 hash
        if current.previous_hash != previous.hash {
            return false;
        }
    }
    true
}

fn main() {
    println!("=== Simple Blockchain Demo ===\n");

    // 在 Rust 中使用 Vec (動態陣列) 儲存區塊是最安全且效能極高的方式
    let mut blockchain: Vec<Block> = Vec::new();

    // 1. 創世區塊 (Genesis Block)
    let genesis_block = Block::new(
        0,
        "Genesis Block",
        "0000000000000000000000000000000000000000000000000000000000000000",
    );
    blockchain.push(genesis_block);

    // 2. 加入區塊 1
    // 使用 .last().unwrap().hash 取出陣列中最後一個區塊的 hash 作為 previous_hash
    let block1 = Block::new(
        1,
        "Alice sends 10 coins to Bob",
        &blockchain.last().unwrap().hash,
    );
    blockchain.push(block1);

    // 3. 加入區塊 2
    let block2 = Block::new(
        2,
        "Bob sends 5 coins to Charlie",
        &blockchain.last().unwrap().hash,
    );
    blockchain.push(block2);

    // 4. 加入區塊 3
    let block3 = Block::new(
        3,
        "Charlie sends 3 coins to Dave",
        &blockchain.last().unwrap().hash,
    );
    blockchain.push(block3);

    // 列印整條區塊鏈
    for block in &blockchain {
        block.print();
    }

    // 驗證區塊鏈
    println!("=== Chain Validation ===");
    if is_chain_valid(&blockchain) {
        println!("Chain is VALID!");
    } else {
        println!("Chain is INVALID!");
    }

    // 程式結束時，blockchain 陣列與字串會自動被 Rust 釋放，無須手動 Free
}