# WebSocket

**標籤**: #WebSocket #即時通訊 #雙向通信 #Node.js #Socket.io
**日期**: 2024-01-15

WebSocket 是一種在單一 TCP 連線上提供全雙工通訊的協定，實現了瀏覽器與伺服器之間的雙向即時通訊。與傳統 HTTP 請求-回應模式不同，WebSocket 允許伺服器主動推送訊息給客戶端，非常適合聊天室、即時更新、線上遊戲等需要即時互動的應用場景。

## WebSocket 的發展歷程

### HTTP 長輪詢的問題

在 WebSocket 出現之前，開發者使用 HTTP 長輪詢 (Long Polling) 或 Server-Sent Events (SSE) 來實現即時通訊。但這些方法都有明顯缺點：
- 每次都需要重新建立 HTTP 連線
- 伺服器負擔大
- 即時性受限

### WebSocket 的誕生

WebSocket 協定在 2011 年由 IETF 標準化 (RFC 6455)，並很快被所有現代瀏覽器支援。WebSocket 通過 HTTP Upgrade 機制建立連線，然後轉換為持久的 TCP 連線，實現真正的雙向通訊。

## WebSocket 與 HTTP 的比較

| 特性 | HTTP | WebSocket |
|------|------|-----------|
| 通訊模式 | 請求-回應 | 雙向 |
| 連線建立 | 每次請求新建 | 一次建立，持續使用 |
| 協定層 | HTTP/1.1 | TCP |
| 標頭大小 | 較大 | 極小 (2-14 bytes) |
| 伺服器推送 | 不支援 | 支援 |
| 資源消耗 | 較高 | 較低 |
| 適用場景 | REST API | 即時應用 |

## WebSocket 協定詳解

### 連線建立過程 (Handshake)

客戶端發送 HTTP Upgrade 請求：

```http
GET /ws HTTP/1.1
Host: example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
```

伺服器回應：

```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
```

### WebSocket 訊息框架

WebSocket 訊息使用二元框架傳輸：

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+[F|R|R|R|Opcode|M|M|M|M]
+Payload Length|Navigator|
+                 Extended Payload Length
+                 ...
+                 Masking Key (if mask bit is 1)
+                 ...
+                 Payload Data
+                 ...
```

### Opcode 類型

| Opcode | 意義 |
|--------|------|
| 0x0 | 延續框架 |
| 0x1 | 文字框架 |
| 0x2 | 二元框架 |
| 0x8 | 關閉連線 |
| 0x9 | Ping |
| 0xA | Pong |

## 客戶端 WebSocket API

### 基本使用

```javascript
const ws = new WebSocket('wss://api.example.com/ws');

ws.onopen = () => {
    console.log('WebSocket 連線已建立');
    
    // 發送連線確認訊息
    ws.send(JSON.stringify({
        type: 'auth',
        token: 'your-auth-token'
    }));
};

ws.onmessage = (event) => {
    try {
        const data = JSON.parse(event.data);
        handleMessage(data);
    } catch (e) {
        console.log('收到文字訊息:', event.data);
    }
};

ws.onerror = (error) => {
    console.error('WebSocket 錯誤:', error);
};

ws.onclose = (event) => {
    console.log('WebSocket 連線已關閉', {
        code: event.code,
        reason: event.reason,
        wasClean: event.wasClean
    });
};

function handleMessage(data) {
    switch (data.type) {
        case 'chat':
            console.log('收到聊天訊息:', data.content);
            break;
        case 'notification':
            showNotification(data.title, data.body);
            break;
        case 'update':
            updateData(data.content);
            break;
    }
}

// 發送訊息
ws.send(JSON.stringify({
    type: 'chat',
    content: 'Hello, World!'
}));

// 關閉連線
ws.close(1000, '正常關閉');
```

### 二元資料傳輸

```javascript
// 發送二進制資料
const buffer = new ArrayBuffer(8);
const view = new DataView(buffer);
view.setInt32(0, 42, true);
ws.send(buffer);

