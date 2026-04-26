# CSS

CSS（Cascading Style Sheets，層疊樣式表）是一種用於描述 HTML 或 XML 文件外觀的樣式表語言。CSS 控制網頁的版面配置、顏色、字體、間距、動畫等視覺效果，是網頁開發不可或缺的核心技術之一。

## 歷史背景

CSS 最早於 1996 年由 W3C（World Wide Web Consortium）發布 CSS1 規範，此後經歷了 CSS2、CSS3 的重大演進。CSS3 引入了模組化設計，將不同功能分為獨立的模組（如選擇器、盒模型、動畫、變形等），使瀏覽器廠商能夠逐步實現新功能。

## 基本語法

CSS 語法由選擇器（Selector）和宣告區塊（Declaration Block）組成。選擇器用於指定要樣式化的 HTML 元素，宣告區塊則包含多個屬性與值的配對。

### 選擇器類型

```css
/* 元素選擇器 */
h1 {
    color: blue;
    font-size: 24px;
}

/* 類別選擇器 */
.highlight {
    background-color: yellow;
}

/* ID 選擇器 */
#header {
    position: fixed;
}

/* 屬性選擇器 */
input[type="text"] {
    border: 1px solid gray;
}

/* 偽類選擇器 */
a:hover {
    color: red;
}

/* 偽元素選擇器 */
p::first-line {
    font-weight: bold;
}
```

### 層疊與優先權

當多個樣式規則應用於同一元素時，CSS 使用層疊（Cascading）機制決定最終樣式。優先權由高到低為：

1. 行內樣式（Inline Style）
2. ID 選擇器
3. 類別選擇器、偽類、屬性選擇器
4. 元素選擇器、偽元素

```css
/* 優先權範例 */
#title { color: red; }      /* 優先權 1-0-1-0 */
.title { color: blue; }     /* 優先權 0-1-1-0 */
h1 { color: green; }       /* 優先權 0-0-1-0 */
```

## 盒模型

盒模型（Box Model）是 CSS 的核心概念，每個 HTML 元素都被視為一個矩形盒子，由以下組成：

- **Content（內容）**：元素的實際內容，如文字、圖片
- **Padding（內邊距）**：內容與邊框之間的空間
- **Border（邊框）**：圍繞內容和內邊距的邊線
- **Margin（外邊距）**：元素與其他元素之間的空間

```css
.box {
    width: 200px;
    padding: 20px;
    border: 5px solid black;
    margin: 10px;
    box-sizing: border-box;
}
```

`box-sizing` 屬性決定 width 和 height 如何計算：
- `content-box`（預設）：width 只包含內容
- `border-box`：width 包含內容、內邊距和邊框

## 版面配置

### Flexbox

Flexbox（彈性盒模型）提供一種更有效率的排列對齊方式，適合一維布局（單行或單欄）。

```css
.container {
    display: flex;
    flex-direction: row;        /* row, row-reverse, column, column-reverse */
    justify-content: center;   /* flex-start, flex-end, center, space-between, space-around */
    align-items: center;       /* stretch, flex-start, flex-end, center, baseline */
    flex-wrap: wrap;           /* nowrap, wrap, wrap-reverse */
    gap: 10px;
}

.item {
    flex-grow: 1;              /* 伸展比例 */
    flex-shrink: 1;            /* 收縮比例 */
    flex-basis: 200px;         /* 基礎寬度 */
    flex: 1 1 auto;            /* shorthand: grow shrink basis */
    align-self: center;        /* 單一項目對齊 */
}
```

Flexbox 解決了傳統浮動布局的諸多問題，讓垂直居中、均分空間、自適應布局變得簡單。

### CSS Grid

CSS Grid 是二維布局系統，適合複雜的網格布局。

