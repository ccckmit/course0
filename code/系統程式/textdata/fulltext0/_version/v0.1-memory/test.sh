#!/bin/bash
set -x

gcc -o fulltext0 fulltext0.c -Wall

echo "=== Test 1: Search single Chinese char '電' ==="
./fulltext0 "電"

echo ""
echo "=== Test 2: Search Chinese word '電腦' ==="
./fulltext0 "電腦"

echo ""
echo "=== Test 3: Search English 'system' ==="
./fulltext0 "system"

echo ""
echo "=== Test 4: Search multi-word 'machine learning' ==="
./fulltext0 "machine learning"

echo ""
echo "=== Test 5: Search Chinese '人工智慧' ==="
./fulltext0 "人工智慧"

echo ""
echo "=== Test 6: Verify result correctness ==="
./fulltext0 "系統" > /tmp/result.txt 2>/dev/null
grep "^\s*\[" /tmp/result.txt | while read line; do
    CONTENT=$(echo "$line" | sed 's/.*\] //')
    if ! echo "$CONTENT" | grep -q "系統"; then
        echo "FAIL: Missing keyword '系統' in: $CONTENT"
        exit 1
    fi
done
echo "PASS: All results contain keyword"

echo ""
echo "=== Test 7: Search English 'platform' ==="
./fulltext0 "platform"

echo ""
echo "=== Test 8: Search Chinese '資料庫' ==="
./fulltext0 "資料庫"