# HTML (超文本標記語言)

**標籤**: #網頁 #標記語言 #HTML5 #前端
**日期**: 2024-01-15

HTML (HyperText Markup Language，超文本標記語言) 是建立網頁的基礎標記語言，由 Tim Berners-Lee 於 1990 年發明，當初作為 CERN 的資訊共享系統。HTML 是 World Wide Web 的核心技術之一，負責定義網頁的結構和內容。

## HTML 的發展歷史

### HTML 1.0 (1991)
1991 年，Tim Berners-Lee 發布了 HTML 的第一個版本，當時只有約 18 個標籤，功能非常基本。這是 Web 的起點，開啟了資訊共享的新時代。

### HTML 2.0 (1995)
HTML 2.0 成為第一個標準化版本，添加了表單、圖片嵌入等重要功能。這個版本奠定了 HTML 作為 Web 標準的基礎。

### HTML 4.01 (1999)
HTML 4.01 是 HTML4 的最後版本，引入了 CSS 的支援，大幅提升了網頁的呈現能力。同時也加入了表格、框架等複雜元素。

### HTML5 (2014)
HTML5 是 HTML 的重大更新，引入了許多革命性的新特性：
- 語義化標籤 (`<header>`, `<footer>`, `<nav>`, `<article>`, `<section>`)
- 多媒體支援 (`<video>`, `<audio>`)
- 圖形繪製 (`<canvas>`, `<svg>`)
- 本地存儲 (Local Storage, Session Storage)
- Web Workers
- Web Socket

## HTML 文件的基本結構

每個 HTML 文件都遵循特定的結構，這個結構確保瀏覽器能正確解析和呈現網頁內容。

### 文件類型宣告 (DOCTYPE)

```html
<!DOCTYPE html>
```

這個宣告告訴瀏覽器即將使用 HTML5 標準。沒有這個宣告，瀏覽器可能會進入怪異模式 (Quirks Mode)，導致網頁呈現異常。

### 完整的基本結構

```html
<!DOCTYPE html>
<html lang="zh-TW">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="網頁描述">
    <meta name="keywords" content="關鍵字1, 關鍵字2">
    <meta name="author" content="作者名稱">
    <title>網頁標題</title>
    
    <!-- 外部CSS -->
    <link rel="stylesheet" href="styles.css">
    
    <!-- 外部JavaScript -->
    <script src="script.js" defer></script>
</head>
<body>
    <header>
        <nav>
            <ul>
                <li><a href="/">首頁</a></li>
                <li><a href="/about">關於</a></li>
                <li><a href="/contact">聯絡</a></li>
            </ul>
        </nav>
    </header>
    
    <main>
        <article>
            <h1>主要標題</h1>
            <p>這是段落文字，包含 <strong>粗體</strong> 和 <em>斜體</em> 文字。</p>
            
            <section>
                <h2>子標題</h2>
                <p>更多內容...</p>
            </section>
        </article>
        
        <aside>
            <h3>側邊欄</h3>
            <p>附加資訊...</p>
        </aside>
    </main>
    
    <footer>
        <p>&copy; 2024 網站名稱. All rights reserved.</p>
    </footer>
</body>
</html>
```

## HTML 語法規則

### 標籤語法

HTML 標籤由尖括號包圍，分為起始標籤和結束標籤：

```html
<tagname>內容</tagname>
```

有些標籤是自閉合的 (Void Elements)：

```html
<br>
<img src="image.jpg" alt="圖片">
<input type="text">
<meta charset="UTF-8">
```

### 屬性語法

標籤可以包含屬性，提供額外的資訊：

```html
<a href="https://example.com" target="_blank" rel="noopener">連結文字</a>
<img src="photo.jpg" alt="圖片描述" width="800" height="600">
<input type="email" required placeholder="請輸入 Email">
```

### 常見屬性分類

| 屬性類型 | 說明 | 範例 |
|----------|------|------|
| 全域屬性 | 適用於所有標籤 | id, class, style, title |
| 連結屬性 | 用於 `<a>` 標籤 | href, target, rel |
| 圖片屬性 | 用於 `<img>` 標籤 | src, alt, width, height |
| 表單屬性 | 用於 `<form>` 和 `<input>` | action, method, required, placeholder |

## 常見 HTML 標籤詳解

### 文字結構標籤

