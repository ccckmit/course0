# RESTful

## 概述

RESTful API 是一種基於 REST（Representational State Transfer）架構風格的 API 設計方式，由 Roy Fielding 在其 2000 年的博士論文中提出。RESTful API 目前是 Web 服務設計的主流標準，廣泛應用於 Web 應用、行動應用和微服務架構中。

REST 的核心概念是將所有內容抽象為資源（Resource），每個資源由唯一的 URI 標識，用標準的 HTTP 方法對資源進行操作。RESTful API 以其簡潔性、可擴展性和良好的緩存支持，成為現代 Web API 設計的首選。

## REST 六大原則

### 1. 客戶端-伺服器分離（Client-Server）

客戶端和伺服器獨立演進，客戶端不需要了解伺服器的實現細節，伺服器也不需要了解客戶端的業務邏輯。這種分離允許兩邊獨立開發和擴展。

```python
# 客戶端（不知道伺服器如何實現）
response = requests.get('/api/users/123')

# 伺服器（不知道客戶端如何處理）
# 可以用任何語言實現：Python, Node.js, Go, Java...
```

### 2. 無狀態（Stateless）

每次請求必須包含伺服器處理該請求所需的所有資訊，伺服器不存儲任何客戶端狀態。這簡化了伺服器設計，提高可擴展性。

```python
# 每個請求都攜帶認證資訊
headers = {'Authorization': 'Bearer token123'}
response = requests.get('/api/users', headers=headers)
```

### 3. 可緩存（Cacheable）

回應可以標記為可緩存或不可緩存，客戶端和中介層可以緩存回應以提高效能。

```python
# 伺服器端設置緩存控制
response.headers['Cache-Control'] = 'public, max-age=3600'

# 客戶端使用緩存
# GET /api/products 請求會先檢查本地緩存
```

### 4. 分層系統（Layered System）

客戶端不需要知道連接的是最終伺服器還是中介層（如負載均衡器、代理伺服器），這允許系統在不改變客戶端的情況下擴展架構。

```
客戶端 → 負載均衡器 → API 閘道 → 服務A/服務B/服務C
```

### 5. 統一介面（Uniform Interface）

REST API 使用標準的 HTTP 方法和一致的資源表示方式，簡化了客戶端和伺服器之間的交互。

### 6. 可發現性（Self-Descriptive）

回應包含足夠的資訊，讓客戶端知道如何處理回應（如透過 Content-Type header）。

## HTTP 方法對照

| 方法 | 用途 | 安全性 | 幂等性 |
|------|------|--------|--------|
| GET | 讀取資源 | 安全 | 幂等 |
| POST | 創建資源 | 不安全 | 非幂等 |
| PUT | 完全更新 | 不安全 | 幂等 |
| PATCH | 部分更新 | 不安全 | 非幂等 |
| DELETE | 刪除資源 | 不安全 | 幂等 |

```python
# 範例：使用者資源的完整 CRUD 操作

# 取得使用者列表
GET /api/users

# 取得單一使用者
GET /api/users/123

# 新增使用者
POST /api/users
Body: {"name": "John", "email": "john@example.com"}

# 完全更新使用者
PUT /api/users/123
Body: {"name": "John Doe", "email": "john@example.com"}

# 部分更新使用者
PATCH /api/users/123
Body: {"email": "newemail@example.com"}

# 刪除使用者
DELETE /api/users/123
```

## URL 設計最佳實踐

### 資源命名

```python
# 使用名詞而非動詞
# ✓ Good
GET /api/products
POST /api/orders

# ✗ Bad
GET /api/getProducts
POST /api/createOrder

# 使用複數形式
GET /api/users          # ✓
GET /api/user           # ✗

# 使用層級結構表示關係
GET /api/users/123/orders          # 使用者123的訂單
GET /api/orders/456/items         # 訂單456的商品
GET /api/products/789/reviews    # 商品789的評論
```

### 查詢參數

```python
# 篩選
GET /api/products?category=electronics&price<1000

# 排序
GET /api/products?sort=price&order=desc

# 分頁
GET /api/products?page=2&limit=20

# 欄位選擇
GET /api/users?fields=name,email

# 搜尋
GET /api/products?q=laptop
```

## HTTP 狀態碼

