# FastAPI

## 概述

FastAPI 是現代、快速（高效能）的 Python Web 框架，基於 Starlette 和 Pydantic，支援自動生成 API 文件。

## 基本架構

```python
from fastapi import FastAPI

app = FastAPI(
    title="我的 API",
    description="這是一個 FastAPI 應用",
    version="1.0.0"
)

@app.get("/")
async def root():
    return {"message": "Hello World"}
```

## 安裝與執行

```bash
pip install fastapi uvicorn

# 執行
uvicorn main:app --reload --host 0.0.0.0 --port 8000

# 或 Python 程式碼內執行
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
```

## 路徑參數

```python
from fastapi import FastAPI

app = FastAPI()

# 簡單參數
@app.get("/items/{item_id}")
async def read_item(item_id: int):
    return {"item_id": item_id}

# 多個參數
@app.get("/users/{user_id}/items/{item_id}")
async def read_user_item(user_id: int, item_id: int):
    return {"user_id": user_id, "item_id": item_id}

# 路徑優先順序
# /items/me 會在 /items/{item_id} 之前匹配
@app.get("/items/me")
async def read_current_item():
    return {"item_id": "current"}
```

## 查詢參數

```python
from typing import Optional

@app.get("/items")
async def read_items(
    skip: int = 0,
    limit: int = 10,
    category: Optional[str] = None,
    q: str = ""
):
    return {
        "skip": skip,
        "limit": limit,
        "category": category,
        "q": q
    }

# 布林值解析
@app.get("/items/{item_id}")
async def read_item(item_id: int, q: Optional[str] = None, short: bool = False):
    return {
        "item_id": item_id,
        "q": q,
        "short": short
    }
```

## 請求體 (Request Body)

```python
from pydantic import BaseModel, EmailStr, Field
from typing import Optional
from datetime import datetime

class Item(BaseModel):
    name: str = Field(..., min_length=1, max_length=100)
    description: Optional[str] = None
    price: float = Field(..., gt=0)
    tax: Optional[float] = None
    tags: list[str] = []
    created_at: Optional[datetime] = None

class User(BaseModel):
    username: str
    email: EmailStr
    full_name: Optional[str] = None
    is_active: bool = True

@app.post("/items/")
async def create_item(item: Item):
    item_dict = item.dict()
    if item.tax:
        item_dict["price_with_tax"] = item.price + item.tax
    return item_dict

@app.post("/users/")
async def create_user(user: User):
    return user
```

## 回應模型

```python
from typing import List, Optional
from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class Item(BaseModel):
    name: str
    description: Optional[str] = None
    price: float
    tags: List[str] = []

class ItemResponse(BaseModel):
    name: str
    price: float

class ItemsResponse(BaseModel):
    items: List[ItemResponse]
    total: int

@app.get("/items/", response_model=ItemsResponse)
async def get_items():
    return {
        "items": [
            {"name": "Foo", "price": 50.2}
        ],
        "total": 1
    }
```

## 表單和檔案

```python
from fastapi import File, Form, UploadFile
from typing import Annotated

@app.post("/upload/")
async def upload_file(
    file: UploadFile = File(...),
    token: str = Form(...)
):
    return {
        "filename": file.filename,
        "content_type": file.content_type,
        "token": token
    }

@app.post("/files/")
async def create_files(
    files: Annotated[list[UploadFile], File(description="多個檔案")]
):
    return {"filenames": [f.filename for f in files]}
```

## HTTP 狀態碼

```python
from fastapi import FastAPI, status
from fastapi.responses import JSONResponse

app = FastAPI()

@app.get("/items/{item_id}", status_code=status.HTTP_200_OK)
async def read_item(item_id: int):
    return {"item_id": item_id}

@app.post("/items/", status_code=status.HTTP_201_CREATED)
async def create_item(item: dict):
    return item

@app.delete("/items/{item_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_item(item_id: int):
    pass

@app.get("/items/{item_id}", responses={
    200: {"model": Item, "description": "成功"},
    404: {"description": "找不到"}
})
async def read_item(item_id: int):
    return {"item_id": item_id}
```

## 錯誤處理

```python
from fastapi import HTTPException, status

@app.get("/items/{item_id}")
async def read_item(item_id: int):
    if item_id not in db:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="找不到項目"
        )
    return db[item_id]

# 自訂例外處理器
from fastapi import Request
from fastapi.responses import JSONResponse

@app.exception_handler(ValueError)
async def value_error_handler(request: Request, exc: ValueError):
    return JSONResponse(
        status_code=400,
        content={"detail": str(exc)}
    )
```

## 依賴注入 (Dependency Injection)

```python
from fastapi import Depends, Header, HTTPException

async def verify_token(x_token: str = Header(...)):
    if x_token != "secret":
        raise HTTPException(status_code=401, detail="無效的 Token")
    return x_token

@app.get("/items/", dependencies=[Depends(verify_token)])
async def read_items():
    return [{"item": "Foo"}, {"item": "Bar"}]

# 可複用的依賴
from functools import lru_cache

@lru_cache()
def get_settings():
    return Settings()

@app.get("/info")
async def get_info(settings: Settings = Depends(get_settings)):
    return settings
```

## 安全性

```python
from fastapi import Depends
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

@app.post("/token")
async def login(form_data: OAuth2PasswordRequestForm = Depends()):
    return {"access_token": "fake_token", "token_type": "bearer"}

@app.get("/me")
async def read_users_me(token: str = Depends(oauth2_scheme)):
    return {"token": token}
```

## 資料庫整合

```python
from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session

SQLALCHEMY_DATABASE_URL = "sqlite:///./test.db"

engine = create_engine(SQLALCHEMY_DATABASE_URL)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

class User(Base):
    __tablename__ = "users"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, index=True)
    email = Column(String, unique=True, index=True)

Base.metadata.create_all(bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.post("/users/", response_model=UserSchema)
def create_user(user: UserCreate, db: Session = Depends(get_db)):
    db_user = User(**user.dict())
    db.add(db_user)
    db.commit()
    db.refresh(db_user)
    return db_user
```

## CORS

```python
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
```

## 中介軟體

```python
from fastapi import FastAPI, Request
from time import time

app = FastAPI()

@app.middleware("http")
async def add_process_time_header(request: Request, call_next):
    start_time = time()
    response = await call_next(request)
    process_time = time() - start_time
    response.headers["X-Process-Time"] = str(process_time)
    return response
```

## WebSocket

```python
from fastapi import WebSocket, WebSocketDisconnect

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            data = await websocket.receive_text()
            await websocket.send_text(f"收到: {data}")
    except WebSocketDisconnect:
        print("Client disconnected")
```

## 測試

```python
from fastapi.testclient import TestClient

client = TestClient(app)

def test_read_main():
    response = client.get("/")
    assert response.status_code == 200
    assert response.json() == {"msg": "Hello World"}

def test_create_item():
    response = client.post("/items/", json={
        "name": "Test",
        "price": 45.0
    })
    assert response.status_code == 200
    assert "item_id" in response.json()
```

## 相關資源

- 相關概念：[API](API.md)
- 相關主題：[RESTful](RESTful.md)
- 相關主題：[WebSocket](WebSocket.md)

## Tags

#FastAPI #Python #Web框架 #API
