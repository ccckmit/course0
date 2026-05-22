use rusqlite::{Connection, Result};

fn main() -> Result<()> {
    let path = "test.db";
    let conn = Connection::open(path)?;

    conn.execute("DROP TABLE IF EXISTS users", [])?;
    conn.execute(
        "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, email TEXT NOT NULL)",
        [],
    )?;

    println!("=== 新增 (INSERT) ===");
    conn.execute("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')", [])?;
    conn.execute("INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com')", [])?;
    conn.execute("INSERT INTO users (name, email) VALUES ('Charlie', 'charlie@example.com')", [])?;
    println!("已新增 3 筆資料");

    println!("\n=== 查詢 (SELECT) ===");
    let mut stmt = conn.prepare("SELECT id, name, email FROM users")?;
    let users = stmt.query_map([], |row| {
        Ok((row.get::<_, i32>(0)?, row.get::<_, String>(1)?, row.get::<_, String>(2)?))
    })?;
    for user in users {
        let (id, name, email) = user?;
        println!("ID: {}, Name: {}, Email: {}", id, name, email);
    }

    println!("\n=== 修改 (UPDATE) ===");
    conn.execute("UPDATE users SET email = 'alice@new.com' WHERE name = 'Alice'", [])?;
    println!("已修改 Alice 的 email");

    println!("\n=== 刪除 (DELETE) ===");
    conn.execute("DELETE FROM users WHERE name = 'Bob'", [])?;
    println!("已刪除 Bob");

    println!("\n=== 最終資料 ===");
    let mut stmt = conn.prepare("SELECT id, name, email FROM users")?;
    let users = stmt.query_map([], |row| {
        Ok((row.get::<_, i32>(0)?, row.get::<_, String>(1)?, row.get::<_, String>(2)?))
    })?;
    for user in users {
        let (id, name, email) = user?;
        println!("ID: {}, Name: {}, Email: {}", id, name, email);
    }

    println!("\n=== 條件查詢 ===");
    let mut stmt = conn.prepare("SELECT name, email FROM users WHERE id > ?")?;
    let users = stmt.query_map([1], |row| {
        Ok((row.get::<_, String>(0)?, row.get::<_, String>(1)?))
    })?;
    for user in users {
        let (name, email) = user?;
        println!("Name: {}, Email: {}", name, email);
    }

    println!("\n=== 聚合查詢 ===");
    let count: i32 = conn.query_row("SELECT COUNT(*) FROM users", [], |row| row.get(0))?;
    println!("總筆數: {}", count);

    Ok(())
}