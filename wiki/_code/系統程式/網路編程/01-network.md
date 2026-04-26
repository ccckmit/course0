# 網路編程範例測試結果

## 執行方式
```bash
cd _code/系統程式/網路編程
rustc 01-network.rs -o 01-network
./01-network
```

## 執行結果

```
=== Rust 網路編程範例 ===

=== 1. TCP 伺服器 ===
TCP 伺服器 listening on port 8080
（無連線，超時）

=== 2. TCP 客戶端 ===
無法連接: Connection refused (os error 61)
（需要先運行 TCP 伺服器）

=== 3. UDP Socket ===
UDP socket bind 成功
發送 11 bytes
接收: Resource temporarily unavailable (os error 35)

=== 4. HTTP 請求/響應 ===
HTTP 請求:
GET /api/data HTTP/1.1\r\n


解析的 HTTP 響應:
  Status: 200 OK
  Body: Ok("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"data\": \"test\"}")

=== 5. WebSocket ===
WebSocket 連線到: ws://example.com/socket
發送 WebSocket frame: [129, 17, 72, 101, 108, 108, 111, 44, 32, 87, 101, 98, 83, 111, 99, 107, 101, 116, 33]
發送 WebSocket frame: [129, 14, 83, 101, 99, 111, 110, 100, 32, 109, 101, 115, 115, 97, 103, 101]
收到: Second message
WebSocket 關閉連線

=== 6. DNS 解析 ===
解析 example.com: [127.0.0.1:80]
快取後: [127.0.0.1:80]

=== 7. Socket 選項 ===
常見 Socket 選項:
  SO_REUSEADDR - 允許重用地址
  SO_REUSEPORT - 允許重用端口
  TCP_NODELAY - 禁用 Nagle 演算法
  SO_TIMEOUT - 讀取超時

設定範例:
  stream.set_nonblocking(true).ok();
  stream.set_read_timeout(Some(Duration::from_secs(30)));

=== 8. 網路錯誤處理 ===
錯誤: 連線被拒絕
錯誤: 連線超時
錯誤: 無效的地址

網路編程範例完成!
```

## 相關頁面
- [網路概念](../系統程式/網路.md)
- [TCP-IP協定](../系統程式/TCP-IP協定.md)
- [HTTP概念](../系統程式/HTTP.md)
