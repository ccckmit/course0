use serde::{Deserialize, Serialize};
use sled::Db;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Todo {
    pub id: u64,
    pub text: String,
    pub done: bool,
}

pub struct Store {
    db: Arc<Db>,
    next_id: AtomicU64,
}

impl Store {
    pub fn open(path: &str) -> Result<Self, sled::Error> {
        let db = Arc::new(sled::open(path)?);
        let next = db
            .open_tree("meta")?
            .get("next_id")?
            .map(|v| u64::from_be_bytes(v.as_ref().try_into().unwrap()))
            .unwrap_or(1);
        Ok(Self {
            db,
            next_id: AtomicU64::new(next),
        })
    }

    pub fn add(&self, text: &str) -> Result<u64, Box<dyn std::error::Error>> {
        let id = self.next_id.fetch_add(1, Ordering::SeqCst);
        let todo = Todo { id, text: text.into(), done: false };
        let tree = self.db.open_tree("todos")?;
        tree.insert(id.to_be_bytes(), bincode::serialize(&todo)?)?;
        let meta = self.db.open_tree("meta")?;
        meta.insert("next_id", &(id + 1).to_be_bytes())?;
        Ok(id)
    }

    pub fn list(&self, include_done: bool) -> Result<Vec<Todo>, Box<dyn std::error::Error>> {
        let tree = self.db.open_tree("todos")?;
        let mut out = Vec::new();
        for item in tree.iter() {
            let (_, v) = item?;
            let todo: Todo = bincode::deserialize(&v)?;
            if include_done || !todo.done {
                out.push(todo);
            }
        }
        out.sort_by_key(|t| t.id);
        Ok(out)
    }

    pub fn set_done(&self, id: u64) -> Result<bool, Box<dyn std::error::Error>> {
        let tree = self.db.open_tree("todos")?;
        match tree.get(id.to_be_bytes())? {
            Some(v) => {
                let mut todo: Todo = bincode::deserialize(&v)?;
                if todo.done {
                    return Ok(false);
                }
                todo.done = true;
                tree.insert(id.to_be_bytes(), bincode::serialize(&todo)?)?;
                Ok(true)
            }
            None => Ok(false),
        }
    }

    pub fn delete(&self, id: u64) -> Result<bool, Box<dyn std::error::Error>> {
        let tree = self.db.open_tree("todos")?;
        let prev = tree.remove(id.to_be_bytes())?;
        Ok(prev.is_some())
    }
}
