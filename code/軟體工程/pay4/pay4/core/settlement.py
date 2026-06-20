import uuid
from datetime import datetime, timezone
from typing import Optional
from ..database import get_conn, close_conn
from ..models import Settlement
from .merchant import get_merchant, update_merchant_balance
from .ledger import add_transaction


def create_settlement(merchant_id: str) -> Settlement:
    merchant = get_merchant(merchant_id)
    if not merchant:
        raise ValueError(f"merchant not found: {merchant_id}")

    conn = get_conn()
    rows = conn.execute(
        "SELECT COALESCE(SUM(amount), 0) AS total FROM orders "
        "WHERE merchant_id=? AND status='paid' AND id NOT IN "
        "(SELECT order_id FROM transactions WHERE type='settlement')",
        (merchant_id,),
    ).fetchall()
    total_amount = rows[0]["total"]

    if total_amount <= 0:
        close_conn(conn)
        raise ValueError(f"no payable orders for merchant: {merchant_id}")

    fee_rows = conn.execute(
        "SELECT COALESCE(SUM(amount), 0) AS total FROM transactions "
        "WHERE type='fee' AND from_account LIKE ?",
        (f"merchant:{merchant_id}:%",),
    ).fetchall()
    total_fee = fee_rows[0]["total"]

    net_amount = total_amount - total_fee
    sid = f"s_{uuid.uuid4().hex[:12]}"
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")

    conn.execute(
        "INSERT INTO settlements (id, merchant_id, amount, fee, net_amount, status, period_start, period_end, settled_at) "
        "VALUES (?, ?, ?, ?, ?, 'completed', ?, ?, ?)",
        (sid, merchant_id, total_amount, total_fee, net_amount, merchant.created_at.strftime("%Y-%m-%d"), now, now),
    )
    conn.commit()

    add_transaction(
        type="settlement",
        from_account=f"merchant:{merchant_id}:balance",
        to_account="pay4:bank",
        amount=net_amount,
        settlement_id=sid,
    )

    update_merchant_balance(merchant_id, -net_amount)

    row = conn.execute("SELECT * FROM settlements WHERE id=?", (sid,)).fetchone()
    close_conn(conn)

    from .webhook import enqueue_webhook
    enqueue_webhook(merchant_id, "settlement.completed", {
        "settlement_id": sid, "amount": total_amount, "fee": total_fee, "net": net_amount,
    })

    return Settlement(**dict(row))


def list_settlements(merchant_id: str = "") -> list[Settlement]:
    conn = get_conn()
    if merchant_id:
        rows = conn.execute(
            "SELECT * FROM settlements WHERE merchant_id=? ORDER BY created_at DESC", (merchant_id,)
        ).fetchall()
    else:
        rows = conn.execute("SELECT * FROM settlements ORDER BY created_at DESC").fetchall()
    close_conn(conn)
    return [Settlement(**dict(r)) for r in rows]


def get_settlement(settlement_id: str) -> Optional[Settlement]:
    conn = get_conn()
    row = conn.execute("SELECT * FROM settlements WHERE id=?", (settlement_id,)).fetchone()
    close_conn(conn)
    return Settlement(**dict(row)) if row else None
