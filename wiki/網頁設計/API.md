# API (應用程式介面)

**標籤**: #API #REST #RESTful #GraphQL #Web_API
**日期**: 2024-01-15

API (Application Programming Interface，應用程式介面) 是軟體元件之間溝通的介面，定義了如何請求和交換資料。在現代軟體開發中，API 是系統之間互動的基礎，無論是前端與後端、微服務之間，還是與第三方服務整合，都依賴於 API。

## API 的發展歷程

### 早期 API (1960s-1980s)

早期的 API 主要存在于操作系統和資料庫中，如 Windows API、POSIX。這些 API 通常使用 C 語言編寫的函式庫形式發布。

### Web Service 時代 (1990s-2000s)

隨著網際網路的普及，SOAP (Simple Object Access Protocol) 成為企業級 Web Service 的標準。SOAP 使用 XML 格式傳輸資料，透過 HTTP 協定進行通訊。

### RESTful API 時代 (2000s-現在)

REST (Representational State Transfer) 由 Roy Fielding 於 2000 年提出，成為 Web API 的主流設計風格。RESTful API 使用標準 HTTP 方法，透過 JSON 格式傳輸資料，簡單易用且擴展性強。

### 現代 API 時代 (2010s-現在)

GraphQL、gRPC、WebSocket 等新技術持續演進，API 的設計趨向於更靈活、更高效。

## REST API 設計

### REST 架構約束

REST API 遵循六個架構約束：

| 約束 | 說明 |
|------|------|
| 客戶端-伺服器 | 客戶端和伺服器分離，各自獨立演化 |
| 無狀態 | 每個請求包含所有必要資訊，伺服器不儲存客戶端狀態 |
| 可快取 |  response 可標記為可快取或不可快取 |
| 統一介面 | 使用標準的資源識別和操作方法 |
| 分層系統 | 客戶端不需要知道直接連接還是透過中介層 |
| 按需程式碼 (可選) | 客戶端可下載並執行伺服器提供的程式碼 |

### 資源導向設計

REST API 以資源 (Resource) 為中心設計，使用 URI 識別資源。

```http
# 資源命名 convention
/users              # 使用者集合
/users/123          # 特定使用者
/users/123/orders   # 使用者的訂單
/orders             # 訂單集合
/products           # 產品集合
```

### HTTP 方法映射

| 方法 | 用途 | 安全性 | 幂等性 |
|------|------|--------|--------|
| GET | 讀取資源 | 安全 | 幂等 |
| POST | 建立資源 | 不安全 | 不幂等 |
| PUT | 完全更新資源 | 不安全 | 幂等 |
| PATCH | 部分更新資源 | 不安全 | 不幂等 |
| DELETE | 刪除資源 | 不安全 | 幂等 |
| HEAD | 讀取資源header | 安全 | 幂等 |
| OPTIONS | 取得支援的方法 | 安全 | 幂等 |

### HTTP 狀態碼

```http
// 成功回應 (2xx)
200 OK                          - 請求成功
201 Created                     - 資源建立成功
202 Accepted                    - 請求已接受，處理中
204 No Content                  - 請求成功但無回應內容

// 客戶端錯誤 (4xx)
400 Bad Request                 - 請求格式錯誤
401 Unauthorized                - 需要身份驗證
403 Forbidden                   - 權限不足
404 Not Found                   - 資源不存在
405 Method Not Allowed          - 不支援此 HTTP 方法
409 Conflict                    - 資源衝突
422 Unprocessable Entity        - 請求語法正確但無法處理
429 Too Many Requests           - 請求次數過多

// 伺服器錯誤 (5xx)
500 Internal Server Error       - 伺服器內部錯誤
502 Bad Gateway                - 閘道錯誤
503 Service Unavailable        - 服務不可用
504 Gateway Timeout            - 閘道逾時
```

### RESTful API 範例