```html
<!-- 標題標籤 -->
<h1>一級標題</h1>
<h2>二級標題</h2>
<h3>三級標題</h3>
<h4>四級標題</h4>
<h5>五級標題</h5>
<h6>六級標題</h6>

<!-- 文字格式化 -->
<p>這是段落文字。</p>
<strong>重要文字（粗體）</strong>
<em>強調文字（斜體）</em>
<mark>標記文字（螢光筆效果）</mark>
<small>較小文字</small>
<sub>下標文字</sub>
<sup>上標文字</sup>

<!-- 引用 -->
<blockquote cite="來源網址">
    <p>這是引用的文字區塊。</p>
    <cite>— 引言來源</cite>
</blockquote>

<code>程式碼</code>
<pre><code>預先格式化
    的文字區塊</code></pre>
```

### 清單標籤

```html
<!-- 無序清單 -->
<ul>
    <li>項目一</li>
    <li>項目二</li>
    <li>項目三</li>
</ul>

<!-- 有序清單 -->
<ol type="1" start="1">
    <li>第一項</li>
    <li>第二項</li>
    <li>第三項</li>
</ol>

<!-- 定義清單 -->
<dl>
    <dt>術語一</dt>
    <dd>術語一的定義說明</dd>
    <dt>術語二</dt>
    <dd>術語二的定義說明</dd>
</dl>
```

### 表格標籤

```html
<table border="1">
    <thead>
        <tr>
            <th>標題欄位一</th>
            <th>標題欄位二</th>
            <th>標題欄位三</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>資料格一</td>
            <td>資料格二</td>
            <td>資料格三</td>
        </tr>
        <tr>
            <td colspan="2">跨兩欄</td>
            <td>資料格</td>
        </tr>
    </tbody>
    <tfoot>
        <tr>
            <td>表尾一</td>
            <td>表尾二</td>
            <td>表尾三</td>
        </tr>
    </tfoot>
</table>
```

### 表單標籤

```html
<form action="/submit" method="POST">
    <!-- 文字輸入 -->
    <label for="name">姓名：</label>
    <input type="text" id="name" name="name" required>
    
    <!-- Email 輸入 -->
    <label for="email">Email：</label>
    <input type="email" id="email" name="email" placeholder="example@mail.com">
    
    <!-- 密碼輸入 -->
    <label for="password">密碼：</label>
    <input type="password" id="password" name="password">
    
    <!-- 單選 -->
    <fieldset>
        <legend>性別</legend>
        <input type="radio" id="male" name="gender" value="male">
        <label for="male">男</label>
        <input type="radio" id="female" name="gender" value="female">
        <label for="female">女</label>
    </fieldset>
    
    <!-- 核取方塊 -->
    <input type="checkbox" id="agree" name="agree" required>
    <label for="agree">我同意服務條款</label>
    
    <!-- 下拉選單 -->
    <label for="country">國家：</label>
    <select id="country" name="country">
        <option value="tw">台灣</option>
        <option value="us">美國</option>
        <option value="jp">日本</option>
    </select>
    
    <!-- 多行文字 -->
    <label for="message">訊息：</label>
    <textarea id="message" name="message" rows="4" cols="50"></textarea>
    
    <!-- 按鈕 -->
    <button type="submit">提交</button>
    <button type="reset">重設</button>
    <button type="button">一般按鈕</button>
</form>
```

## HTML5 語義化標籤

HTML5 引入的語義化標籤讓開發者能夠更精確地描述網頁的結構，這對 SEO 和程式碼可讀性都有很大幫助。

### 語義化標籤說明

| 標籤 | 說明 | 使用情境 |
|------|------|----------|
| `<header>` | 頁面或區塊的標題區域 | 導航、品牌標誌、搜尋框 |
| `<nav>` | 導航連結的區域 | 主選單、麵包屑導航 |
| `<main>` | 頁面的主要內容 | 文章、部落格文章、產品頁面 |
| `<article>` | 獨立的內容區塊 | 文章、評論、貼文 |
| `<section>` | 文件的區段 | 章節、主題群組 |
| `<aside>` | 與主要內容相關的資訊 | 側邊欄、相關連結 |
| `<footer>` | 頁面或區塊的底部 | 版權資訊、聯絡方式 |
| `<figure>` | 獨立的圖片或圖表區域 | 圖片、圖表、程式碼片段 |
| `<figcaption>` | `<figure>` 的標題 | 圖片說明、圖表標題 |

### 語義化範例

