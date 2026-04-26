#!/usr/bin/env bash
# test.sh — nosql0 測試腳本
# set -euo pipefail
set -x

BIN=./nosql0
SRC=nosql0.c
PASS=0
FAIL=0
TOTAL=0

# ── 顏色 ──────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

# ── 編譯 ──────────────────────────────────────────
echo -e "${BOLD}${CYAN}=== nosql0 測試套件 ===${NC}"
echo -e "${YELLOW}[BUILD]${NC} 編譯 $SRC ..."
gcc -O2 -Wall -o "$BIN" "$SRC"
echo -e "${GREEN}[BUILD OK]${NC}"
echo ""

# ── 輔助函數 ──────────────────────────────────────
run_cmd() {
    # 送指令到 nosql0，回傳輸出
    printf '%s\nQUIT\n' "$1" | "$BIN" 2>&1
}

assert_contains() {
    local desc="$1" input="$2" pattern="$3"
    TOTAL=$((TOTAL+1))
    local out
    out=$(printf '%s\nQUIT\n' "$input" | "$BIN" 2>&1)
    if echo "$out" | grep -q "$pattern"; then
        echo -e "  ${GREEN}✓${NC} $desc"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}✗${NC} $desc"
        echo -e "    ${YELLOW}期望包含:${NC} $pattern"
        echo -e "    ${YELLOW}實際輸出:${NC}"
        echo "$out" | sed 's/^/      /'
        FAIL=$((FAIL+1))
    fi
}

assert_not_contains() {
    local desc="$1" input="$2" pattern="$3"
    TOTAL=$((TOTAL+1))
    local out
    out=$(printf '%s\nQUIT\n' "$input" | "$BIN" 2>&1)
    if ! echo "$out" | grep -q "$pattern"; then
        echo -e "  ${GREEN}✓${NC} $desc"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}✗${NC} $desc"
        echo -e "    ${YELLOW}不應包含:${NC} $pattern"
        FAIL=$((FAIL+1))
    fi
}

multi_assert() {
    # 多指令連續執行，最後檢查輸出
    local desc="$1" cmds="$2" pattern="$3"
    TOTAL=$((TOTAL+1))
    local out
    out=$(printf '%s\nQUIT\n' "$cmds" | "$BIN" 2>&1)
    if echo "$out" | grep -q "$pattern"; then
        echo -e "  ${GREEN}✓${NC} $desc"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}✗${NC} $desc"
        echo -e "    ${YELLOW}期望包含:${NC} $pattern"
        echo -e "    ${YELLOW}實際輸出:${NC}"
        echo "$out" | sed 's/^/      /'
        FAIL=$((FAIL+1))
    fi
}

# ── 測試群組 1：資料表管理 ────────────────────────
echo -e "${BOLD}[1] 資料表管理${NC}"

assert_contains "CREATETABLE 成功" \
    "CREATETABLE users" \
    "建立資料表 'users'"

assert_contains "重複 CREATETABLE 顯示警告" \
    "CREATETABLE t1
CREATETABLE t1" \
    "已存在"

assert_contains "LISTTABLES 顯示已建立的表" \
    "CREATETABLE t1
CREATETABLE t2
LISTTABLES" \
    "t1"

assert_contains "DROPTABLE 成功" \
    "CREATETABLE drop_me
DROPTABLE drop_me" \
    "刪除資料表 'drop_me'"

assert_contains "DROPTABLE 不存在的表顯示錯誤" \
    "DROPTABLE no_such_table" \
    "ERR"

# ── 測試群組 2：PUT / GET ─────────────────────────
echo ""
echo -e "${BOLD}[2] PUT / GET 基本操作${NC}"

multi_assert "PUT 並 GET 取回相同值" \
    "CREATETABLE users
PUT users alice info:name Alice
GET users alice info:name" \
    "Alice"

multi_assert "GET 不存在的 row" \
    "CREATETABLE users
GET users nobody info:name" \
    "無此 row"

multi_assert "GET 不存在的欄位" \
    "CREATETABLE users
PUT users bob info:name Bob
GET users bob info:email" \
    "無此欄位"

multi_assert "PUT 多個欄位" \
    "CREATETABLE users
PUT users carol info:name Carol
PUT users carol info:age 30
PUT users carol contact:email carol@example.com
DUMP users" \
    "carol@example.com"

multi_assert "欄位格式錯誤顯示錯誤" \
    "CREATETABLE users
PUT users alice badformat value" \
    "ERR"

# ── 測試群組 3：多版本 ────────────────────────────
echo ""
echo -e "${BOLD}[3] 多版本 (Multi-version) 支援${NC}"

multi_assert "GET 預設取最新版本" \
    "CREATETABLE t
PUT t row1 d:v v1
PUT t row1 d:v v2
GET t row1 d:v" \
    "v2"

multi_assert "GET 2 個版本可看到歷史" \
    "CREATETABLE t
PUT t row1 d:v v1
PUT t row1 d:v v2
GET t row1 d:v 2" \
    "v1"

multi_assert "GET 2 個版本最新在 v0" \
    "CREATETABLE t
PUT t row1 d:v first
PUT t row1 d:v second
GET t row1 d:v 2" \
    "second"

multi_assert "DUMP 顯示所有版本" \
    "CREATETABLE t
PUT t r d:x alpha
PUT t r d:x beta
PUT t r d:x gamma
DUMP t" \
    "3 versions"

