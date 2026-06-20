# course0 - 陳鍾誠的課程教材

這是金門大學資訊工程系課程教材，覆蓋六門科目。**不是軟體專案** — 無 root-level 的 build/test/lint 指令。

## Structure

```
course0/
├── code/           # 程式碼範例 (每科有多個獨立子專案)
├── wiki/           # LLM 知識庫 (wiki/AGENTS.md 定義操作 schema)
├── lecture/        # 課程講稿
├── docs/html/      # GitHub Pages 部署來源 (static_ccc.yml)
└── docs/skill/     # OpenCode skills 定義
```

## Workflow

1. **問使用者要改哪個科目** (不要假設)
2. 查 `code/[subject]/` (程式), `lecture/[subject]/` (講稿), `wiki/` (知識庫)
3. 查該 project 下的 `AGENTS.md` 取得專屬 build/test 指令

## Key Facts

- **每 `code/[subject]/[project]/` 獨立** — 無跨專案依賴，各自可能有 `package.json`, `pyproject.toml`, `Makefile`, 或純單檔。
- **`code/README.md`** 是科目索引 — 若有專案遺漏，先讀它。
- `wiki/AGENTS.md` 仍包含舊 `_wiki/` 路徑引用 — 該 schema 描述的 `_wiki/` 實際路徑是 `wiki/`。
- **GitHub Pages**: 只從 `docs/html/` 直接部署 (`static_ccc.yml`)，無建置步驟。
- `docs/skill/` 裡的 skill 定義由 OpenCode 的 `course0` skill 載入。
- 無 root-level `opencode.json`, `.cursorrules`, `.github/copilot-instructions.md`。

## Sub-AGENTS.md (code/ 內有專屬指引的專案)

| 路徑 | 說明 |
|------|------|
| `code/系統程式/os/mini-riscv-os2/` | RISC-V OS (stages 01-09, `riscv64-unknown-elf-gcc`) |
| `code/系統程式/os/xv7/` | xv7 OS (`make qemu`, QEMU RISC-V) |
| `code/系統程式/database/sql0/` | SQL + B+Tree (`gcc -O3 sql0.c`, `./test.sh`) |
| `code/系統程式/crpyto/ssl0/` | SSL/TLS 框架 (`make test`, 目錄名 misspelled "crpyto") |
| `code/網站設計/05-form/blog1form/` | Node.js + Express + SQLite blog |
| `code/網站設計/05-form/blog4form/` | 同上進階版 |
| `code/機器學習/mini-llm/` | Toy LLM (pure PyTorch, `python mini-llm.py`) |
| `code/數學/_ai/lean4py/` | Python 形式數學庫 (`pip install -e .`, `pytest tests/`) |
| `code/軟體工程/todo/todo-py/` | Python FastAPI + SQLite + React (`./test.sh`) |
| `code/軟體工程/todo/todo-rust/` | Rust Axum + SQLite (`cargo test`) |
| `code/軟體工程/todo/todo-rust-fast/` | Rust Axum + sled + Redis + Nginx (`cargo test`) |

## Common Commands (run from individual project dirs)

```bash
python main.py           # Python entry point (most subjects)
python -m pytest         # Python tests
npm install && npm run dev  # Node.js projects
make                     # C projects with Makefile
make test                # C projects with test target
```

## Wiki Operations

詳見 `wiki/AGENTS.md`。快速參考:
- **Ingest**: 放來源到 `wiki/source/`，然後請 LLM 攝取
- **Query**: 搜 `wiki/` 找既有知識
- **Lint**: 檢查過時/矛盾/孤立頁面