```html
<body>
    <header>
        <h1>我的部落格</h1>
        <nav>
            <ul>
                <li><a href="/">首頁</a></li>
                <li><a href="/articles">文章</a></li>
                <li><a href="/about">關於</a></li>
            </ul>
        </nav>
    </header>
    
    <main>
        <article>
            <header>
                <h2>文章標題</h2>
                <p>發布日期：2024-01-15</p>
            </header>
            
            <p>文章內容...</p>
            
            <section>
                <h3>相關主題</h3>
                <p>更多相關內容...</p>
            </section>
            
            <footer>
                <p>作者：admin@example.com</p>
            </footer>
        </article>
        
        <aside>
            <h3>熱門文章</h3>
            <ul>
                <li><a href="#">文章一</a></li>
                <li><a href="#">文章二</a></li>
            </ul>
        </aside>
    </main>
    
    <footer>
        <p>&copy; 2024 我的部落格. All rights reserved.</p>
    </footer>
</body>
```

## 多媒體元素

### 圖片標籤

```html
<!-- 基本用法 -->
<img src="images/photo.jpg" alt="風景照片">

<!-- 回應式圖片 -->
<img src="photo.jpg" 
     srcset="photo-small.jpg 480w, 
             photo-medium.jpg 800w, 
             photo-large.jpg 1200w"
     sizes="(max-width: 600px) 100vw, 50vw"
     alt="風景照片">

<!-- 圖片搭配 figure -->
<figure>
    <img src="photo.jpg" alt="美麗的日落">
    <figcaption>圖1：夕陽下的海灘</figcaption>
</figure>
```

### 影片和音訊標籤

```html
<!-- 影片 -->
<video controls width="640" height="480" poster="poster.jpg">
    <source src="video.mp4" type="video/mp4">
    <source src="video.webm" type="video/webm">
    <track kind="captions" src="captions.vtt" srclang="zh-TW" label="中文">
    您的瀏覽器不支援影片播放。
</video>

<!-- 音訊 -->
<audio controls>
    <source src="audio.mp3" type="audio/mpeg">
    <source src="audio.ogg" type="audio/ogg">
    您的瀏覽器不支援音訊播放。
</audio>
```

### SVG 向量圖形

```html
<!-- 內聯 SVG -->
<svg width="100" height="100" viewBox="0 0 100 100">
    <circle cx="50" cy="50" r="40" fill="blue" stroke="black" stroke-width="2"/>
    <text x="50" y="55" text-anchor="middle" fill="white">文字</text>
</svg>

<!-- 引用外部 SVG -->
<img src="icon.svg" alt="SVG 圖示">
```

## 內聯元素與區塊元素

### 區塊元素 (Block Elements)

區塊元素會佔據整行的寬度，自動換行：

```html
<div>, <h1>-<h6>, <p>, <ul>, <ol>, <li>, <table>, 
<form>, <header>, <footer>, <nav>, <section>, <article>, <aside>
```

### 內聯元素 (Inline Elements)

內聯元素只佔據所需的寬度，不會自動換行：

```html
<span>, <a>, <strong>, <em>, <img>, <input>, 
<label>, <button>, <code>, <small>
```

### 區塊與內聯的轉換

使用 CSS 可以改變元素的顯示模式：

```css
/* 讓 span 變成區塊元素 */
span {
    display: block;
}

/* 讓 div 變成內聯元素 */
div {
    display: inline;
}

/* 讓 div 變成行內區塊元素（可設定寬高） */
div {
    display: inline-block;
}
```

## HTML 表單驗證

HTML5 提供了多種表單驗證屬性，無需 JavaScript 即可實現基本的輸入驗證。

### 內建驗證屬性

```html
<!-- 必填欄位 -->
<input type="text" required>

<!-- 最小/最大長度 -->
<input type="text" minlength="3" maxlength="20">

<!-- 數值範圍 -->
<input type="number" min="1" max="100">

<!-- 電子郵件格式 -->
<input type="email" required>

<!-- URL 格式 -->
<input type="url">

<!-- 圖案匹配 (正規表達式) -->
<input type="text" pattern="[A-Za-z]{3,}">

<!-- 顏色選擇 -->
<input type="color">

<!-- 日期和時間 -->
<input type="date">
<input type="time">
<input type="datetime-local">
```

### 自訂驗證訊息

```javascript
// 使用 JavaScript 自訂驗證訊息
const input = document.getElementById('email');

input.addEventListener('invalid', function(event) {
    if (input.validity.valueMissing) {
        input.setCustomValidity('請輸入電子郵件地址');
    } else if (input.validity.typeMismatch) {
        input.setCustomValidity('請輸入有效的電子郵件格式');
    } else {
        input.setCustomValidity('');
    }
});

input.addEventListener('input', function(event) {
    input.setCustomValidity('');
});
```

