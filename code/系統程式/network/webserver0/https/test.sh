#!/bin/bash

echo "=== HTTPS 伺服器測試 ==="

cd "$(dirname "$0")"

echo "1. 編譯..."
gcc -o https_server https_server.c -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto
if [ $? -ne 0 ]; then
    echo "編譯失敗，請確認 OpenSSL 路徑"
    exit 1
fi

echo "2. 啟動伺服器..."
./https_server 8443 cert.pem key.pem &
SERVER_PID=$!
sleep 1

echo "3. 測試..."
curl -k https://localhost:8443/ | head -20

echo ""
echo "4. 清理..."
kill $SERVER_PID 2>/dev/null

echo "=== 完成 ==="