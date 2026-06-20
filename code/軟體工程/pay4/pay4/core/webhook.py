import uuid
import json
import hmac
import hashlib
from datetime import datetime, timezone
from ..database import get_conn, close_conn
from ..models import WebhookEvent
from .merchant import get_merchant

WEBHOOK_SECRET = "pay4_webhook_secret_demo"


def enqueue_webhook(merchant_id: str, event_type: str, data: dict) -> WebhookEvent:
    merchant = get_merchant(merchant_id)
    if not merchant or not merchant.webhook_url:
        return None

    conn = get_conn()
    wid = f"w_{uuid.uuid4().hex[:12]}"
    payload = json.dumps({"type": event_type, "data": data, "created_at": datetime.now(timezone.utc).isoformat()})
    conn.execute(
        "INSERT INTO webhook_events (id, merchant_id, event_type, payload) VALUES (?, ?, ?, ?)",
        (wid, merchant_id, event_type, payload),
    )
    conn.commit()
    row = conn.execute("SELECT * FROM webhook_events WHERE id=?", (wid,)).fetchone()
    close_conn(conn)
    return WebhookEvent(**dict(row))


def sign_payload(payload: str) -> str:
    return hmac.new(WEBHOOK_SECRET.encode(), payload.encode(), hashlib.sha256).hexdigest()


def process_webhook_queue(max_attempts: int = 3):
    conn = get_conn()
    rows = conn.execute(
        "SELECT * FROM webhook_events WHERE status='pending' AND retry_count < ?",
        (max_attempts,),
    ).fetchall()
    close_conn(conn)

    import urllib.request
    for row in rows:
        ev = WebhookEvent(**dict(row))
        merchant = get_merchant(ev.merchant_id)
        if not merchant or not merchant.webhook_url:
            continue

        signature = sign_payload(ev.payload)
        req = urllib.request.Request(
            merchant.webhook_url,
            data=ev.payload.encode(),
            headers={
                "Content-Type": "application/json",
                "X-Pay4-Signature": signature,
            },
        )
        try:
            urllib.request.urlopen(req, timeout=5)
            _update_webhook_status(ev.id, "sent")
        except Exception:
            _update_webhook_retry(ev.id)


def _update_webhook_status(event_id: str, status: str):
    conn = get_conn()
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
    conn.execute("UPDATE webhook_events SET status=?, last_attempt_at=? WHERE id=?", (status, now, event_id))
    conn.commit()
    close_conn(conn)


def _update_webhook_retry(event_id: str):
    conn = get_conn()
    now = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
    conn.execute(
        "UPDATE webhook_events SET retry_count=retry_count+1, status= CASE WHEN retry_count+1>=3 THEN 'failed' ELSE 'pending' END, last_attempt_at=? WHERE id=?",
        (now, event_id),
    )
    conn.commit()
    close_conn(conn)


def list_webhook_events(merchant_id: str = "") -> list[WebhookEvent]:
    conn = get_conn()
    if merchant_id:
        rows = conn.execute(
            "SELECT * FROM webhook_events WHERE merchant_id=? ORDER BY created_at DESC", (merchant_id,)
        ).fetchall()
    else:
        rows = conn.execute("SELECT * FROM webhook_events ORDER BY created_at DESC").fetchall()
    close_conn(conn)
    return [WebhookEvent(**dict(r)) for r in rows]
