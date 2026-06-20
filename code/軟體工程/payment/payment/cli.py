import argparse
import sys
import os
import tempfile
from .models import PaymentRequest
from .stripe_gateway import StripeGateway
from .ecpay_gateway import ECPayGateway
from .pay4_gateway import Pay4Gateway
from .gateway import PaymentGateway

_PAY4_DB: str | None = None


def _get_pay4_db() -> str:
    global _PAY4_DB
    if _PAY4_DB is None:
        _PAY4_DB = os.environ.get("PAY4_DB_PATH") or os.path.join(tempfile.gettempdir(), "pay4_gw.db")
    return _PAY4_DB


def _build_gateway() -> PaymentGateway:
    return PaymentGateway({
        "stripe": StripeGateway(api_key="sk_test_mock", webhook_secret="whsec_mock"),
        "ecpay": ECPayGateway(merchant_id="2000132", hash_key="mock_HashKey", hash_iv="mock_HashIV"),
        "pay4": Pay4Gateway(db_path=_get_pay4_db()),
    })


def cmd_pay(args: argparse.Namespace) -> None:
    gw = _build_gateway()
    req = PaymentRequest(order_id=args.order_id, amount=args.amount, description=args.desc)
    result = gw.create_payment(req, provider=args.provider)
    print(f"[{result.gateway}] {result.order_id}: {result.status.value} (id={result.payment_id})")
    if result.error_message:
        print(f"  error: {result.error_message}", file=sys.stderr)


def cmd_refund(args: argparse.Namespace) -> None:
    gw = _build_gateway()
    result = gw.refund(args.payment_id, provider=args.provider)
    print(f"[{result.gateway}] refund: {result.status.value} (id={result.payment_id})")


def main() -> None:
    parser = argparse.ArgumentParser(description="Payment CLI")
    sub = parser.add_subparsers(dest="command")

    pay = sub.add_parser("pay")
    pay.add_argument("--provider", default="stripe", choices=["stripe", "ecpay", "pay4"])
    pay.add_argument("--order-id", required=True)
    pay.add_argument("--amount", type=int, required=True)
    pay.add_argument("--desc", default=None)

    refund = sub.add_parser("refund")
    refund.add_argument("--provider", default="stripe", choices=["stripe", "ecpay", "pay4"])
    refund.add_argument("--payment-id", required=True)

    args = parser.parse_args()
    if args.command == "pay":
        cmd_pay(args)
    elif args.command == "refund":
        cmd_refund(args)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