```javascript
const express = require('express');
const app = express();
app.use(express.json());

const users = new Map();

// GET - 取得所有使用者
app.get('/api/users', (req, res) => {
    const page = parseInt(req.query.page) || 1;
    const limit = parseInt(req.query.limit) || 10;
    const offset = (page - 1) * limit;
    
    const allUsers = Array.from(users.values());
    const paginatedUsers = allUsers.slice(offset, offset + limit);
    
    res.json({
        data: paginatedUsers,
        pagination: {
            page,
            limit,
            total: allUsers.length,
            totalPages: Math.ceil(allUsers.length / limit)
        }
    });
});

// GET - 取得特定使用者
app.get('/api/users/:id', (req, res) => {
    const user = users.get(parseInt(req.params.id));
    
    if (!user) {
        return res.status(404).json({ error: '使用者不存在' });
    }
    
    res.json(user);
});

// POST - 建立使用者
app.post('/api/users', (req, res) => {
    const { name, email } = req.body;
    
    if (!name || !email) {
        return res.status(400).json({ error: '姓名和 Email 為必填欄位' });
    }
    
    const id = Date.now();
    const user = { id, name, email, createdAt: new Date() };
    users.set(id, user);
    
    res.status(201).json(user);
});

// PUT - 完全更新使用者
app.put('/api/users/:id', (req, res) => {
    const id = parseInt(req.params.id);
    
    if (!users.has(id)) {
        return res.status(404).json({ error: '使用者不存在' });
    }
    
    const { name, email } = req.body;
    const user = {
        id,
        name: name || users.get(id).name,
        email: email || users.get(id).email,
        updatedAt: new Date()
    };
    users.set(id, user);
    
    res.json(user);
});

// PATCH - 部分更新使用者
app.patch('/api/users/:id', (req, res) => {
    const id = parseInt(req.params.id);
    
    if (!users.has(id)) {
        return res.status(404).json({ error: '使用者不存在' });
    }
    
    const existingUser = users.get(id);
    const updatedUser = {
        ...existingUser,
        ...req.body,
        updatedAt: new Date()
    };
    users.set(id, updatedUser);
    
    res.json(updatedUser);
});

// DELETE - 刪除使用者
app.delete('/api/users/:id', (req, res) => {
    const id = parseInt(req.params.id);
    
    if (!users.has(id)) {
        return res.status(404).json({ error: '使用者不存在' });
    }
    
    users.delete(id);
    res.status(204).send();
});
```

## JSON 資料格式

### JSON 結構

```json
{
    "id": 1,
    "name": "王小明",
    "email": "wang@example.com",
    "age": 25,
    "isActive": true,
    "skills": ["JavaScript", "Python", "Go"],
    "address": {
        "city": "台北市",
        "district": "信義區",
        "zipCode": "110"
    },
    "createdAt": "2024-01-15T10:30:00Z",
    "roles": [
        {
            "name": "admin",
            "permissions": ["read", "write", "delete"]
        },
        {
            "name": "user",
            "permissions": ["read"]
        }
    ]
}
```

### JSON Schema

```json
{
    "$schema": "http://json-schema.org/draft-07/schema#",
    "type": "object",
    "required": ["name", "email"],
    "properties": {
        "id": {
            "type": "integer",
            "description": "使用者 ID"
        },
        "name": {
            "type": "string",
            "minLength": 1,
            "maxLength": 100
        },
        "email": {
            "type": "string",
            "format": "email"
        },
        "age": {
            "type": "integer",
            "minimum": 0,
            "maximum": 150
        },
        "roles": {
            "type": "array",
            "items": {
                "type": "object",
                "properties": {
                    "name": { "type": "string" },
                    "permissions": {
                        "type": "array",
                        "items": { "type": "string" }
                    }
                }
            }
        }
    }
}
```

## Fetch API

### 基本請求

```javascript
// GET 請求
fetch('https://api.example.com/users')
    .then(response => {
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        return response.json();
    })
    .then(data => console.log(data))
    .catch(error => console.error('錯誤:', error));

// POST 請求
fetch('https://api.example.com/users', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
        'Authorization': 'Bearer token123'
    },
    body: JSON.stringify({
        name: '王小明',
        email: 'wang@example.com'
    })
})
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.error('錯誤:', error));
```

### 進階選項

```javascript
// PUT 請求
fetch('https://api.example.com/users/1', {
    method: 'PUT',
    headers: {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify({ name: '王小明' })
});

// DELETE 請求
fetch('https://api.example.com/users/1', {
    method: 'DELETE'
});

// 自訂錯誤處理
async function fetchWithErrorHandling(url, options) {
    const response = await fetch(url, options);
    
    if (!response.ok) {
        const error = await response.json().catch(() => ({}));
        throw new ApiError(response.status, error.message || '請求失敗');
    }
    
    const contentType = response.headers.get('content-type');
    if (contentType && contentType.includes('application/json')) {
        return response.json();
    }
    
    return response.text();
}

class ApiError extends Error {
    constructor(status, message) {
        super(message);
        this.status = status;
    }
}
```

### Async/Await 版本

