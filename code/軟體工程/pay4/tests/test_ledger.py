import os
import pytest
from pay4.database import init_db, reset_db
from pay4.core.merchant import create_merchant
from pay4.core.engine import create_order, pay_order
from pay4.core.ledger import add_transaction, get_balance, get_merchant_transactions, get_transactions


@pytest.fixture(autouse=True)
def setup_db():
    os.environ["PAY4_DB_PATH"] = ":memory:"
    reset_db()
    init_db()
    yield


class TestAddTransaction:
    def test_add_and_query(self):
        t = add_transaction("payment", "merchant:123:receivable", "merchant:123:balance", 500)
        assert t.type == "payment"
        assert t.amount == 500

    def test_get_balance(self):
        add_transaction("payment", "a:receivable", "a:balance", 1000)
        add_transaction("fee", "a:balance", "pay4:revenue", 30)
        bal_a = get_balance("a:balance")
        assert bal_a == 970
        rev = get_balance("pay4:revenue")
        assert rev == 30

    def test_empty_account_balance(self):
        assert get_balance("nonexistent") == 0


class TestMerchantTransactions:
    def test_auto_transactions_on_pay(self):
        m = create_merchant("txn_test", "txn@t.com")
        o = create_order(m.id, 1000)
        pay_order(o.id)

        txns = get_merchant_transactions(m.id)
        assert len(txns) == 2
        types = {t.type for t in txns}
        assert "payment" in types
        assert "fee" in types

    def test_get_transactions_by_account(self):
        m = create_merchant("ta", "ta@t.com")
        o = create_order(m.id, 2000)
        pay_order(o.id)
        txns = get_transactions(f"merchant:{m.id}:balance")
        assert len(txns) >= 2
