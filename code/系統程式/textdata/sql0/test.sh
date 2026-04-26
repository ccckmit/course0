#!/bin/bash

# 設定顏色輸出
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "======================================"
echo "    sql0 (B+ Tree Database) Tester    "
echo "======================================"


# 2. 清理舊資料庫檔案
echo "=> Cleaning up old database file..."
rm -f mydb.db

# ---------------------------------------------------------
# 測試情境 1: 基本寫入與查詢
# ---------------------------------------------------------
echo -e "\n=> Running Test 1: Basic Insert and Select..."
# 使用 Here-Doc (<<EOF) 將指令自動餵給 sql0
OUTPUT1=$(./sql0 <<EOF
insert 3 charlie charlie@email.com
insert 1 alice alice@email.com
insert 2 bob bob@email.com
select 2
select_all
.exit
EOF
)

# 驗證特定查詢
if echo "$OUTPUT1" | grep -q "(2, 'bob', 'bob@email.com')"; then
    echo -e "  [${GREEN}PASS${NC}] Select specific row."
else
    echo -e "  [${RED}FAIL${NC}] Select specific row."
fi

# 驗證總數 (應該是 3)
if echo "$OUTPUT1" | grep -q "Total rows: 3"; then
    echo -e "  [${GREEN}PASS${NC}] Total row count."
else
    echo -e "  [${RED}FAIL${NC}] Total row count."
fi

# ---------------------------------------------------------
# 測試情境 2: 硬碟持久化 (Persistence) 與 B+ Tree 節點分裂
# ---------------------------------------------------------
echo -e "\n=> Running Test 2: Disk Persistence & B+ Tree Splitting..."
# 再次啟動 sql0 (不刪除 mydb.db)，並寫入更多資料觸發 Split
OUTPUT2=$(./sql0 <<EOF
insert 5 eve eve@email.com
insert 4 dave dave@email.com
select_all
.exit
EOF
)

# 驗證舊資料是否還在，且新資料也成功加入 (總數應該是 5)
if echo "$OUTPUT2" | grep -q "Total rows: 5"; then
    echo -e "  [${GREEN}PASS${NC}] Data persisted to disk successfully."
else
    echo -e "  [${RED}FAIL${NC}] Data persistence failed. Expected 5 rows."
fi

# 驗證 B+ Tree 排序是否正確 (alice 應該在 dave 前面)
# [修正點]：將 grep -oP 替換為 macOS (BSD) 支援的 awk
# ORDER_CHECK=$(echo "$OUTPUT2" | grep "^(" | awk -F'[(,]' '{print $2}' | tr '\n' ' ')
ORDER_CHECK=$(echo "$OUTPUT2" | grep "(" | awk -F'[(,]' '{print $2}' | tr '\n' ' ')

if [[ "$ORDER_CHECK" == *"1 2 3 4 5 "* ]]; then
    echo -e "  [${GREEN}PASS${NC}] B+ Tree kept rows sorted after split."
else
    echo -e "  [${RED}FAIL${NC}] B+ Tree sorting failed."
    echo "Actual order: $ORDER_CHECK"
fi

# ---------------------------------------------------------
# 測試情境 3: 重複的 Primary Key
# ---------------------------------------------------------
echo -e "\n=> Running Test 3: Duplicate Primary Key Handling..."
OUTPUT3=$(./sql0 <<EOF
insert 2 fake_bob fake@email.com
.exit
EOF
)

if echo "$OUTPUT3" | grep -q "Error: Duplicate primary key 2"; then
    echo -e "  [${GREEN}PASS${NC}] Duplicate key correctly rejected."
else
    echo -e "  [${RED}FAIL${NC}] Duplicate key was not rejected."
fi

echo -e "\n======================================"
echo -e "          All tests finished!           "
echo "======================================"