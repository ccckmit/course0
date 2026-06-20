use std::process::Command;
use std::sync::atomic::{AtomicU32, Ordering};

static COUNTER: AtomicU32 = AtomicU32::new(0);

fn fresh_db() -> String {
    let n = COUNTER.fetch_add(1, Ordering::SeqCst);
    format!("/tmp/test_rust_todo_{}.db", n)
}

fn todo(args: &[&str], db: &str) -> String {
    let output = Command::new("cargo")
        .args(["run", "--quiet", "--"])
        .args(args)
        .env("TODO_DB_PATH", db)
        .output()
        .expect("cargo run failed");
    let stdout = String::from_utf8_lossy(&output.stdout).to_string();
    let stderr = String::from_utf8_lossy(&output.stderr);
    if !stderr.is_empty() {
        eprintln!("stderr: {}", stderr);
    }
    stdout
}

#[test]
fn test_add() {
    let db = fresh_db();
    let out = todo(&["add", "hello"], &db);
    assert!(out.contains("Added: #1 hello"), "got: {}", out);
}

#[test]
fn test_list_empty() {
    let db = fresh_db();
    let out = todo(&["list"], &db);
    assert!(out.contains("No todos"), "got: {}", out);
}

#[test]
fn test_add_then_list() {
    let db = fresh_db();
    todo(&["add", "hello"], &db);
    todo(&["add", "world"], &db);
    let out = todo(&["list"], &db);
    assert!(out.contains("1. hello"), "got: {}", out);
    assert!(out.contains("2. world"), "got: {}", out);
}

#[test]
fn test_done() {
    let db = fresh_db();
    todo(&["add", "hello"], &db);
    let out = todo(&["done", "1"], &db);
    assert!(out.contains("Done: #1"), "got: {}", out);
}

#[test]
fn test_done_not_found() {
    let db = fresh_db();
    let out = todo(&["done", "999"], &db);
    assert!(out.contains("Not found: #999"), "got: {}", out);
}

#[test]
fn test_delete() {
    let db = fresh_db();
    todo(&["add", "hello"], &db);
    let out = todo(&["delete", "1"], &db);
    assert!(out.contains("Deleted: #1"), "got: {}", out);
}
