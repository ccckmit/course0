// Rust 網路編程範例
// 展示 TCP/UDP、Socket 編程、HTTP

use std::collections::HashMap;
use std::io::{Read, Write};
use std::net::{SocketAddr, TcpListener, TcpStream, UdpSocket};
use std::time::Duration;

// ============ 1. TCP 伺服器 ============
fn create_tcp_server(port: u16) -> std::io::Result<TcpListener> {
    let addr = format!("127.0.0.1:{}", port);
    TcpListener::bind(&addr)
}

fn demo_tcp_server() {
    println!("=== 1. TCP 伺服器 ===");

    // 嘗試綁定（可能失敗）
    match create_tcp_server(8080) {
        Ok(listener) => {
            println!("TCP 伺服器 listening on port 8080");

            // 使用 non-blocking 模式
            listener.set_nonblocking(true).ok();

            // 只嘗試接受一個連線
            match listener.accept() {
                Ok((mut stream, addr)) => {
                    println!("新連線: {:?}", addr);

                    let mut buffer = [0u8; 1024];
                    if let Ok(n) = stream.read(&mut buffer) {
                        let request = String::from_utf8_lossy(&buffer[..n]);
                        println!("收到: {}", request.trim());
                    }
                }
                Err(ref e) if e.kind() == std::io::ErrorKind::WouldBlock => {
                    println!("（無連線，超時）");
                }
                Err(e) => println!("連線錯誤: {}", e),
            }
        }
        Err(e) => {
            println!("無法綁定端口: {}", e);
            println!("（這是正常的，端口可能已被佔用）");
        }
    }
    println!();
}

// ============ 2. TCP 客戶端 ============
fn demo_tcp_client() {
    println!("=== 2. TCP 客戶端 ===");

    match TcpStream::connect("127.0.0.1:8080") {
        Ok(mut stream) => {
            println!("已連接到伺服器");

            stream.set_read_timeout(Some(Duration::from_secs(5))).ok();

            let message = "Hello, Server!";
            stream.write_all(message.as_bytes()).ok();
            println!("發送: {}", message);

            let mut buffer = [0u8; 1024];
            match stream.read(&mut buffer) {
                Ok(n) => {
                    let response = String::from_utf8_lossy(&buffer[..n]);
                    println!("收到: {}", response);
                }
                Err(e) => println!("讀取超時: {}", e),
            }
        }
        Err(e) => {
            println!("無法連接: {}", e);
            println!("（需要先運行 TCP 伺服器）");
        }
    }
    println!();
}

// ============ 3. UDP Socket ============
fn demo_udp() {
    println!("=== 3. UDP Socket ===");

    // UDP 客戶端範例
    match UdpSocket::bind("127.0.0.1:0") {
        Ok(socket) => {
            println!("UDP socket bind 成功");

            let msg = b"Hello, UDP!";
            socket.set_nonblocking(true).ok();

            match socket.send_to(msg, "127.0.0.1:8081") {
                Ok(n) => println!("發送 {} bytes", n),
                Err(e) => println!("發送失敗: {}", e),
            }

            // 接收緩衝區
            let mut buf = [0u8; 1024];
            match socket.recv_from(&mut buf) {
                Ok((n, addr)) => {
                    println!("收到 {} bytes from {}", n, addr);
                    println!("內容: {:?}", std::str::from_utf8(&buf[..n]));
                }
                Err(e) => println!("接收: {}", e),
            }
        }
        Err(e) => println!("UDP bind 失敗: {}", e),
    }
    println!();
}

// ============ 4. HTTP 客戶端（概念）============
struct HttpRequest {
    method: String,
    path: String,
    headers: HashMap<String, String>,
    body: Option<String>,
}

impl HttpRequest {
    fn get(url: &str) -> Self {
        HttpRequest {
            method: "GET".to_string(),
            path: url.to_string(),
            headers: HashMap::new(),
            body: None,
        }
    }

    fn post(url: &str, body: &str) -> Self {
        HttpRequest {
            method: "POST".to_string(),
            path: url.to_string(),
            headers: HashMap::new(),
            body: Some(body.to_string()),
        }
    }

    fn to_bytes(&self) -> Vec<u8> {
        let mut request = format!("{} {} HTTP/1.1\r\n", self.method, self.path);

        for (key, value) in &self.headers {
            request.push_str(&format!("{}: {}\r\n", key, value));
        }

        if let Some(ref body) = self.body {
            request.push_str(&format!("Content-Length: {}\r\n", body.len()));
        }

        request.push_str("\r\n");

        if let Some(ref body) = self.body {
            request.push_str(body);
        }

        request.into_bytes()
    }
}

struct HttpResponse {
    status_code: u16,
    status_text: String,
    headers: HashMap<String, String>,
    body: Vec<u8>,
}

impl HttpResponse {
    fn parse(data: &[u8]) -> Option<Self> {
        let text = String::from_utf8_lossy(data);
        let lines: Vec<&str> = text.lines().collect();

        if lines.is_empty() {
            return None;
        }

        // 解析狀態行
        let status_line = lines[0];
        let parts: Vec<&str> = status_line.splitn(3, ' ').collect();

        let status_code = parts.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
        let status_text = parts.get(2).unwrap_or(&"").to_string();

        Some(HttpResponse {
            status_code,
            status_text,
            headers: HashMap::new(),
            body: data.to_vec(),
        })
    }
}

