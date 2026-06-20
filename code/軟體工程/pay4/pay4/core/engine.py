import uuid
import json
from datetime import datetime, timezone
from typing import Optional
from ..database import get_conn, close_conn
from ..models import Order
from .merchant import get_merchant, update_merchant_balance
from .ledger import add_transaction


def create_order(merchant_id: str, amount: int, currency: str = "TWD",
                 description: str = "", idempotency_key: str = "") -> Order:
    merchant = get_merchant(merchant_id)
    if not merchant:
        raise ValueError(f"merchant not found: {merchant_id}")
    if merchant.status != "active":
        raise ValueError(f"merchant is not active: {merchant.status}")

    conn = get_conn()
    oid = f"o_{uuid.uuid4().hex[:12]}"
    conn.execute(
        "INSERT INTO orders (id, merchant_id, amount, currency, description, idempotency_key) VALUES (?, ?, ?, ?, ?, ?)",
        (oid, merchant_id, amount, currency, description, idempotency_key),
    )
    conn.commit()
    row = conn.execute("SELECT * FROM orders WHERE id=?", (oid,)).fetchone()
    close_conn(conn)
    return Order(**dict(row))


def get_order(order_id: str) -> Optional[Order]:
    conn = get_conn()
    row = conn.execute("SELECT * FROM orders WHERE id=?", (order_id,)).fetchone()
    close_conn(conn)
    return Order(**dict(row)) if row else None


def list_orders(merchant_id: str = "") -> list[Order]:
    conn = get_conn()
    if merchant_id:
        rows = conn.execute("SELECT * FROM orders WHERE merchant_id=? ORDER BY created_at DESC", (merchant_id,)).fetchall()
    else:
        rows = conn.execute("SELECT * FROM orders ORDER BY created_at DESC").fetchall()
    close_conn(conn)
    return [Order(**dict(r)) for r in rows]


def pay_order(order_id: str) -> Order:
    order = get_order(order_id)
    if not order:
        raise ValueError(f"order not found: {order_id}")
    if order.status != "pending":
        raise ValueError(f"order cannot be paid (status={order.status})")

    merchant = get_merchant(order.merchant_id)
    fee = int(order.amount * merchant.fee_rate)
    net = order.amount - fee

    conn = get_conn()
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
    conn.execute(
        "UPDATE orders SET status='paid', paid_at=?, updated_at=? WHERE id=?",
        (now, now, order_id),
    )
    conn.commit()
    close_conn(conn)

    add_transaction(
        type="payment",
        from_account=f"merchant:{order.merchant_id}:receivable",
        to_account=f"merchant:{order.merchant_id}:balance",
        amount=net,
        order_id=order_id,
    )
    add_transaction(
        type="fee",
        from_account=f"merchant:{order.merchant_id}:balance",
        to_account="pay4:revenue",
        amount=fee,
        order_id=order_id,
    )
    update_merchant_balance(order.merchant_id, net)

    from .webhook import enqueue_webhook
    enqueue_webhook(order.merchant_id, "order.paid", {"order_id": order_id, "amount": order.amount, "net": net, "fee": fee})

    return get_order(order_id)


def refund_order(order_id: str) -> Order:
    order = get_order(order_id)
    if not order:
        raise ValueError(f"order not found: {order_id}")
    if order.status != "paid":
        raise ValueError(f"order cannot be refunded (status={order.status})")

    conn = get_conn()
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
    conn.execute(
        "UPDATE orders SET status='refunding', updated_at=? WHERE id=?",
        (now, order_id),
    )
    conn.commit()
    close_conn(conn)

    merchant = get_merchant(order.merchant_id)
    fee = int(order.amount * merchant.fee_rate)
    net = order.amount - fee

    add_transaction(
        type="refund",
        from_account=f"merchant:{order.merchant_id}:balance",
        to_account=f"merchant:{order.merchant_id}:receivable",
        amount=net,
        order_id=order_id,
    )
    add_transaction(
        type="fee_reversal",
        from_account="pay4:revenue",
        to_account=f"merchant:{order.merchant_id}:balance",
        amount=fee,
        order_id=order_id,
    )
    update_merchant_balance(order.merchant_id, -net)

    conn = get_conn()
    conn.execute("UPDATE orders SET status='refunded', updated_at=? WHERE id=?", (now, order_id))
    conn.commit()
    close_conn(conn)

    from .webhook import enqueue_webhook
    enqueue_webhook(order.merchant_id, "order.refunded", {"order_id": order_id, "amount": order.amount})

    return get_order(order_id)


def expire_old_orders(minutes: int = 30):
    conn = get_conn()
    conn.execute(
        "UPDATE orders SET status='expired', updated_at=datetime('now') "
        "WHERE status='pending' AND datetime('now') >= datetime(created_at, '+' || ? || ' minutes')",
        (minutes,),
    )
    conn.commit()
    close_conn(conn)