// 接收二進制資料
ws.onmessage = (event) => {
    if (event.data instanceof Blob) {
        handleBlob(event.data);
    } else if (event.data instanceof ArrayBuffer) {
        handleArrayBuffer(event.data);
    }
};

function handleArrayBuffer(buffer) {
    const view = new DataView(buffer);
    console.log('整數值:', view.getInt32(0, true));
}
```

### 心跳機制

```javascript
class HeartbeatWebSocket {
    constructor(url, options = {}) {
        this.url = url;
        this.heartbeatInterval = options.heartbeatInterval || 30000;
        this.reconnectInterval = options.reconnectInterval || 3000;
        this.connect();
    }
    
    connect() {
        this.ws = new WebSocket(this.url);
        this.setupEventHandlers();
    }
    
    setupEventHandlers() {
        this.ws.onopen = () => {
            console.log('連線建立');
            this.startHeartbeat();
        };
        
        this.ws.onmessage = (event) => {
            if (event.data === 'pong') {
                console.log('收到 pong');
            } else {
                this.handleMessage(event);
            }
        };
        
        this.ws.onclose = () => {
            console.log('連線關閉');
            this.stopHeartbeat();
            this.reconnect();
        };
        
        this.ws.onerror = (error) => {
            console.error('錯誤:', error);
        };
    }
    
    startHeartbeat() {
        this.heartbeatTimer = setInterval(() => {
            if (this.ws.readyState === WebSocket.OPEN) {
                this.ws.send('ping');
            }
        }, this.heartbeatInterval);
    }
    
    stopHeartbeat() {
        if (this.heartbeatTimer) {
            clearInterval(this.heartbeatTimer);
        }
    }
    
    reconnect() {
        this.reconnectTimer = setInterval(() => {
            console.log('嘗試重新連線...');
            this.connect();
        }, this.reconnectInterval);
    }
    
    handleMessage(event) {
        // 處理一般訊息
    }
    
    send(data) {
        if (this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(JSON.stringify(data));
        }
    }
    
    close() {
        this.stopHeartbeat();
        if (this.reconnectTimer) {
            clearInterval(this.reconnectTimer);
        }
        this.ws.close();
    }
}
```

## 伺服端 WebSocket 實現

### 原生 Node.js ws 庫

```javascript
const WebSocket = require('ws');
const http = require('http');

const server = http.createServer((req, res) => {
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('WebSocket Server');
});

const wss = new WebSocket.Server({ server });

// 客戶端連線計數器
let clientId = 0;

// 處理新連線
wss.on('connection', (ws, req) => {
    const id = ++clientId;
    const ip = req.socket.remoteAddress;
    console.log(`客戶端 ${id} 連線 from ${ip}`);
    
    // 廣播新使用者加入
    broadcast({
        type: 'system',
        content: `使用者 ${id} 加入`
    });
    
    ws.id = id;
    ws.isAlive = true;
    
    // 心跳 Pong 回應
    ws.on('pong', () => {
        ws.isAlive = true;
    });
    
    // 處理訊息
    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            handleMessage(ws, data);
        } catch (e) {
            console.error('訊息解析錯誤:', e);
        }
    });
    
    // 處理關閉
    ws.on('close', (code, reason) => {
        console.log(`客戶端 ${id} 關閉: ${code} - ${reason}`);
        broadcast({
            type: 'system',
            content: `使用者 ${id} 離開`
        });
    });
    
    // 錯誤處理
    ws.on('error', (error) => {
        console.error(`客戶端 ${id} 錯誤:`, error);
    });
    
    // 發送歡迎訊息
    ws.send(JSON.stringify({
        type: 'welcome',
        id: id,
        content: '歡迎加入!'
    }));
});

