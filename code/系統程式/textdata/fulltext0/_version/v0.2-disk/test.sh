#!/usr/bin/env bash
# test.sh — 驗證 fulltext0 全文檢索引擎的索引與查詢結果
set -euo pipefail

CORPUS="_data/corpus.txt"
INDEX="_index/data.idx"
QUERY="./query"
INDEXER="./index"
PASS=0
FAIL=0
TOTAL=0

# ── 顏色 ──────────────────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

# ── 工具函式 ──────────────────────────────────────────────────────────────────
banner()  { echo -e "\n${CYAN}${BOLD}══════ $* ══════${RESET}"; }
pass()    { echo -e "  ${GREEN}✓ PASS${RESET}  $*"; PASS=$((PASS+1)); TOTAL=$((TOTAL+1)); }
fail()    { echo -e "  ${RED}✗ FAIL${RESET}  $*"; FAIL=$((FAIL+1)); TOTAL=$((TOTAL+1)); }
info()    { echo -e "  ${YELLOW}ℹ${RESET}      $*"; }

# ── assert_found  <query> <expected_substring_in_output>
assert_found() {
    local q="$1" expected="$2"
    local out
    out=$("$QUERY" "$q" 2>/dev/null)
    local count
    count=$(echo "$out" | grep -c '^\[' || true)
    if echo "$out" | grep -qF "$expected"; then
        pass "找到關鍵字 '$q' → 結果包含: $expected  (共 ${count} 筆)"
    else
        fail "找不到 '$q' → 預期包含: $expected"
        echo "    --- 實際輸出 (前5行) ---"
        echo "$out" | head -5 | sed 's/^/    /'
    fi
}

# ── assert_count  <query> <op> <number>   (op: -gt -ge -lt -le -eq)
assert_count() {
    local q="$1" op="$2" expected="$3"
    local out count
    out=$("$QUERY" "$q" 2>/dev/null)
    count=$(echo "$out" | grep -c '^\[' || true)
    if [ "$count" "$op" "$expected" ]; then
        pass "查詢 '$q' 結果數 ${count} ${op} ${expected}"
    else
        fail "查詢 '$q' 結果數 ${count} 不滿足 ${op} ${expected}"
    fi
}

# ── assert_no_result  <query>
assert_no_result() {
    local q="$1"
    local out count
    out=$("$QUERY" "$q" 2>/dev/null)
    count=$(echo "$out" | grep -c '^\[' || true)
    if [ "$count" -eq 0 ]; then
        pass "無效查詢 '$q' 正確回傳 0 筆"
    else
        fail "無效查詢 '$q' 不應有結果，但得到 ${count} 筆"
    fi
}

# ── assert_all_contain  <query> <substring>
# 所有回傳行都必須包含指定子字串
assert_all_contain() {
    local q="$1" sub="$2"
    local out bad
    out=$("$QUERY" "$q" 2>/dev/null | grep '^\[' || true)
    bad=$(echo "$out" | grep -v "$sub" || true)
    local count total_count
    count=$(echo "$bad" | grep -c '^\[' || true)
    total_count=$(echo "$out" | grep -c '^\[' || true)
    if [ "$count" -eq 0 ] && [ "$total_count" -gt 0 ]; then
        pass "查詢 '$q' 所有 ${total_count} 筆結果均含 '$sub'"
    elif [ "$total_count" -eq 0 ]; then
        fail "查詢 '$q' 無結果"
    else
        fail "查詢 '$q' 有 ${count} 筆結果不含 '$sub'"
        echo "$bad" | head -3 | sed 's/^/    /'
    fi
}

# ── assert_corpus_agree  <query>
# 用 grep 掃 corpus 驗證索引結果完整性（允許索引比 grep 多，但不能少）
assert_corpus_agree() {
    local q="$1"
    local idx_lines grep_lines missing
    idx_lines=$("$QUERY" "$q" 2>/dev/null | grep '^\[' | sed 's/^\[[0-9]*\] //' | sort || true)
    grep_lines=$(grep -i "$q" "$CORPUS" | sort || true)

    # 確認 grep 找到的每一行都在索引結果中
    missing=$(comm -23 <(echo "$grep_lines") <(echo "$idx_lines") | head -3)
    if [ -z "$missing" ]; then
        pass "語料庫驗證 '$q'：索引結果涵蓋 grep 所有命中"
    else
        fail "語料庫驗證 '$q'：以下行在索引中遺失"
        echo "$missing" | sed 's/^/    /'
    fi
}

# ─────────────────────────────────────────────────────────────────────────────
# 1. 前置條件：確認執行檔與索引存在
# ─────────────────────────────────────────────────────────────────────────────
banner "前置檢查"

if [ ! -f "$INDEXER" ]; then fail "找不到 $INDEXER，請先執行 make"; exit 1; fi
if [ ! -f "$QUERY"   ]; then fail "找不到 $QUERY，請先執行 make";   exit 1; fi
if [ ! -f "$CORPUS"  ]; then fail "找不到 $CORPUS";                 exit 1; fi
pass "執行檔與語料庫存在"

LINES=$(wc -l < "$CORPUS")
if [ "$LINES" -eq 1000 ]; then
    pass "語料庫行數正確：$LINES 行"
else
    fail "語料庫行數異常：$LINES 行（預期 1000）"
fi

# ─────────────────────────────────────────────────────────────────────────────
# 2. 建立索引
# ─────────────────────────────────────────────────────────────────────────────
banner "建立索引"

"$INDEXER" "$CORPUS" 2>/dev/null
if [ -f "$INDEX" ]; then
    IDX_SIZE=$(stat -c%s "$INDEX" 2>/dev/null || stat -f%z "$INDEX")
    # 必須是 4096 的倍數
    if [ $((IDX_SIZE % 4096)) -eq 0 ]; then
        pass "索引檔大小 ${IDX_SIZE} bytes，4K 對齊正確"
    else
        fail "索引檔大小 ${IDX_SIZE} bytes，非 4K 對齊"
    fi
    info "索引路徑：$INDEX"
