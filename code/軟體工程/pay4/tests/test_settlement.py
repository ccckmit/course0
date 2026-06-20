import os
import pytest
from pay4.database import init_db, reset_db
from pay4.core.merchant import create_merchant
from pay4.core.engine import create_order, pay_order
from pay4.core.settlement import create_settlement, list_settlements, get_settlement


@pytest.fixture(autouse=True)
def setup_db():
    os.environ["PAY4_DB_PATH"] = ":memory:"
    reset_db()
    init_db()
    yield


class TestSettlement:
    def test_create_settlement(self):
        m = create_merchant("settle_test", "st@t.com")
        o1 = create_order(m.id, 3000)
        o2 = create_order(m.id, 2000)
        pay_order(o1.id)
        pay_order(o2.id)
        s = create_settlement(m.id)
        assert s.merchant_id == m.id
        assert s.amount == 5000
        expected_fee = int(3000 * m.fee_rate) + int(2000 * m.fee_rate)
        assert s.fee == expected_fee
        assert s.net_amount == 5000 - expected_fee
        assert s.status == "completed"

    def test_settlement_updates_balance(self):
        m = create_merchant("sb", "sb@t.com")
        o1 = create_order(m.id, 6000)
        o2 = create_order(m.id, 4000)
        pay_order(o1.id)
        pay_order(o2.id)

        create_settlement(m.id)
        from pay4.core.merchant import get_merchant
        after = get_merchant(m.id)
        assert after.balance == 0

    def test_settlement_no_orders(self):
        m = create_merchant("empty", "empty@t.com")
        with pytest.raises(ValueError, match="no payable orders"):
            create_settlement(m.id)

    def test_list_settlements(self):
        m = create_merchant("ls", "ls@t.com")
        o1 = create_order(m.id, 1000)
        o2 = create_order(m.id, 2000)
        pay_order(o1.id)
        pay_order(o2.id)
        create_settlement(m.id)
        all_s = list_settlements(m.id)
        assert len(all_s) == 1

    def test_get_settlement(self):
        m = create_merchant("gs", "gs@t.com")
        o = create_order(m.id, 1000)
        pay_order(o.id)
        s = create_settlement(m.id)
        found = get_settlement(s.id)
        assert found is not None
        assert found.id == s.id
