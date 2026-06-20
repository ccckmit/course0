# Payment 金流範例 — AI 操作手冊

展示 Stripe 與綠界 (ECPay) 兩種金流閘道的整合方式。

## 結構

```
payment/
├── payment/
│   ├── __init__.py
│   ├── models.py         # 共用資料模型 (PaymentRequest, PaymentResponse)
│   ├── stripe_gateway.py # Stripe 金流整合
│   ├── ecpay_gateway.py  # 綠界金流整合
│   ├── gateway.py        # 統一處理器 (Provider 模式)
│   └── cli.py            # CLI 測試介面
├── tests/
│   ├── test_stripe.py    # Stripe 單元測試 (mock)
│   └── test_ecpay.py     # 綠界單元測試 (CheckMacValue, callback)
├── test.sh               # 測試入口
├── requirements.txt
├── AGENTS.md
└── README.md
```

## Commands

```bash
pip install -r requirements.txt   # 安裝依賴
./test.sh                          # 執行測試 (mock, 不需真實金流帳號)
python -m payment.cli pay --provider stripe --order-id ORD-001 --amount 5000
python -m payment.cli pay --provider ecpay --order-id ORD-002 --amount 500
python -m payment.cli refund --provider stripe --payment-id pi_abc123
```

## 開發紀律

- 金流實作順序: 閘道封裝 → 統一處理器 → CLI
- 測試用 pytest + mock，不呼叫真實 API
- 修改後跑 `./test.sh`
- 金流相關常數 (HashKey, API Key) 只放在環境變數或設定檔，不寫死在程式碼
