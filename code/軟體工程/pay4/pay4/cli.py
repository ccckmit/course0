import argparse
import sys
from .database import init_db
from .core.merchant import create_merchant, list_merchants, get_merchant
from .core.engine import create_order, pay_order, refund_order, list_orders, expire_old_orders
from .core.settlement import create_settlement, list_settlements
from .core.ledger import get_balance, get_merchant_transactions
from .core.webhook import process_webhook_queue, list_webhook_events


def cmd_init(args):
    init_db()
    print("database initialized")


def cmd_merchant_create(args):
    m = create_merchant(args.name, args.email, args.webhook_url or "", args.fee_rate)
    print(f"created merchant: {m.id}")
    print(f"  api_key: {m.api_key}")
    print(f"  fee_rate: {m.fee_rate}")


def cmd_merchant_list(args):
    for m in list_merchants():
        balance_ym = "-" if m.balance < 0 else ""
        print(f"{m.id}  {m.name:20s}  {m.status:10s}  balance={balance_ym}{abs(m.balance)}")


def cmd_merchant_show(args):
    m = get_merchant(args.merchant_id)
    if not m:
        print(f"merchant not found: {args.merchant_id}", file=sys.stderr)
        return
    print(f"id:         {m.id}")
    print(f"name:       {m.name}")
    print(f"email:      {m.email}")
    print(f"api_key:    {m.api_key}")
    print(f"status:     {m.status}")
    print(f"balance:    {m.balance}")
    print(f"fee_rate:   {m.fee_rate}")
    print(f"webhook:    {m.webhook_url}")


def cmd_order_create(args):
    o = create_order(args.merchant_id, args.amount, args.currency, args.description or "")
    print(f"created order: {o.id}  amount={o.amount}  status={o.status}")


def cmd_order_pay(args):
    o = pay_order(args.order_id)
    print(f"paid: {o.id}  status={o.status}")


def cmd_order_refund(args):
    o = refund_order(args.order_id)
    print(f"refunded: {o.id}  status={o.status}")


def cmd_order_list(args):
    for o in list_orders(args.merchant_id or ""):
        print(f"{o.id}  {o.merchant_id}  amount={o.amount}  status={o.status}")


def cmd_settle(args):
    s = create_settlement(args.merchant_id)
    print(f"settlement: {s.id}  amount={s.amount}  fee={s.fee}  net={s.net_amount}  status={s.status}")


def cmd_settle_list(args):
    for s in list_settlements(args.merchant_id or ""):
        print(f"{s.id}  {s.merchant_id}  amount={s.amount}  net={s.net_amount}  status={s.status}")


def cmd_balance(args):
    bal = get_balance(args.account)
    print(f"balance of '{args.account}': {bal}")


def cmd_ledger(args):
    for t in get_merchant_transactions(args.merchant_id):
        print(f"{t.created_at}  {t.type:15s}  {t.from_account:30s} -> {t.to_account:30s}  {t.amount}")


def cmd_webhook_process(args):
    process_webhook_queue()
    print("webhook queue processed")


def cmd_webhook_list(args):
    for w in list_webhook_events(args.merchant_id or ""):
        print(f"{w.id}  {w.event_type:20s}  status={w.status}  retry={w.retry_count}")


def cmd_expire(args):
    expire_old_orders(args.minutes)
    print("expired old orders")


def main():
    parser = argparse.ArgumentParser(description="MyPay — 自建金流系統")
    sub = parser.add_subparsers(dest="command")

    p_init = sub.add_parser("init", help="初始化資料庫")

    p_mc = sub.add_parser("merchant", help="商家管理")
    mc_sub = p_mc.add_subparsers(dest="action")
    mc_create = mc_sub.add_parser("create", help="註冊商家")
    mc_create.add_argument("name")
    mc_create.add_argument("email")
    mc_create.add_argument("--webhook-url", default="")
    mc_create.add_argument("--fee-rate", type=float, default=0.03)
    mc_list = mc_sub.add_parser("list", help="列出商家")
    mc_show = mc_sub.add_parser("show", help="檢視商家")
    mc_show.add_argument("merchant_id")

    p_o = sub.add_parser("order", help="訂單管理")
    o_sub = p_o.add_subparsers(dest="action")
    o_create = o_sub.add_parser("create", help="建立訂單")
    o_create.add_argument("merchant_id")
    o_create.add_argument("amount", type=int)
    o_create.add_argument("--currency", default="TWD")
    o_create.add_argument("--description", default="")
    o_pay = o_sub.add_parser("pay", help="模擬付款")
    o_pay.add_argument("order_id")
    o_refund = o_sub.add_parser("refund", help="退款")
    o_refund.add_argument("order_id")
    o_list = o_sub.add_parser("list", help="列出訂單")
    o_list.add_argument("--merchant-id", default="")

    p_s = sub.add_parser("settle", help="清算撥款")
    s_sub = p_s.add_subparsers(dest="action")
    s_create = s_sub.add_parser("create", help="執行清算")
    s_create.add_argument("merchant_id")
    s_list = s_sub.add_parser("list", help="列出清算記錄")
    s_list.add_argument("--merchant-id", default="")

    p_b = sub.add_parser("balance", help="查詢科目餘額")
    p_b.add_argument("account")

    p_l = sub.add_parser("ledger", help="檢視帳本")
    p_l.add_argument("merchant_id")

    p_w = sub.add_parser("webhook", help="Webhook 管理")
    w_sub = p_w.add_subparsers(dest="action")
    w_process = w_sub.add_parser("process", help="處理佇列")
    w_list = w_sub.add_parser("list", help="列出事件")
    w_list.add_argument("--merchant-id", default="")

    p_e = sub.add_parser("expire", help="逾時訂單過期")
    p_e.add_argument("--minutes", type=int, default=30)

    args = parser.parse_args()
    if args.command == "init":
        cmd_init(args)
    elif args.command == "merchant":
        if args.action == "create":
            cmd_merchant_create(args)
        elif args.action == "list":
            cmd_merchant_list(args)
        elif args.action == "show":
            cmd_merchant_show(args)
        else:
            p_mc.print_help()
    elif args.command == "order":
        if args.action == "create":
            cmd_order_create(args)
        elif args.action == "pay":
            cmd_order_pay(args)
        elif args.action == "refund":
            cmd_order_refund(args)
        elif args.action == "list":
            cmd_order_list(args)
        else:
            p_o.print_help()
    elif args.command == "settle":
        if args.action == "create":
            cmd_settle(args)
        elif args.action == "list":
            cmd_settle_list(args)
        else:
            p_s.print_help()
    elif args.command == "balance":
        cmd_balance(args)
    elif args.command == "ledger":
        cmd_ledger(args)
    elif args.command == "webhook":
        if args.action == "process":
            cmd_webhook_process(args)
        elif args.action == "list":
            cmd_webhook_list(args)
        else:
            p_w.print_help()
    elif args.command == "expire":
        cmd_expire(args)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