```css
.grid-container {
    display: grid;
    grid-template-columns: repeat(3, 1fr);  /* 三欄等寬 */
    grid-template-rows: auto 100px auto;   /* 自適應高度 */
    gap: 20px;
    grid-template-areas: 
        "header header header"
        "sidebar main main"
        "footer footer footer";
}

.grid-item {
    grid-column: 1 / 3;        /* 起始線 / 結束線 */
    grid-row: span 2;          /* 佔據兩列 */
    grid-area: header;         /* 命名區域 */
}
```

Grid 與 Flexbox 的選擇：
- Flexbox：單維度布局（水平或垂直）
- Grid：二維度布局（同時需要行列控制）

### 定位系統

```css
.positioned {
    position: relative;        /* 相對定位，相對於正常位置 */
    position: absolute;        /* 絕對定位，相對於最近已定位祖先 */
    position: fixed;           /* 固定定位，相對於視窗 */
    position: sticky;          /* 黏性定位，滾動到阈值後固定 */
    top: 10px;
    left: 20px;
    z-index: 100;              /* 堆疊順序 */
}
```

## 響應式設計

### Media Queries

```css
/* 基本語法 */
@media (max-width: 768px) {
    .container {
        flex-direction: column;
    }
}

/* 多重條件 */
@media (min-width: 600px) and (max-width: 1200px) {
    .sidebar {
        width: 200px;
    }
}

/* 裝置類型 */
@media print {
    .no-print {
        display: none;
    }
}
```

### 響應式圖片

```css
img {
    max-width: 100%;
    height: auto;
}

.picture {
    background-image: url("small.jpg");
}

@media (min-width: 768px) {
    .picture {
        background-image: url("large.jpg");
    }
}
```

## 變數與自訂屬性

CSS 變數（自訂屬性）讓樣式更具維護性和一致性。

```css
:root {
    --primary-color: #007bff;
    --secondary-color: #6c757d;
    --spacing: 16px;
    --font-family: "Helvetica Neue", Arial, sans-serif;
}

.button {
    background-color: var(--primary-color);
    padding: var(--spacing);
    font-family: var(--font-family);
}
```

## 動畫與轉場

### Transitions

```css
.button {
    transition: background-color 0.3s ease, transform 0.2s ease-in-out;
}

.button:hover {
    background-color: #0056b3;
    transform: scale(1.05);
}
```

### Animations

```css
@keyframes fadeIn {
    from { opacity: 0; }
    to { opacity: 1; }
}

@keyframes slideIn {
    0% { transform: translateX(-100%); }
    100% { transform: translateX(0); }
}

.animated {
    animation: fadeIn 1s ease-in-out;
    animation-fill-mode: forwards;
    animation-iteration-count: infinite;
    animation-direction: alternate;
}
```

## 轉換 Transform

```css
.transformed {
    transform: rotate(45deg);              /* 旋轉 */
    transform: scale(1.5);                 /* 縮放 */
    transform: translate(20px, 10px);     /* 移動 */
    transform: skew(20deg, 10deg);         /* 傾斜 */
    transform: matrix(1, 0, 0, 1, 0, 0); /* 矩陣變換 */
    transform-origin: center center;       /* 變換原點 */
}
```

## 陰影與效果

```css
.box {
    /* 盒陰影 */
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1), 0 1px 3px rgba(0, 0, 0, 0.08);
    /* 文字陰影 */
    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);
    /* 模糊半徑 */
    filter: blur(5px);
    filter: brightness(1.2);
    filter: contrast(150%);
    filter: grayscale(100%);
}
```

## 字體與文字样式

```css
.text {
    font-family: "Noto Sans TC", "Helvetica Neue", Arial, sans-serif;
    font-size: 16px;
    font-weight: 700;
    line-height: 1.6;
    text-align: justify;
    text-indent: 2em;
    letter-spacing: 2px;
    word-spacing: 4px;
    text-transform: uppercase;
    text-decoration: underline;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}
```

## CSS 框架 - Bootstrap

Bootstrap 是最流行的 CSS 框架之一，由 Twitter 開發並於 2011 年開源。它提供了豐富的預設樣式、元件和響應式工具，讓開發者能夠快速構建美觀的網頁。

### 主要特點

