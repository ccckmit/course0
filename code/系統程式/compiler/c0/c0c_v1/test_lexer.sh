#!/usr/bin/env bash
# test_lexer.sh – Stage 2: verify lexer can lex itself without errors
set -euo pipefail

SRCDIR="$(cd "$(dirname "$0")/src" && pwd)"
TESTDIR="$(cd "$(dirname "$0")/tests" && pwd)"
BUILD="$TESTDIR/build"
mkdir -p "$BUILD"

echo "=== Stage 2: Lexer self-test ==="

# ── Compile the lexer test binary ────────────────────────────────────────────
echo "[1] Compiling lexer_test..."
cc -std=c11 -Wall -Wextra -Wpedantic -Werror \
   -fsanitize=address,undefined \
   -DLEXER_STANDALONE \
   -g \
   -o "$BUILD/lexer_test" \
   "$SRCDIR/lexer.c"
echo "    OK: compiled without warnings"

# ── Lex lexer.c itself ────────────────────────────────────────────────────────
echo "[2] Lexing lexer.c (self-lex)..."
if "$BUILD/lexer_test" "$SRCDIR/lexer.c" > "$BUILD/lexer_self.tok" 2>"$BUILD/lexer_self.err"; then
    echo "    OK: $(wc -l < "$BUILD/lexer_self.tok") tokens"
else
    echo "    FAIL"
    cat "$BUILD/lexer_self.err"
    exit 1
fi

# ── Check: no UNKNOWN tokens ──────────────────────────────────────────────────
# Token output format: file:line:col<TAB>KIND<TAB>|text|
# We check column 2 (KIND field) equals UNKNOWN
if awk -F'\t' '$2 ~ /^UNKNOWN/' "$BUILD/lexer_self.tok" | grep -q .; then
    echo "    FAIL: UNKNOWN tokens found:"
    awk -F'\t' '$2 ~ /^UNKNOWN/' "$BUILD/lexer_self.tok"
    exit 1
fi
echo "    OK: no UNKNOWN tokens"

# ── Check: token counts are sane (lexer.c > 500 tokens) ─────────────────────
COUNT=$(grep -c "." "$BUILD/lexer_self.tok" || true)
if [ "$COUNT" -lt 500 ]; then
    echo "    FAIL: only $COUNT tokens – something went wrong"
    exit 1
fi
echo "    OK: $COUNT tokens (>= 500)"

# ── Lex key tokens spot-check ────────────────────────────────────────────────
echo "[3] Spot-checking token kinds..."
check_present() {
    local kind="$1"
    if grep -q "$kind" "$BUILD/lexer_self.tok"; then
        echo "    OK: found $kind"
    else
        echo "    FAIL: missing $kind"
        exit 1
    fi
}
check_present "int"
check_present "char"
check_present "struct"
check_present "return"
check_present "IDENT"
check_present "INT_LIT"
check_present "STR_LIT"
check_present "{"
check_present "}"

# ── Determinism: lex twice, compare ──────────────────────────────────────────
echo "[4] Determinism check (lex twice)..."
"$BUILD/lexer_test" "$SRCDIR/lexer.c" > "$BUILD/lexer_self2.tok" 2>/dev/null
if diff -q "$BUILD/lexer_self.tok" "$BUILD/lexer_self2.tok" > /dev/null; then
    echo "    OK: output is deterministic"
else
    echo "    FAIL: two runs produced different output"
    diff "$BUILD/lexer_self.tok" "$BUILD/lexer_self2.tok" | head -20
    exit 1
fi

echo ""
echo "=== Stage 2 PASSED ==="
