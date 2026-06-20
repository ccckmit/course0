import hashlib
import urllib.parse
from datetime import datetime
from typing import Optional
from .models import PaymentRequest, PaymentResponse, PaymentStatus


class ECPayGateway:
    """綠界科技 (ECPay) 第三方支付閘道整合

    綠界 AIO (All-In-One) API 流程：
    1. 伺服器產生表單資料 + CheckMacValue（防篡改檢查碼）
    2. 用戶在綠界頁面選擇付款方式並完成付款
    3. 綠界呼叫 ReturnURL（同步跳轉）和 PaymentInfoURL（非同步通知）
    4. 伺服器驗證回调的 CheckMacValue
    5. 根據 RtnCode 判斷付款結果 (1=成功)
    """

    def __init__(self, merchant_id: str, hash_key: str, hash_iv: str):
        self.merchant_id = merchant_id
        self.hash_key = hash_key
        self.hash_iv = hash_iv

    def _generate_check_mac_value(self, params: dict) -> str:
        sorted_params = sorted(params.items())
        raw = urllib.parse.urlencode(sorted_params)
        raw = f"HashKey={self.hash_key}&{raw}&HashIV={self.hash_iv}"
        raw = urllib.parse.quote(raw, safe='=&')
        return hashlib.sha256(raw.lower().encode('utf-8')).hexdigest().upper()

    def create_payment_form(self, req: PaymentRequest) -> dict:
        now = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
        params = {
            "MerchantID": self.merchant_id,
            "MerchantTradeNo": req.order_id,
            "MerchantTradeDate": now,
            "PaymentType": "aio",
            "TotalAmount": str(req.amount),
            "TradeDesc": req.description or "商品描述",
            "ItemName": req.description or "商品",
            "ReturnURL": "https://example.com/ecpay/return",
            "PaymentInfoURL": "https://example.com/ecpay/notify",
            "ChoosePayment": "ALL",
            "EncryptType": "1",
        }
        params["CheckMacValue"] = self._generate_check_mac_value(params)
        return params

    def verify_callback(self, params: dict) -> bool:
        params_copy = dict(params)
        received = params_copy.pop("CheckMacValue", "")
        expected = self._generate_check_mac_value(params_copy)
        return expected == received

    def handle_callback(self, params: dict) -> PaymentResponse:
        if not self.verify_callback(params):
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="ecpay",
                payment_id="",
                order_id=params.get("MerchantTradeNo", ""),
                amount=0,
                currency="TWD",
                error_message="CheckMacValue 驗證失敗",
            )
        rtn_code = params.get("RtnCode", "0")
        status = PaymentStatus.SUCCESS if rtn_code == "1" else PaymentStatus.FAILED
        return PaymentResponse(
            status=status,
            gateway="ecpay",
            payment_id=params.get("TradeNo", ""),
            order_id=params.get("MerchantTradeNo", ""),
            amount=int(params.get("TradeAmt", 0)),
            currency="TWD",
        )

    def query_trade(self, trade_no: str) -> PaymentResponse:
        params = {
            "MerchantID": self.merchant_id,
            "MerchantTradeNo": trade_no,
            "TimeStamp": str(int(datetime.now().timestamp())),
        }
        params["CheckMacValue"] = self._generate_check_mac_value(params)
        return PaymentResponse(
            status=PaymentStatus.PENDING,
            gateway="ecpay",
            payment_id=trade_no,
            order_id=trade_no,
            amount=0,
            currency="TWD",
            error_message="query_trade: 請使用 ECPay SDK 查詢實際交易狀態",
        )

    def refund(self, trade_no: str, amount: int) -> PaymentResponse:
        params = {
            "MerchantID": self.merchant_id,
            "MerchantTradeNo": trade_no,
            "RefundAmount": str(amount),
        }
        params["CheckMacValue"] = self._generate_check_mac_value(params)
        return PaymentResponse(
            status=PaymentStatus.REFUNDED,
            gateway="ecpay",
            payment_id=trade_no,
            order_id=trade_no,
            amount=amount,
            currency="TWD",
            error_message="refund: 請使用 ECPay SDK 執行實際退款",
        )
