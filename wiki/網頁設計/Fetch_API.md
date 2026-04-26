# Fetch API

## 概述

Fetch API 是現代瀏覽器提供的網路請求介面，用於在 JavaScript 中發送 HTTP 請求，是 XMLHttpRequest 的現代替代方案。

## 基本用法

### GET 請求

```javascript
// 基本 GET 請求
fetch('https://api.example.com/data')
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.error('Error:', error));

// async/await 語法
async function getData() {
    try {
        const response = await fetch('https://api.example.com/data');
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Error:', error);
    }
}
```

### POST 請求

```javascript
async function postData() {
    const response = await fetch('https://api.example.com/users', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': 'Bearer token123'
        },
        body: JSON.stringify({
            name: 'Alice',
            email: 'alice@example.com'
        })
    });
    
    if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    return await response.json();
}
```

## Response 物件

| 屬性 | 說明 |
|------|------|
| `status` | HTTP 狀態碼 |
| `ok` | 是否成功 (200-299) |
| `statusText` | 狀態文字 |
| `headers` | 回應標頭 |
| `url` | 請求 URL |

### 讀取不同格式

```javascript
const response = await fetch('https://api.example.com/data');

// JSON
const json = await response.json();

// 純文字
const text = await response.text();

// Blob (檔案)
const blob = await response.blob();

// FormData
const formData = await response.formData();

// ArrayBuffer
const buffer = await response.arrayBuffer();
```

## 請求選項

```javascript
const options = {
    method: 'GET',           // GET, POST, PUT, DELETE, etc.
    headers: {               // 請求標頭
        'Content-Type': 'application/json',
        'Accept': 'application/json',
        'Authorization': 'Bearer token'
    },
    body: null,              // 請求主體（字串、FormData、Blob 等）
    mode: 'cors',            // cors, no-cors, same-origin
    credentials: 'include', // omit, same-origin, include
    cache: 'default',        // default, no-cache, reload, force-cache
    redirect: 'follow',       // follow, error, manual
    referrer: 'client',      // 參照來源
    referrerPolicy: 'no-referrer-when-downgrade',
    integrity: '',           // 完整性校驗值
    signal: null             // AbortController 訊號
};
```

## 錯誤處理

```javascript
async function safeFetch(url, options = {}) {
    try {
        const response = await fetch(url, options);
        
        if (!response.ok) {
            const error = new Error(`HTTP ${response.status}: ${response.statusText}`);
            error.response = response;
            throw error;
        }
        
        return await response.json();
    } catch (error) {
        if (error.name === 'AbortError') {
            console.log('Request was aborted');
        } else if (error.message.includes('Failed to fetch')) {
            console.log('Network error or CORS issue');
        }
        throw error;
    }
}
```

## AbortController

取消請求：

```javascript
const controller = new AbortController();
const signal = controller.signal;

// 5 秒後取消
const timeoutId = setTimeout(() => controller.abort(), 5000);

try {
    const response = await fetch(url, { signal });
    const data = await response.json();
    clearTimeout(timeoutId);
    return data;
} catch (error) {
    if (error.name === 'AbortError') {
        console.log('Request cancelled');
    }
}
```

## 並行請求

### Promise.all

```javascript
async function fetchAll() {
    const [users, posts, comments] = await Promise.all([
        fetch('/api/users').then(r => r.json()),
        fetch('/api/posts').then(r => r.json()),
        fetch('/api/comments').then(r => r.json())
    ]);
    
    return { users, posts, comments };
}
```

### Promise.allSettled

```javascript
const results = await Promise.allSettled([
    fetch('/api/users').then(r => r.json()),
    fetch('/api/posts').then(r => r.json()),
    fetch('/api/comments').then(r => r.json())
]);

results.forEach((result, i) => {
    if (result.status === 'fulfilled') {
        console.log(`API ${i}:`, result.value);
    } else {
        console.log(`API ${i} failed:`, result.reason);
    }
});
```

## 進階用法

### 檔案上傳

```javascript
async function uploadFile(file) {
    const formData = new FormData();
    formData.append('file', file);
    
    const response = await fetch('/api/upload', {
        method: 'POST',
        body: formData
        // 不需要 Content-Type，瀏覽器會自動設定
    });
    
    return await response.json();
}
```

### 進度追蹤

```javascript
async function uploadWithProgress(file) {
    return new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        
        xhr.upload.addEventListener('progress', (e) => {
            const percent = (e.loaded / e.total) * 100;
            console.log(`Upload: ${percent.toFixed(2)}%`);
        });
        
        xhr.addEventListener('load', () => {
            resolve(JSON.parse(xhr.responseText));
        });
        
        xhr.addEventListener('error', reject);
        
        xhr.open('POST', '/api/upload');
        xhr.send(file);
    });
}
```

## Fetch vs XMLHttpRequest

| 特性 | Fetch API | XMLHttpRequest |
|------|-----------|----------------|
| API 設計 | Promise-based | Event-based |
| 錯誤處理 | 只處理網路錯誤 | 處理所有 HTTP 錯誤 |
| 跨域請求 | 需 CORS | 需代理或 CORS |
| 支援度 | 現代瀏覽器 | 所有瀏覽器 |

## Polyfill

```html
<script src="https://cdn.jsdelivr.net/npm/whatwg-fetch@3.6.2/dist/fetch.umd.js"></script>
```

## 相關資源

- 相關概念：[API](API.md)
- 相關主題：[RESTful](RESTful.md)
- 相關主題：[WebSocket](WebSocket.md)

## Tags

#Fetch #API #HTTP #JavaScript #非同步