1. **響應式網格系統**：12 欄布局，支援手機、平板、桌面
2. **預設元件**：導航欄、按鈕、表格、表單、卡片、Modal 等
3. **實用工具類**：Spacing、Typography、Colors、Display 等
4. **JavaScript 元件**：輪播、下拉選單、標籤頁、工具提示等

### 基本使用

```html
<!DOCTYPE html>
<html lang="zh-TW">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bootstrap 示例</title>
    <!-- 引入 Bootstrap CSS -->
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body>
    <!-- 導航欄 -->
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
        <div class="container">
            <a class="navbar-brand" href="#">我的網站</a>
            <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav">
                <span class="navbar-toggler-icon"></span>
            </button>
            <div class="collapse navbar-collapse" id="navbarNav">
                <ul class="navbar-nav ms-auto">
                    <li class="nav-item"><a class="nav-link active" href="#">首頁</a></li>
                    <li class="nav-item"><a class="nav-link" href="#">關於</a></li>
                    <li class="nav-item dropdown">
                        <a class="nav-link dropdown-toggle" href="#" data-bs-toggle="dropdown">服務</a>
                        <ul class="dropdown-menu">
                            <li><a class="dropdown-item" href="#">網頁設計</a></li>
                            <li><a class="dropdown-item" href="#">開發</a></li>
                        </ul>
                    </li>
                </ul>
            </div>
        </div>
    </nav>

    <!-- 內容區域 -->
    <main class="container my-5">
        <div class="row">
            <div class="col-md-8">
                <h1>歡迎光臨</h1>
                <p class="lead">這是一個使用 Bootstrap 构建的響應式網頁。</p>
                <button class="btn btn-primary">主要按鈕</button>
                <button class="btn btn-secondary">次要按鈕</button>
                <button class="btn btn-outline-success">邊框按鈕</button>
            </div>
            <div class="col-md-4">
                <div class="card">
                    <div class="card-header">側邊欄</div>
                    <ul class="list-group list-group-flush">
                        <li class="list-group-item">項目一</li>
                        <li class="list-group-item">項目二</li>
                        <li class="list-group-item">項目三</li>
                    </ul>
                </div>
            </div>
        </div>
    </main>

    <!-- 引入 Bootstrap JS -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
</body>
</html>
```

### 網格系統

```html
<div class="container">
    <div class="row">
        <div class="col-12 col-sm-6 col-md-4 col-lg-3">
            <!-- 響應式：手機全寬、平板2欄、桌面3欄、大型螢幕4欄 -->
        </div>
    </div>
</div>
```

### 常用元件

```html
<!-- 按鈕 -->
<button class="btn btn-primary btn-lg">大型主要按鈕</button>
<button class="btn btn-outline-secondary btn-sm">小型邊框次要按鈕</button>

<!-- 卡片 -->
<div class="card">
    <img src="image.jpg" class="card-img-top" alt="...">
    <div class="card-body">
        <h5 class="card-title">標題</h5>
        <p class="card-text">內容文字</p>
        <a href="#" class="btn btn-primary">詳細資訊</a>
    </div>
</div>

<!-- 表單 -->
<form>
    <div class="mb-3">
        <label for="email" class="form-label">電子郵件</label>
        <input type="email" class="form-control" id="email" placeholder="name@example.com">
    </div>
    <div class="mb-3">
        <label for="password" class="form-label">密碼</label>
        <input type="password" class="form-control" id="password">
    </div>
    <div class="mb-3 form-check">
        <input type="checkbox" class="form-check-input" id="remember">
        <label class="form-check-label" for="remember">記住我</label>
    </div>
    <button type="submit" class="btn btn-primary">登入</button>
</form>

<!-- 導航標籤 -->
<ul class="nav nav-tabs">
    <li class="nav-item"><a class="nav-link active" href="#">首頁</a></li>
    <li class="nav-item"><a class="nav-link" href="#">個人資料</a></li>
    <li class="nav-item"><a class="nav-link" href="#">設定</a></li>
</ul>

<!-- 警示框 -->
<div class="alert alert-success" role="alert">操作成功！</div>
<div class="alert alert-warning" role="alert">請注意：此操作不可撤銷。</div>
<div class="alert alert-danger" role="alert">錯誤：請檢查輸入內容。</div>
```

