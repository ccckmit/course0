use axum::{
    extract::Path,
    http::StatusCode,
    routing::{get, put},
    Json, Router,
};
use serde::Deserialize;
use tower_http::cors::CorsLayer;
use crate::database;

#[derive(Deserialize)]
struct CreateBody {
    text: String,
}

#[derive(Deserialize)]
struct UpdateBody {
    done: bool,
}

async fn get_todos() -> Json<Vec<database::Todo>> {
    Json(database::list(true).unwrap_or_default())
}

async fn create_todo(Json(body): Json<CreateBody>) -> Json<serde_json::Value> {
    match database::add(&body.text) {
        Ok(id) => Json(serde_json::json!({"id": id, "text": body.text, "done": false})),
        Err(e) => Json(serde_json::json!({"error": e.to_string()})),
    }
}

async fn update_todo(
    Path(id): Path<i64>,
    Json(body): Json<UpdateBody>,
) -> Result<Json<serde_json::Value>, StatusCode> {
    if body.done {
        database::set_done(id).map_err(|_| StatusCode::INTERNAL_SERVER_ERROR)?;
    }
    Ok(Json(serde_json::json!({"id": id, "done": body.done})))
}

async fn delete_todo(Path(id): Path<i64>) -> Result<Json<serde_json::Value>, StatusCode> {
    database::delete(id)
        .map_err(|_| StatusCode::INTERNAL_SERVER_ERROR)?
        .then(|| Json(serde_json::json!({"id": id, "deleted": true})))
        .ok_or(StatusCode::NOT_FOUND)
}

pub fn app() -> Router {
    Router::new()
        .route("/api/todos", get(get_todos).post(create_todo))
        .route("/api/todos/{id}", put(update_todo).delete(delete_todo))
        .layer(CorsLayer::permissive())
}
