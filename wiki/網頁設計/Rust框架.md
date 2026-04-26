# Rust 伺服器框架

## 概述

Rust 提供了多個高效的 Web 框架，結合了記憶體安全和高效能，是建構高效能後端服務的熱門選擇。

## 框架比較

| 框架 | 特點 | 適用場景 |
|------|------|----------|
| Actix-web | 功能完整、高效能 | 生產級應用 |
| Axum | 現代、型別安全 | 新專案 |
| Rocket | 簡潔、宣告式 | 快速開發 |
| Warp | 輕量、組合式 | 微服務 |

## Actix-web

### 安裝

```toml
[dependencies]
actix-web = "4"
actix-rt = "2"
serde = { version = "1", features = ["derive"] }
serde_json = "1"
tokio = { version = "1", features = ["full"] }
```

### 基本範例

```rust
use actix_web::{web, App, HttpResponse, HttpServer};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct User {
    id: u32,
    name: String,
    email: String,
}

async fn index() -> HttpResponse {
    HttpResponse::Ok().json(User {
        id: 1,
        name: "Alice".to_string(),
        email: "alice@example.com".to_string(),
    })
}

async fn get_user(path: web::Path<u32>) -> HttpResponse {
    let user_id = path.into_inner();
    HttpResponse::Ok().json(User {
        id: user_id,
        name: "Bob".to_string(),
        email: format!("user{}@example.com", user_id),
    })
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    HttpServer::new(|| {
        App::new()
            .route("/", web::get().to(index))
            .route("/user/{id}", web::get().to(get_user))
    })
    .bind("127.0.0.1:8080")?
    .run()
    .await
}
```

### REST API 完整範例

```rust
use actix_web::{web, App, HttpResponse, HttpServer, middleware};
use serde::{Deserialize, Serialize};
use std::sync::Mutex;
use std::collections::HashMap;

#[derive(Serialize, Deserialize, Clone)]
struct Item {
    id: u32,
    name: String,
    price: f64,
}

struct AppState {
    items: Mutex<HashMap<u32, Item>>,
    next_id: Mutex<u32>,
}

async fn create_item(
    item: web::Json<Item>,
    data: web::Data<AppState>,
) -> HttpResponse {
    let mut next_id = data.next_id.lock().unwrap();
    let mut new_item = item.into_inner();
    new_item.id = *next_id;
    *next_id += 1;
    
    let mut items = data.items.lock().unwrap();
    items.insert(new_item.id, new_item.clone());
    
    HttpResponse::Created().json(new_item)
}

async fn get_items(data: web::Data<AppState>) -> HttpResponse {
    let items = data.items.lock().unwrap();
    HttpResponse::Ok().json(items.values().cloned().collect::<Vec<_>>())
}

async fn get_item(
    path: web::Path<u32>,
    data: web::Data<AppState>,
) -> HttpResponse {
    let id = path.into_inner();
    let items = data.items.lock().unwrap();
    
    match items.get(&id) {
        Some(item) => HttpResponse::Ok().json(item),
        None => HttpResponse::NotFound().body("Item not found"),
    }
}

async fn update_item(
    path: web::Path<u32>,
    item: web::Json<Item>,
    data: web::Data<AppState>,
) -> HttpResponse {
    let id = path.into_inner();
    let mut items = data.items.lock().unwrap();
    
    match items.get_mut(&id) {
        Some(existing) => {
            *existing = item.into_inner();
            HttpResponse::Ok().json(existing.clone())
        }
        None => HttpResponse::NotFound().body("Item not found"),
    }
}

async fn delete_item(
    path: web::Path<u32>,
    data: web::Data<AppState>,
) -> HttpResponse {
    let id = path.into_inner();
    let mut items = data.items.lock().unwrap();
    
    match items.remove(&id) {
        Some(_) => HttpResponse::NoContent().finish(),
        None => HttpResponse::NotFound().body("Item not found"),
    }
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let app_state = web::Data::new(AppState {
        items: Mutex::new(HashMap::new()),
        next_id: Mutex::new(1),
    });
    
    HttpServer::new(move || {
        App::new()
            .app_data(app_state.clone())
            .wrap(middleware::Logger::default())
            .route("/items", web::post().to(create_item))
            .route("/items", web::get().to(get_items))
            .route("/items/{id}", web::get().to(get_item))
            .route("/items/{id}", web::put().to(update_item))
            .route("/items/{id}", web::delete().to(delete_item))
    })
    .bind("127.0.0.1:8080")?
    .run()
    .await
}
```

## Axum

### 安裝

```toml
[dependencies]
axum = "0.7"
tokio = { version = "1", features = ["full"] }
serde = { version = "1", features = ["derive"] }
serde_json = "1"
tower = "0.4"
tower-http = { version = "0.5", features = ["cors", "fs"] }
```

### 基本範例

```rust
use axum::{
    routing::{get, post},
    Router,
    Json, extract,
};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct User {
    id: u32,
    name: String,
}

async fn get_user(extract::Path(id): extract::Path<u32>) -> Json<User> {
    Json(User {
        id,
        name: "Alice".to_string(),
    })
}

async fn create_user(Json(payload): Json<User>) -> Json<User> {
    Json(payload)
}

#[tokio::main]
async fn main() {
    let app = Router::new()
        .route("/users/{id}", get(get_user))
        .route("/users", post(create_user));
    
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000")
        .await
        .unwrap();
    
    axum::serve(listener, app).await.unwrap();
}
```

### State 管理

