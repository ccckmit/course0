from payment.models import PaymentRequest
from payment.ecpay_gateway import ECPayGateway


def make_gateway():
    return ECPayGateway(
        merchant_id="2000132",
        hash_key="5294y06JbISpM5x9",
        hash_iv="v77hoKGq4kWxNNIS",
    )


class TestECPayCheckMacValue:
    def test_generates_consistent_hash(self):
        gw = make_gateway()
        params = {
            "MerchantID": "2000132",
            "MerchantTradeNo": "ORD-001",
            "TotalAmount": "100",
        }
        cmv1 = gw._generate_check_mac_value(params)
        cmv2 = gw._generate_check_mac_value(params)
        assert cmv1 == cmv2
        assert len(cmv1) == 64  # SHA256 hex

    def test_order_matters(self):
        gw = make_gateway()
        a = {"a": "1", "b": "2"}
        b = {"b": "2", "a": "1"}
        assert gw._generate_check_mac_value(a) == gw._generate_check_mac_value(b)


class TestECPayCreatePayment:
    def test_returns_form_params(self):
        gw = make_gateway()
        req = PaymentRequest(order_id="ORD-001", amount=500, description="測試商品")
        form = gw.create_payment_form(req)

        assert form["MerchantID"] == "2000132"
        assert form["MerchantTradeNo"] == "ORD-001"
        assert form["TotalAmount"] == "500"
        assert form["ItemName"] == "測試商品"
        assert "CheckMacValue" in form
        assert len(form["CheckMacValue"]) == 64

    def test_check_mac_value_is_last_param(self):
        gw = make_gateway()
        req = PaymentRequest(order_id="ORD-002", amount=1000)
        form = gw.create_payment_form(req)
        keys = list(form.keys())
        assert keys[-1] == "CheckMacValue"


class TestECPayCallback:
    def test_valid_callback(self):
        gw = make_gateway()
        req = PaymentRequest(order_id="ORD-003", amount=200)
        form = gw.create_payment_form(req)

        callback_params = {
            "MerchantID": "2000132",
            "MerchantTradeNo": "ORD-003",
            "TradeNo": "TX123456",
            "TradeAmt": "200",
            "RtnCode": "1",
            "PaymentType": "Credit_CreditCard",
            "PaymentDate": "2026/06/20 14:30:00",
        }
        callback_params["CheckMacValue"] = gw._generate_check_mac_value(callback_params)

        resp = gw.handle_callback(callback_params)
        assert resp.status.value == "success"
        assert resp.payment_id == "TX123456"
        assert resp.amount == 200

    def test_invalid_check_mac_value(self):
        gw = make_gateway()
        params = {
            "MerchantID": "2000132",
            "MerchantTradeNo": "ORD-004",
            "TradeNo": "TX999999",
            "TradeAmt": "500",
            "RtnCode": "1",
            "CheckMacValue": "INVALID_HASH",
        }
        resp = gw.handle_callback(params)
        assert resp.status.value == "failed"
        assert "驗證失敗" in resp.error_message

    def test_failed_payment(self):
        gw = make_gateway()
        params = {
            "MerchantID": "2000132",
            "MerchantTradeNo": "ORD-005",
            "TradeNo": "TX555",
            "TradeAmt": "500",
            "RtnCode": "0",
            "PaymentType": "ATM",
        }
        params["CheckMacValue"] = gw._generate_check_mac_value(params)

        resp = gw.handle_callback(params)
        assert resp.status.value == "failed"

    def test_verify_callback_utility(self):
        gw = make_gateway()
        params = {
            "MerchantID": "2000132",
            "MerchantTradeNo": "ORD-006",
            "TotalAmount": "1000",
        }
        params["CheckMacValue"] = gw._generate_check_mac_value(params)
        assert gw.verify_callback(params) is True

        params["CheckMacValue"] = "TAMPERED"
        assert gw.verify_callback(params) is False


class TestECPayRefund:
    def test_refund_returns_notice(self):
        gw = make_gateway()
        resp = gw.refund("ORD-007", 500)
        assert resp.status.value == "refunded"
        assert "請使用 ECPay SDK" in resp.error_message
