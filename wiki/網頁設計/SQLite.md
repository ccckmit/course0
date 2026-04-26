# SQLite

## 概述

SQLite 是一個輕量級的嵌入式關聯式資料庫引擎，檔案型儲存、無需伺服器程序，廣泛用於行動應用、嵌入式系統和小型網站。

## 特性

| 特性 | 說明 |
|------|------|
| 零配置 | 無需安裝或管理 |
| 無伺服器 | 嵌入式資料庫 |
| 單一檔案 | 整個資料庫一個檔案 |
| 跨平臺 | Windows, macOS, Linux |
| 交易支援 | ACID 相容 |
| 類型靈活 | 動態類型系統 |

## 基本操作

### 建立資料庫

```bash
# 命令列建立
sqlite3 myapp.db

# 或在 SQLite 命令列
sqlite> .open myapp.db
```

### 建立表格

```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE posts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    content TEXT,
    published BOOLEAN DEFAULT FALSE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

CREATE INDEX idx_posts_user_id ON posts(user_id);
CREATE INDEX idx_posts_published ON posts(published);
```

## CRUD 操作

### 插入 (INSERT)

```sql
-- 單筆插入
INSERT INTO users (username, email, password_hash)
VALUES ('alice', 'alice@example.com', 'hashed_password');

-- 多筆插入
INSERT INTO users (username, email, password_hash)
VALUES 
    ('bob', 'bob@example.com', 'hash1'),
    ('carol', 'carol@example.com', 'hash2');

-- 或使用 REPLACE（遇唯一鍵衝突時替換）
REPLACE INTO users (id, username, email, password_hash)
VALUES (1, 'alice_new', 'alice@example.com', 'new_hash');
```

### 查詢 (SELECT)

```sql
-- 基本查詢
SELECT * FROM users;

-- 條件查詢
SELECT username, email FROM users WHERE id = 1;

-- 排序和限制
SELECT * FROM posts 
WHERE published = TRUE 
ORDER BY created_at DESC 
LIMIT 10 OFFSET 20;

-- 聚合函數
SELECT 
    COUNT(*) as total_users,
    COUNT(DISTINCT email) as unique_emails
FROM users;

-- 分組
SELECT 
    user_id,
    COUNT(*) as post_count
FROM posts
GROUP BY user_id
HAVING COUNT(*) > 5;
```

### 更新 (UPDATE)

```sql
-- 基本更新
UPDATE users 
SET email = 'new_email@example.com' 
WHERE id = 1;

-- 多欄位更新
UPDATE users 
SET 
    email = 'new@example.com',
    updated_at = CURRENT_TIMESTAMP
WHERE username = 'alice';
```

### 刪除 (DELETE)

```sql
-- 基本刪除
DELETE FROM posts WHERE id = 1;

-- 條件刪除
DELETE FROM posts WHERE published = FALSE AND created_at < '2024-01-01';

-- 刪除所有（保留表格結構）
DELETE FROM posts;
```

## 進階查詢

### JOIN

```sql
-- INNER JOIN
SELECT 
    users.username,
    posts.title,
    posts.created_at
FROM users
INNER JOIN posts ON users.id = posts.user_id
WHERE posts.published = TRUE;

-- LEFT JOIN
SELECT 
    users.username,
    COUNT(posts.id) as post_count
FROM users
LEFT JOIN posts ON users.id = posts.user_id
GROUP BY users.id;

-- 多表 JOIN
SELECT 
    u.username,
    p.title,
    c.content as comment
FROM users u
JOIN posts p ON u.id = p.user_id
JOIN comments c ON p.id = c.post_id
WHERE p.published = TRUE;
```

### 子查詢

```sql
-- 在 WHERE 中使用
SELECT * FROM posts
WHERE user_id IN (
    SELECT id FROM users WHERE username LIKE 'a%'
);

-- 在 FROM 中使用
SELECT AVG(post_count) FROM (
    SELECT user_id, COUNT(*) as post_count
    FROM posts
    GROUP BY user_id
);
```

### 常見表達式

```sql
-- CASE 表達式
SELECT 
    username,
    CASE 
        WHEN created_at < '2024-01-01' THEN 'Legacy'
        WHEN created_at < '2025-01-01' THEN 'Active'
        ELSE 'New'
    END as user_type
FROM users;

-- COALESCE（空值處理）
SELECT 
    username,
    COALESCE(email, 'no email') as email
FROM users;

-- NULLIF
SELECT NULLIF(1, 1);  -- 結果: NULL
SELECT NULLIF(1, 2);  -- 結果: 1
```

## 索引

```sql
-- 單欄位索引
CREATE INDEX idx_users_email ON users(email);

-- 多欄位索引
CREATE INDEX idx_posts_user_published ON posts(user_id, published);

-- 唯一索引
CREATE UNIQUE INDEX idx_users_username ON users(username);

-- 移除索引
DROP INDEX idx_posts_user_published;
```

## 觸發器 (Triggers)

