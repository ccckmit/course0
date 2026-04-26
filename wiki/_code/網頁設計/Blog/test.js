const http = require('http');
const { spawn } = require('child_process');

const BASE_URL = 'http://localhost:3000';
let server;

function request(method, path, data = null) {
  return new Promise((resolve, reject) => {
    const url = new URL(path, BASE_URL);
    const options = {
      hostname: url.hostname,
      port: url.port,
      path: url.pathname,
      method,
      headers: data ? { 'Content-Type': 'application/json' } : {}
    };
    
    const req = http.request(options, (res) => {
      let body = '';
      res.on('data', chunk => body += chunk);
      res.on('end', () => {
        try {
          const json = body ? JSON.parse(body) : null;
          resolve({ status: res.statusCode, body: json });
        } catch (e) {
          resolve({ status: res.statusCode, body });
        }
      });
    });
    
    req.on('error', reject);
    
    if (data) req.write(JSON.stringify(data));
    req.end();
  });
}

async function test() {
  let passed = 0;
  let failed = 0;
  
  function assert(condition, message) {
    if (condition) {
      console.log('✓ ' + message);
      passed++;
    } else {
      console.log('✗ ' + message);
      failed++;
    }
  }
  
  console.log('=== Blog API Tests ===\n');
  
  console.log('--- GET /api/posts (空列表) ---');
  let res = await request('GET', '/api/posts');
  assert(res.status === 200, 'Status 200');
  assert(Array.isArray(res.body), '回傳陣列');
  assert(res.body.length === 0, '初始長度為 0');
  
  console.log('\n--- POST /api/posts (新增文章) ---');
  res = await request('POST', '/api/posts', {
    title: 'Hello World',
    content: '這是我的第一篇文章',
    author: 'John'
  });
  assert(res.status === 201, 'Status 201');
  assert(res.body.id === 1, '文章 ID 為 1');
  assert(res.body.title === 'Hello World', '標題正確');
  
  const postId = res.body.id;
  
  console.log('\n--- GET /api/posts (有資料) ---');
  res = await request('GET', '/api/posts');
  assert(res.status === 200, 'Status 200');
  assert(res.body.length === 1, '長度為 1');
  assert(res.body[0].title === 'Hello World', '標題正確');
  
  console.log('\n--- GET /api/posts/:id ---');
  res = await request('GET', '/api/posts/' + postId);
  assert(res.status === 200, 'Status 200');
  assert(res.body.title === 'Hello World', '標題正確');
  assert(res.body.author === 'John', '作者正確');
  
  console.log('\n--- GET /api/posts/999 (不存在) ---');
  res = await request('GET', '/api/posts/999');
  assert(res.status === 404, 'Status 404');
  
  console.log('\n--- POST /api/posts (缺少欄位) ---');
  res = await request('POST', '/api/posts', { title: 'Test' });
  assert(res.status === 400, 'Status 400');
  
  console.log('\n--- PUT /api/posts/:id (更新) ---');
  res = await request('PUT', '/api/posts/' + postId, {
    title: 'Hello World Updated',
    content: '更新後的內容'
  });
  assert(res.status === 200, 'Status 200');
  assert(res.body.success === true, '更新成功');
  
  console.log('\n--- GET /api/posts/:id (驗證更新) ---');
  res = await request('GET', '/api/posts/' + postId);
  assert(res.body.title === 'Hello World Updated', '標題已更新');
  assert(res.body.content === '更新後的內容', '內容已更新');
  
  console.log('\n--- DELETE /api/posts/:id ---');
  res = await request('DELETE', '/api/posts/' + postId);
  assert(res.status === 200, 'Status 200');
  assert(res.body.success === true, '刪除成功');
  
  console.log('\n--- GET /api/posts (驗證刪除) ---');
  res = await request('GET', '/api/posts');
  assert(res.body.length === 0, '列表为空');
  
  console.log('\n--- DELETE /api/posts/999 (不存在) ---');
  res = await request('DELETE', '/api/posts/999');
  assert(res.status === 404, 'Status 404');
  
  console.log('\n=== Multiple Posts Test ===');
  await request('POST', '/api/posts', { title: 'Post 1', content: 'Content 1', author: 'A' });
  await new Promise(resolve => setTimeout(resolve, 1100));
  await request('POST', '/api/posts', { title: 'Post 2', content: 'Content 2', author: 'B' });
  await new Promise(resolve => setTimeout(resolve, 1100));
  await request('POST', '/api/posts', { title: 'Post 3', content: 'Content 3', author: 'C' });
  
  res = await request('GET', '/api/posts');
  assert(res.body.length === 3, '三篇文章');
  assert(res.body[0].title === 'Post 3', '第一篇是最新');
  assert(res.body[2].title === 'Post 1', '最後是最舊');
  
  console.log('\n=== Results ===');
  console.log('Passed: ' + passed);
  console.log('Failed: ' + failed);
  console.log('Total: ' + (passed + failed));
  
  return failed;
}

async function main() {
  console.log('Starting server...');
  server = spawn('node', ['server.js'], { cwd: __dirname });
  
  await new Promise(resolve => setTimeout(resolve, 1000));
  
  try {
    const failed = await test();
    process.exit(failed > 0 ? 1 : 0);
  } catch (err) {
    console.error('Test error:', err.message);
    process.exit(1);
  } finally {
    if (server) {
      server.kill();
    }
  }
}

main();
