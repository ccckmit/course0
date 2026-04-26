# Node.js

**標籤**: #Node.js #後端 #JavaScript #Express #npm
**日期**: 2024-01-15

Node.js 是基於 Chrome V8 JavaScript 引擎建構的 JavaScript 執行環境，讓開發者能夠使用 JavaScript 撰寫伺服器端程式。由 Ryan Dahl 於 2009 年首次發布，Node.js 的出現徹底改變了 Web 開發，讓 JavaScript 成為真正的全端語言。

## Node.js 的核心特性

### 事件驅動架構

Node.js 採用事件驅動、非阻塞 I/O 模型，這使其在處理大量並發連接時表現優異。

```javascript
const EventEmitter = require('events');

class MyEmitter extends EventEmitter {}

const myEmitter = new MyEmitter();

myEmitter.on('event', () => {
    console.log('事件觸發！');
});

myEmitter.emit('event');
```

### 非阻塞 I/O

Node.js 使用非阻塞 I/O 操作，讓程式能夠在等待 I/O 完成時繼續執行其他任務。

```javascript
const fs = require('fs');

// 阻塞 I/O（不推薦）
const data = fs.readFileSync('file.txt');
console.log(data);

// 非阻塞 I/O（推薦）
fs.readFile('file.txt', (err, data) => {
    if (err) {
        return console.error('錯誤:', err);
    }
    console.log(data);
});

// 使用 Promise（async/await）
const fsPromises = require('fs').promises;

async function readFile() {
    try {
        const data = await fsPromises.readFile('file.txt', 'utf8');
        console.log(data);
    } catch (err) {
        console.error('錯誤:', err);
    }
}
```

### 單執行緒事件循環

雖然 Node.js 是單執行緒，但通過事件循環和回調函數，它能夠高效處理並發請求。

```javascript
const http = require('http');

const server = http.createServer((req, res) => {
    console.log(`收到請求: ${req.url}`);
    
    if (req.url === '/blocking') {
        // 模擬阻塞操作 - 會阻塞整個伺服器
        let sum = 0;
        for (let i = 0; i < 1e9; i++) {
            sum += i;
        }
        res.end(`結果: ${sum}`);
    } else if (req.url === '/non-blocking') {
        // 非阻塞操作 - 不會影響其他請求
        setTimeout(() => {
            res.end('非阻塞操作完成');
        }, 100);
    } else {
        res.end('Hello World');
    }
});

server.listen(3000, () => {
    console.log('伺服器執行於 http://localhost:3000');
});
```

## 模組系統

### CommonJS 模組

Node.js 使用 CommonJS 規範作為預設的模組系統。

```javascript
// 匯出模組 (myModule.js)
module.exports = {
    name: 'MyModule',
    version: '1.0.0',
    greet: function(name) {
        return `Hello, ${name}!`;
    }
};

// 或者使用 exports
exports.add = function(a, b) {
    return a + b;
};

exports.multiply = function(a, b) {
    return a * b;
};

// 匯入模組
const myModule = require('./myModule');
const { add, multiply } = require('./myModule');

console.log(myModule.greet('World'));
console.log(add(2, 3));
console.log(multiply(2, 3));
```

### 內建模組

```javascript
const path = require('path');
const os = require('os');
const fs = require('fs');
const http = require('http');
const https = require('https');
const url = require('url');
const crypto = require('crypto');
const events = require('events');
const stream = require('stream');
const util = require('util');

console.log('平台:', os.platform());
console.log('CPU 核心數:', os.cpus().length);
console.log('主目錄:', os.homedir());
console.log('hostname:', os.hostname());
```

## 檔案系統操作

### 基本檔案操作

```javascript
const fs = require('fs');
const fsPromises = require('fs').promises;
const path = require('path');

async function fileOperations() {
    const dir = 'example';
    const file = path.join(dir, 'test.txt');
    
    await fsPromises.mkdir(dir, { recursive: true });
    
    await fsPromises.writeFile(file, 'Hello, World!', 'utf8');
    
    const content = await fsPromises.readFile(file, 'utf8');
    console.log('檔案內容:', content);
    
    await fsPromises.appendFile(file, '\n新內容', 'utf8');
    
    await fsPromises.rename(file, path.join(dir, 'renamed.txt'));
    
    await fsPromises.unlink(path.join(dir, 'renamed.txt'));
    
    await fsPromises.rmdir(dir, { recursive: true });
}

fileOperations().catch(console.error);
```

