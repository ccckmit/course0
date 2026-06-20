use rusqlite::{params, Connection, Result};

fn path() -> String {
    std::env::var("TODO_DB_PATH").unwrap_or_else(|_| "todo.db".to_string())
}

pub fn init_db() -> Result<()> {
    let conn = Connection::open(path())?;
    conn.execute_batch(
        "CREATE TABLE IF NOT EXISTS todos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            text TEXT NOT NULL,
            done INTEGER NOT NULL DEFAULT 0
        )",
    )?;
    Ok(())
}

fn conn() -> Result<Connection> {
    let c = Connection::open(path())?;
    c.execute_batch("PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL;")?;
    Ok(c)
}

#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct Todo {
    pub id: i64,
    pub text: String,
    pub done: bool,
}

pub fn add(text: &str) -> Result<i64> {
    let c = conn()?;
    c.execute("INSERT INTO todos (text) VALUES (?1)", params![text])?;
    Ok(c.last_insert_rowid())
}

pub fn list(include_done: bool) -> Result<Vec<Todo>> {
    let c = conn()?;
    let sql = if include_done {
        "SELECT id, text, done FROM todos ORDER BY id"
    } else {
        "SELECT id, text, done FROM todos WHERE done = 0 ORDER BY id"
    };
    let mut stmt = c.prepare(sql)?;
    let rows = stmt.query_map([], |row| {
        Ok(Todo {
            id: row.get(0)?,
            text: row.get(1)?,
            done: row.get::<_, i32>(2)? != 0,
        })
    })?;
    let mut out = Vec::new();
    for r in rows {
        out.push(r?);
    }
    Ok(out)
}

pub fn set_done(id: i64) -> Result<bool> {
    let c = conn()?;
    let n = c.execute("UPDATE todos SET done = 1 WHERE id = ?1", params![id])?;
    Ok(n > 0)
}

pub fn delete(id: i64) -> Result<bool> {
    let c = conn()?;
    let n = c.execute("DELETE FROM todos WHERE id = ?1", params![id])?;
    Ok(n > 0)
}
