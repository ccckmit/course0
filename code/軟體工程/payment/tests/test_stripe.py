from payment.models import PaymentRequest
from payment.stripe_gateway import StripeGateway
from unittest.mock import patch, MagicMock
import stripe


def make_gateway():
    return StripeGateway(api_key="sk_test_mock", webhook_secret="whsec_mock")


class TestStripeCreatePayment:
    def test_success(self):
        gw = make_gateway()
        req = PaymentRequest(order_id="ORD-001", amount=5000, currency="usd", description="test product")
        mock_intent = MagicMock()
        mock_intent.id = "pi_abc123"
        mock_intent.amount = 5000
        mock_intent.currency = "usd"

        with patch("stripe.PaymentIntent.create", return_value=mock_intent):
            resp = gw.create_payment(req)

        assert resp.gateway == "stripe"
        assert resp.status.value == "pending"
        assert resp.payment_id == "pi_abc123"
        assert resp.amount == 5000

    def test_stripe_error(self):
        gw = make_gateway()
        req = PaymentRequest(order_id="ORD-002", amount=5000, currency="usd")

        with patch("stripe.PaymentIntent.create", side_effect=stripe.error.StripeError("card_declined")):
            resp = gw.create_payment(req)

        assert resp.status.value == "failed"
        assert "card_declined" in resp.error_message


class TestStripeHandleWebhook:
    def test_payment_success_event(self):
        gw = make_gateway()
        payload = b'{"type":"payment_intent.succeeded","data":{"object":{"id":"pi_abc","amount":5000,"currency":"usd","metadata":{"order_id":"ORD-001"}}}}'
        mock_event = {
            "type": "payment_intent.succeeded",
            "data": {
                "object": {
                    "id": "pi_abc",
                    "amount": 5000,
                    "currency": "usd",
                    "metadata": {"order_id": "ORD-001"},
                }
            },
        }

        with patch("stripe.Webhook.construct_event", return_value=mock_event):
            result = gw.handle_webhook(payload, "test_sig")

        assert result["type"] == "payment_success"
        assert result["payment_id"] == "pi_abc"
        assert result["order_id"] == "ORD-001"

    def test_payment_failed_event(self):
        gw = make_gateway()
        payload = b'{"type":"payment_intent.payment_failed","data":{"object":{"id":"pi_def","last_payment_error":{"message":"insufficient_funds"}}}}'
        mock_event = {
            "type": "payment_intent.payment_failed",
            "data": {
                "object": {
                    "id": "pi_def",
                    "last_payment_error": {"message": "insufficient_funds"},
                }
            },
        }

        with patch("stripe.Webhook.construct_event", return_value=mock_event):
            result = gw.handle_webhook(payload, "test_sig")

        assert result["type"] == "payment_failed"
        assert result["payment_id"] == "pi_def"

    def test_unknown_event(self):
        gw = make_gateway()
        mock_event = {"type": "charge.refunded", "data": {"object": {}}}

        with patch("stripe.Webhook.construct_event", return_value=mock_event):
            result = gw.handle_webhook(b"{}", "test_sig")

        assert result["status"] == "unhandled"


class TestStripeRefund:
    def test_refund_success(self):
        gw = make_gateway()
        mock_refund = MagicMock()
        mock_refund.id = "re_abc"
        mock_refund.amount = 5000
        mock_refund.currency = "usd"

        with patch("stripe.Refund.create", return_value=mock_refund):
            resp = gw.refund("pi_abc")

        assert resp.status.value == "refunded"
        assert resp.payment_id == "re_abc"

    def test_refund_error(self):
        gw = make_gateway()
        with patch("stripe.Refund.create", side_effect=stripe.error.StripeError("already_refunded")):
            resp = gw.refund("pi_abc")

        assert resp.status.value == "failed"
