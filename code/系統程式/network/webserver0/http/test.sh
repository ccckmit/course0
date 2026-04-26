#!/bin/bash

echo "=== HTTP 伺服器測試 ==="

cd "$(dirname "$0")"

echo "1. 編譯..."
gcc -o webserver webserver.c

echo "2. 啟動伺服器..."
./webserver 8080 &
SERVER_PID=$!
sleep 1

echo "3. 測試..."
curl -s http://localhost:8080/ | head -20

echo ""
echo "4. 清理..."
kill $SERVER_PID 2>/dev/null

echo "=== 完成 ==="