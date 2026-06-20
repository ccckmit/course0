from datetime import datetime
from typing import Optional
from pydantic import BaseModel


class Merchant(BaseModel):
    id: str
    name: str
    email: str
    api_key: str
    webhook_url: str = ""
    fee_rate: float = 0.03
    status: str = "active"
    balance: int = 0
    created_at: datetime = datetime.now()


class Order(BaseModel):
    id: str
    merchant_id: str
    amount: int
    currency: str = "TWD"
    status: str = "pending"
    description: str = ""
    idempotency_key: str = ""
    created_at: datetime = datetime.now()
    paid_at: Optional[datetime] = None
    updated_at: datetime = datetime.now()


class Transaction(BaseModel):
    id: str
    order_id: str = ""
    settlement_id: str = ""
    type: str
    from_account: str
    to_account: str
    amount: int
    currency: str = "TWD"
    created_at: datetime = datetime.now()


class Settlement(BaseModel):
    id: str
    merchant_id: str
    amount: int
    fee: int
    net_amount: int
    status: str = "pending"
    period_start: datetime
    period_end: datetime
    settled_at: Optional[datetime] = None


class WebhookEvent(BaseModel):
    id: str
    merchant_id: str
    event_type: str
    payload: str
    status: str = "pending"
    retry_count: int = 0
    last_attempt_at: Optional[datetime] = None
    created_at: datetime = datetime.now()