## SEO 與元資料

### Meta 標籤

```html
<!-- 字元編碼 -->
<meta charset="UTF-8">

<!-- 行動裝置 viewport -->
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<!-- 網頁描述 (SEO 重要) -->
<meta name="description" content="這是一個關於網頁開發的教學網站...">

<!-- 關鍵字 -->
<meta name="keywords" content="HTML, CSS, JavaScript, 網頁開發">

<!-- 作者 -->
<meta name="author" content="作者名稱">

<!-- Robots 指示 -->
<meta name="robots" content="index, follow">

<!-- Open Graph (社群媒體分享) -->
<meta property="og:title" content="網頁標題">
<meta property="og:description" content="網頁描述">
<meta property="og:image" content="preview.jpg">
<meta property="og:url" content="https://example.com">

<!-- Twitter Card -->
<meta name="twitter:card" content="summary_large_image">
<meta name="twitter:title" content="網頁標題">
<meta name="twitter:description" content="網頁描述">
<meta name="twitter:image" content="preview.jpg">
```

### 結構化資料 (Schema Markup)

```html
<!-- 組織資訊 -->
<script type="application/ld+json">
{
    "@context": "https://schema.org",
    "@type": "Organization",
    "name": "公司名稱",
    "url": "https://example.com",
    "logo": "https://example.com/logo.png",
    "contactPoint": {
        "@type": "ContactPoint",
        "telephone": "+886-2-1234-5678",
        "contactType": "customer service"
    }
}
</script>

<!-- 個人 -->
<script type="application/ld+json">
{
    "@context": "https://schema.org",
    "@type": "Person",
    "name": "王小明",
    "jobTitle": "網頁開發工程師",
    "email": "wang@example.com",
    "url": "https://wang.example.com"
}
</script>
```

## 無障礙網頁設計 (Accessibility)

### ARIA 屬性

```html
<!-- 標記區域角色 -->
<nav role="navigation" aria-label="主選單">
    <ul>...</ul>
</nav>

<!-- 標記彈出視窗 -->
<button aria-expanded="false" aria-controls="menu" onclick="toggleMenu()">
    選單
</button>
<div id="menu" role="menu" hidden>...</div>

<!-- 標記必要欄位 -->
<label for="name">姓名 <span aria-label="必填">*</span></label>
<input id="name" type="text" aria-required="true">

<!-- 標記錯誤訊息 -->
<input id="email" type="email" aria-invalid="true" aria-describedby="email-error">
<span id="email-error" role="alert">請輸入有效的電子郵件地址</span>
```

### 無障礙最佳實踐

```html
<!-- 確保圖片有替代文字 -->
<img src="photo.jpg" alt="一群人在海灘玩耍">

<!-- 對於裝飾性圖片，使用空的 alt -->
<img src="decorative.jpg" alt="">

<!-- 連結文字要有意義 -->
<a href="...">按這裡</a>        <!-- 不佳 -->
<a href="...">了解更多</a>      <!-- 不佳 -->
<a href="/products">查看我們的產品</a>  <!-- 較佳 -->

<!-- 確保表單標籤與輸入關聯 -->
<label for="username">使用者名稱</label>
<input id="username" type="text">

<!-- 使用標題層次結構 -->
<h1>主標題</h1>
<h2>二級標題</h2>
<h3>三級標題</h3>
<!-- 不要跳過 h2 直接用 h4 -->
```

## HTML 與 CSS、JavaScript 的關係

HTML 負責定義網頁的結構和內容，CSS 負責呈現樣式，JavaScript 負責互動行為。三者分工明確，共同構成現代網頁開發的基礎。

詳見 [CSS](CSS.md) 了解網頁樣式設計。

詳見 [JavaScript](JavaScript.md) 了解網頁互動程式設計。

## 相關資源

- [MDN Web Docs - HTML](https://developer.mozilla.org/zh-TW/docs/Web/HTML)
- [HTML Living Standard](https://html.spec.whatwg.org/)
- [WHATWG](https://whatwg.org/)

---

**參考來源**: 
- [MDN Web Docs](https://developer.mozilla.org/zh-TW/docs/Web/HTML)
- [HTML Living Standard](https://html.spec.whatwg.org/)