```javascript
async function getUsers() {
    try {
        const response = await fetch('https://api.example.com/users');
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }
        
        const users = await response.json();
        return users;
    } catch (error) {
        console.error('取得使用者失敗:', error);
        return [];
    }
}

async function createUser(userData) {
    try {
        const response = await fetch('https://api.example.com/users', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(userData)
        });
        
        if (!response.ok) {
            const error = await response.json();
            throw new Error(error.message || '建立失敗');
        }
        
        return await response.json();
    } catch (error) {
        console.error('建立使用者失敗:', error);
        throw error;
    }
}
```

## GraphQL

### GraphQL 基礎

GraphQL 是 Facebook 開發的 API 查詢語言，允許客戶端精確指定需要的資料，減少過度或不足fetch的問題。

```graphql
# 查詢 - 取得使用者及其文章
query {
    user(id: 1) {
        id
        name
        email
        posts {
            id
            title
            content
            createdAt
        }
    }
}

# 變更 - 建立新文章
mutation {
    createPost(
        userId: 1,
        title: "新文章",
        content: "文章內容"
    ) {
        id
        title
        createdAt
    }
}

# 訂閱 - 即時更新
subscription {
    newPost {
        id
        title
        author {
            name
        }
    }
}
```

### GraphQL 伺服器 (Node.js)

```javascript
const { GraphQLSchema, GraphQLObjectType, GraphQLString, GraphQLInt, GraphQLList } = require('graphql');

const users = [
    { id: 1, name: '王小明', email: 'wang@example.com' },
    { id: 2, name: '李小華', email: 'lee@example.com' }
];

const posts = [
    { id: 1, userId: 1, title: '第一篇文章', content: '內容' },
    { id: 2, userId: 1, title: '第二篇文章', content: '內容' }
];

const UserType = new GraphQLObjectType({
    name: 'User',
    fields: {
        id: { type: GraphQLInt },
        name: { type: GraphQLString },
        email: { type: GraphQLString },
        posts: {
            type: new GraphQLList(PostType),
            resolve: (user) => posts.filter(p => p.userId === user.id)
        }
    }
});

const PostType = new GraphQLObjectType({
    name: 'Post',
    fields: {
        id: { type: GraphQLInt },
        userId: { type: GraphQLInt },
        title: { type: GraphQLString },
        content: { type: GraphQLString }
    }
});

const QueryType = new GraphQLObjectType({
    name: 'Query',
    fields: {
        user: {
            type: UserType,
            args: { id: { type: GraphQLInt } },
            resolve: (_, { id }) => users.find(u => u.id === id)
        },
        users: {
            type: new GraphQLList(UserType),
            resolve: () => users
        }
    }
});

const MutationType = new GraphQLObjectType({
    name: 'Mutation',
    fields: {
        createUser: {
            type: UserType,
            args: {
                name: { type: GraphQLString },
                email: { type: GraphQLString }
            },
            resolve: (_, { name, email }) => {
                const newUser = { id: users.length + 1, name, email };
                users.push(newUser);
                return newUser;
            }
        }
    }
});

const schema = new GraphQLSchema({
    query: QueryType,
    mutation: MutationType
});
```

## WebSocket API

### 基本連線

```javascript
// 建立 WebSocket 連線
const ws = new WebSocket('wss://api.example.com/ws');

ws.onopen = () => {
    console.log('連線已建立');
    ws.send(JSON.stringify({ type: 'hello' }));
};

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    console.log('收到訊息:', data);
};

ws.onerror = (error) => {
    console.error('WebSocket 錯誤:', error);
};

ws.onclose = () => {
    console.log('連線已關閉');
};

// 發送訊息
ws.send(JSON.stringify({
    type: 'message',
    content: 'Hello WebSocket'
}));

// 關閉連線
ws.close();
```

### WebSocket 伺服器 (Node.js)

```javascript
const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', (ws) => {
    console.log('客戶端連線');
    
    ws.on('message', (message) => {
        const data = JSON.parse(message);
        console.log('收到:', data);
        
        // 廣播給所有客戶端
        wss.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(JSON.stringify({
                    type: 'broadcast',
                    data: data
                }));
            }
        });
    });
    
    ws.on('close', () => {
        console.log('客戶端離開');
    });
    
    // 發送歡迎訊息
    ws.send(JSON.stringify({ type: 'welcome' }));
});
```

## API 版本控制

### URL 版本控制

```http
GET /api/v1/users
GET /api/v2/users
```

### Header 版本控制

```http
GET /api/users
Accept: application/vnd.example.v1+json
```

```http
GET /api/users
Accept: application/vnd.example.v2+json
```

### 查詢參數版本控制

