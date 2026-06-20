import sys
from .database import init_db, add_todo, list_todos, done_todo, delete_todo


def main(argv: list[str] | None = None):
    if argv is None:
        argv = sys.argv[1:]
    if not argv:
        print("Usage: todo <command> [args]")
        print("Commands: add <text>, list, done <id>, delete <id>")
        return
    init_db()
    cmd = argv[0]
    match cmd:
        case "add":
            text = " ".join(argv[1:]) if len(argv) > 1 else ""
            if not text:
                print("Error: text is required")
                return
            tid = add_todo(text)
            print(f"Added: #{tid} {text}")
        case "list":
            todos = list_todos()
            if not todos:
                print("No todos")
                return
            for t in todos:
                print(f"{t['id']}. {t['text']}")
        case "done":
            if len(argv) < 2:
                print("Error: id is required")
                return
            tid = int(argv[1])
            if done_todo(tid):
                print(f"Done: #{tid}")
            else:
                print(f"Not found: #{tid}")
        case "delete":
            if len(argv) < 2:
                print("Error: id is required")
                return
            tid = int(argv[1])
            if delete_todo(tid):
                print(f"Deleted: #{tid}")
            else:
                print(f"Not found: #{tid}")
        case _:
            print(f"Unknown command: {cmd}")