### 自訂 Bootstrap

可以通過 Sass 變數自訂 Bootstrap：

```scss
// 自訂顏色
$primary: #007bff;
$secondary: #6c757d;
$success: #28a745;
$info: #17a2b8;
$warning: #ffc107;
$danger: #dc3545;
$light: #f8f9fa;
$dark: #343a40;

// 自訂字體
$font-family-sans-serif: "Noto Sans TC", system-ui, -apple-system, sans-serif;
$headings-font-family: "Noto Sans TC", sans-serif;

// 自訂主題
$border-radius: 0.5rem;
$border-radius-lg: 0.75rem;
$border-radius-sm: 0.25rem;

// 引入 Bootstrap
@import "bootstrap/scss/bootstrap";
```

## 其他 CSS 框架

### Tailwind CSS

Utility-First 框架，透過組合小型工具類建構樣式：

```html
<div class="flex items-center justify-between p-4 bg-white shadow-md rounded-lg">
    <h1 class="text-2xl font-bold text-gray-800">標題</h1>
    <button class="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600">
        按鈕
    </button>
</div>
```

### Foundation

另一個成熟的回應式 CSS 框架，支援 Sass。

### Bulma

純 CSS 框架，無 JavaScript 依賴，語法簡潔。

## 現代 CSS 功能

### CSS Variables

```css
:root {
    --theme-color: #3498db;
    --spacing: 1rem;
}

@media (prefers-color-scheme: dark) {
    :root {
        --theme-color: #2ecc71;
    }
}
```

### Container Queries

```css
.card-container {
    container-type: inline-size;
}

@container (min-width: 400px) {
    .card {
        display: grid;
        grid-template-columns: 1fr 2fr;
    }
}
```

### Subgrid

```css
.parent {
    display: grid;
    grid-template-columns: 1fr 1fr;
}

.child {
    display: grid;
    grid-column: span 2;
    grid-template-columns: subgrid;
}
```

## CSS 預處理器

### Sass/SCSS

```scss
$primary-color: blue;

@mixin flex-center {
    display: flex;
    justify-content: center;
    align-items: center;
}

.container {
    $padding: 20px;  // 區域變數
    padding: $padding;
    
    @include flex-center;
    
    &:hover {
        background-color: darken($primary-color, 10%);
    }
    
    .item {
        color: $primary-color;
    }
}
```

### Less

```less
@primary: blue;

.container {
    padding: 20px;
    
    &:hover {
        background-color: @primary;
    }
}
```

## 開發工具

### Chrome DevTools

Chrome 瀏覽器內建的開發者工具讓您可以：
- 檢視和編輯 CSS 樣式
- 調試 Flexbox 和 Grid 布局
- 查看計算後的樣式（Computed）
- 測試響應式視口

### CSS Validator

W3C CSS Validator（https://jigsaw.w3.org/css-validator/）可用於驗證 CSS 語法和標準規範。

## 最佳實踐

1. **使用 CSS 變數**：提高可維護性和一致性
2. **行動優先**：先為手機設計，再為桌面擴展
3. **使用 Semantic HTML**：配合适當的 CSS 選擇器
4. **壓縮 CSS**：生產環境使用 minified 版本
5. **使用 Short-hand Properties**：如 `margin: 10px 20px;`
6. **避免 !important**：優先使用具體的選擇器
7. **使用 BEM 命名**：如 `block__element--modifier`

## 相關概念

- [HTML](HTML.md) - 網頁結構標記語言
- [JavaScript](JavaScript.md) - 網頁互動腳本語言
- [響應式設計](響應式設計.md) - 適應不同裝置的設計方法
- [前端框架](前端框架.md) - React、Vue、Angular 等框架