### 成功回應 (2xx)

```python
200 OK                    # 請求成功
201 Created              # 資源創建成功
202 Accepted            # 請求已接受，但處理未完成
204 No Content          # 請求成功，無回應內容
```

### 客戶端錯誤 (4xx)

```python
400 Bad Request         # 請求格式錯誤
401 Unauthorized        # 需要認證
403 Forbidden          # 無權限訪問
404 Not Found         # 資源不存在
405 Method Not Allowed # 不允許的 HTTP 方法
409 Conflict           # 資源衝突
422 Unprocessable Entity # 請求格式正確但無法處理
429 Too Many Requests  # 請求過多
```

### 伺服器錯誤 (5xx)

```python
500 Internal Server Error   # 伺服器內部錯誤
502 Bad Gateway            # 閘道錯誤
503 Service Unavailable     # 服務不可用
504 Gateway Timeout         # 閘道超時
```

## 認證與授權

### API Key

```python
# 簡單的 API Key 認證
headers = {'X-API-Key': 'your-api-key-here'}
response = requests.get('/api/data', headers=headers)
```

### Bearer Token (JWT)

```python
# 使用 JWT Token
headers = {'Authorization': 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...'}
response = requests.get('/api/data', headers=headers)
```

### OAuth 2.0

```python
# OAuth 2.0 授權流程
# 1. 獲取授權碼
# 2. 交換為訪問令牌
# 3. 使用訪問令牌訪問 API

auth_header = {'Authorization': f'Bearer {access_token}'}
```

## 版本控制

```python
# 常見的 API 版本控制方式

# URL 路徑（最常見）
GET /api/v1/users
GET /api/v2/users

# Header
GET /api/users
Headers: {'Accept-Version': 'v1'}

# Query Parameter
GET /api/users?version=1
```

## HATEOAS

Hypermedia as the Engine of Application State 是 REST 的可選擴展，提供回應中的連結，讓客戶端能動態探索 API。

```python
# 伺服器回應
{
    "user": {
        "id": 123,
        "name": "John"
    },
    "links": {
        "self": "/api/users/123",
        "orders": "/api/users/123/orders",
        "settings": "/api/users/123/settings"
    }
}
```

## GraphQL 與 RESTful 比較

| 特性 | RESTful | GraphQL |
|------|---------|---------|
| 數據獲取 | 固定端點 | 靈活查詢 |
| 請求數量 | 可能需要多次 | 一次請求 |
| 類型系統 | 無（可選用 JSON Schema） | 固有 |
| 緩存 | HTTP 緩存簡單 | 需要特殊處理 |

## 實際範例

### Python Flask 實現

```python
from flask import Flask, jsonify, request

app = Flask(__name__)

products = [
    {'id': 1, 'name': 'Laptop', 'price': 999},
    {'id': 2, 'name': 'Mouse', 'price': 29}
]

@app.route('/api/products', methods=['GET'])
def get_products():
    return jsonify(products)

@app.route('/api/products/<int:product_id>', methods=['GET'])
def get_product(product_id):
    product = next((p for p in products if p['id'] == product_id), None)
    if product:
        return jsonify(product)
    return jsonify({'error': 'Not found'}), 404

@app.route('/api/products', methods=['POST'])
def create_product():
    data = request.get_json()
    new_id = max(p['id'] for p in products) + 1
    product = {'id': new_id, **data}
    products.append(product)
    return jsonify(product), 201

@app.route('/api/products/<int:product_id>', methods=['DELETE'])
def delete_product(product_id):
    global products
    products = [p for p in products if p['id'] != product_id]
    return '', 204
```

### JavaScript 前端調用

```javascript
// 使用 Fetch API 調用 RESTful API

// GET
async function getProducts() {
    const response = await fetch('/api/products');
    const data = await response.json();
    return data;
}

// POST
async function createProduct(product) {
    const response = await fetch('/api/products', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify(product)
    });
    return response.json();
}
```

## 相關概念

- [API](API.md) - API 基礎概念
- [後端開發](後端開發.md) - 伺服器端開發
- [HTTP](HTTP.md) - HTTP 協定
- [Fetch_API](Fetch_API.md) - 前端 HTTP 請求

## Tags

#RESTful #API #REST #WebService #HTTP #JSON