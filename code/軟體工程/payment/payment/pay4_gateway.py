import os
import sys
from .models import PaymentRequest, PaymentResponse, PaymentStatus


class Pay4Gateway:
    """自建金流 Pay4 閘道 — 將 payment/ 的統一介面接到 pay4 系統"""

    def __init__(self, db_path: str | None = None):
        if db_path:
            os.environ["PAY4_DB_PATH"] = db_path
        self._ensure_pay4_importable()
        from pay4.database import ensure_tables
        ensure_tables()
        from pay4.core.merchant import create_merchant, list_merchants
        merchants = list_merchants()
        if merchants:
            self.merchant = merchants[0]
        else:
            self.merchant = create_merchant("payment_gateway", "gw@pay4.local")

    def _ensure_pay4_importable(self):
        pay4_dir = os.path.join(os.path.dirname(__file__), "..", "..", "pay4")
        pay4_dir = os.path.abspath(pay4_dir)
        if pay4_dir not in sys.path:
            sys.path.insert(0, pay4_dir)

    def create_payment(self, req: PaymentRequest) -> PaymentResponse:
        try:
            from pay4.core.engine import create_order, pay_order
            pay4_order = create_order(
                self.merchant.id, req.amount, req.currency,
                description=f"[payment-gw] {req.order_id}: {req.description or ''}",
            )
            paid = pay_order(pay4_order.id)
            return PaymentResponse(
                status=PaymentStatus.SUCCESS,
                gateway="pay4",
                payment_id=paid.id,
                order_id=req.order_id,
                amount=paid.amount,
                currency=paid.currency,
            )
        except Exception as e:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="pay4",
                payment_id="",
                order_id=req.order_id,
                amount=req.amount,
                currency=req.currency,
                error_message=str(e),
            )

    def refund(self, payment_id: str, amount: int | None = None) -> PaymentResponse:
        try:
            from pay4.core.engine import refund_order, get_order
            order = get_order(payment_id)
            if not order:
                raise ValueError(f"pay4 order not found: {payment_id}")
            refunded = refund_order(payment_id)
            return PaymentResponse(
                status=PaymentStatus.REFUNDED,
                gateway="pay4",
                payment_id=refunded.id,
                order_id=refunded.merchant_id,
                amount=refunded.amount,
                currency=refunded.currency,
            )
        except Exception as e:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="pay4",
                payment_id=payment_id,
                order_id="",
                amount=0,
                currency="TWD",
                error_message=str(e),
            )
