# md0r - Markdown 瀏覽器

一個輕量級的 Markdown 文件瀏覽器，使用 C 語言和 SDL2 開發。

## 功能特色

- 純 C + SDL2 實現，輕量快速
- 支援本地 Markdown 文件瀏覽
- **支援網址連結（HTTP/HTTPS）**
- 支援相對路徑解析
- 可指定根目錄
- 支援中文顯示（UTF-8）
- 滑鼠滾輪滾動

## 編譯方式

```bash
bash test.sh
```

### 環境需求

- macOS (已測試)
- SDL2
- SDL2_ttf
- OpenSSL

### 安裝依賴（macOS）

```bash
brew install sdl2 sdl2_ttf openssl
```

## 使用方式

```bash
./browser0 <file> [root_dir]
```

### 參數說明

| 參數 | 說明 |
|------|------|
| `file` | Markdown 文件路徑或 URL |
| `root_dir` | 根目錄（可選，預設為目前目錄） |

### 使用範例

**開啟本地文件（指定根目錄）：**
```bash
./browser0 index.md md
```
這會在 `md/` 目錄下尋找 `index.md`

**開啟本地文件（自動偵測）：**
```bash
./browser0 md/index.md
```
自動使用 `md/` 作為根目錄

**直接開啟 URL：**
```bash
./browser0 "https://raw.githubusercontent.com/ccc-c/c0computer/refs/heads/main/README.md"
```

**測試 HTTP/HTTPS 功能：**
```bash
./test_http
```

## 支援的連結格式

### 本地連結
```markdown
[前往第二頁](page2.md)
[關於本工具](about.md)
```

### 網址連結
```markdown
[GitHub](https://github.com/user/repo)
[遠端文檔](https://raw.githubusercontent.com/user/repo/main/doc.md)
```

## 專案架構

```
.
├── browser0.c          # 主程式入口
├── md0render/
│   ├── md0render.h     # 標頭檔
│   └── md0render.c     # 渲染引擎
├── net/
│   ├── http.h          # HTTP API 標頭
│   └── http.c          # HTTP/HTTPS 客戶端實現
├── md/                 # Markdown 文件範例
│   ├── index.md
│   ├── test_url.md     # URL 連結測試
│   └── ...
└── test.sh             # 編譯腳本
```

## 技術實現

### 網路功能（net/http.c）

從 socket 層級實現的 HTTP/HTTPS 客戶端：

- 使用 BSD socket 進行 TCP 連接
- 使用 OpenSSL 實現 HTTPS 加密傳輸
- 支援 HTTP 重新導向（301, 302, 303, 307, 308）
- 自動解析 URL 取得 Host、Port、Path

### 路徑解析邏輯

1. **URL 判斷**：若開頭為 `http://` 或 `https://` 則為網址
2. **絕對路徑**：若開頭為 `/` 則直接使用
3. **相對路徑**：與 `root_dir` 組合後使用

```c
// 路徑組合邏輯
if (is_url(filename)) {
    // 直接使用 URL
} else if (filename[0] == '/') {
    // 絕對路徑
} else {
    // root_dir + "/" + filename
}
```

## 鍵盤/滑鼠操作

| 操作 | 功能 |
|------|------|
| 滑鼠滾輪 | 上下滾動頁面 |
| 點擊連結 | 開啟連結（本地檔案或 URL） |
| 關閉視窗 | 結束程式 |

## License

MIT License
