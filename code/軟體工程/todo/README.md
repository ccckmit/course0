# Todo 範例專案

同一個 Todo 應用的多種實作，展示語言、框架、架構對開發體驗與執行效能的影響。

## 專案一覽

```
todo/
├── frontend/           # React + Vite 前端 (與任一後端搭配)
├── todo-py/            # Python FastAPI + SQLite
├── todo-rust/          # Rust Axum + SQLite (rusqlite)
└── todo-rust-fast/     # Rust Axum + sled + Redis + Nginx
```

| 專案 | 語言 | HTTP 框架 | 資料庫 | 快取 | 反向代理 | 壓縮 | 相對吞吐量 |
|------|------|-----------|--------|------|---------|------|-----------|
| todo-py | Python 3.12 | FastAPI | SQLite | 無 | 無 | 無 | 1x |
| todo-rust | Rust | Axum 0.7 | SQLite (rusqlite) | 無 | 無 | 無 | ~10x |
| todo-rust-fast | Rust | Axum 0.7 | sled (mmap) | Redis (選用) | Nginx (選用) | brotli+gzip | ~37x |

## 架構

```
┌─────────────────────────────────────────────────────────┐
│                    frontend/ (React + Vite)              │
│               http://localhost:5173                      │
└─────────────────────┬───────────────────────────────────┘
                      │  /api/todos
                      ▼
┌─────────────────────────────────────────────────────────┐
│  todo-py/            todo-rust/        todo-rust-fast/  │
│  FastAPI + SQLite    Axum + rusqlite   Axum + sled      │
│  :8000               :8000             :8000             │
│                                         ↓               │
│                                    Redis (選用)          │
│                                         ↓               │
│                                    sled (mmap)          │
└─────────────────────────────────────────────────────────┘
                      │
                      ▼ (選用)
              Nginx (proxy_cache, keepalive, gzip)
              http://todo.local
```

三個後端共用同一 REST API 合約 (`/api/todos`)，frontend 可搭配任一後端使用（透過 Vite proxy 或直接設定 API URL）。

## 快速入門

```bash
# Python 版
cd todo-py
pip install -r requirements.txt
./test.sh
python -m todo add "hello"
uvicorn todo.api:app

# Rust 版
cd todo-rust
cargo test
cargo run -- add "hello"
cargo run -- serve

# Rust 高速版
cd todo-rust-fast
cargo test
cargo run -- add "hello"
cargo run -- serve

# 前端 (搭配任一後端)
cd frontend
npm install && npm run dev
```

## 開發順序

遵循 CLI → REST API → React 前端的三層模式：

1. **CLI** — 先有命令列操作 (`add/list/done/delete`)
2. **REST API** — 包裝成 HTTP 介面 (`GET/POST/PUT/DELETE /api/todos`)
3. **前端** — React 消費 REST API

## 效能對比

| 指標 | todo-py | todo-rust | todo-rust-fast |
|------|---------|-----------|----------------|
| 啟動時間 | ~500ms | ~8ms | ~10ms |
| 記憶體 (idle) | ~80MB | ~5MB | ~12MB |
| 吞吐量 (GET) | ~12k req/s | ~128k req/s | ~450k req/s |
| 延遲 p99 | ~15ms | ~1.2ms | ~0.2ms |

詳見各專案內的 `BENCHMARKS.md`。

## 設計原則

- **KISS** — 最小依賴、簡單實作
- **測試先行** — 每個專案有 `test.sh` 統一測試入口
- **逐步優化** — Python → Rust → sled → Redis → Nginx，每層可獨立理解
