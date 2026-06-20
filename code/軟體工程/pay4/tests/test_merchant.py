import os
import pytest
from pay4.database import init_db, reset_db
from pay4.core.merchant import create_merchant, list_merchants, get_merchant, get_merchant_by_api_key


@pytest.fixture(autouse=True)
def setup_db():
    os.environ["PAY4_DB_PATH"] = ":memory:"
    reset_db()
    init_db()
    yield


class TestMerchantCreate:
    def test_create_merchant(self):
        m = create_merchant("test_shop", "shop@test.com")
        assert m.name == "test_shop"
        assert m.email == "shop@test.com"
        assert m.api_key.startswith("pay4_")
        assert m.status == "active"
        assert m.fee_rate == 0.03

    def test_create_merchant_custom_fee(self):
        m = create_merchant("premium", "premium@test.com", fee_rate=0.01)
        assert m.fee_rate == 0.01

    def test_create_with_webhook(self):
        m = create_merchant("web", "web@test.com", webhook_url="https://example.com/hook")
        assert m.webhook_url == "https://example.com/hook"


class TestMerchantQuery:
    def test_get_merchant(self):
        m = create_merchant("test", "t@t.com")
        found = get_merchant(m.id)
        assert found is not None
        assert found.id == m.id
        assert found.name == "test"

    def test_get_merchant_not_found(self):
        assert get_merchant("nonexistent") is None

    def test_get_by_api_key(self):
        m = create_merchant("key_test", "k@t.com")
        found = get_merchant_by_api_key(m.api_key)
        assert found is not None
        assert found.id == m.id

    def test_get_by_api_key_invalid(self):
        assert get_merchant_by_api_key("bad_key") is None

    def test_list_merchants(self):
        create_merchant("a", "a@a.com")
        create_merchant("b", "b@b.com")
        all_m = list_merchants()
        assert len(all_m) == 2
