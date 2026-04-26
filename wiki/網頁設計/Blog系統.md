# Blog 系統

完整的部落格系統，包含前後端程式碼。

## 技術棧

- **後端**: Node.js + Express
- **資料庫**: SQLite (in-memory)
- **前端**: 原生 HTML + CSS + JavaScript

## 功能

- 新增文章 (標題、作者、內容)
- 編輯文章
- 刪除文章
- 列表展示 (依時間排序)
- 表單驗證

## 程式碼

- [server.js](../_code/網頁設計/Blog/server.js) - 後端 API
- [index.html](../_code/網頁設計/Blog/public/index.html) - 前端頁面
- [styles.css](../_code/網頁設計/Blog/public/styles.css) - 樣式
- [app.js](../_code/網頁設計/Blog/public/app.js) - 前端邏輯
- [test.js](../_code/網頁設計/Blog/test.js) - 自動化測試

## API

| 方法 | 路徑 | 說明 |
|------|------|------|
| GET | /api/posts | 取得所有文章 |
| GET | /api/posts/:id | 取得單篇文章 |
| POST | /api/posts | 新增文章 |
| PUT | /api/posts/:id | 更新文章 |
| DELETE | /api/posts/:id | 刪除文章 |

## 測試結果

所有 25 項測試通過 ✓

## 相關頁面

- [Node.js](Node.js.md) - 執行環境
- [Express](Express.md) - Web 框架
- [SQLite](SQLite.md) - 資料庫
