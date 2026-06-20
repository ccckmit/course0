from datetime import datetime
from enum import Enum
from typing import Optional
from pydantic import BaseModel


class PaymentStatus(str, Enum):
    PENDING = "pending"
    SUCCESS = "success"
    FAILED = "failed"
    REFUNDED = "refunded"


class PaymentRequest(BaseModel):
    order_id: str
    amount: int
    currency: str = "TWD"
    description: Optional[str] = None


class PaymentResponse(BaseModel):
    status: PaymentStatus
    gateway: str
    payment_id: str
    order_id: str
    amount: int
    currency: str
    error_message: Optional[str] = None
    created_at: datetime = datetime.now()
