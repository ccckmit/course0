from fastapi import FastAPI, HTTPException, Header
from .models import Merchant, Order
from .database import init_db
from .core.merchant import create_merchant, get_merchant, get_merchant_by_api_key, list_merchants
from .core.engine import create_order, pay_order, refund_order, list_orders, get_order
from .core.settlement import create_settlement, list_settlements, get_settlement
from .core.ledger import get_merchant_transactions, get_balance

app = FastAPI(title="MyPay API", version="0.1")


@app.on_event("startup")
def startup():
    init_db()


def _verify_api_key(api_key: str = Header("")):
    m = get_merchant_by_api_key(api_key)
    if not m:
        raise HTTPException(status_code=401, detail="invalid api_key")
    if m.status != "active":
        raise HTTPException(status_code=403, detail=f"merchant {m.status}")
    return m


@app.post("/v1/merchants")
def api_create_merchant(name: str, email: str, webhook_url: str = "", fee_rate: float = 0.03):
    m = create_merchant(name, email, webhook_url, fee_rate)
    return {"merchant_id": m.id, "api_key": m.api_key}


@app.get("/v1/merchants")
def api_list_merchants():
    return [{"id": m.id, "name": m.name, "status": m.status, "balance": m.balance} for m in list_merchants()]


@app.post("/v1/orders")
def api_create_order(amount: int, currency: str = "TWD", description: str = "",
                     merchant: Merchant = None, merchant=Depends(_verify_api_key)):
    o = create_order(merchant.id, amount, currency, description)
    return {"order_id": o.id, "amount": o.amount, "status": o.status}


@app.get("/v1/orders")
def api_list_orders(merchant: Merchant = Depends(_verify_api_key)):
    return [{"id": o.id, "amount": o.amount, "status": o.status, "created_at": o.created_at}
            for o in list_orders(merchant.id)]


@app.get("/v1/orders/{order_id}")
def api_get_order(order_id: str, merchant: Merchant = Depends(_verify_api_key)):
    o = get_order(order_id)
    if not o or o.merchant_id != merchant.id:
        raise HTTPException(status_code=404, detail="order not found")
    return {"id": o.id, "amount": o.amount, "status": o.status, "created_at": o.created_at, "paid_at": o.paid_at}


@app.post("/v1/orders/{order_id}/pay")
def api_pay_order(order_id: str, merchant: Merchant = Depends(_verify_api_key)):
    o = get_order(order_id)
    if not o or o.merchant_id != merchant.id:
        raise HTTPException(status_code=404, detail="order not found")
    o = pay_order(order_id)
    return {"order_id": o.id, "status": o.status}


@app.post("/v1/orders/{order_id}/refund")
def api_refund_order(order_id: str, merchant: Merchant = Depends(_verify_api_key)):
    o = get_order(order_id)
    if not o or o.merchant_id != merchant.id:
        raise HTTPException(status_code=404, detail="order not found")
    o = refund_order(order_id)
    return {"order_id": o.id, "status": o.status}


@app.get("/v1/merchants/{merchant_id}/balance")
def api_get_balance(merchant_id: str, merchant: Merchant = Depends(_verify_api_key)):
    if merchant.id != merchant_id:
        raise HTTPException(status_code=403, detail="access denied")
    return {"merchant_id": merchant_id, "balance": merchant.balance}


@app.get("/v1/merchants/{merchant_id}/transactions")
def api_get_transactions(merchant_id: str, merchant: Merchant = Depends(_verify_api_key)):
    if merchant.id != merchant_id:
        raise HTTPException(status_code=403, detail="access denied")
    txns = get_merchant_transactions(merchant_id)
    return [{"id": t.id, "type": t.type, "amount": t.amount, "created_at": t.created_at} for t in txns]


from fastapi import Depends
