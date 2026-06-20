import uuid
import secrets
from typing import Optional
from ..database import get_conn, close_conn
from ..models import Merchant


def generate_api_key() -> str:
    return f"pay4_{secrets.token_hex(24)}"


def create_merchant(name: str, email: str, webhook_url: str = "", fee_rate: float = 0.03) -> Merchant:
    conn = get_conn()
    mid = f"m_{uuid.uuid4().hex[:12]}"
    api_key = generate_api_key()
    conn.execute(
        "INSERT INTO merchants (id, name, email, api_key, webhook_url, fee_rate) VALUES (?, ?, ?, ?, ?, ?)",
        (mid, name, email, api_key, webhook_url, fee_rate),
    )
    conn.commit()
    row = conn.execute("SELECT * FROM merchants WHERE id=?", (mid,)).fetchone()
    close_conn(conn)
    return Merchant(**dict(row))


def get_merchant(merchant_id: str) -> Optional[Merchant]:
    conn = get_conn()
    row = conn.execute("SELECT * FROM merchants WHERE id=?", (merchant_id,)).fetchone()
    close_conn(conn)
    return Merchant(**dict(row)) if row else None


def get_merchant_by_api_key(api_key: str) -> Optional[Merchant]:
    conn = get_conn()
    row = conn.execute("SELECT * FROM merchants WHERE api_key=?", (api_key,)).fetchone()
    close_conn(conn)
    return Merchant(**dict(row)) if row else None


def list_merchants() -> list[Merchant]:
    conn = get_conn()
    rows = conn.execute("SELECT * FROM merchants ORDER BY created_at DESC").fetchall()
    close_conn(conn)
    return [Merchant(**dict(r)) for r in rows]


def update_merchant_balance(merchant_id: str, delta: int) -> Merchant | None:
    conn = get_conn()
    conn.execute("UPDATE merchants SET balance = balance + ? WHERE id=?", (delta, merchant_id))
    conn.commit()
    row = conn.execute("SELECT * FROM merchants WHERE id=?", (merchant_id,)).fetchone()
    close_conn(conn)
    return Merchant(**dict(row)) if row else None
