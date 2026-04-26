# JavaScript

**標籤**: #JavaScript #前端 #DOM #非同步 #ES6
**日期**: 2024-01-15

JavaScript 是網頁的程式語言，實現互動功能。由 Brendan Eich 於 1995 年為 Netscape Navigator 瀏覽器開發，最初命名為 LiveScript，後來改為 JavaScript。作為 Web 的三大核心技術之一（HTML、CSS、JavaScript），JavaScript 負責網頁的互動行為和動態內容。

## JavaScript 的發展歷程

### 早期時代 (1995-2005)

JavaScript 1.0-1.3 是最早的版本，實現了基本的語言功能。2000 年，微軟推出 JScript，導致瀏覽器相容性問題。

### AJAX 革命 (2005-2008)

Google Maps 和 Gmail 的成功展示了 AJAX 的潛力，JavaScript 開始從簡單的腳本語言演變為強大的應用程式開發語言。

### jQuery 時代 (2006-2015)

jQuery 的出現大幅簡化了 DOM 操作和跨瀏覽器相容性問題，成為最流行的 JavaScript 庫。

### Modern JavaScript (2015-現在)

ES6 (ECMAScript 2015) 帶來了類 (class)、模組 (module)、箭頭函數 (arrow function)、解構賦值 (destructuring) 等重大更新。此後每年都有新版本發布。

## 基本語法

### 變數宣告

```javascript
// let - 區塊作用域，可重新賦值
let count = 0;
count = 1;

// const - 區塊作用域，不可重新賦值
const PI = 3.14159;
const config = { debug: true };

// var - 函式作用域（不推薦使用）
var name = 'JavaScript';
```

### 資料類型

```javascript
// 基本類型
let str = 'Hello';           // 字串
let num = 42;                // 數字
let bool = true;             // 布林值
let empty = null;            // 空值
let notDefined = undefined;  // 未定義
let sym = Symbol('id');      // 符號
let bigInt = 9007199254740991n; // 大整數

// 物件類型
let arr = [1, 2, 3];         // 陣列
let obj = { name: 'obj' };   // 物件
let func = function() {};    // 函數

// 類型檢查
typeof str === 'string';     // true
typeof num === 'number';     // true
Array.isArray(arr);          // true
```

### 函數

```javascript
// 函數宣告
function greet(name) {
    return `Hello, ${name}!`;
}

// 函數表達式
const greet = function(name) {
    return `Hello, ${name}!`;
};

// 箭頭函數
const greet = (name) => `Hello, ${name}!`;
const add = (a, b) => a + b;

// 預設參數
function greet(name = 'World') {
    return `Hello, ${name}!`;
}

// 剩餘參數
function sum(...numbers) {
    return numbers.reduce((a, b) => a + b, 0);
}

// 範例
sum(1, 2, 3, 4); // 10
```

### 類別

```javascript
class Person {
    constructor(name, age) {
        this.name = name;
        this.age = age;
    }
    
    greet() {
        return `Hello, I'm ${this.name}`;
    }
    
    static species() {
        return 'Homo Sapiens';
    }
}

class Student extends Person {
    constructor(name, age, grade) {
        super(name, age);
        this.grade = grade;
    }
    
    greet() {
        return `${super.greet()}, grade ${this.grade}`;
    }
}

const student = new Student('王小明', 20, 'A');
student.greet(); // "Hello, I'm 王小明, grade A"
```

### 解構賦值

```javascript
// 陣列解構
const [a, b, ...rest] = [1, 2, 3, 4, 5];
// a = 1, b = 2, rest = [3, 4, 5]

// 物件解構
const { name, age } = { name: '王小明', age: 25 };
// name = '王小明', age = 25

// 預設值
const { name, age = 18 } = { name: '王小明' };
```

## DOM 操作

### 選擇元素

```javascript
// 選擇單一元素
const el = document.querySelector('#app');
const el = document.getElementById('app');

