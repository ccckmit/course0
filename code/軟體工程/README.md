# 軟體工程 — 程式碼與約定

本目錄包含 AI 導向軟體工程的程式碼約定與範例專案。

## 核心約定

- [ccc_code_skill.md](./ccc_code_skill.md) — 寫程式約定（測試、lint、KISS、CLI→REST→React）
- [ccc_doc_skill.md](./ccc_doc_skill.md) — 寫文件約定（wiki、README、AGENTS.md）

## 範例專案

### todo-py/ — Python (FastAPI + SQLite + React)

完整展示 CLI → REST → React 三層架構，使用 FastAPI + SQLite (stdlib) + React (Vite)。

```
todo-py/
├── todo/cli.py          # Git 風格 CLI
├── todo/api.py          # FastAPI REST API
├── todo/database.py     # SQLite 儲存層
├── tests/test_cli.py    # pytest CLI 測試 (7 項)
├── tests/test_api.py    # TestClient API 測試 (6 項)
├── test.sh              # 統一測試入口
└── AGENTS.md            # AI 操作手冊
```

```bash
cd todo/todo-py
pip install -r requirements.txt
./test.sh                         # 13 tests pass
python -m todo add "hello"        # CLI
uvicorn todo.api:app              # API at :8000
cd ../frontend && npm run dev     # Frontend at :5173
```

### todo-rust/ — Rust (Axum + SQLite)

與 Python 版完全相同的功能，使用 Rust + Axum + rusqlite，用於對比語言效能。

```
todo-rust/
├── src/
│   ├── main.rs          # CLI + API 入口
│   ├── api.rs           # Axum REST API
│   ├── cli.rs           # Git 風格 CLI (clap)
│   └── database.rs      # SQLite 儲存層 (rusqlite)
├── tests/cli_test.rs    # 整合測試 (6 項)
├── test.sh              # 統一測試入口
├── AGENTS.md            # AI 操作手冊
└── BENCHMARKS.md        # Rust vs Python 效能對比
```

```bash
cd todo/todo-rust
cargo build                        # 首次建置 ~120s
cargo test                         # 6 tests pass
cargo run -- add "hello"           # CLI
cargo run -- serve                 # API at :8000
```

### 效能對比重點

| 指標 | Python (FastAPI) | Rust (Axum) |
|------|-----------------|-------------|
| 啟動時間 | ~500ms | ~8ms |
| 記憶體 (idle) | ~80MB | ~5MB |
| 吞吐量 | ~12k req/s | ~128k req/s |
| 延遲 p99 | ~15ms | ~1.2ms |

詳見 [todo/todo-rust/BENCHMARKS.md](./todo/todo-rust/BENCHMARKS.md)。

## 相關資源

- [wiki/軟體工程/](https://github.com/ccckmit/course0/tree/main/wiki/%E8%BB%9F%E9%AB%94%E5%B7%A5%E7%A8%8B) — 詳細知識庫
- [lecture/軟體工程/](https://github.com/ccckmit/course0/tree/main/lecture/%E8%BB%9F%E9%AB%94%E5%B7%A5%E7%A8%8B) — 課程講稿
- [The Modern Software](https://themodernsoftware.dev/) — 課程參考教材
