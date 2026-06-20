# Payment 金流整合範例

展示 Stripe（國際）與綠界 ECPay（台灣）兩種金流閘道的 Python 整合方式。

## 快速開始

```bash
cd code/軟體工程/payment
pip install -r requirements.txt
./test.sh
```

## 範例內容

### Stripe Gateway (`payment/stripe_gateway.py`)

- `create_payment()` — 建立 PaymentIntent（信用卡授權）
- `confirm_payment()` — 確認付款
- `handle_webhook()` — 處理 Webhook 事件（支付成功/失敗）
- `refund()` — 退款

### 綠界 ECPay Gateway (`payment/ecpay_gateway.py`)

- `create_payment_form()` — 產生 CheckMacValue 表單資料
- `verify_callback()` — 驗證回調的檢查碼
- `handle_callback()` — 處理付款結果通知
- `query_trade()` / `refund()` — 查詢與退款

## 統一處理器

`PaymentGateway` 封裝多個金流後端，用 provider 字串選擇：

```python
gw = PaymentGateway({"stripe": ..., "ecpay": ...})
resp = gw.create_payment(req, provider="stripe")
```

## 測試

所有測試使用 mock，不須真實金流帳號即可執行：

```bash
./test.sh
```
