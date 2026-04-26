/**
 * 導入 SQLite3 模組
 * .verbose() 會啟用更詳細的錯誤訊息與堆疊追蹤，這在開發環境中對於除錯（Debug）非常有幫助。
 */
const sqlite3 = require('sqlite3').verbose();

/**
 * 【關鍵：資料庫連接與創建時機】
 * * 當執行 new sqlite3.Database() 時：
 * 1. 檢查檔案：SQLite 會檢查當前目錄下是否存在 './blog.db' 這個檔案。
 * 2. 自動創建：如果檔案「不存在」，SQLite 會立刻在硬碟上創建一個新的、空的 'blog.db'。
 * 3. 開啟連接：如果檔案「已存在」，則直接開啟連接。
 */
const db = new sqlite3.Database('./blog.db');

/**
 * db.serialize() 確保內部的資料庫操作會「依序執行」
 * 因為 Node.js 是非同步的，serialize 能保證在執行後續查詢前，表結構已經初始化完成。
 */
db.serialize(() => {
  /**
   * 【關鍵：資料表初始化】
   * * CREATE TABLE IF NOT EXISTS posts:
   * - 這是最保險的寫法。
   * - 執行時機：每次應用程式啟動並載入此模組時都會執行。
   * - 行為：如果資料庫裡還沒有名為 'posts' 的表，它就會根據定義建立新表。
   * - 意義：確保了即使是第一天執行程式（或是資料庫檔案不小心被刪除），系統也能自動「再生」出儲存空間。
   */
  db.run(`
    CREATE TABLE IF NOT EXISTS posts (
      id INTEGER PRIMARY KEY AUTOINCREMENT, -- 自動遞增的主鍵，每篇文章唯一的身份證字號
      title TEXT NOT NULL,                   -- 標題，不允許空白
      content TEXT NOT NULL,                 -- 內容，儲存 Markdown 文本
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP -- 自動記錄時間，若沒給值則設為「現在時間」
    )
  `);
});

/**
 * 將 db 物件導出，讓 app.js 可以透過 require('./database') 取得同一個資料庫實例。
 */
module.exports = db;