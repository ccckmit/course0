# 範例專案

## todo-py/ — Python (FastAPI + SQLite + React)

**狀態：已實作**，完整原始碼在 [todo/todo-py/](./todo/todo-py/)。

## todo-rust/ — Rust (Axum + SQLite)

**狀態：已實作**，完整原始碼在 [todo/todo-rust/](./todo/todo-rust/)。

同一份 Todo 應用的兩種語言實作。詳見 [todo/todo-rust/BENCHMARKS.md](./todo/todo-rust/BENCHMARKS.md) 效能對比。

## todo-rust-fast/ — Rust (sled + Redis + Nginx)

**狀態：已實作**，完整原始碼在 [todo/todo-rust-fast/](./todo/todo-rust-fast/)。

Ultra-fast 版本，使用 sled (lock-free 嵌入式資料庫) + Redis 快取 + Nginx 反向代理，比 Python 版快 ~37x。

## 對應概念

| 概念 | Python 版 | Rust (SQLite) 版 | Rust (sled+Redis) 版 |
|------|-----------|-----------------|---------------------|
| CLI 設計 | `todo/cli.py` (手動 match) | `cli.rs` (clap) | `cli.rs` (clap) |
| REST API | `todo/api.py` (FastAPI) | `api.rs` (Axum) | `api.rs` (Axum + 快取) |
| 資料庫 | `database.py` (sqlite3) | `database.rs` (rusqlite WAL) | `db.rs` (sled, bincode) |
| 快取 | 無 | 無 | `cache.rs` (Redis + 10s TTL) |
| CLI 測試 | `tests/test_cli.py` (7 項) | `tests/cli_test.rs` (6 項) | `tests/cli_test.rs` (6 項) |
| API 測試 | `tests/test_api.py` (TestClient) | 無 | 無 |
| 壓縮 | 無 | 無 | brotli + gzip |
| Nginx | 無 | 無 | proxy_cache + keepalive |
| test.sh | shell script | shell script | shell script |
| AGENTS.md | 有 | 有 | 有 |
| CI | GitHub Actions | GitHub Actions | GitHub Actions |
