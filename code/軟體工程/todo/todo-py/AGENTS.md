# Todo 範例專案 — AI 操作手冊

示範 CLI → REST → React 三層架構 + 完整測試的 Todo 應用。

## 結構

```
todo-py/
├── todo/
│   ├── cli.py       # CLI 介面 (todo add/list/done)
│   ├── api.py       # FastAPI REST API
│   ├── models.py    # Pydantic 資料模型
│   └── database.py  # SQLite 儲存層
├── tests/
│   ├── test_cli.py  # CLI 單元測試
│   ├── test_api.py  # API 整合測試
│   └── test_e2e.py  # Playwright E2E 測試
├── test.sh          # 統一測試入口
└── AGENTS.md        # 本文件
```

## Commands

```bash
pip install -r requirements.txt   # 安裝依賴
playwright install chromium        # Playwright 瀏覽器
./test.sh                          # 執行測試
python -m todo add "text"          # CLI 新增
python -m todo list                # CLI 列表
uvicorn todo.api:app               # 啟動 API (http://localhost:8000)
cd ../frontend && npm install && npm run dev  # 啟動前端
```

## 開發紀律

- 實作順序: CLI → API → 前端 (ccc_code_skill 第9條)
- CLI 用 `cmd op args` 模式
- 測試用 pytest, E2E 用 Playwright
- 修改後跑 `./test.sh`
