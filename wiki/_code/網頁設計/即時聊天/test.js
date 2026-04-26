const http = require('http');
const { spawn } = require('child_process');

const BASE_URL = 'http://localhost:3001';

function request(path) {
  return new Promise((resolve, reject) => {
    http.get(BASE_URL + path, (res) => {
      let body = '';
      res.on('data', chunk => body += chunk);
      res.on('end', () => resolve({ status: res.statusCode, body }));
    }).on('error', reject);
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
  
  console.log('=== Chat System Tests ===\n');
  
  console.log('--- HTTP Server ---');
  const res = await request('/');
  assert(res.status === 200, 'Server responds to HTTP');
  assert(res.body.includes('<html'), 'HTML page returned');
  
  const res2 = await request('/index.html');
  assert(res2.status === 200, 'Static file served');
  assert(res2.body.includes('即時聊天') || res2.body.includes('&#38718;&#26144;&#32887;'), 'Chat page title present');
  
  console.log('\n--- Server Status ---');
  const res3 = await request('/nonexistent');
  assert(res3.status === 404, '404 for unknown routes');
  
  console.log('\n=== Results ===');
  console.log('Passed: ' + passed);
  console.log('Failed: ' + failed);
  console.log('Total: ' + (passed + failed));
  
  return failed;
}

async function main() {
  console.log('Starting server...');
  const server = spawn('node', ['server.js'], { cwd: __dirname, stdio: 'pipe' });
  
  await new Promise(r => setTimeout(r, 1000));
  
  try {
    const failed = await test();
    process.exit(failed > 0 ? 1 : 0);
  } catch (err) {
    console.error('Test error:', err.message);
    process.exit(1);
  } finally {
    server.kill();
  }
}

main();
