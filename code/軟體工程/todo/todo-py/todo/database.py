import sqlite3
import os

def _get_db_path() -> str:
    return os.environ.get("TODO_DB_PATH") or os.path.join(
        os.path.dirname(__file__), "..", "todo.db"
    )


def get_connection():
    conn = sqlite3.connect(_get_db_path())
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    with get_connection() as conn:
        conn.execute("""
            CREATE TABLE IF NOT EXISTS todos (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                text TEXT NOT NULL,
                done INTEGER NOT NULL DEFAULT 0
            )
        """)


def add_todo(text: str) -> int:
    with get_connection() as conn:
        cur = conn.execute("INSERT INTO todos (text) VALUES (?)", (text,))
        return cur.lastrowid


def list_todos(include_done: bool = False):
    with get_connection() as conn:
        if include_done:
            rows = conn.execute("SELECT id, text, done FROM todos ORDER BY id").fetchall()
        else:
            rows = conn.execute("SELECT id, text, done FROM todos WHERE done = 0 ORDER BY id").fetchall()
        return [dict(r) for r in rows]


def done_todo(todo_id: int) -> bool:
    with get_connection() as conn:
        cur = conn.execute("UPDATE todos SET done = 1 WHERE id = ?", (todo_id,))
        return cur.rowcount > 0


def delete_todo(todo_id: int) -> bool:
    with get_connection() as conn:
        cur = conn.execute("DELETE FROM todos WHERE id = ?", (todo_id,))
        return cur.rowcount > 0


def clear_db():
    with get_connection() as conn:
        conn.execute("DELETE FROM todos")
        conn.execute("DELETE FROM sqlite_sequence WHERE name='todos'")
