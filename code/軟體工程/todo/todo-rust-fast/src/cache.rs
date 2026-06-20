/// Cache module — requires Redis to be available.
/// If REDIS_URL env var is not set, runs without caching.
use redis::aio::ConnectionManager;
use serde_json::Value;
use tokio::sync::Mutex;

const CACHE_TTL: u64 = 10;

pub struct Cache {
    conn: Mutex<Option<ConnectionManager>>,
}

impl Cache {
    pub async fn try_connect() -> Self {
        let url = std::env::var("REDIS_URL").unwrap_or_else(|_| "redis://127.0.0.1:6379".into());
        match redis::Client::open(url.as_str()) {
            Ok(client) => match client.get_connection_manager().await {
                Ok(conn) => {
                    println!("[cache] Redis connected");
                    Self { conn: Mutex::new(Some(conn)) }
                }
                Err(e) => {
                    eprintln!("[cache] Redis connect failed (running without cache): {}", e);
                    Self { conn: Mutex::new(None) }
                }
            },
            Err(e) => {
                eprintln!("[cache] Redis URL invalid (running without cache): {}", e);
                Self { conn: Mutex::new(None) }
            }
        }
    }

    pub async fn get(&self, key: &str) -> Option<Value> {
        use redis::AsyncCommands;
        let mut guard = self.conn.lock().await;
        let conn = guard.as_mut()?;
        let s: Option<String> = conn.get(key).await.ok()?;
        s.and_then(|s| serde_json::from_str(&s).ok())
    }

    pub async fn set(&self, key: &str, value: &Value) {
        use redis::AsyncCommands;
        let mut guard = self.conn.lock().await;
        let conn = match guard.as_mut() {
            Some(c) => c,
            None => return,
        };
        let s = serde_json::to_string(value).unwrap();
        let _: Result<(), _> = conn.set_ex(key, s, CACHE_TTL).await;
    }

    pub async fn invalidate(&self, key: &str) {
        use redis::AsyncCommands;
        let mut guard = self.conn.lock().await;
        if let Some(conn) = guard.as_mut() {
            let _: Result<(), _> = conn.del(key).await;
        }
    }

    pub async fn invalidate_all(&self) {
        let mut guard = self.conn.lock().await;
        if let Some(conn) = guard.as_mut() {
            let _: Result<(), _> = redis::cmd("FLUSHALL").query_async(conn).await;
        }
    }
}
