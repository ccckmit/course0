# Blog 系統

## 技術棧

- **後端**: Node.js + Express
- **資料庫**: SQLite (in-memory)
- **前端**: 原生 HTML + CSS + JavaScript

## 專案結構

```
Blog/
├── package.json      # npm 設定
├── server.js         # Express 後端伺服器
├── test.js          # 自動化測試
└── public/
    ├── index.html   # 首頁
    ├── styles.css  # 樣式
    └── app.js      # 前端邏輯
```

## API

| 方法 | 路徑 | 說明 |
|------|------|------|
| GET | /api/posts | 取得所有文章 |
| GET | /api/posts/:id | 取得單篇文章 |
| POST | /api/posts | 新增文章 |
| PUT | /api/posts/:id | 更新文章 |
| DELETE | /api/posts/:id | 刪除文章 |

## 測試結果

```
=== Blog API Tests ===

--- GET /api/posts (空列表) ---
✓ Status 200
✓ 回傳陣列
✓ 初始長度為 0

--- POST /api/posts (新增文章) ---
✓ Status 201
✓ 文章 ID 為 1
✓ 標題正確

--- GET /api/posts (有資料) ---
✓ Status 200
✓ 長度為 1
✓ 標題正確

--- GET /api/posts/:id ---
✓ Status 200
✓ 標題正確
✓ 作者正確

--- GET /api/posts/999 (不存在) ---
✓ Status 404

--- POST /api/posts (缺少欄位) ---
✓ Status 400

--- PUT /api/posts/:id (更新) ---
✓ Status 200
✓ 更新成功

--- GET /api/posts/:id (驗證更新) ---
✓ 標題已更新
✓ 內容已更新

--- DELETE /api/posts/:id ---
✓ Status 200
✓ 刪除成功

--- GET /api/posts (驗證刪除) ---
✓ 列表为空

--- DELETE /api/posts/999 (不存在) ---
✓ Status 404

=== Multiple Posts Test ===
✓ 三篇文章
✓ 第一篇是最新
✓ 最後是最舊

=== Results ===
Passed: 25
Failed: 0
Total: 25
```

## 執行

```bash
cd Blog
npm install
node server.js        # 啟動伺服器 (port 3000)
node test.js          # 執行測試
```