```sql
-- 更新時間戳觸發器
CREATE TRIGGER update_users_timestamp
AFTER UPDATE ON users
BEGIN
    UPDATE users 
    SET updated_at = CURRENT_TIMESTAMP 
    WHERE id = NEW.id;
END;

-- 自動刪除相關文章
CREATE TRIGGER delete_user_posts
AFTER DELETE ON users
BEGIN
    DELETE FROM posts WHERE user_id = OLD.id;
END;
```

## 交易 (Transactions)

```sql
BEGIN TRANSACTION;

UPDATE accounts SET balance = balance - 1000 WHERE id = 1;
UPDATE accounts SET balance = balance + 1000 WHERE id = 2;

-- 檢查並決定提交或回滾
-- COMMIT;
-- ROLLBACK;
```

## Python 整合

### sqlite3 標準庫

```python
import sqlite3
from contextlib import contextmanager

@contextmanager
def get_db(db_path='app.db'):
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    try:
        yield conn
        conn.commit()
    except Exception:
        conn.rollback()
        raise
    finally:
        conn.close()

# 基本操作
with get_db() as conn:
    # 查詢
    cursor = conn.execute(
        'SELECT * FROM users WHERE id = ?',
        (1,)
    )
    user = cursor.fetchone()
    
    # 插入
    conn.execute(
        'INSERT INTO users (username, email, password_hash) VALUES (?, ?, ?)',
        ('alice', 'alice@example.com', 'hash')
    )
    
    # 多筆插入
    users = [
        ('bob', 'bob@example.com', 'hash1'),
        ('carol', 'carol@example.com', 'hash2')
    ]
    conn.executemany(
        'INSERT INTO users (username, email, password_hash) VALUES (?, ?, ?)',
        users
    )
```

### ORM (SQLAlchemy)

```python
from sqlalchemy import create_engine, Column, Integer, String, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from datetime import datetime

engine = create_engine('sqlite:///app.db', echo=True)
Base = declarative_base()
Session = sessionmaker(bind=engine)

class User(Base):
    __tablename__ = 'users'
    
    id = Column(Integer, primary_key=True)
    username = Column(String, unique=True, nullable=False)
    email = Column(String, unique=True, nullable=False)
    created_at = Column(DateTime, default=datetime.utcnow)

# 建立表格
Base.metadata.create_all(engine)

# 操作
session = Session()
user = User(username='alice', email='alice@example.com')
session.add(user)
session.commit()
```

### asyncpg (非同步)

```python
import aiosqlite

async def main():
    async with aiosqlite.connect('app.db') as db:
        # 查詢
        async with db.execute(
            'SELECT * FROM users WHERE id = ?',
            (1,)
        ) as cursor:
            rows = await cursor.fetchall()
        
        # 插入
        await db.execute(
            'INSERT INTO users (username, email) VALUES (?, ?)',
            ('alice', 'alice@example.com')
        )
        await db.commit()
```

## Node.js 整合

### better-sqlite3

```javascript
import Database from 'better-sqlite3'

const db = new Database('app.db')

// 基本操作
const stmt = db.prepare('SELECT * FROM users WHERE id = ?')
const user = stmt.get(1)

// 參數化查詢（防止 SQL 注入）
const insert = db.prepare(
    'INSERT INTO users (username, email) VALUES (?, ?)'
)
const result = insert.run('alice', 'alice@example.com')

// 批次操作
const insertMany = db.transaction((users) => {
    for (const user of users) {
        db.prepare(
            'INSERT INTO users (username, email) VALUES (?, ?)'
        ).run(user.username, user.email)
    }
})
insertMany([
    { username: 'bob', email: 'bob@example.com' },
    { username: 'carol', email: 'carol@example.com' }
])
```

## 備份和恢復

```bash
# 備份
sqlite3 app.db ".backup backup.db"

# 匯出 SQL
sqlite3 app.db ".dump" > backup.sql

# 恢復
sqlite3 new.db < backup.sql
```

## 效能優化

| 優化 | 說明 |
|------|------|
| 索引 | 為常用查詢欄位建立索引 |
| ANALYZE | 更新統計資訊 |
| PRAGMA | 調整設定 |
| WAL 模式 | 改善並發讀取 |

```sql
-- 開啟 WAL 模式（較好的並發效能）
PRAGMA journal_mode=WAL;

-- 同步模式（平衡效能和安全性）
PRAGMA synchronous=NORMAL;

-- 快取大小（MB）
PRAGMA cache_size=-2000;

-- 更新統計
ANALYZE;
```

## 限制

| 限制 | 值 |
|------|-----|
| 最大資料庫大小 | 281 TB |
| 最大表格大小 | 281 TB |
| 最大資料列 | 無限制 |
| 最大欄位數 | 32767 |

## 相關資源

- 相關主題：[資料庫](資料庫.md)
- 相關工具：[Node.js](Node.js.md)

## Tags

#SQLite #資料庫 #SQL #嵌入式 #輕量級