# ── 測試群組 4：SCAN ──────────────────────────────
echo ""
echo -e "${BOLD}[4] SCAN 前綴掃描${NC}"

multi_assert "SCAN 以前綴找多個 row" \
    "CREATETABLE logs
PUT logs 2024-01-01 msg:text 'New Year'
PUT logs 2024-01-02 msg:text 'Day2'
PUT logs 2024-02-01 msg:text 'Feb'
PUT logs 2023-12-31 msg:text 'Old'
SCAN logs 2024-01" \
    "2024-01-01"

multi_assert "SCAN 前綴只匹配指定前綴" \
    "CREATETABLE logs
PUT logs user:alice d:x 1
PUT logs user:bob d:x 2
PUT logs admin:root d:x 3
SCAN logs user:" \
    "user:alice"

multi_assert "SCAN 前綴不含其他 row" \
    "CREATETABLE logs
PUT logs user:alice d:x 1
PUT logs admin:root d:x 3
SCAN logs user:" \
    "user:alice"

multi_assert "SCAN 不存在前綴" \
    "CREATETABLE logs
PUT logs abc d:x 1
SCAN logs xyz" \
    "無符合"

# ── 測試群組 5：DEL ───────────────────────────────
echo ""
echo -e "${BOLD}[5] DEL 刪除欄位${NC}"

multi_assert "DEL 後 GET 顯示無此欄位" \
    "CREATETABLE t
PUT t r d:v hello
DEL t r d:v
GET t r d:v" \
    "無此欄位"

multi_assert "DEL 只刪一個欄位，其他保留" \
    "CREATETABLE t
PUT t r d:a keep
PUT t r d:b delete_me
DEL t r d:b
GET t r d:a" \
    "keep"

multi_assert "DEL 不存在欄位顯示錯誤" \
    "CREATETABLE t
PUT t r d:a val
DEL t r d:no_such" \
    "ERR"

# ── 測試群組 6：排序 (SSTable 概念) ─────────────────
echo ""
echo -e "${BOLD}[6] Row Key 排序 (SSTable)${NC}"

multi_assert "SCAN 結果按字典序排列" \
    "CREATETABLE t
PUT t c d:x 3
PUT t a d:x 1
PUT t b d:x 2
SCAN t row_prefix_none
SCAN t a" \
    "ROW: a"

# ── 測試群組 7：持久化 SAVE / LOAD ────────────────
echo ""
echo -e "${BOLD}[7] 持久化 SAVE / LOAD${NC}"

# 先刪舊的 db 檔
rm -f persist_test.db

multi_assert "SAVE 產生 .db 檔案" \
    "CREATETABLE persist_test
PUT persist_test key1 info:name TestUser
SAVE persist_test" \
    "儲存至 persist_test.db"

# 確認檔案存在
TOTAL=$((TOTAL+1))
if [ -f persist_test.db ]; then
    echo -e "  ${GREEN}✓${NC} persist_test.db 已建立"
    PASS=$((PASS+1))
else
    echo -e "  ${RED}✗${NC} persist_test.db 不存在"
    FAIL=$((FAIL+1))
fi

multi_assert "LOAD 後可 GET 到儲存的資料" \
    "LOAD persist_test
GET persist_test key1 info:name" \
    "TestUser"

multi_assert "LOAD 後 LISTTABLES 顯示該表" \
    "LOAD persist_test
LISTTABLES" \
    "persist_test"

# ── 測試群組 8：帶空格的值（引號） ─────────────────
echo ""
echo -e "${BOLD}[8] 帶引號的值${NC}"

multi_assert "帶引號的值可正確儲存與取回" \
    'CREATETABLE t
PUT t r d:v "Hello World"
GET t r d:v' \
    "Hello World"

# ── 測試群組 9：大量資料 ─────────────────────────
echo ""
echo -e "${BOLD}[9] 大量資料壓力測試${NC}"

# 產生 100 個 PUT 指令
{
    echo "CREATETABLE stress"
    for i in $(seq 1 100); do
        printf "PUT stress row%04d info:idx %d\n" "$i" "$i"
    done
    echo "SCAN stress row"
} | "$BIN" > /tmp/stress_out.txt 2>&1

TOTAL=$((TOTAL+1))
cnt=$(grep -c "ROW:" /tmp/stress_out.txt || true)
if [ "$cnt" -eq 100 ]; then
    echo -e "  ${GREEN}✓${NC} 100 筆 row 全部寫入並 SCAN 到 (共 $cnt 筆)"
    PASS=$((PASS+1))
else
    echo -e "  ${RED}✗${NC} 預期 100 筆，實際 SCAN 到 $cnt 筆"
    FAIL=$((FAIL+1))
fi

# ── 清理 ─────────────────────────────────────────
rm -f persist_test.db /tmp/stress_out.txt

# ── 總結 ─────────────────────────────────────────
echo ""
echo -e "${BOLD}${CYAN}=== 測試結果 ===${NC}"
echo -e "  總計: ${BOLD}$TOTAL${NC}  通過: ${GREEN}${BOLD}$PASS${NC}  失敗: ${RED}${BOLD}$FAIL${NC}"
echo ""

if [ "$FAIL" -eq 0 ]; then
    echo -e "${GREEN}${BOLD}✓ 所有測試通過！${NC}"
    exit 0
else
    echo -e "${RED}${BOLD}✗ 有 $FAIL 個測試失敗${NC}"
    exit 1
fi
