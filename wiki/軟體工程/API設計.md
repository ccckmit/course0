# API 設計

API（Application Programming Interface）是軟體系統之間的通訊契約。良好的 API 設計能提升系統的可維護性、可測試性和開發效率。

## 設計原則

### KISS 原則

保持簡單，不要過度設計：

```
CLI → REST API → React 前端
```

從最簡單的介面開始，逐步疊加複雜度。每個階段都可獨立測試和驗證。

### 向後相容

API 一旦發布就要維持向後相容。不相容的變更需要新版 API。

## CLI 設計（Git 風格）

使用 `cmd op args` 模式，類似 Git 的命令列介面。

### 結構

```
$ program <command> [<args>]
```

### CLI 範例

```python
# cli.py
import sys

def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "help"
    match cmd:
        case "add":
            add_item(sys.argv[2])
        case "list":
            list_items()
        case "remove":
            remove_item(sys.argv[2])
        case "help":
            print("Usage: cli.py <add|list|remove> [args]")

if __name__ == "__main__":
    main()
```

### CLI 測試

使用 case script 測試：

```bash
# case1.sh
set -x
python cli.py add "hello"
python cli.py add "world"
python cli.py list
python cli.py remove "hello"
```

## REST API 設計

在 CLI 基礎上封裝 REST API，支援 JSON 格式的請求和回應。

### 資源導向

```
GET    /items          # 列表
POST   /items          # 新增
GET    /items/:id      # 查詢
PUT    /items/:id      # 更新
DELETE /items/:id      # 刪除
```

### REST API 範例

```python
# api.py
from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class Item(BaseModel):
    name: str

items = []

@app.get("/items")
def list_items():
    return items

@app.post("/items")
def create_item(item: Item):
    items.append(item)
    return {"status": "ok"}

@app.delete("/items/{item_id}")
def delete_item(item_id: int):
    items.pop(item_id)
    return {"status": "ok"}
```

### API 測試

```python
# test_api.py
from fastapi.testclient import TestClient
from api import app

client = TestClient(app)

def test_create_item():
    resp = client.post("/items", json={"name": "hello"})
    assert resp.status_code == 200
```

## React 前端

最後使用 React 開發前端，透過 REST API 與後端溝通。

```jsx
// App.jsx
function App() {
  const [items, setItems] = useState([]);

  useEffect(() => {
    fetch('/api/items')
      .then(r => r.json())
      .then(setItems);
  }, []);

  return (
    <ul>
      {items.map((i, idx) => (
        <li key={idx}>{i.name}</li>
      ))}
    </ul>
  );
}
```

## 參考

- [ccc_code_skill.md](https://github.com/ccckmit/course0/blob/main/code/%E8%BB%9F%E9%AB%94%E5%B7%A5%E7%A8%8B/ccc_code_skill.md) — 寫程式約定（第9條）
- [微服務架構](微服務架構.md) — 服務拆分與 API 閘道
