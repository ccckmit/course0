---
tags: [Rust, 測試, cargo, 整合測試]
date: 2026-06-20
---

# Rust 測試

Rust 語言內建測試框架，不需額外安裝測試函式庫。配合 `cargo test` 指令，可執行單元測試、整合測試、文件測試。

## 測試類型

### 1. 單元測試 (Unit Tests)

寫在原始碼檔案中，使用 `#[cfg(test)]` 條件編譯：

```rust
// src/database.rs
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_add_todo() {
        let db = database::connect(":memory:");
        db.add("hello").unwrap();
        let items = db.list().unwrap();
        assert_eq!(items.len(), 1);
    }
}
```

- `#[cfg(test)]` 確保測試程式碼不會編入正式版本
- `#[test]` 標記測試函式
- `assert_eq!`、`assert!`、`assert_ne!` 等巨集做斷言
- `#[should_panic]` 標記預期 panic 的測試

### 2. 整合測試 (Integration Tests)

放在專案根目錄的 `tests/` 資料夾下：

```rust
// tests/cli_test.rs
use std::process::Command;

#[test]
fn test_add() {
    let output = Command::new("cargo")
        .args(["run", "--quiet", "--", "add", "hello"])
        .output()
        .expect("cargo run failed");
    assert!(String::from_utf8_lossy(&output.stdout).contains("Added: #1"));
}
```

- 每個 `tests/*.rs` 檔案編譯為獨立的測試二進位
- 可以測試公開 API 的行為（黑箱測試）
- `tests/` 下的檔案不共用程式碼（若要共用，需使用 `tests/common/mod.rs` 模式）

### 3. 文件測試 (Doc Tests)

寫在文件註解中，同時作為使用範例和測試：

```rust
/// 將兩數相加
///
/// ```
/// let result = crate::add(2, 3);
/// assert_eq!(result, 5);
/// ```
pub fn add(a: i32, b: i32) -> i32 {
    a + b
}
```

- 使用 `cargo test` 自動執行
- 文件中的任何 Rust 程式碼區塊都會被編譯並執行
- 確保 API 範例永遠是最新的

## cargo test 指令

```bash
cargo test                    # 執行所有測試
cargo test test_add           # 執行名稱包含 test_add 的測試
cargo test -- --nocapture     # 顯示 stdout
cargo test -- --test-threads=1  # 單執行緒執行
cargo test --test cli_test    # 只執行 tests/cli_test.rs
cargo test --doc              # 只執行文件測試
```

## Todo-Rust 的測試模式

`code/軟體工程/todo/todo-rust/` 專案採用 CLI 子行程測試：

```rust
static COUNTER: AtomicU32 = AtomicU32::new(0);

fn fresh_db() -> String {
    let n = COUNTER.fetch_add(1, Ordering::SeqCst);
    format!("/tmp/test_rust_todo_{}.db", n)
}

fn todo(args: &[&str], db: &str) -> String {
    let output = Command::new("cargo")
        .args(["run", "--quiet", "--"])
        .args(args)
        .env("TODO_DB_PATH", db)
        .output()
        .expect("cargo run failed");
    String::from_utf8_lossy(&output.stdout).to_string()
}
```

測試案例涵蓋：

| 測試 | 說明 |
|------|------|
| `test_add` | 新增待辦事項 |
| `test_list_empty` | 空列表顯示 "No todos" |
| `test_add_then_list` | 新增多筆後列出 |
| `test_done` | 標記完成 |
| `test_done_not_found` | 不存在的編號 |
| `test_delete` | 刪除待辦事項 |

## 與 pytest 對比

| 面向 | pytest (Python) | cargo test (Rust) |
|------|----------------|-------------------|
| 安裝 | `pip install pytest` | 內建 (no install) |
| 測試發現 | 自動發現 `test_*.py` | 自動發現 `#[test]` + `tests/*.rs` |
| 斷言 | `assert x == y` | `assert_eq!(x, y)` |
| Fixture | `@pytest.fixture` | 自行實作 (如 fresh_db) |
| 參數化 | `@pytest.mark.parametrize` | 需自行迭代 |
| 非同步 | `pytest-asyncio` | `tokio::test` (attribute) |
| 覆蓋率 | `pytest-cov` | `tarpaulin` / `llvm-cov` |
| 隔離 | 函數間共享狀態 | 每個測試獨立二進位 (整合測試) |

## 測試配置

Cargo.toml 中的 dev-dependencies：

```toml
[dev-dependencies]
reqwest = { version = "0.12", features = ["json"] }  # 用於 API 整合測試
```

## 相關主題

- [測試](測試.md) — 測試策略總覽
- [E2E測試](E2E測試.md) — 端到端測試（Playwright）
- [持續整合](持續整合.md) — CI pipeline
- [todo-rust 範例](../../code/軟體工程/todo/todo-rust/) — 完整 Rust 專案