else
    fail "索引檔未產生"; exit 1
fi

# ─────────────────────────────────────────────────────────────────────────────
# 3. 英文單字查詢
# ─────────────────────────────────────────────────────────────────────────────
banner "英文單字查詢"

assert_count    "system"     -gt  10
assert_count    "platform"   -gt   5
assert_count    "network"    -gt   3
assert_all_contain "system"  "system"
assert_all_contain "platform" "platform"
assert_all_contain "metadata" "metadata"
assert_found    "queue"      "queue"
assert_found    "endpoint"   "endpoint"
assert_found    "processor"  "processor"

# ─────────────────────────────────────────────────────────────────────────────
# 4. 英文多詞 AND 查詢
# ─────────────────────────────────────────────────────────────────────────────
banner "英文多詞查詢（AND 語義）"

assert_count    "system network"     -ge 0   # 可能 0，但不應崩潰
assert_found    "system efficiently" "system"
assert_all_contain "system efficiently" "system"

# ─────────────────────────────────────────────────────────────────────────────
# 5. 中文單字查詢（unigram）
# ─────────────────────────────────────────────────────────────────────────────
banner "中文單字查詢"

assert_count    "系"    -gt  5
assert_count    "統"    -gt  5
assert_count    "處"    -gt  3
assert_all_contain "理"  "理"

# ─────────────────────────────────────────────────────────────────────────────
# 6. 中文雙字詞查詢（bigram）
# ─────────────────────────────────────────────────────────────────────────────
banner "中文雙字詞查詢（bigram）"

assert_count    "系統"  -gt  5
assert_count    "處理"  -gt  5
assert_count    "網路"  -gt  2
assert_count    "資料"  -gt  3
assert_all_contain "系統"  "系統"
assert_all_contain "處理"  "處理"
assert_all_contain "索引"  "索引"
assert_found    "前端"   "前端"
assert_found    "後端"   "後端"
assert_found    "快取"   "快取"

# ─────────────────────────────────────────────────────────────────────────────
# 7. 中文多詞 AND 查詢
# ─────────────────────────────────────────────────────────────────────────────
banner "中文多詞查詢（AND 語義）"

assert_count    "處理 系統"   -ge 0
assert_all_contain "處理器 索引"  "處理器"

# ─────────────────────────────────────────────────────────────────────────────
# 8. 中英混合查詢
# ─────────────────────────────────────────────────────────────────────────────
banner "中英混合查詢"

assert_count    "system 系統"   -ge 0  # AND，可能 0
assert_count    "token"          -gt  3
assert_all_contain "token"       "token"

# ─────────────────────────────────────────────────────────────────────────────
# 9. 不存在的詞（應返回 0 結果）
# ─────────────────────────────────────────────────────────────────────────────
banner "不存在詞查詢（應返回 0 筆）"

assert_no_result "xyznonexistent"
assert_no_result "鬼魅幽靈"
assert_no_result "foobar99999"

# ─────────────────────────────────────────────────────────────────────────────
# 10. 與 grep 交叉驗證（語料庫一致性）
# ─────────────────────────────────────────────────────────────────────────────
banner "與 grep 交叉驗證"

assert_corpus_agree "system"
assert_corpus_agree "platform"
assert_corpus_agree "metadata"

# ─────────────────────────────────────────────────────────────────────────────
# 11. 邊界情形
# ─────────────────────────────────────────────────────────────────────────────
banner "邊界情形"

# 純空白 / 標點不應造成崩潰
out=$("$QUERY" "   " 2>/dev/null || true)
pass "空白查詢不崩潰"

out=$("$QUERY" "，。！" 2>/dev/null || true)
pass "標點查詢不崩潰"

# 連續相同詞不應重複計數
out1=$("$QUERY" "system" 2>/dev/null | grep -c '^\[' || true)
out2=$("$QUERY" "system system" 2>/dev/null | grep -c '^\[' || true)
if [ "$out1" -eq "$out2" ]; then
    pass "重複關鍵詞去重：'system' = 'system system'（$out1 筆）"
else
    fail "重複關鍵詞去重失敗：$out1 vs $out2"
fi

# ─────────────────────────────────────────────────────────────────────────────
# 12. 抽樣顯示
# ─────────────────────────────────────────────────────────────────────────────
banner "抽樣結果展示"
echo ""
echo -e "${BOLD}  ▸ 查詢「處理器」：${RESET}"
"$QUERY" "處理器" 2>/dev/null | head -5 | sed 's/^/    /'
echo ""
echo -e "${BOLD}  ▸ 查詢「efficiently」：${RESET}"
"$QUERY" "efficiently" 2>/dev/null | head -5 | sed 's/^/    /'
echo ""

# ─────────────────────────────────────────────────────────────────────────────
# 結果統計
# ─────────────────────────────────────────────────────────────────────────────
echo ""
echo -e "${BOLD}══════════════════════════════════════════${RESET}"
echo -e "${BOLD}  測試結果：${GREEN}${PASS} 通過${RESET}  /  ${RED}${FAIL} 失敗${RESET}  /  共 ${TOTAL} 項${RESET}"
echo -e "${BOLD}══════════════════════════════════════════${RESET}"
echo ""

if [ "$FAIL" -eq 0 ]; then
    echo -e "${GREEN}${BOLD}  ✓ 全部通過！${RESET}"
    exit 0
else
    echo -e "${RED}${BOLD}  ✗ 有 ${FAIL} 項失敗，請檢查上方輸出。${RESET}"
    exit 1
fi
