import os
import tempfile
from todo.database import init_db, clear_db, add_todo
from todo.cli import main


def _fresh_db():
    db = tempfile.mktemp(suffix=".db")
    os.environ["TODO_DB_PATH"] = db
    init_db()
    return db


def test_add(capsys):
    _fresh_db()
    main(["add", "hello"])
    captured = capsys.readouterr().out
    assert "Added: #1 hello" in captured


def test_list_empty(capsys):
    _fresh_db()
    main(["list"])
    captured = capsys.readouterr().out
    assert "No todos" in captured


def test_add_then_list(capsys):
    _fresh_db()
    main(["add", "hello"])
    main(["add", "world"])
    main(["list"])
    captured = capsys.readouterr().out
    assert "1. hello" in captured
    assert "2. world" in captured


def test_done(capsys):
    _fresh_db()
    main(["add", "hello"])
    main(["done", "1"])
    captured = capsys.readouterr().out
    assert "Done: #1" in captured


def test_done_not_found(capsys):
    _fresh_db()
    main(["done", "999"])
    captured = capsys.readouterr().out
    assert "Not found: #999" in captured


def test_delete(capsys):
    _fresh_db()
    main(["add", "hello"])
    main(["delete", "1"])
    captured = capsys.readouterr().out
    assert "Deleted: #1" in captured


def test_no_args(capsys):
    _fresh_db()
    main([])
    captured = capsys.readouterr().out
    assert "Usage:" in captured