```rust
use std::sync::Arc;
use axum::{
    extract::State,
    Router,
    routing::get,
    Json,
};
use tokio::sync::Mutex;

#[derive(Clone)]
struct AppState {
    counter: Arc<Mutex<u32>>,
}

async fn increment(State(state): State<AppState>) -> Json<u32> {
    let mut counter = state.counter.lock().await;
    *counter += 1;
    Json(*counter)
}

#[tokio::main]
async fn main() {
    let state = AppState {
        counter: Arc::new(Mutex::new(0)),
    };
    
    let app = Router::new()
        .route("/counter", get(increment))
        .with_state(state);
    
    // ...
}
```

## Rocket

### 安裝

```toml
[dependencies]
rocket = { version = "0.5", features = ["json"] }
serde = { version = "1", features = ["derive"] }
serde_json = "1"
```

### 基本範例

```rust
#[macro_use]
extern crate rocket;

use rocket::serde::json::Json;
use rocket::serde::{Deserialize, Serialize};
use rocket::State;
use std::sync::Mutex;

#[derive(Serialize, Deserialize)]
#[serde(crate = "rocket::serde")]
struct User {
    id: u32,
    name: String,
}

#[get("/user/<id>")]
fn get_user(id: u32) -> Json<User> {
    Json(User {
        id,
        name: "Alice".to_string(),
    })
}

#[post("/user", data = "<user>")]
fn create_user(user: Json<User>) -> Json<User> {
    user.into_inner()
}

#[launch]
fn rocket() -> _ {
    rocket::build()
        .mount("/", routes![get_user, create_user])
}
```

## Warp

### 安裝

```toml
[dependencies]
warp = "0.3"
tokio = { version = "1", features = ["full"] }
serde = { version = "1", features = ["derive"] }
serde_json = "1"
```

### 基本範例

```rust
use warp::Filter;
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize)]
struct User {
    id: u32,
    name: String,
}

#[tokio::main]
async fn main() {
    let hello = warp::path!("hello" / String)
        .map(|name| warp::reply::html(format!("Hello, {}!", name)));
    
    let users = warp::path!("api" / "users")
        .and(warp::get())
        .map(|| warp::reply::json(&vec![
            User { id: 1, name: "Alice".to_string() }
        ]));
    
    let routes = hello.or(users);
    
    warp::serve(routes)
        .run(([127, 0, 0, 1], 3030))
        .await;
}
```

## Middleware 和攔截器

### Logging

```rust
use tower_http::trace::TraceLayer;
use tracing_subscriber;

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();
    
    let app = Router::new()
        .route("/api/items", get(|| async { "Items" }))
        .layer(TraceLayer::new_for_http());
    
    // ...
}
```

### CORS

```rust
use tower_http::cors::{CorsLayer, Any};

let cors = CorsLayer::new()
    .allow_origin(Any)
    .allow_methods(Any)
    .allow_headers(Any);

let app = Router::new()
    .route("/api/items", get(|| async { "Items" }))
    .layer(cors);
```

## 資料庫整合

### SQLx

```rust
use sqlx::{PgPool, Row};

async fn get_users(pool: &PgPool) -> Result<Vec<User>, sqlx::Error> {
    let rows = sqlx::query("SELECT id, name FROM users")
        .fetch_all(pool)
        .await?;
    
    Ok(rows.iter()
        .map(|r| User {
            id: r.get("id"),
            name: r.get("name"),
        })
        .collect())
}
```

### Diesel (同步)

```rust
use diesel::prelude::*;
use diesel::pg::PgConnection;

#[table_name = "users"]
#[derive(Queryable, Selectable)]
#[diesel(check_for_backend(diesel::pg::Pg))]
struct User {
    id: i32,
    name: String,
}

fn get_users(conn: &mut PgConnection) -> QueryResult<Vec<User>> {
    use crate::schema::users::dsl::*;
    
    users.load(conn)
}
```

## WebSocket

### Actix-web WebSocket

```rust
use actix_web::{web, Error, HttpRequest, HttpResponse};
use actix_web_actors::ws;
use std::time::{Duration, Instant};

const HEARTBEAT_INTERVAL: Duration = Duration::from_secs(5);
const CLIENT_TIMEOUT: Duration = Duration::from_secs(30);

struct MyWs;

impl actix::Actor for MyWs {
    type Context = ws::WebsocketContext<Self>;
}

impl actix::StreamHandler<Result<ws::Message, ws::ProtocolError>> for MyWs {
    fn handle(&mut self, msg: Result<ws::Message, ws::ProtocolError>, ctx: &mut Self::Context) {
        match msg {
            Ok(ws::Message::Ping(msg)) => ctx.pong(&msg),
            Ok(ws::Message::Text(text)) => ctx.text(text),
            Ok(ws::Message::Close(reason)) => ctx.close(reason),
            _ => ctx.stop(),
        }
    }
}

async fn ws_handler(req: HttpRequest, stream: web::Payload) -> Result<HttpResponse, Error> {
    let resp = ws::start(MyWs {}, &req, stream);
    resp.map_err(|e| actix_web::error::ErrorInternalServerError(e))
}
```

## 部署

### Docker

```dockerfile
FROM rust:latest as builder
WORKDIR /app
COPY . .
RUN cargo build --release

FROM debian:bookworm-slim
COPY --from=builder /app/target/release/myapp /app/
CMD ["/app/myapp"]
```

### 效能考量

| 優化 | 說明 |
|------|------|
| `#[tokio::main]` | 啟用 async runtime |
| `tower-http` | HTTP 中介軟體 |
| `Release build` | 使用 `--release` 編譯 |
| Connection pooling | 資料庫連線池 |

## 相關資源

- 相關概念：[API](API.md)
- 相關主題：[RESTful](RESTful.md)
- 相關工具：[Node.js](Node.js.md)

## Tags

#Rust #Actix #Axum #Rocket #Warp #Web框架