// 訊息處理
function handleMessage(ws, data) {
    switch (data.type) {
        case 'chat':
            broadcast({
                type: 'chat',
                sender: ws.id,
                content: data.content,
                timestamp: Date.now()
            });
            break;
            
        case 'broadcast':
            broadcast({
                type: 'broadcast',
                sender: ws.id,
                content: data.content,
                timestamp: Date.now()
            });
            break;
            
        case 'private':
            sendTo(ws, data.targetId, {
                type: 'private',
                sender: ws.id,
                content: data.content,
                timestamp: Date.now()
            });
            break;
            
        case 'ping':
            ws.send(JSON.stringify({ type: 'pong' }));
            break;
    }
}

// 廣播給所有客戶端
function broadcast(data) {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(data));
        }
    });
}

// 發送給特定客戶端
function sendTo(sender, targetId, data) {
    const target = Array.from(wss.clients)
        .find(client => client.id === targetId);
    
    if (target && target.readyState === WebSocket.OPEN) {
        target.send(JSON.stringify(data));
        sender.send(JSON.stringify({
            type: 'sent',
            targetId: targetId,
            content: data.content
        }));
    } else {
        sender.send(JSON.stringify({
            type: 'error',
            content: `使用者 ${targetId} 不存在或離線`
        }));
    }
}

// 心跳檢查
const heartbeatInterval = setInterval(() => {
    wss.clients.forEach((ws) => {
        if (ws.isAlive === false) {
            return ws.terminate();
        }
        
        ws.isAlive = false;
        ws.ping();
    });
}, 30000);

wss.on('close', () => {
    clearInterval(heartbeatInterval);
});

server.listen(8080, () => {
    console.log('WebSocket 伺服器執行於 ws://localhost:8080');
});
```

### Socket.io 框架

Socket.io 是更高级的 WebSocket 封裝，提供自動重連、房間、廣播等功能。

```javascript
const { Server } = require('socket.io');
const http = require('http');

const server = http.createServer();
const io = new Server(server, {
    cors: {
        origin: '*',
        methods: ['GET', 'POST']
    }
});

// 連線事件
io.on('connection', (socket) => {
    console.log(`使用者連線: ${socket.id}`);
    
    // 加入房間
    socket.on('join', (room) => {
        socket.join(room);
        console.log(`Socket ${socket.id} 加入房間 ${room}`);
    });
    
    // 離開房間
    socket.on('leave', (room) => {
        socket.leave(room);
    });
    
    // 聊天訊息
    socket.on('chat message', (data) => {
        // 廣播給所有人
        io.emit('chat message', data);
        
        // 廣播給房間內的人
        io.to(data.room).emit('chat message', data);
    });
    
    // 私人訊息
    socket.on('private message', (data) => {
        io.to(data.to).emit('private message', {
            from: socket.id,
            message: data.message
        });
    });
    
    // 斷線
    socket.on('disconnect', () => {
        console.log(`使用者離線: ${socket.id}`);
    });
});

// 發送給所有人
io.emit('broadcast', { message: 'Hello everyone' });

// 發送給特定房間
io.to('room1').emit('room message', { message: 'Hello room1' });

server.listen(3000, () => {
    console.log('Socket.io 伺服器執行於 http://localhost:3000');
});
```

### Socket.io 客戶端

```html
<script src="/socket.io/socket.io.js"></script>
<script>
    const socket = io();
    
    // 連線
    socket.on('connect', () => {
        console.log('已連線', socket.id);
    });
    
    // 聊天訊息
    socket.on('chat message', (data) => {
        console.log('收到:', data);
    });
    
    // 私人訊息
    socket.on('private message', (data) => {
        console.log('私人訊息:', data);
    });
    
    // 發送聊天訊息
    function sendMessage(message) {
        socket.emit('chat message', {
            message: message,
            room: 'general'
        });
    }
    
    // 發送私人訊息
    function sendPrivate(to, message) {
        socket.emit('private message', {
            to: to,
            message: message
        });
    }
    
    // 加入房間
    socket.emit('join', 'general');