### 串流 (Streams)

串流用於處理大型檔案，減少記憶體佔用。

```javascript
const fs = require('fs');
const zlib = require('zlib');

const readStream = fs.createReadStream('input.txt', 'utf8');
const writeStream = fs.createWriteStream('output.txt');
const gzip = zlib.createGzip();

readStream.on('data', (chunk) => {
    console.log('收到資料:', chunk.length);
    writeStream.write(chunk);
});

readStream.on('end', () => {
    console.log('讀取完成');
    writeStream.end();
});

readStream.on('error', (err) => {
    console.error('錯誤:', err);
});

// 管道操作（更簡潔）
const input = fs.createReadStream('input.txt');
const output = fs.createWriteStream('output.txt');
const gzipStream = zlib.createGzip();

input.pipe(gzipStream).pipe(output);
```

## HTTP 伺服器

### 基本 HTTP 伺服器

```javascript
const http = require('http');
const url = require('url');

const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const pathname = parsedUrl.pathname;
    const query = parsedUrl.query;
    
    res.setHeader('Content-Type', 'text/html; charset=utf-8');
    res.setHeader('X-Powered-By', 'Node.js');
    
    if (pathname === '/') {
        res.statusCode = 200;
        res.end(`
            <!DOCTYPE html>
            <html>
            <head><title>Node.js 伺服器</title></head>
            <body>
                <h1>歡迎來到 Node.js</h1>
                <p>路徑: ${pathname}</p>
            </body>
            </html>
        `);
    } else if (pathname === '/api/users') {
        res.setHeader('Content-Type', 'application/json');
        res.statusCode = 200;
        res.end(JSON.stringify([
            { id: 1, name: '王小明' },
            { id: 2, name: '李小華' }
        ]));
    } else {
        res.statusCode = 404;
        res.end('404 Not Found');
    }
});

server.listen(3000, () => {
    console.log('伺服器執行於 http://localhost:3000');
});
```

### Express 框架

Express 是最流行的 Node.js Web 框架，提供簡潔的 API 和靈活的擴展性。

```javascript
const express = require('express');
const app = express();
const port = 3000;

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'));

app.get('/', (req, res) => {
    res.send('Hello World!');
});

app.get('/api/users', (req, res) => {
    const users = [
        { id: 1, name: '王小明', email: 'wang@example.com' },
        { id: 2, name: '李小華', email: 'lee@example.com' }
    ];
    res.json(users);
});

app.get('/api/users/:id', (req, res) => {
    const userId = parseInt(req.params.id);
    const user = { id: userId, name: 'User ' + userId };
    res.json(user);
});

app.post('/api/users', (req, res) => {
    console.log('收到的資料:', req.body);
    const newUser = {
        id: Date.now(),
        ...req.body
    };
    res.status(201).json(newUser);
});

app.put('/api/users/:id', (req, res) => {
    const userId = parseInt(req.params.id);
    const updatedUser = {
        id: userId,
        ...req.body
    };
    res.json(updatedUser);
});

app.delete('/api/users/:id', (req, res) => {
    const userId = parseInt(req.params.id);
    res.status(204).send();
});

app.get('/api/search', (req, res) => {
    const { q, page = 1, limit = 10 } = req.query;
    res.json({
        query: q,
        page: parseInt(page),
        limit: parseInt(limit),
        results: []
    });
});

app.listen(port, () => {
    console.log(`Express 伺服器執行於 http://localhost:${port}`);
});
```

### 中間件

Express 的中間件是一種可以在請求-回應週期中執行代碼的函數。

```javascript
const express = require('express');
const app = express();

const loggerMiddleware = (req, res, next) => {
    const start = Date.now();
    res.on('finish', () => {
        const duration = Date.now() - start;
        console.log(`${req.method} ${req.url} - ${res.statusCode} - ${duration}ms`);
    });
    next();
};

const authMiddleware = (req, res, next) => {
    const token = req.headers.authorization;
    if (token === 'Bearer my-secret-token') {
        next();
    } else {
        res.status(401).json({ error: '未授權' });
    }
};

const validateMiddleware = (req, res, next) => {
    if (req.method === 'POST' && !req.body) {
        return res.status(400).json({ error: '請求主體為空' });
    }
    next();
};

