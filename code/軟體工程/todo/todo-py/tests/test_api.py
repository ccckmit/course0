import os
import tempfile
from fastapi.testclient import TestClient
from todo.database import init_db
from todo.api import app

client = TestClient(app)


def _fresh_db():
    db = tempfile.mktemp(suffix=".db")
    os.environ["TODO_DB_PATH"] = db
    init_db()
    return db


def test_list_empty():
    _fresh_db()
    resp = client.get("/api/todos")
    assert resp.status_code == 200
    assert resp.json() == []


def test_create_todo():
    _fresh_db()
    resp = client.post("/api/todos", json={"text": "hello"})
    assert resp.status_code == 200
    data = resp.json()
    assert data["text"] == "hello"
    assert data["done"] is False


def test_create_then_list():
    _fresh_db()
    client.post("/api/todos", json={"text": "hello"})
    resp = client.get("/api/todos")
    data = resp.json()
    assert len(data) == 1
    assert data[0]["text"] == "hello"


def test_done_todo():
    _fresh_db()
    post = client.post("/api/todos", json={"text": "hello"})
    tid = post.json()["id"]
    resp = client.put(f"/api/todos/{tid}", json={"done": True})
    assert resp.status_code == 200
    assert resp.json()["done"] is True


def test_done_not_found():
    _fresh_db()
    resp = client.put("/api/todos/999", json={"done": True})
    assert resp.status_code == 404


def test_delete():
    _fresh_db()
    post = client.post("/api/todos", json={"text": "hello"})
    tid = post.json()["id"]
    resp = client.delete(f"/api/todos/{tid}")
    assert resp.status_code == 200
    resp2 = client.get("/api/todos")
    assert resp2.json() == []
