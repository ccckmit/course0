#!/usr/bin/env bash
# test.sh - 測試 mail0c 程式
# 用法：bash test.sh

set -euo pipefail

MAIL0C="./mail0c"
OUTPUT="mailtoday.md"
PASS=0
FAIL=0

# ── 顏色 ──────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'
YELLOW='\033[1;33m'; CYAN='\033[0;36m'; NC='\033[0m'

# ── 工具函式 ──────────────────────────────────────
info()  { echo -e "${CYAN}▶ $*${NC}"; }
ok()    { echo -e "  ${GREEN}✓ PASS${NC} $*"; PASS=$((PASS + 1)); }
fail()  { echo -e "  ${RED}✗ FAIL${NC} $*"; FAIL=$((FAIL + 1)); }
header(){ echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; \
          echo -e "${YELLOW}  $*${NC}"; \
          echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"; }

# 檢查 OUTPUT 中是否含有字串
contains() {
    local pattern="$1" desc="$2"
    if grep -qF -- "$pattern" "$OUTPUT" 2>/dev/null; then
        ok "$desc"
    else
        fail "$desc  (找不到: $(printf '%s' "$pattern" | head -c 60))"
    fi
}

not_contains() {
    local pattern="$1" desc="$2"
    if ! grep -qF -- "$pattern" "$OUTPUT" 2>/dev/null; then
        ok "$desc"
    else
        fail "$desc  (不應出現: $(printf '%s' "$pattern" | head -c 60))"
    fi
}

# 送一封信，並測試退出碼
send_mail() {
    printf '%s' "$1" | "$MAIL0C"
}

# ── 前置作業 ──────────────────────────────────────
echo -e "\n${CYAN}╔══════════════════════════════════════════╗"
echo    "║       mail0c 自動測試腳本                ║"
echo -e "╚══════════════════════════════════════════╝${NC}"

# 確認 mail0c 已編譯
if [[ ! -x "$MAIL0C" ]]; then
    info "找不到 $MAIL0C ，嘗試編譯..."
    gcc -Wall -o mail0c mail0c.c 2>&1
fi

# 每次測試前清空輸出檔
rm -f "$OUTPUT"

# ══════════════════════════════════════════════════
header "TEST 1：純文字信件"
# ══════════════════════════════════════════════════
info "送出純文字信件"
send_mail "From: Alice <alice@example.com>
To: Bob <bob@example.com>
Subject: Hello from Alice
Date: Mon, 27 Apr 2026 10:30:00 +0800
Content-Type: text/plain; charset=UTF-8

Hi Bob,

This is a plain text email.
Have a great day!

-- Alice" 2>/dev/null

contains "## time: 2026-04-27 10:30:00"   "Date header 解析正確"
contains "from: Alice <alice@example.com>" "From 欄位正確"
contains "title: Hello from Alice"         "Subject 欄位正確"
contains "body:"                           "body 標記存在"
contains "Hi Bob,"                         "body 文字第一行"
contains "Have a great day!"              "body 文字第二行"
contains "-- Alice"                        "body 簽名行"
not_contains "attach:"                     "純文字無 attach 行"

# ══════════════════════════════════════════════════
header "TEST 2：multipart/mixed 含附件"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
info "送出 multipart 含 2 個附件的信件"
send_mail "From: Carol <carol@example.com>
To: Bob <bob@example.com>
Subject: Project files attached
Date: Mon, 27 Apr 2026 14:00:00 +0800
MIME-Version: 1.0
Content-Type: multipart/mixed; boundary=\"==BOUND42==\"

--==BOUND42==
Content-Type: text/plain; charset=UTF-8

Hi Bob,

Please find the project files attached.

Regards, Carol

--==BOUND42==
Content-Type: application/pdf; name=\"report.pdf\"
Content-Disposition: attachment; filename=\"report.pdf\"
Content-Transfer-Encoding: base64

JVBERi0xLjQKdGVzdA==

--==BOUND42==
Content-Type: image/png; name=\"screenshot.png\"
Content-Disposition: attachment; filename=\"screenshot.png\"
Content-Transfer-Encoding: base64

iVBORw0KGgo=

--==BOUND42==--" 2>/dev/null

contains "from: Carol <carol@example.com>"              "From 欄位正確"
contains "title: Project files attached"               "Subject 欄位正確"
contains "attach: report.pdf, screenshot.png"          "兩個附件檔名正確"
contains "Hi Bob,"                                     "multipart body 文字"
contains "Please find the project files attached."     "multipart body 完整"
not_contains "JVBERi0x"                                "base64 附件內容不出現在 md"

# ══════════════════════════════════════════════════
header "TEST 3：無 Date header → 改用本機時間"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
info "送出無 Date 的信件"
send_mail "From: System <noreply@system.local>
Subject: Automated alert
Content-Type: text/plain

ALERT: disk usage above 90%
Action required immediately." 2>/dev/null

# 本機時間：只檢查 ## time: 行存在且含有年份
if grep -qE "^## time: 20[0-9]{2}-" "$OUTPUT"; then
    ok "無 Date header 時以本機時間填充"
else
    fail "無 Date header 時本機時間格式錯誤"
fi
contains "from: System <noreply@system.local>" "From 欄位正確"
contains "title: Automated alert"              "Subject 欄位正確"
contains "ALERT: disk usage above 90%"         "body 第一行"
contains "Action required immediately."        "body 第二行"

# ══════════════════════════════════════════════════
header "TEST 4：無 Subject / 無 From"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
info "送出缺少 Subject 與 From 的最簡信件"
send_mail "Date: Mon, 27 Apr 2026 09:00:00 +0000

Just a body, nothing else." 2>/dev/null

contains "(unknown)"     "缺 From 時顯示 (unknown)"
contains "(no subject)"  "缺 Subject 時顯示 (no subject)"
contains "Just a body"   "body 仍正確輸出"

# ══════════════════════════════════════════════════
header "TEST 5：多封信累積到同一個 mailtoday.md"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
info "連續送出 3 封信，確認全部累積"

for i in 1 2 3; do
    send_mail "From: user${i}@example.com
Subject: Mail number ${i}
Date: Mon, 27 Apr 2026 0${i}:00:00 +0800

Body of mail ${i}." 2>/dev/null
done

COUNT=$(grep -c "^## time:" "$OUTPUT")
if [[ "$COUNT" -eq 3 ]]; then
    ok "3 封信都寫入（共 ${COUNT} 個 ## time: 區塊）"
else
    fail "預期 3 個 ## time: 區塊，實際找到 ${COUNT}"
fi

contains "from: user1@example.com" "第 1 封 From"
contains "from: user2@example.com" "第 2 封 From"
contains "from: user3@example.com" "第 3 封 From"
contains "Body of mail 1."         "第 1 封 body"
contains "Body of mail 2."         "第 2 封 body"
contains "Body of mail 3."         "第 3 封 body"

# ══════════════════════════════════════════════════
header "TEST 6：Header folding（跨行 header）"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
info "送出含 folded Subject 的信件"
send_mail "From: fold@example.com
Subject: This is a very long subject
 that wraps onto the next line
Date: Mon, 27 Apr 2026 08:00:00 +0800

Folded header test." 2>/dev/null

contains "This is a very long subject that wraps onto the next line" \
         "折行 Subject 正確展開"

# ══════════════════════════════════════════════════
header "TEST 7：\$MAIL0C 退出碼為 0"
# ══════════════════════════════════════════════════
rm -f "$OUTPUT"
echo "From: exitcode@test.com
Subject: Exit test

body." | "$MAIL0C" 2>/dev/null
EXIT=$?
if [[ $EXIT -eq 0 ]]; then
    ok "程式正常結束（exit 0）"
else
    fail "程式異常結束（exit ${EXIT}）"
fi

# ══════════════════════════════════════════════════
# 顯示最後一次 mailtoday.md 內容（供目視確認）
# ══════════════════════════════════════════════════
echo -e "\n${CYAN}── 最後一次 mailtoday.md 內容 ──${NC}"
cat "$OUTPUT"

# ══════════════════════════════════════════════════
# 總結
# ══════════════════════════════════════════════════
TOTAL=$((PASS + FAIL))
echo -e "\n${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "結果：${GREEN}${PASS} passed${NC}  /  ${RED}${FAIL} failed${NC}  /  ${TOTAL} total"
echo -e "${YELLOW}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"

[[ $FAIL -eq 0 ]]   # 全部通過則 exit 0，否則 exit 1