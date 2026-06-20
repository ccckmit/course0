# MyPay 自建金流系統 — AI 操作手冊

展示一套從頭實作的金流引擎：商家管理、訂單收款、帳務帳本、清算撥款、Webhook。

## 結構

```
pay4/
├── pay4/
│   ├── __init__.py
│   ├── models.py         # Pydantic 資料模型
│   ├── database.py       # SQLite 儲存層
│   ├── cli.py            # CLI 管理介面
│   ├── api.py            # FastAPI REST API
│   └── core/
│       ├── __init__.py
│       ├── merchant.py   # 商家管理 (create/get/list/api_key)
│       ├── engine.py     # 付款引擎 (create/pay/refund/expire)
│       ├── ledger.py     # 帳務帳本 (double-entry)
│       ├── settlement.py # 清算撥款 (batch settlement)
│       └── webhook.py    # Webhook 發送 (queue + retry)
├── tests/
│   ├── test_merchant.py
│   ├── test_engine.py
│   ├── test_ledger.py
│   └── test_settlement.py
├── test.sh
├── requirements.txt
├── AGENTS.md
└── README.md
```

## Commands

```bash
pip install -r requirements.txt   # 安裝依賴
./test.sh                          # 執行測試

# CLI 管理
python -m pay4.cli init
python -m pay4.cli merchant create my_shop shop@test.com
python -m pay4.cli merchant list
python -m pay4.cli order create <merchant_id> 1000
python -m pay4.cli order pay <order_id>
python -m pay4.cli order refund <order_id>
python -m pay4.cli settle create <merchant_id>
python -m pay4.cli balance <account>
python -m pay4.cli ledger <merchant_id>

# REST API
uvicorn pay4.api:app --reload
```

## 開發紀律

- 實作順序: 核心模組 → CLI → REST API
- 測試用 pytest + SQLite `:memory:`，不產生真實檔案
- 修改後跑 `./test.sh`
- 遵循雙式記帳原則，每筆交易不可篡改