```http
GET /api/users?version=1
```

## API 安全性

### 身份驗證

```javascript
// JWT 驗證
function authenticateToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1];
    
    if (!token) {
        return res.status(401).json({ error: '缺少存取權杖' });
    }
    
    jwt.verify(token, process.env.JWT_SECRET, (err, user) => {
        if (err) {
            return res.status(403).json({ error: '權杖無效' });
        }
        req.user = user;
        next();
    });
}

app.get('/api/profile', authenticateToken, (req, res) => {
    res.json({ user: req.user });
});
```

### API Key

```javascript
// API Key 驗證
function validateApiKey(req, res, next) {
    const apiKey = req.headers['x-api-key'];
    
    if (!apiKey) {
        return res.status(401).json({ error: '缺少 API Key' });
    }
    
    const validKey = await getApiKey(apiKey);
    if (!validKey) {
        return res.status(403).json({ error: 'API Key 無效' });
    }
    
    req.apiKey = validKey;
    next();
}
```

### CORS 設定

```javascript
const cors = require('cors');

app.use(cors({
    origin: ['https://example.com', 'https://app.example.com'],
    methods: ['GET', 'POST', 'PUT', 'DELETE'],
    allowedHeaders: ['Content-Type', 'Authorization'],
    exposedHeaders: ['X-Total-Count'],
    credentials: true,
    maxAge: 86400
}));
```

### 速率限制

```javascript
const rateLimit = require('express-rate-limit');

const limiter = rateLimit({
    windowMs: 15 * 60 * 1000, // 15 分鐘
    max: 100, // 每個 IP 最多 100 個請求
    message: { error: '請求次數過多，請稍後再試' },
    standardHeaders: true,
    legacyHeaders: false
});

app.use('/api/', limiter);
```

## API 文件

### OpenAPI (Swagger)

```yaml
openapi: 3.0.0
info:
  title: 使用者管理 API
  version: 1.0.0
  description: 使用者管理 API 文件

servers:
  - url: https://api.example.com
    description: 生產環境
  - url: https://staging-api.example.com
    description: 測試環境

paths:
  /users:
    get:
      summary: 取得使用者列表
      parameters:
        - name: page
          in: query
          schema:
            type: integer
            default: 1
        - name: limit
          in: query
          schema:
            type: integer
            default: 10
      responses:
        '200':
          description: 成功
          content:
            application/json:
              schema:
                type: object
                properties:
                  data:
                    type: array
                    items:
                      $ref: '#/components/schemas/User'
                  pagination:
                    $ref: '#/components/schemas/Pagination'

components:
  schemas:
    User:
      type: object
      required:
        - name
        - email
      properties:
        id:
          type: integer
        name:
          type: string
        email:
          type: string
          format: email
        createdAt:
          type: string
          format: date-time
```

## API 閘道

### 常見功能

- 路由轉發
- 身份驗證/授權
- 速率限制
- 日誌記錄
- 快取
- 負載平衡
- SSL/TLS 終止

### 範例：Kong API 閘道

```yaml
# Kong 服務配置
services:
  - name: user-service
    url: http://user-service:3000
    routes:
      - name: user-route
        paths:
          - /api/users
    plugins:
      - name: jwt
      - name: rate-limiting
        config:
          minute: 100
      - name: cors
```

## 常見 API 模式

### HATEOAS

```json
{
    "id": 1,
    "name": "王小明",
    "links": [
        {
            "rel": "self",
            "href": "/api/users/1"
        },
        {
            "rel": "orders",
            "href": "/api/users/1/orders"
        },
        {
            "rel": "avatar",
            "href": "/api/users/1/avatar"
        }
    ]
}
```

### API as a Service 平台

| 平台 | 說明 |
|------|------|
| AWS API Gateway | AWS 的 API 閘道服務 |
| Azure API Management | Azure 的 API 管理 |
| Apigee | Google 的 API 管理平台 |
| Kong | 開源 API 閘道 |
| Express Gateway | 開源 API 閘道 |

## 相關概念

- [RESTful](RESTful.md) - 了解 RESTful API 設計
- [JavaScript](JavaScript.md) - 了解 JavaScript 基礎
- [後端開發](後端開發.md) - 了解後端開發

---

**參考來源**:
- [RESTful API 設計](https://restfulapi.net/)
- [MDN Web Docs - Fetch API](https://developer.mozilla.org/zh-TW/docs/Web/API/Fetch_API)
- [GraphQL 官方網站](https://graphql.org/)
- [OpenAPI 規範](https://swagger.io/specification/)
