import sqlite3
import os
from typing import Optional

_DEFAULT_DB_PATH = os.path.join(os.path.dirname(__file__), "pay4.db")
_shared_mem_conn: sqlite3.Connection | None = None


def get_db_path() -> str:
    return os.environ.get("PAY4_DB_PATH", _DEFAULT_DB_PATH)


def reset_db():
    global _shared_mem_conn
    if _shared_mem_conn is not None:
        _shared_mem_conn.close()
        _shared_mem_conn = None


def get_conn() -> sqlite3.Connection:
    global _shared_mem_conn
    db_path = get_db_path()
    if db_path == ":memory:":
        if _shared_mem_conn is None:
            _shared_mem_conn = sqlite3.connect(":memory:", check_same_thread=False)
            _shared_mem_conn.row_factory = sqlite3.Row
            _shared_mem_conn.execute("PRAGMA journal_mode=WAL")
            _shared_mem_conn.execute("PRAGMA foreign_keys=ON")
        return _shared_mem_conn
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA journal_mode=WAL")
    conn.execute("PRAGMA foreign_keys=ON")
    return conn


def close_conn(conn: sqlite3.Connection | None = None):
    global _shared_mem_conn
    if conn is None or conn is _shared_mem_conn:
        return
    conn.close()


def init_db():
    conn = get_conn()
    conn.executescript("""
        DROP TABLE IF EXISTS webhook_events;
        DROP TABLE IF EXISTS transactions;
        DROP TABLE IF EXISTS settlements;
        DROP TABLE IF EXISTS orders;
        DROP TABLE IF EXISTS merchants;
    """)
    _create_tables(conn)
    conn.commit()


_SCHEMA_SQL = """
    CREATE TABLE IF NOT EXISTS merchants (
        id TEXT PRIMARY KEY,
        name TEXT NOT NULL,
        email TEXT NOT NULL,
        api_key TEXT NOT NULL UNIQUE,
        webhook_url TEXT DEFAULT '',
        fee_rate REAL DEFAULT 0.03,
        status TEXT DEFAULT 'active',
        balance INTEGER DEFAULT 0,
        created_at TEXT DEFAULT (datetime('now'))
    );
    CREATE TABLE IF NOT EXISTS orders (
        id TEXT PRIMARY KEY,
        merchant_id TEXT NOT NULL,
        amount INTEGER NOT NULL,
        currency TEXT DEFAULT 'TWD',
        status TEXT DEFAULT 'pending',
        description TEXT DEFAULT '',
        idempotency_key TEXT DEFAULT '',
        created_at TEXT DEFAULT (datetime('now')),
        paid_at TEXT,
        updated_at TEXT DEFAULT (datetime('now')),
        FOREIGN KEY (merchant_id) REFERENCES merchants(id)
    );
    CREATE TABLE IF NOT EXISTS transactions (
        id TEXT PRIMARY KEY,
        order_id TEXT DEFAULT '',
        settlement_id TEXT DEFAULT '',
        type TEXT NOT NULL,
        from_account TEXT NOT NULL,
        to_account TEXT NOT NULL,
        amount INTEGER NOT NULL,
        currency TEXT DEFAULT 'TWD',
        created_at TEXT DEFAULT (datetime('now'))
    );
    CREATE TABLE IF NOT EXISTS settlements (
        id TEXT PRIMARY KEY,
        merchant_id TEXT NOT NULL,
        amount INTEGER NOT NULL,
        fee INTEGER NOT NULL,
        net_amount INTEGER NOT NULL,
        status TEXT DEFAULT 'pending',
        period_start TEXT NOT NULL,
        period_end TEXT NOT NULL,
        settled_at TEXT,
        created_at TEXT DEFAULT (datetime('now')),
        FOREIGN KEY (merchant_id) REFERENCES merchants(id)
    );
    CREATE TABLE IF NOT EXISTS webhook_events (
        id TEXT PRIMARY KEY,
        merchant_id TEXT NOT NULL,
        event_type TEXT NOT NULL,
        payload TEXT NOT NULL,
        status TEXT DEFAULT 'pending',
        retry_count INTEGER DEFAULT 0,
        last_attempt_at TEXT,
        created_at TEXT DEFAULT (datetime('now')),
        FOREIGN KEY (merchant_id) REFERENCES merchants(id)
    );
"""


def _create_tables(conn):
    conn.executescript(_SCHEMA_SQL)


def ensure_tables():
    """僅建立不存在的表格，不影響既有資料 (用於 gateway 整合)"""
    conn = get_conn()
    _create_tables(conn)
    conn.commit()
