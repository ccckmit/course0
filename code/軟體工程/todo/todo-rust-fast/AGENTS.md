# todo-rust-fast — Ultra-Fast Todo Backend

## Build & Test

```bash
cargo build --release        # 編譯 Release
cargo test                   # 執行測試 (7 tests)
./test.sh                    # 同上
```

## 架構

```
Client → Nginx (proxy, cache, gzip)
           → Axum (tokio, brotli/gzip, keep-alive)
               → Redis Cache (redis-rs connection manager)
                   → sled Database (embedded, lock-free, mmap I/O)
```

## 效能特徵

| 特性 | todo-rust (SQLite) | todo-rust-fast (sled+Redis+Nginx) | 改善 |
|------|-------------------|-----------------------------------|------|
| 儲存引擎 | SQLite WAL | sled (lock-free, mmap) | 更強並發 |
| 快取 | 無 | Redis (redis-rs, 10s TTL) | 讀取加速 |
| 反向代理 | 無 | Nginx (proxy_cache, keepalive 64) | 連線重用 |
| 壓縮 | 無 | brotli + gzip | 頻寬節省 |
| HTTP/2 | 無 | Axum 原生 | 多工 |

## CLI 使用

```bash
cargo run -- add "買牛奶"
cargo run -- list
cargo run -- done 1
cargo run -- delete 1
cargo run -- serve        # 啟動 API 伺服器 (port 8000)
```

## 環境變數

- `TODO_DB_PATH` — sled 資料庫路徑 (預設: `todo-fast.db`)
- `REDIS_URL` — Redis 連線字串 (如未設定則無快取)

## Redis 測試

```bash
docker run -d -p 6379:6379 redis:7-alpine
REDIS_URL=redis://127.0.0.1:6379 cargo run -- serve
```
