use axum::{
    extract::Path,
    http::{HeaderValue, StatusCode},
    response::Json,
    routing::{get, put},
    Router,
};
use serde::Deserialize;
use std::sync::OnceLock;
use tower_http::{
    compression::CompressionLayer,
    cors::CorsLayer,
    set_header::SetResponseHeaderLayer,
};

use crate::cache::Cache;
use crate::db::Store;

static DB: OnceLock<Store> = OnceLock::new();
static CACHE: OnceLock<Cache> = OnceLock::new();

pub fn init(store: Store, cache: Cache) {
    DB.set(store).ok();
    CACHE.set(cache).ok();
}

fn cache() -> &'static Cache {
    CACHE.get().expect("Cache not initialized")
}


#[derive(Deserialize)]
struct CreateBody {
    text: String,
}

#[derive(Deserialize)]
struct UpdateBody {
    done: bool,
}

pub fn app() -> Router {
    Router::new()
        .route("/api/todos", get(get_todos).post(create_todo))
        .route("/api/todos/{id}", put(update_todo).delete(delete_todo))
        .layer(CompressionLayer::new().gzip(true).br(true).no_zstd())
        .layer(CorsLayer::permissive())
        .layer(SetResponseHeaderLayer::overriding(
            axum::http::header::SERVER,
            HeaderValue::from_static("todo-rust-fast/0.1"),
        ))
}

async fn get_todos() -> Json<Vec<crate::db::Todo>> {
    if let Some(cached) = cache().get("todos:list").await {
        if let Ok(todos) = serde_json::from_value(cached) {
            return Json(todos);
        }
    }
    let db = DB.get().unwrap();
    let todos = db.list(true).unwrap_or_default();
    let val = serde_json::to_value(&todos).unwrap();
    cache().set("todos:list", &val).await;
    Json(todos)
}

async fn create_todo(Json(body): Json<CreateBody>) -> Json<serde_json::Value> {
    let db = DB.get().unwrap();
    let result = match db.add(&body.text) {
        Ok(id) => serde_json::json!({"id": id, "text": body.text, "done": false}),
        Err(e) => serde_json::json!({"error": e.to_string()}),
    };
    if result.get("id").is_some() {
        cache().invalidate("todos:list").await;
    }
    Json(result)
}

async fn update_todo(
    Path(id): Path<u64>,
    Json(body): Json<UpdateBody>,
) -> Result<Json<serde_json::Value>, StatusCode> {
    let db = DB.get().unwrap();
    if body.done {
        db.set_done(id).map_err(|_| StatusCode::INTERNAL_SERVER_ERROR)?;
    }
    cache().invalidate_all().await;
    Ok(Json(serde_json::json!({"id": id, "done": body.done})))
}

async fn delete_todo(Path(id): Path<u64>) -> Result<Json<serde_json::Value>, StatusCode> {
    let db = DB.get().unwrap();
    let deleted = db
        .delete(id)
        .map_err(|_| StatusCode::INTERNAL_SERVER_ERROR)?;
    if deleted {
        cache().invalidate_all().await;
    }
    if deleted {
        Ok(Json(serde_json::json!({"id": id, "deleted": true})))
    } else {
        Err(StatusCode::NOT_FOUND)
    }
}
