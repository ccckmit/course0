use std::sync::Arc;
use std::sync::OnceLock;

fn test_store() -> Arc<todo_fast::db::Store> {
    static STORE: OnceLock<Arc<todo_fast::db::Store>> = OnceLock::new();
    STORE.get_or_init(|| {
        let dir = std::env::temp_dir().join(format!("todo-fast-test-{}", std::process::id()));
        let _ = std::fs::remove_dir_all(&dir);
        Arc::new(todo_fast::db::Store::open(dir.to_str().unwrap()).unwrap())
    }).clone()
}

#[test]
fn test_add_and_list() {
    let store = test_store();
    let id = store.add("Test task").unwrap();
    assert!(id > 0);
    let todos = store.list(true).unwrap();
    assert!(todos.iter().any(|t| t.id == id && t.text == "Test task"));
}

#[test]
fn test_done() {
    let store = test_store();
    let id = store.add("Finish this").unwrap();
    assert!(store.set_done(id).unwrap());
    let todos = store.list(true).unwrap();
    let t = todos.iter().find(|t| t.id == id).unwrap();
    assert!(t.done);
}

#[test]
fn test_delete() {
    let store = test_store();
    let id = store.add("Delete me").unwrap();
    assert!(store.delete(id).unwrap());
    let todos = store.list(true).unwrap();
    assert!(!todos.iter().any(|t| t.id == id));
}

#[test]
fn test_done_twice_returns_false() {
    let store = test_store();
    let id = store.add("Double done").unwrap();
    assert!(store.set_done(id).unwrap());
    assert!(!store.set_done(id).unwrap());
}

#[test]
fn test_delete_missing_returns_false() {
    let store = test_store();
    assert!(!store.delete(99999).unwrap());
}

#[test]
fn test_list_excludes_done() {
    let store = test_store();
    let id1 = store.add("Active").unwrap();
    let id2 = store.add("Done task").unwrap();
    store.set_done(id2).unwrap();
    let active = store.list(false).unwrap();
    assert!(active.iter().any(|t| t.id == id1));
    assert!(!active.iter().any(|t| t.id == id2));
}