// 選擇多個元素
const els = document.querySelectorAll('.item');
const els = document.getElementsByClassName('item');

// 選擇父子元素
const parent = el.parentElement;
const children = el.children;
const firstChild = el.firstElementChild;
const lastChild = el.lastElementChild;

// 兄弟元素
const next = el.nextElementSibling;
const prev = el.previousElementSibling;
```

### 修改元素

```javascript
// 修改內容
el.textContent = '新內容';
el.innerHTML = '<strong>HTML 內容</strong>';

// 修改屬性
el.setAttribute('data-id', '123');
el.getAttribute('data-id');
el.removeAttribute('data-id');

// 修改類別
el.classList.add('active');
el.classList.remove('active');
el.classList.toggle('active');
el.classList.contains('active');

// 修改樣式
el.style.color = 'red';
el.style.fontSize = '16px';
el.style.display = 'none';
```

### 建立和插入元素

```javascript
// 建立元素
const newEl = document.createElement('div');
newEl.textContent = '新元素';
newEl.className = 'card';

// 插入元素
parent.appendChild(newEl);           // 添加到末尾
parent.insertBefore(newEl, reference); // 添加到參考元素前

// 插入 HTML
el.insertAdjacentHTML('beforeend', '<span>內容</span>');
// 'beforebegin' - 元素前
// 'afterbegin' - 元素內最前
// 'beforeend' - 元素內最後
// 'afterend' - 元素後

// 刪除元素
el.remove();
parent.removeChild(el);
```

### 事件處理

```javascript
// 添加事件監聽
el.addEventListener('click', function(event) {
    console.log('Clicked!', event.target);
});

// 箭頭函數
el.addEventListener('click', (event) => {
    console.log('Clicked at', event.clientX, event.clientY);
});

// 事件委託
document.querySelector('.list').addEventListener('click', (event) => {
    if (event.target.matches('.item')) {
        console.log('Item clicked:', event.target);
    }
});

// 移除事件監聽
const handler = () => console.log('Click');
el.addEventListener('click', handler);
el.removeEventListener('click', handler);

// 常見事件
el.addEventListener('click');        // 點擊
el.addEventListener('dblclick');     // 雙擊
el.addEventListener('mouseenter');   // 滑入
el.addEventListener('mouseleave');   // 滑出
el.addEventListener('keydown');      // 按鍵按下
el.addEventListener('keyup');        // 按鍵放開
el.addEventListener('input');        // 輸入
el.addEventListener('submit');      // 表單提交
el.addEventListener('change');       // 變更
```

## 非同步程式設計

### Promise

```javascript
// 建立 Promise
const promise = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve('完成！');
    }, 1000);
});

// 使用 Promise
promise
    .then(result => console.log(result))
    .catch(error => console.error(error))
    .finally(() => console.log('完成'));

// Promise 鏈
fetch('/api/data')
    .then(res => res.json())
    .then(data => processData(data))
    .then(result => console.log(result))
    .catch(error => console.error(error));

// Promise.all - 等待所有完成
Promise.all([promise1, promise2, promise3])
    .then(([result1, result2, result3]) => {
        console.log('全部完成');
    });

// Promise.race - 任一完成
Promise.race([promise1, promise2])
    .then(result => console.log(result));

// Promise.allSettled - 所有結算
Promise.allSettled([promise1, promise2])
    .then(results => {
        results.forEach(({ status, value }) => {
            console.log(status, value);
        });
    });
```

### Async/Await

```javascript
// async 函數
async function fetchData() {
    try {
        const res = await fetch('/api/data');
        const data = await res.json();
        return data;
    } catch (error) {
        console.error('錯誤:', error);
    }
}

// 並行執行
async function fetchAll() {
    const [users, posts] = await Promise.all([
        fetch('/api/users').then(r => r.json()),
        fetch('/api/posts').then(r => r.json())
    ]);
    return { users, posts };
}

