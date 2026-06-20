---
tags: [Rust, Axum, 效能, 型別安全, 系統程式]
date: 2026-06-20
---

# Rust 在軟體工程的應用

本課程使用 Rust 實作高效能後端服務，與 Python (FastAPI) 版本對比，展示語言選擇對開發體驗與執行效能的影響。

## 為什麼在軟體工程課程中使用 Rust

| 面向 | 說明 |
|------|------|
| 效能 | 編譯為原生碼，零成本抽象，吞吐量比 Python 高 10x |
| 型別安全 | 編譯期檢查，消滅空指標、釋放後使用等類別錯誤 |
| 所有權系統 | 不需 GC 就能保證記憶體安全 |
| 靜態二進位 | 單一檔案部署，無需 runtime 依賴 |
| 生態系 | Axum (Web)、rusqlite/sled (資料庫)、clap (CLI) |

## 工具鏈

```bash
rustc          # Rust 編譯器
cargo          # 套件管理 + 建置工具
cargo build    # 編譯
cargo run      # 編譯 + 執行
cargo test     # 執行測試
cargo fmt      # 自動格式化
cargo clippy   # lint 檢查
rustup         # 版本管理
```

## 專案結構 (以 todo-rust 為例)

```
todo-rust/
├── Cargo.toml          # 套件宣告 + 依賴
├── src/
│   ├── main.rs         # 入口：CLI 或 serve 模式
│   ├── api.rs          # Axum REST API
│   ├── cli.rs          # CLI 介面 (clap)
│   └── database.rs     # SQLite 儲存層 (rusqlite)
├── tests/
│   └── cli_test.rs     # 整合測試
├── test.sh
└── AGENTS.md
```

## Key Crates

### Axum (Web 框架)

```toml
[dependencies]
axum = "0.7"
tokio = { version = "1", features = ["full"] }
tower-http = { version = "0.5", features = ["cors", "compression-gzip"] }
```

- 基於 tokio async 執行器
- 多執行緒處理請求
- 使用 `Router` 建構 API：
  ```rust
  let app = Router::new()
      .route("/api/todos", get(list_todos).post(add_todo))
      .layer(CorsLayer::permissive());
  ```

### rusqlite (SQLite)

```toml
[dependencies]
rusqlite = { version = "0.31", features = ["bundled"] }
```

- `bundled` 功能編譯內建 SQLite，不需系統安裝
- Rust 所有權與 SQLite 生命週期：
  ```rust
  let conn = Connection::open(path)?;
  conn.execute("CREATE TABLE IF NOT EXISTS todos (...)")?;
  ```

### sled (嵌入式資料庫, todo-rust-fast)

```toml
[dependencies]
sled = "0.34"
```

- Lock-free, mmap-based 嵌入式 KV 儲存
- 比 SQLite 更快的並發讀取
- 適用於高吞吐量場景

### clap (CLI 解析)

```toml
[dependencies]
clap = { version = "4", features = ["derive"] }
```

- 使用 derive 巨集定義 CLI 參數：
  ```rust
  #[derive(Parser)]
  enum Commands {
      Add { text: String },
      List,
      Done { id: u32 },
  }
  ```

## 效能對比

| 指標 | Python (FastAPI) | Rust (Axum) | Rust + sled + Redis |
|------|-----------------|-------------|-------------------|
| 啟動時間 | ~500ms | ~8ms | ~10ms |
| 記憶體 (idle) | ~80MB | ~5MB | ~12MB |
| 吞吐量 | ~12k req/s | ~128k req/s | ~450k req/s |
| 延遲 p99 | ~15ms | ~1.2ms | ~0.2ms |
| 二進位大小 | N/A | ~5MB | ~7MB |

詳見 [BENCHMARKS.md](../../code/軟體工程/todo/todo-rust/BENCHMARKS.md)。

## 開發體驗

### 優點

- **編譯器是你的夥伴** — 編譯錯誤通常代表邏輯錯誤
- **重構信心** — 型別系統保證修改後不會遺漏
- **零成本抽象** — 高階語法不犧牲效能
- **單一二進位** — 部署只需複製一個檔案

### 挑戰

- **編譯時間** — 首次建置約 120 秒，增量約 5 秒
- **學習曲線** — 所有權、生命週期需要時間理解
- **表達力** — 某些動態模式需要更多程式碼

## 選擇建議

**選 Rust 當：**
- 需要高吞吐量 / 低延遲
- 記憶體受限環境 (容器、邊緣運算)
- 需要編譯期安全保證
- 部署為靜態二進位

**選 Python 當：**
- 快速原型開發
- 開發速度優先於執行效能
- 團隊 Python 經驗豐富
- 部署在 Serverless 平台

## 相關主題

- [Rust測試](Rust測試.md) — cargo test 與測試模式
- [測試](測試.md) — 測試策略總覽
- [持續整合](持續整合.md) — CI pipeline
- [todo-rust 範例](../../code/軟體工程/todo/todo-rust/) — 完整 Rust 專案
- [todo-rust-fast 範例](../../code/軟體工程/todo/todo-rust-fast/) — 高效能 Rust 專案