</script>
```

## 訊息協定設計

### 訊息類型

```javascript
// 常用訊息類型定義
const MessageTypes = {
    // 系統
    SYSTEM: 'system',
    ERROR: 'error',
    
    // 認證
    AUTH_REQUEST: 'auth_request',
    AUTH_RESPONSE: 'auth_response',
    
    // 聊天
    CHAT: 'chat',
    CHAT_IMAGE: 'chat_image',
    TYPING: 'typing',
    READ: 'read',
    
    // 即時協作
    CURSOR_MOVE: 'cursor_move',
    CONTENT_CHANGE: 'content_change',
    
    // 遊戲
    GAME_ACTION: 'game_action',
    GAME_STATE: 'game_state',
    
    // 即時更新
    NOTIFICATION: 'notification',
    UPDATE: 'update'
};
```

### 訊息格式

```javascript
// 標準訊息格式
const message = {
    id: 'msg_123',
    type: 'chat',
    sender: {
        id: 'user_1',
        name: '王小明',
        avatar: 'https://...'
    },
    content: 'Hello!',
    timestamp: 1704067200000,
    metadata: {
        replyTo: 'msg_122'
    }
};

// 聊天訊息
{
    type: 'chat',
    roomId: 'room_1',
    content: {
        text: '訊息內容',
        mentions: ['user_2'],
        replyTo: 'msg_122'
    }
}

// 即時協作
{
    type: 'content_change',
    roomId: 'doc_1',
    operations: [
        { op: 'insert', pos: 10, text: 'a' },
        { op: 'delete', pos: 11, length: 2 }
    ],
    revision: 42
}
```

## 重連機制

```javascript
class ReconnectingWebSocket {
    constructor(url, options = {}) {
        this.url = url;
        this.maxRetries = options.maxRetries || 5;
        this.retryInterval = options.retryInterval || 1000;
        this.retryCount = 0;
        this.connect();
    }
    
    connect() {
        this.ws = new WebSocket(this.url);
        this.setupEventHandlers();
    }
    
    setupEventHandlers() {
        this.ws.onopen = (event) => {
            console.log('連線建立');
            this.retryCount = 0;
            if (this.onopen) this.onopen(event);
        };
        
        this.ws.onclose = (event) => {
            console.log('連線關閉');
            if (this.onclose) this.onclose(event);
            this.attemptReconnect();
        };
        
        this.ws.onerror = (event) => {
            console.error('錯誤');
            if (this.onerror) this.onerror(event);
        };
        
        this.ws.onmessage = (event) => {
            if (this.onmessage) this.onmessage(event);
        };
    }
    
    attemptReconnect() {
        if (this.retryCount < this.maxRetries) {
            this.retryCount++;
            const delay = this.retryInterval * Math.pow(2, this.retryCount - 1);
            console.log(`${delay}ms 後重試 (${this.retryCount}/${this.maxRetries})`);
            
            setTimeout(() => {
                this.connect();
            }, delay);
        } else {
            console.error('達到最大重試次數');
        }
    }
    
    send(data) {
        if (this.ws.readyState === WebSocket.OPEN) {
            this.ws.send(data);
        } else {
            console.error('連線未建立');
        }
    }
    
    close(code, reason) {
        this.maxRetries = 0;
        this.ws.close(code, reason);
    }
}
```

## 安全性考量

### 來源驗證

```javascript
const allowedOrigins = [
    'https://example.com',
    'https://app.example.com'
];

wss.on('connection', (ws, req) => {
    const origin = req.headers.origin;
    
    if (!allowedOrigins.includes(origin)) {
        console.log(`拒絕連線 from ${origin}`);
        return ws.close(4001, '不允許的來源');
    }
    
    // 繼續處理連線
});
```

### 訊息驗證

```javascript
function validateMessage(data) {
    const schema = {
        type: 'string',
        content: 'string',
        roomId: 'string'
    };
    
    for (const [key, type] of Object.entries(schema)) {
        if (typeof data[key] !== type) {
            return { valid: false, error: `欄位 ${key} 類型錯誤` };
        }
    }
    
    return { valid: true };
}

ws.on('message', (message) => {
    const data = JSON.parse(message);
    const validation = validateMessage(data);
    
    if (!validation.valid) {
        ws.send(JSON.stringify({
            type: 'error',
            content: validation.error
        }));
        return;
    }
    
    // 處理驗證通過的訊息
});
```

### WSS (WebSocket Secure)

```javascript
const https = require('https');
const fs = require('fs');
const WebSocket = require('ws');