app.use(loggerMiddleware);

app.get('/protected', authMiddleware, (req, res) => {
    res.json({ message: '受保護的資源' });
});

app.post('/api/data', validateMiddleware, (req, res) => {
    res.json({ received: req.body });
});

app.use((err, req, res, next) => {
    console.error('錯誤:', err);
    res.status(500).json({ error: '伺服器內部錯誤' });
});
```

## npm 套件管理

### 基本指令

```bash
npm init -y

npm install express
npm install express@4.18.2
npm install express --save
npm install express --save-dev
npm install express -g

npm uninstall express
npm update express

npm list
npm list -g
npm list --depth=0

npm search express
npm info express

npm run dev
npm start
npm test
```

### package.json

```json
{
  "name": "my-app",
  "version": "1.0.0",
  "description": "我的 Node.js 應用程式",
  "main": "index.js",
  "scripts": {
    "start": "node index.js",
    "dev": "nodemon index.js",
    "test": "jest",
    "build": "node build.js",
    "lint": "eslint ."
  },
  "keywords": ["nodejs", "express", "api"],
  "author": "Your Name",
  "license": "MIT",
  "dependencies": {
    "express": "^4.18.2",
    "dotenv": "^16.0.3"
  },
  "devDependencies": {
    "nodemon": "^2.0.22",
    "eslint": "^8.42.0"
  },
  "engines": {
    "node": ">=16.0.0"
  }
}
```

### 常見熱門套件

| 類別 | 套件 | 說明 |
|------|------|------|
| Web 框架 | Express | 最受歡迎的 Web 框架 |
| Web 框架 | Koa | Express 團隊開發的下一代框架 |
| Web 框架 | Fastify | 高效能 Web 框架 |
| 路由 | Express Router | Express 路由管理 |
| 範本引擎 | EJS, Pug, Handlebars | 伺服器端範本渲染 |
| 資料庫 | Mongoose | MongoDB ODM |
| 資料庫 | Sequelize | SQL ORM |
| 驗證 | Passport | 身份驗證中間件 |
| 驗證 | JWT | JSON Web Token |
| HTTP 客戶端 | Axios | HTTP 客戶端 |
| 日誌 | Morgan, Winston | 日誌記錄 |
| 快取 | Redis | Redis 客戶端 |
| 即時通訊 | Socket.io | WebSocket 即時通訊 |

## 非同步程式設計

### Callback

```javascript
const fs = require('fs');

fs.readFile('file1.txt', 'utf8', (err, data1) => {
    if (err) return console.error(err);
    
    fs.readFile('file2.txt', 'utf8', (err, data2) => {
        if (err) return console.error(err);
        
        fs.readFile('file3.txt', 'utf8', (err, data3) => {
            if (err) return console.error(err);
            
            console.log('所有檔案:', data1, data2, data3);
        });
    });
});
```

### Promise

```javascript
const fs = require('fs').promises;

async function readAllFiles() {
    try {
        const data1 = await fs.readFile('file1.txt', 'utf8');
        const data2 = await fs.readFile('file2.txt', 'utf8');
        const data3 = await fs.readFile('file3.txt', 'utf8');
        
        console.log('所有檔案:', data1, data2, data3);
    } catch (err) {
        console.error('錯誤:', err);
    }
}

readAllFiles();

// 並行讀取
async function readAllFilesParallel() {
    const [data1, data2, data3] = await Promise.all([
        fs.readFile('file1.txt', 'utf8'),
        fs.readFile('file2.txt', 'utf8'),
        fs.readFile('file3.txt', 'utf8')
    ]);
    
    console.log('所有檔案:', data1, data2, data3);
}

readAllFilesParallel();
```

### Async/Await

```javascript
const axios = require('axios');

async function fetchUserData(userId) {
    try {
        const userResponse = await axios.get(`/api/users/${userId}`);
        const user = userResponse.data;
        
        const postsResponse = await axios.get(`/api/users/${userId}/posts`);
        const posts = postsResponse.data;
        
        return { user, posts };
    } catch (err) {
        if (err.response && err.response.status === 404) {
            return null;
        }
        throw err;
    }
}

async function main() {
    const data = await fetchUserData(1);
    console.log(data);
}

main();
```

## 錯誤處理

```javascript
const express = require('express');
const app = express();