// 錯誤處理
async function safeFetch(url) {
    try {
        const res = await fetch(url);
        if (!res.ok) {
            throw new Error(`HTTP ${res.status}`);
        }
        return await res.json();
    } catch (error) {
        console.error('請求失敗:', error);
        return null;
    }
}
```

### 事件發布/訂閱

```javascript
class EventEmitter {
    constructor() {
        this.events = {};
    }
    
    on(event, callback) {
        if (!this.events[event]) {
            this.events[event] = [];
        }
        this.events[event].push(callback);
    }
    
    off(event, callback) {
        if (!this.events[event]) return;
        this.events[event] = this.events[event].filter(cb => cb !== callback);
    }
    
    emit(event, ...args) {
        if (!this.events[event]) return;
        this.events[event].forEach(callback => callback(...args));
    }
    
    once(event, callback) {
        const wrapper = (...args) => {
            callback(...args);
            this.off(event, wrapper);
        };
        this.on(event, wrapper);
    }
}

const emitter = new EventEmitter();
emitter.on('message', (msg) => console.log('收到:', msg));
emitter.emit('message', 'Hello');
```

## 陣列操作

```javascript
const arr = [1, 2, 3, 4, 5];

// map - 轉換
const doubled = arr.map(x => x * 2); // [2, 4, 6, 8, 10]

// filter - 篩選
const evens = arr.filter(x => x % 2 === 0); // [2, 4]

// reduce - 累加
const sum = arr.reduce((acc, x) => acc + x, 0); // 15

// find - 查找
const found = arr.find(x => x > 3); // 4

// some - 是否有符合
const hasEven = arr.some(x => x % 2 === 0); // true

// every - 是否全部符合
const allPositive = arr.every(x => x > 0); // true

// forEach - 遍歷
arr.forEach((x, i) => console.log(i, x));

// sort - 排序
const sorted = [...arr].sort((a, b) => b - a); // [5, 4, 3, 2, 1]

// 鏈式操作
const result = arr
    .filter(x => x > 2)
    .map(x => x * 2)
    .reduce((a, b) => a + b, 0); // 18
```

## 物件操作

```javascript
const obj = { a: 1, b: 2, c: 3 };

// Object.keys - 取得鍵
Object.keys(obj); // ['a', 'b', 'c']

// Object.values - 取得值
Object.values(obj); // [1, 2, 3]

// Object.entries - 鍵值對
Object.entries(obj); // [['a', 1], ['b', 2], ['c', 3]]

// Object.assign - 複製/合併
const copy = Object.assign({}, obj);
const merged = Object.assign({}, obj, { d: 4 });

// 展開運算符
const copy = { ...obj };
const merged = { ...obj, d: 4 };

// 解構賦值
const { a, b } = obj;
// a = 1, b = 2

// 可選鏈
const value = obj?.nested?.value;

// 空值合併
const value = obj.value ?? 'default';
```

## 模組系統

### ES Modules

```javascript
// 導出
// math.js
export const add = (a, b) => a + b;
export const subtract = (a, b) => a - b;
export default multiply;

// 導入
import { add, subtract } from './math.js';
import multiply from './math.js';
import * as math from './math.js';
```

### CommonJS (Node.js)

```javascript
// 導出
module.exports = { add, subtract };
exports.multiply = multiply;

// 導入
const { add, subtract } = require('./math.js');
```

## 錯誤處理

```javascript
try {
    const result = riskyOperation();
} catch (error) {
    console.error('錯誤:', error.message);
} finally {
    console.log('完成');
}

// 拋出錯誤
function divide(a, b) {
    if (b === 0) {
        throw new Error('不能除以零');
    }
    return a / b;
}

// 自訂錯誤類別
class AppError extends Error {
    constructor(message, code) {
        super(message);
        this.code = code;
    }
}
```

## 常用 API

### JSON

```javascript
const obj = { name: '王小明', age: 25 };

