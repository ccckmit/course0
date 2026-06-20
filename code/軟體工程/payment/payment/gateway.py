from .models import PaymentRequest, PaymentResponse, PaymentStatus


class PaymentGateway:
    """統一的付款處理器 — 支援多個金流閘道後端"""

    def __init__(self, providers: dict):
        self.providers = providers

    def create_payment(self, req: PaymentRequest, provider: str = "stripe") -> PaymentResponse:
        gw = self.providers.get(provider)
        if not gw:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway=provider,
                payment_id="",
                order_id=req.order_id,
                amount=req.amount,
                currency=req.currency,
                error_message=f"unknown provider: {provider}",
            )
        return gw.create_payment(req)

    def refund(self, payment_id: str, provider: str = "stripe", amount: int | None = None) -> PaymentResponse:
        gw = self.providers.get(provider)
        if not gw:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway=provider,
                payment_id=payment_id,
                order_id="",
                amount=0,
                currency="usd",
                error_message=f"unknown provider: {provider}",
            )
        return gw.refund(payment_id, amount)
