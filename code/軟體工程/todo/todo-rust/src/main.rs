mod api;
mod cli;
mod database;

use std::env;

#[tokio::main]
async fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() > 1 && args[1] == "serve" {
        let _ = database::init_db();
        let app = api::app();
        let listener = tokio::net::TcpListener::bind("0.0.0.0:8000").await.unwrap();
        println!("API running on http://0.0.0.0:8000");
        axum::serve(listener, app).await.unwrap();
    } else {
        cli::run(&args);
    }
}
