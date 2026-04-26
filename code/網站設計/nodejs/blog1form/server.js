/**
 * 導入必要模組
 */
const express = require('express'); // Web 框架，用於處理 HTTP 請求
const db = require('./database');   // 自定義的資料庫模組（推測為 SQLite 配置）
const marked = require('marked');   // 將 Markdown 語法轉換為 HTML 的工具

const app = express();

/**
 * 中間件 (Middleware) 設定
 */
app.use(express.json()); // 解析內容類型為 application/json 的請求
app.use(express.urlencoded({ extended: true })); // 解析來自 HTML 表單的 URL 編碼數據

/**
 * 資料庫操作函數 - 獲取所有文章列表
 * @param {Function} callback - 處理查詢結果的回呼函式
 */
function getPosts(callback) {
  // 查詢文章 ID、標題、前 200 字內容（摘要）以及建立時間，並按時間倒序排列
  db.all('SELECT id, title, substr(content, 1, 200) as excerpt, created_at FROM posts ORDER BY created_at DESC', [], (err, rows) => {
    callback(err, rows);
  });
}

/**
 * 資料庫操作函數 - 獲取單篇文章詳細內容
 * @param {number|string} id - 文章 ID
 * @param {Function} callback - 處理查詢結果的回呼函式
 */
function getPost(id, callback) {
  // 使用參數化查詢（?）防止 SQL 注入
  db.get('SELECT * FROM posts WHERE id = ?', [id], (err, row) => {
    callback(err, row);
  });
}

/**
 * 資料庫操作函數 - 新增文章
 * @param {string} title - 文章標題
 * @param {string} content - 文章內容 (Markdown 格式)
 * @param {Function} callback - 處理結果的回呼函式
 */
function addPost(title, content, callback) {
  db.run('INSERT INTO posts (title, content) VALUES (?, ?)', [title, content], function(err) {
    // this.lastID 是 SQLite 回傳最後插入資料的自動遞增 ID
    callback(err, this.lastID);
  });
}

/**
 * [GET] 首頁路由 - 顯示文章清單與發文表單
 */
app.get('/', (req, res) => {
  getPosts((err, posts) => {
    if (err) return res.status(500).send('Error loading posts');
    
    // 構建 HTML 頁面（含 CSS 樣式）
    let html = `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>My Blog</title>
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }
    h1 { border-bottom: 2px solid #333; padding-bottom: 10px; }
    .post { margin-bottom: 30px; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
    .post h2 { margin-top: 0; }
    .post a { text-decoration: none; color: #333; }
    .post a:hover { color: #0066cc; }
    .meta { color: #666; font-size: 0.9em; }
    .new-post { background: #f5f5f5; padding: 20px; border-radius: 5px; margin-bottom: 30px; }
    input, textarea { width: 100%; padding: 10px; margin: 5px 0; box-sizing: border-box; }
    button { background: #333; color: white; padding: 10px 20px; border: none; cursor: pointer; }
    button:hover { background: #555; }
  </style>
</head>
<body>
  <h1>My Blog</h1>
  <div class="new-post">
    <h3>New Post</h3>
    <form method="post" action="/posts">
      <input type="text" name="title" placeholder="Title" required>
      <textarea name="content" rows="5" placeholder="Content (Markdown supported)" required></textarea>
      <button type="submit">Publish</button>
    </form>
  </div>`;

    // 遍歷文章陣列，動態生成 HTML 內容
    posts.forEach(post => {
      html += `<div class="post">
      <h2><a href="/post/${post.id}">${post.title}</a></h2>
      <p class="meta">${new Date(post.created_at).toLocaleString()}</p>
      <p>${post.excerpt}...</p>
      <a href="/post/${post.id}">Read more →</a>
    </div>`;
    });

    html += `</body></html>`;
    res.send(html);
  });
});

/**
 * [GET] 文章詳情頁路由 - 顯示單篇文章內容
 */
app.get('/post/:id', (req, res) => {
  // 從 URL 參數獲取 id
  getPost(req.params.id, (err, post) => {
    if (err || !post) return res.status(404).send('Post not found');
    
    const html = `<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>${post.title}</title>
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }
    a { color: #333; }
    .meta { color: #666; }
    .content { line-height: 1.8; }
    .content img { max-width: 100%; }
  </style>
</head>
<body>
  <p><a href="/">← Back</a></p>
  <h1>${post.title}</h1>
  <p class="meta">${new Date(post.created_at).toLocaleString()}</p>
  <div class="content">${marked.parse(post.content)}</div>
</body></html>`;
    res.send(html);
  });
});

/**
 * [POST] 處理文章發佈
 */
app.post('/posts', (req, res) => {
  const { title, content } = req.body;
  
  // 基礎驗證：確保標題與內容不為空
  if (!title || !content) return res.status(400).send('Title and content required');
  
  addPost(title, content, (err, id) => {
    if (err) return res.status(500).send('Error saving post');
    // 發佈成功後導回首頁
    res.redirect('/');
  });
});

/**
 * 啟動伺服器，監聽 3000 埠
 * 只有直接執行 server.js 時才會啟動伺服器
 * 匯入測試時不會啟動
 */
if (require.main === module) {
  app.listen(3000, () => {
    console.log('Blog running at http://localhost:3000');
  });
}

/**
 * 匯出 app 供測試使用
 */
module.exports = app;