fn demo_http() {
    println!("=== 4. HTTP 請求/響應 ===");

    // 構建 HTTP 請求
    let request = HttpRequest::get("/api/data");
    let bytes = request.to_bytes();

    println!("HTTP 請求:");
    println!("{}", String::from_utf8_lossy(&bytes));

    // 解析 HTTP 響應（範例）
    let sample_response =
        b"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"data\": \"test\"}";
    if let Some(response) = HttpResponse::parse(sample_response) {
        println!("\n解析的 HTTP 響應:");
        println!(
            "  Status: {} {}",
            response.status_code, response.status_text
        );
        println!("  Body: {:?}", std::str::from_utf8(&response.body));
    }
    println!();
}

// ============ 5. WebSocket 概念 ============
struct WebSocket {
    connected: bool,
    message_buffer: Vec<String>,
}

impl WebSocket {
    fn new() -> Self {
        WebSocket {
            connected: false,
            message_buffer: Vec::new(),
        }
    }

    fn connect(&mut self, url: &str) {
        println!("WebSocket 連線到: {}", url);
        self.connected = true;
    }

    fn send(&mut self, message: &str) {
        if self.connected {
            // WebSocket 訊息需要 mask
            let masked = Self::mask_message(message);
            println!("發送 WebSocket frame: {:?}", masked);
            self.message_buffer.push(message.to_string());
        }
    }

    fn receive(&mut self) -> Option<String> {
        self.message_buffer.pop()
    }

    fn mask_message(msg: &str) -> Vec<u8> {
        // 簡化的 WebSocket frame
        let mut frame = vec![0x81]; // FIN + text opcode

        if msg.len() < 126 {
            frame.push(msg.len() as u8);
        } else {
            frame.push(126);
            frame.push((msg.len() >> 8) as u8);
            frame.push(msg.len() as u8);
        }

        frame.extend(msg.as_bytes());
        frame
    }

    fn close(&mut self) {
        println!("WebSocket 關閉連線");
        self.connected = false;
    }
}

fn demo_websocket() {
    println!("=== 5. WebSocket ===");

    let mut ws = WebSocket::new();
    ws.connect("ws://example.com/socket");

    ws.send("Hello, WebSocket!");
    ws.send("Second message");

    if let Some(msg) = ws.receive() {
        println!("收到: {}", msg);
    }

    ws.close();
    println!();
}

// ============ 6. DNS 解析概念 ============
struct DnsResolver {
    cache: HashMap<String, Vec<SocketAddr>>,
}

impl DnsResolver {
    fn new() -> Self {
        DnsResolver {
            cache: HashMap::new(),
        }
    }

    fn resolve(&self, hostname: &str) -> Vec<SocketAddr> {
        // 簡化的 DNS 解析
        // 實際需要使用系統 DNS resolver 或 crates like trust-dns

        if let Some(addrs) = self.cache.get(hostname) {
            return addrs.clone();
        }

        // 模擬解析結果
        vec!["127.0.0.1:80".parse().unwrap()]
    }

    fn cache_result(&mut self, hostname: &str, addrs: Vec<SocketAddr>) {
        self.cache.insert(hostname.to_string(), addrs);
    }
}

fn demo_dns() {
    println!("=== 6. DNS 解析 ===");

    let mut resolver = DnsResolver::new();

    let host = "example.com";
    let ips = resolver.resolve(host);

    println!("解析 {}: {:?}", host, ips);

    resolver.cache_result(host, ips.clone());
    println!("快取後: {:?}", resolver.resolve(host));
    println!();
}

// ============ 7. Socket 選項 ============
fn demo_socket_options() {
    println!("=== 7. Socket 選項 ===");

    // 展示常用的 socket 選項
    println!("常見 Socket 選項:");
    println!("  SO_REUSEADDR - 允許重用地址");
    println!("  SO_REUSEPORT - 允許重用端口");
    println!("  TCP_NODELAY - 禁用 Nagle 演算法");
    println!("  SO_TIMEOUT - 讀取超時");
    println!();

    // 實際設定範例
    println!("設定範例:");
    println!("  stream.set_nonblocking(true).ok();");
    println!("  stream.set_read_timeout(Some(Duration::from_secs(30)));");
    println!();
}

// ============ 8. 網路錯誤處理 ============
#[derive(Debug)]
enum NetworkError {
    ConnectionRefused,
    Timeout,
    HostUnreachable,
    PermissionDenied,
    InvalidAddress,
    Unknown(String),
}

impl std::fmt::Display for NetworkError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            NetworkError::ConnectionRefused => write!(f, "連線被拒絕"),
            NetworkError::Timeout => write!(f, "連線超時"),
            NetworkError::HostUnreachable => write!(f, "主機不可達"),
            NetworkError::PermissionDenied => write!(f, "權限不足"),
            NetworkError::InvalidAddress => write!(f, "無效的地址"),
            NetworkError::Unknown(e) => write!(f, "未知錯誤: {}", e),
        }
    }
}

fn demo_error_handling() {
    println!("=== 8. 網路錯誤處理 ===");

    // 模擬網路錯誤
    let errors = vec![
        NetworkError::ConnectionRefused,
        NetworkError::Timeout,
        NetworkError::InvalidAddress,
    ];

    for error in errors {
        println!("錯誤: {}", error);
    }
    println!();
}

fn main() {
    println!("=== Rust 網路編程範例 ===\n");

    demo_tcp_server();
    demo_tcp_client();
    demo_udp();
    demo_http();
    demo_websocket();
    demo_dns();
    demo_socket_options();
    demo_error_handling();

    println!("網路編程範例完成!");
}
