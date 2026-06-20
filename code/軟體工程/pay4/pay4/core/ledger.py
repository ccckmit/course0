import uuid
from datetime import datetime, timezone
from ..database import get_conn, close_conn
from ..models import Transaction


def add_transaction(type: str, from_account: str, to_account: str, amount: int,
                    order_id: str = "", settlement_id: str = "") -> Transaction:
    conn = get_conn()
    tid = f"t_{uuid.uuid4().hex[:12]}"
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
    conn.execute(
        "INSERT INTO transactions (id, order_id, settlement_id, type, from_account, to_account, amount, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
        (tid, order_id, settlement_id, type, from_account, to_account, amount, now),
    )
    conn.commit()
    row = conn.execute("SELECT * FROM transactions WHERE id=?", (tid,)).fetchone()
    close_conn(conn)
    return Transaction(**dict(row))


def get_balance(account: str) -> int:
    conn = get_conn()
    row = conn.execute(
        "SELECT COALESCE(SUM(CASE WHEN to_account=? THEN amount ELSE 0 END) - "
        "SUM(CASE WHEN from_account=? THEN amount ELSE 0 END), 0) AS balance "
        "FROM transactions",
        (account, account),
    ).fetchone()
    close_conn(conn)
    return row["balance"]


def get_transactions(account: str = "") -> list[Transaction]:
    conn = get_conn()
    if account:
        rows = conn.execute(
            "SELECT * FROM transactions WHERE from_account=? OR to_account=? ORDER BY created_at DESC",
            (account, account),
        ).fetchall()
    else:
        rows = conn.execute("SELECT * FROM transactions ORDER BY created_at DESC").fetchall()
    close_conn(conn)
    return [Transaction(**dict(r)) for r in rows]


def get_merchant_transactions(merchant_id: str) -> list[Transaction]:
    account_pattern = f"merchant:{merchant_id}:%"
    conn = get_conn()
    rows = conn.execute(
        "SELECT * FROM transactions WHERE from_account LIKE ? OR to_account LIKE ? ORDER BY created_at DESC",
        (account_pattern, account_pattern),
    ).fetchall()
    close_conn(conn)
    return [Transaction(**dict(r)) for r in rows]
