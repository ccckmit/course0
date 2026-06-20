mod api;
mod cache;
mod cli;
mod db;

use std::env;

#[tokio::main]
async fn main() {
    let db_path = env::var("TODO_DB_PATH").unwrap_or_else(|_| "todo-fast.db".into());
    let store = db::Store::open(&db_path).expect("Failed to open sled database");
    let cache = cache::Cache::try_connect().await;

    let args: Vec<String> = env::args().collect();
    if args.len() > 1 && args[1] == "serve" {
        api::init(store, cache);
        let app = api::app();
        let listener = tokio::net::TcpListener::bind("0.0.0.0:8000").await.unwrap();
        println!("[server] listening on http://0.0.0.0:8000");
        println!("[server] database: sled (embedded, lock-free)");
        axum::serve(listener, app).await.unwrap();
    } else {
        cli::run(&args, std::sync::Arc::new(store));
    }
}
