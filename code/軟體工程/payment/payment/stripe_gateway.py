import stripe
from .models import PaymentRequest, PaymentResponse, PaymentStatus


class StripeGateway:
    def __init__(self, api_key: str, webhook_secret: str):
        stripe.api_key = api_key
        self.webhook_secret = webhook_secret

    def create_payment(self, req: PaymentRequest) -> PaymentResponse:
        try:
            intent = stripe.PaymentIntent.create(
                amount=req.amount,
                currency=req.currency.lower(),
                description=req.description,
                metadata={"order_id": req.order_id},
            )
            return PaymentResponse(
                status=PaymentStatus.PENDING,
                gateway="stripe",
                payment_id=intent.id,
                order_id=req.order_id,
                amount=intent.amount,
                currency=intent.currency,
            )
        except stripe.error.StripeError as e:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="stripe",
                payment_id="",
                order_id=req.order_id,
                amount=req.amount,
                currency=req.currency,
                error_message=str(e),
            )

    def confirm_payment(self, payment_intent_id: str) -> PaymentResponse:
        try:
            intent = stripe.PaymentIntent.confirm(payment_intent_id)
            return PaymentResponse(
                status=PaymentStatus.SUCCESS if intent.status == "succeeded" else PaymentStatus.PENDING,
                gateway="stripe",
                payment_id=intent.id,
                order_id=intent.metadata.get("order_id", ""),
                amount=intent.amount,
                currency=intent.currency,
            )
        except stripe.error.StripeError as e:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="stripe",
                payment_id=payment_intent_id,
                order_id="",
                amount=0,
                currency="usd",
                error_message=str(e),
            )

    def handle_webhook(self, payload: bytes, sig_header: str) -> dict:
        event = stripe.Webhook.construct_event(payload, sig_header, self.webhook_secret)
        handler = {
            "payment_intent.succeeded": self._on_payment_success,
            "payment_intent.payment_failed": self._on_payment_failed,
        }
        fn = handler.get(event["type"])
        return fn(event["data"]["object"]) if fn else {"type": event["type"], "status": "unhandled"}

    def _on_payment_success(self, intent: dict) -> dict:
        return {
            "type": "payment_success",
            "payment_id": intent["id"],
            "order_id": intent["metadata"].get("order_id", ""),
            "amount": intent["amount"],
            "currency": intent["currency"],
        }

    def _on_payment_failed(self, intent: dict) -> dict:
        return {
            "type": "payment_failed",
            "payment_id": intent["id"],
            "error": intent.get("last_payment_error", {}).get("message", "unknown"),
        }

    def refund(self, payment_id: str, amount: int | None = None) -> PaymentResponse:
        try:
            refund = stripe.Refund.create(payment_intent=payment_id, amount=amount)
            return PaymentResponse(
                status=PaymentStatus.REFUNDED,
                gateway="stripe",
                payment_id=refund.id,
                order_id="",
                amount=refund.amount,
                currency=refund.currency,
            )
        except stripe.error.StripeError as e:
            return PaymentResponse(
                status=PaymentStatus.FAILED,
                gateway="stripe",
                payment_id=payment_id,
                order_id="",
                amount=0,
                currency="usd",
                error_message=str(e),
            )