const server = https.createServer({
    key: fs.readFileSync('key.pem'),
    cert: fs.readFileSync('cert.pem')
});

const wss = new WebSocket.Server({ server });

wss.on('connection', (ws, req) => {
    console.log('安全連線建立');
});

server.listen(8080, () => {
    console.log('WSS 伺服器執行於 https://localhost:8080');
});
```

## 應用場景

### 即時聊天室

```javascript
// 聊天室訊息處理
function handleChatMessage(ws, data) {
    const { roomId, content } = data;
    
    // 儲存訊息到資料庫
    saveMessage({
        roomId,
        userId: ws.userId,
        content,
        timestamp: Date.now()
    });
    
    // 廣播給房間內所有人
    io.to(roomId).emit('chat message', {
        id: messageId,
        userId: ws.userId,
        userName: ws.userName,
        content,
        timestamp: Date.now()
    });
}
```

### 即時協作編輯

```javascript
// OT (Operational Transformation) 簡化版
function applyOperation(doc, operation) {
    switch (operation.op) {
        case 'insert':
            return doc.slice(0, operation.pos) + 
                   operation.text + 
                   doc.slice(operation.pos);
        case 'delete':
            return doc.slice(0, operation.pos) + 
                   doc.slice(operation.pos + operation.length);
        default:
            return doc;
    }
}

socket.on('content_change', (data) => {
    // 轉發操作給其他人
    socket.to(data.docId).emit('content_change', {
        operations: data.operations,
        revision: data.revision
    });
});
```

### 即時通知系統

```javascript
function sendNotification(userId, notification) {
    io.to(`user_${userId}`).emit('notification', {
        id: notification.id,
        title: notification.title,
        body: notification.body,
        icon: notification.icon,
        timestamp: Date.now()
    });
}
```

### 即時遊戲

```javascript
function handleGameAction(socket, action) {
    const { gameId, action: gameAction } = action;
    
    // 驗證遊戲狀態
    const game = games.get(gameId);
    if (!game || game.status !== 'playing') {
        return socket.emit('error', '遊戲不存在或未開始');
    }
    
    // 驗證玩家
    if (!game.players.includes(socket.id)) {
        return socket.emit('error', '不是遊戲參與者');
    }
    
    // 應用遊戲動作
    const newState = applyGameAction(game, gameAction);
    
    // 廣播遊戲狀態
    io.to(gameId).emit('game_state', newState);
}
```

## 常見問題與解決方案

### 代理伺服器問題

某些代理伺服器可能會中斷 WebSocket 連線。解決方案：

```javascript
// 客戶端定期發送心跳
setInterval(() => {
    if (ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ type: 'ping' }));
    }
}, 25000);
```

### 行動裝置休眠

行動裝置可能會關閉 WebSocket 連線以節省電量。解決方案：

- 使用通知服務（如 Firebase Cloud Messaging）喚醒應用程式
- 在前景時使用 WebSocket，背景時使用輪詢
- 實現快速重連機制

### 連線數限制

瀏覽器對每個網域的 WebSocket 連線數量有限制（通常為 6-30 個）。解決方案：

- 使用單一 WebSocket 連線傳送所有資料
- 使用子網域分散連線
- 使用 Socket.io 的 Multiplexing 功能

## 相關概念

- [後端開發](後端開發.md) - 了解後端開發
- [HTTP](HTTP.md) - 了解 HTTP 協定
- [Node.js](Node.js.md) - 了解 Node.js 開發
- [API](API.md) - 了解 API 設計

---

**參考來源**:
- [WebSocket 協定 RFC 6455](https://tools.ietf.org/html/rfc6455)
- [MDN Web Docs - WebSocket](https://developer.mozilla.org/zh-TW/docs/Web/API/WebSocket)
- [Socket.io 官方文檔](https://socket.io/)