// 序列化
const json = JSON.stringify(obj);
const jsonPretty = JSON.stringify(obj, null, 2);

// 解析
const parsed = JSON.parse(json);
```

### localStorage / sessionStorage

```javascript
// localStorage - 持久化
localStorage.setItem('key', 'value');
localStorage.getItem('key');
localStorage.removeItem('key');
localStorage.clear();

// sessionStorage - 會話級別
sessionStorage.setItem('key', 'value');
sessionStorage.getItem('key');

// 儲存物件
localStorage.setItem('user', JSON.stringify(obj));
const user = JSON.parse(localStorage.getItem('user'));
```

### setTimeout / setInterval

```javascript
// 延遲執行
setTimeout(() => {
    console.log('延遲 1 秒');
}, 1000);

// 清除延遲
const timeoutId = setTimeout(() => {}, 1000);
clearTimeout(timeoutId);

// 定期執行
const intervalId = setInterval(() => {
    console.log('每 1 秒');
}, 1000);

// 清除定期
clearInterval(intervalId);
```

## ES6+ 新特性

### BigInt

```javascript
const bigInt = 9007199254740991n;
const result = bigInt + 1n;
```

### 可選鏈和空值合併

```javascript
// 可選鏈
const value = obj?.nested?.property;
const method = obj?.method?.();

// 空值合併
const value = null ?? 'default'; // 'default'
const value = 0 ?? 'default';    // 0
```

### 標籤模板

```javascript
function highlight(strings, ...values) {
    return strings.reduce((acc, str, i) => {
        return acc + str + (values[i] ? `<mark>${values[i]}</mark>` : '');
    }, '');
}

const name = 'JavaScript';
highlight`Hello ${name}!`; // "Hello <mark>JavaScript</mark>!"
```

### 生成器

```javascript
function* generator() {
    yield 1;
    yield 2;
    yield 3;
}

const gen = generator();
gen.next(); // { value: 1, done: false }
gen.next(); // { value: 2, done: false }
gen.next(); // { value: 3, done: false }
gen.next(); // { value: undefined, done: true }
```

## 實用範例

### 防抖和節流

```javascript
// 防抖 - 等待一段時間後才執行
function debounce(func, wait) {
    let timeout;
    return function(...args) {
        clearTimeout(timeout);
        timeout = setTimeout(() => func.apply(this, args), wait);
    };
}

// 節流 - 限制執行頻率
function throttle(func, limit) {
    let inThrottle;
    return function(...args) {
        if (!inThrottle) {
            func.apply(this, args);
            inThrottle = true;
            setTimeout(() => inThrottle = false, limit);
        }
    };
}

// 使用
window.addEventListener('resize', debounce(() => {
    console.log('調整大小');
}, 300));
```

### 深拷貝

```javascript
function deepClone(obj, hash = new WeakMap()) {
    if (obj === null || typeof obj !== 'object') {
        return obj;
    }
    
    if (hash.has(obj)) {
        return hash.get(obj);
    }
    
    const clone = Array.isArray(obj) ? [] : {};
    hash.set(obj, clone);
    
    for (const key in obj) {
        if (obj.hasOwnProperty(key)) {
            clone[key] = deepClone(obj[key], hash);
        }
    }
    
    return clone;
}
```

## 相關概念

- [HTML](HTML.md) - 了解 HTML 結構
- [前端框架](前端框架.md) - 了解 React、Vue 等框架
- [API](API.md) - 了解 API 設計
- [Node.js](Node.js.md) - 了解伺服器端 JavaScript

---

**參考來源**:
- [MDN Web Docs - JavaScript](https://developer.mozilla.org/zh-TW/docs/Web/JavaScript)
- [JavaScript.info](https://zh.javascript.info/)
- [ECMAScript 官方規範](https://tc39.es/ecma262/)
