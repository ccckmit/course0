import os
import pytest
import tempfile
from payment.models import PaymentRequest
from payment.pay4_gateway import Pay4Gateway


@pytest.fixture
def gw():
    db = os.path.join(tempfile.gettempdir(), f"test_pay4_{os.getpid()}.db")
    # clean up any previous test db
    if os.path.exists(db):
        os.remove(db)
    gateway = Pay4Gateway(db_path=db)
    yield gateway
    if os.path.exists(db):
        os.remove(db)


class TestPay4CreatePayment:
    def test_success(self, gw):
        req = PaymentRequest(order_id="ORD-001", amount=5000, description="test pay4 payment")
        resp = gw.create_payment(req)
        assert resp.gateway == "pay4"
        assert resp.status.value == "success"
        assert resp.order_id == "ORD-001"
        assert resp.amount == 5000
        assert resp.payment_id.startswith("o_")

    def test_multiple_payments(self, gw):
        r1 = gw.create_payment(PaymentRequest(order_id="ORD-001", amount=1000))
        r2 = gw.create_payment(PaymentRequest(order_id="ORD-002", amount=2000))
        assert r1.status.value == "success"
        assert r2.status.value == "success"
        assert r1.payment_id != r2.payment_id


class TestPay4Refund:
    def test_refund_after_payment(self, gw):
        req = PaymentRequest(order_id="ORD-010", amount=3000)
        paid = gw.create_payment(req)
        assert paid.status.value == "success"

        refund_resp = gw.refund(paid.payment_id)
        assert refund_resp.status.value == "refunded"
        assert refund_resp.payment_id == paid.payment_id

    def test_refund_nonexistent(self, gw):
        resp = gw.refund("nonexistent")
        assert resp.status.value == "failed"
        assert "not found" in resp.error_message

    def test_refund_unpaid(self, gw):
        from pay4.core.engine import create_order
        order = create_order(gw.merchant.id, 1000)
        resp = gw.refund(order.id)
        assert resp.status.value == "failed"


class TestPay4DbIsolation:
    def test_separate_gateways_have_separate_merchants(self):
        db1 = os.path.join(tempfile.gettempdir(), f"test_pay4_a_{os.getpid()}.db")
        db2 = os.path.join(tempfile.gettempdir(), f"test_pay4_b_{os.getpid()}.db")
        gw1 = Pay4Gateway(db_path=db1)
        gw2 = Pay4Gateway(db_path=db2)
        assert gw1.merchant.id != gw2.merchant.id
        for d in [db1, db2]:
            if os.path.exists(d):
                os.remove(d)
