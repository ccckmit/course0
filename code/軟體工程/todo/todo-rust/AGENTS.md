# Todo Rust 版 — AI 操作手冊

與 FastAPI 版功能完全相同的 Rust 實作，用於比較語言對開發效率和執行效能的影響。

## Commands

```bash
cargo run -- add "text"     # CLI 新增
cargo run -- list           # CLI 列表
cargo run -- done 1         # CLI 標記完成
cargo run -- delete 1       # CLI 刪除
cargo run -- serve          # 啟動 API (http://0.0.0.0:8000)
cargo test                  # 執行測試
```

## 與 FastAPI 版對比

| 面向 | FastAPI (Python) | Rust (Axum) |
|------|-----------------|-------------|
| 啟動時間 | ~0.5s | ~0.01s (編譯後) |
| 記憶體 | ~80MB | ~5MB |
| 吞吐量 | ~10k req/s | ~100k req/s |
| 依賴安裝 | pip install | cargo build (慢但一次) |
| 開發速度 | 快 | 慢（編譯） |
| 型別安全 | 執行期 | 編譯期 |
