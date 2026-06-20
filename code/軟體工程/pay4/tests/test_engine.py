import os
import pytest
from pay4.database import init_db, reset_db
from pay4.core.merchant import create_merchant, get_merchant
from pay4.core.engine import create_order, pay_order, refund_order, list_orders, get_order, expire_old_orders


@pytest.fixture(autouse=True)
def setup_db():
    os.environ["PAY4_DB_PATH"] = ":memory:"
    reset_db()
    init_db()
    yield


class TestOrderCreate:
    def test_create_order(self):
        m = create_merchant("test", "t@t.com")
        o = create_order(m.id, 1000, description="test item")
        assert o.merchant_id == m.id
        assert o.amount == 1000
        assert o.status == "pending"
        assert o.description == "test item"

    def test_create_order_invalid_merchant(self):
        with pytest.raises(ValueError, match="merchant not found"):
            create_order("bad_id", 500)

    def test_create_order_suspended_merchant(self):
        m = create_merchant("suspended", "s@s.com")
        from pay4.database import get_conn, close_conn
        conn = get_conn()
        conn.execute("UPDATE merchants SET status='suspended' WHERE id=?", (m.id,))
        conn.commit()
        close_conn(conn)
        with pytest.raises(ValueError, match="not active"):
            create_order(m.id, 500)


class TestOrderPay:
    def test_pay_order(self):
        m = create_merchant("pay_test", "p@t.com")
        o = create_order(m.id, 1000)
        paid = pay_order(o.id)
        assert paid.status == "paid"
        assert paid.paid_at is not None

    def test_pay_updates_balance(self):
        m = create_merchant("bal_test", "b@t.com")
        o = create_order(m.id, 1000)
        pay_order(o.id)
        updated = get_merchant(m.id)
        fee = int(1000 * m.fee_rate)
        net = 1000 - fee
        assert updated.balance == net

    def test_pay_twice_fails(self):
        m = create_merchant("t2", "t2@t.com")
        o = create_order(m.id, 500)
        pay_order(o.id)
        with pytest.raises(ValueError, match="cannot be paid"):
            pay_order(o.id)

    def test_pay_non_existent(self):
        with pytest.raises(ValueError, match="order not found"):
            pay_order("bad_id")


class TestOrderRefund:
    def test_refund_order(self):
        m = create_merchant("ref_test", "r@t.com")
        o = create_order(m.id, 2000)
        pay_order(o.id)
        refunded = refund_order(o.id)
        assert refunded.status == "refunded"

    def test_refund_reverses_balance(self):
        m = create_merchant("rb", "rb@t.com")
        o1 = create_order(m.id, 1000)
        o2 = create_order(m.id, 500)
        pay_order(o1.id)
        pay_order(o2.id)
        before = get_merchant(m.id)
        refund_order(o1.id)
        after = get_merchant(m.id)
        fee1 = int(1000 * m.fee_rate)
        net1 = 1000 - fee1
        assert after.balance == before.balance - net1

    def test_refund_unpaid_fails(self):
        m = create_merchant("ru", "ru@t.com")
        o = create_order(m.id, 500)
        with pytest.raises(ValueError, match="cannot be refunded"):
            refund_order(o.id)


class TestOrderExpire:
    def test_expire_old_orders(self):
        m = create_merchant("exp", "e@t.com")
        o = create_order(m.id, 500)
        expire_old_orders(minutes=0)
        expired = get_order(o.id)
        assert expired.status == "expired"


class TestOrderList:
    def test_list_orders(self):
        m = create_merchant("lst", "l@t.com")
        create_order(m.id, 100)
        create_order(m.id, 200)
        orders = list_orders(m.id)
        assert len(orders) == 2
        all_orders = list_orders()
        assert len(all_orders) >= 2
