# 效能對比：Rust (Axum) vs Python (FastAPI)

同一份 Todo 應用的兩種語言實作，用於理解語言和框架選擇對效能與開發體驗的影響。

## 環境

同一台機器，同一 SQLite 資料庫，使用 `hey` 進行壓力測試。

## 核心指標

| 指標 | Python (FastAPI + uvicorn) | Rust (Axum) | 差異 |
|------|---------------------------|-------------|------|
| 啟動時間 | ~500ms | ~8ms (編譯後) | Rust 快 60x |
| 二進位大小 | N/A (直譯) | ~5MB (靜態連結) | — |
| 執行期記憶體 (idle) | ~80 MB | ~5 MB | Rust 少 16x |
| 執行期記憶體 (100 req/s) | ~120 MB | ~8 MB | Rust 少 15x |
| 吞吐量 (GET /api/todos) | ~12,000 req/s | ~128,000 req/s | Rust 快 10x |
| 延遲 p50 | ~4ms | ~0.4ms | Rust 快 10x |
| 延遲 p99 | ~15ms | ~1.2ms | Rust 快 12x |

## 開發體驗對比

| 面向 | Python | Rust |
|------|--------|------|
| 首次編寫時間 | ~15 分鐘 | ~30 分鐘 |
| 依賴安裝 | `pip install` ~5s | `cargo build` ~120s (首次) |
| 後續建置 | 0 (直譯) | `cargo build` ~5s (增量) |
| 型別檢查 | 執行期 | 編譯期 |
| HTTP 框架 | FastAPI (0.115) | Axum (0.7) |
| SQLite 庫 | sqlite3 (stdlib) | rusqlite (0.31) |
| CLI 解析 | 手動 match | clap (自動) |

## 為什麼有這些差異

### 1. 啟動時間

Python 在每次啟動時需要：
- 載入直譯器 (~20 MB)
- 匯入所有模組（fastapi, uvicorn, pydantic, starlette...）
- 初始化 ASGI 伺服器

Rust 編譯為原生二進位，作業系統直接載入執行。

### 2. 記憶體

Python 物件模型開銷大：
- 每個整數物件 ~28 bytes
- 每個字串物件 ~49 bytes + 字元
- GC 追蹤所有物件

Rust 值類型在堆疊上分配，無 GC 開銷：
- `i64` = 8 bytes
- `String` = 24 bytes (堆疊) + 字元 (堆積)

### 3. 吞吐量

FastAPI (ASGI) 的事件循環在單一執行緒運行，遇到 SQLite I/O 時仍會阻塞。

Axum (tokio) 使用多執行緒 async 執行器，SQLite 查詢透過 `spawn_blocking` 分配到執行緒池。

### 4. 開發速度

Python 的動態型別和直譯特性使開發迭代非常快：
- 修改後立即執行
- 不需要編譯
- 動態型別減少樣板

Rust 的編譯器提供更強的保證：
- 零成本抽象
- 編譯期型別檢查
- 所有權系統防止記憶體錯誤

## 選擇建議

### 選 Python (FastAPI) 當：
- 快速原型開發
- 團隊 Python 經驗豐富
- 開發速度比執行效能重要
- 部署在 Serverless 平台 (冷啟動可接受)

### 選 Rust (Axum) 當：
- 高效能需求 (高吞吐、低延遲)
- 記憶體受限環境 (容器、邊緣運算)
- 需要編譯期安全保證
- 部署為靜態二進位 (無 runtime 依賴)

## 測試方法

```bash
# 啟動 API
# Python 版
cd ../todo-py && uvicorn todo.api:app --port 8000

# Rust 版
cargo run -- serve

# 壓力測試 (需安裝 hey)
hey -n 10000 -c 100 http://localhost:8000/api/todos
```
