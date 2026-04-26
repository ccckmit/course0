# Web Server

用 C 語言寫的簡易 HTTP 靜態檔案伺服器，可回傳 `public/` 資料夾下的檔案給瀏覽器。

## 編譯

```bash
gcc -Wall -o webserver webserver.c
```

## 執行

```bash
# 預設 port 8080
./webserver

# 指定 port
./webserver 3000
```

啟動後在瀏覽器打開 http://localhost:8080 即可瀏覽。

## 目錄結構

```
webserver/
├── webserver.c      # 伺服器原始碼
├── public/          # 靜態檔案目錄（網頁放這裡）
│   └── index.html   # 預設首頁
└── README.md
```

## 功能

- 提供 `./public` 目錄下的靜態檔案（HTML、CSS、JS、圖片等）
- `/` 自動導向 `index.html`
- 支援常見 MIME types
- 阻擋 `..` 路徑穿越攻擊
- 終端機即時顯示請求 log