process.on('uncaughtException', (err) => {
    console.error('未捕獲的異常:', err);
    process.exit(1);
});

process.on('unhandledRejection', (reason, promise) => {
    console.error('未處理的 Promise 拒絕:', reason);
});

app.use((err, req, res, next) => {
    console.error('應用程式錯誤:', err);
    
    const statusCode = err.statusCode || err.status || 500;
    const message = err.message || 'Internal Server Error';
    
    res.status(statusCode).json({
        error: message,
        ...(process.env.NODE_ENV === 'development' && { stack: err.stack })
    });
});
```

## 測試

### Jest 測試框架

```javascript
// math.js
function add(a, b) {
    return a + b;
}

function multiply(a, b) {
    return a * b;
}

module.exports = { add, multiply };
```

```javascript
// math.test.js
const { add, multiply } = require('./math');

describe('Math', () => {
    describe('add', () => {
        test('2 + 3 應該等於 5', () => {
            expect(add(2, 3)).toBe(5);
        });
        
        test('-1 + 1 應該等於 0', () => {
            expect(add(-1, 1)).toBe(0);
        });
    });
    
    describe('multiply', () => {
        test('2 * 3 應該等於 6', () => {
            expect(multiply(2, 3)).toBe(6);
        });
        
        test('0 * 5 應該等於 0', () => {
            expect(multiply(0, 5)).toBe(0);
        });
    });
});
```

```bash
npm install --save-dev jest
npx jest
```

## 部署

### 生產環境最佳實踐

```javascript
const express = require('express');
const app = express();

app.use(express.json());

const PORT = process.env.PORT || 3000;
const ENV = process.env.NODE_ENV || 'development';

app.get('/health', (req, res) => {
    res.json({
        status: 'healthy',
        uptime: process.uptime(),
        timestamp: Date.now()
    });
});

app.listen(PORT, () => {
    if (ENV === 'production') {
        console.log('Production server running on port', PORT);
    } else {
        console.log('Development server running on port', PORT);
    }
});
```

### PM2 程序管理器

```bash
npm install -g pm2

pm2 start index.js
pm2 start index.js --name my-app
pm2 list
pm2 logs my-app
pm2 restart my-app
pm2 stop my-app
pm2 delete my-app

pm2 start ecosystem.config.js
```

```javascript
// ecosystem.config.js
module.exports = {
    apps: [{
        name: 'my-app',
        script: 'index.js',
        instances: 'max',
        exec_mode: 'cluster',
        env: {
            NODE_ENV: 'development',
            PORT: 3000
        },
        env_production: {
            NODE_ENV: 'production',
            PORT: 8080
        },
        error_file: '/var/log/pm2-error.log',
        out_file: '/var/log/pm2-out.log',
        log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
        merge_logs: true
    }]
};
```

## Node.js 生態系統

### NestJS

NestJS 是一個漸進式 Node.js 框架，適合構建可擴展的伺服器端應用程式。

```typescript
import { Module, Controller, Get, Post, Body } from '@nestjs/common';

@Controller('users')
class UsersController {
    @Get()
    findAll() {
        return [{ id: 1, name: 'User 1' }];
    }
    
    @Post()
    create(@Body() createUserDto: CreateUserDto) {
        return { id: Date.now(), ...createUserDto };
    }
}

@Module({
    controllers: [UsersController],
})
export class AppModule {}
```

### Koa

Koa 是由 Express 團隊開發的下一代 Node.js 框架，使用 async/await 更簡潔。

```javascript
const Koa = require('koa');
const Router = require('@koa/router');

const app = new Koa();
const router = new Router();

router.get('/', (ctx) => {
    ctx.body = 'Hello Koa!';
});

router.get('/api/users', (ctx) => {
    ctx.body = [{ id: 1, name: 'User 1' }];
});

app.use(router.routes());
app.use(router.allowedMethods());

app.listen(3000);
```

## 相關概念

- [後端開發](後端開發.md) - 了解後端開發概念
- [JavaScript](JavaScript.md) - 了解 JavaScript 基礎
- [RESTful](RESTful.md) - 了解 RESTful API 設計

---

**參考來源**:
- [Node.js 官方文檔](https://nodejs.org/docs/)
- [Express 官方文檔](https://expressjs.com/)
- [npm 官方網站](https://www.npmjs.com/)
