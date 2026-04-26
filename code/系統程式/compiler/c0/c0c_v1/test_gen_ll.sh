#!/usr/bin/env bash
# test_gen_ll.sh – Stage 4: c0c generates LLVM IR that executes correctly
#
# Strategy: functional equivalence
#   1. c0c compiles <file.c> → <file_c0c.ll>
#   2. lli executes the .ll and records exit code
#   3. clang compiles and runs <file.c> natively, records exit code
#   4. PASS if both exit codes match
#
# We do NOT require byte-identical IR with clang (register names differ,
# label names differ, metadata differs). Only execution results must match.

set -euo pipefail

SRCDIR="$(cd "$(dirname "$0")/src"  && pwd)"
CASEDIR="$(cd "$(dirname "$0")/tests/cases" && pwd)"
BUILD="$(cd "$(dirname "$0")" && pwd)/tests/build"
mkdir -p "$BUILD/ll"

# ── Detect tools ──────────────────────────────────────────────────────────────
LLI=""
for candidate in lli lli-18 lli-17 lli-16 lli-15; do
    if command -v "$candidate" >/dev/null 2>&1; then
        LLI="$candidate"; break
    fi
done
if [ -z "$LLI" ]; then
    echo "ERROR: no lli found (install llvm-18 or similar)"
    exit 1
fi

if ! command -v clang >/dev/null 2>&1; then
    echo "ERROR: clang not found"
    exit 1
fi

TARGET="$(cd "$(dirname "$0")" && ./detect_target.sh)"
if [ -z "$TARGET" ]; then
    echo "ERROR: could not detect target"
    exit 1
fi

echo "=== Stage 4: LLVM IR code generation test ==="
echo "    lli:   $LLI"
echo "    clang: $(clang --version | head -1)"
echo "    target: $TARGET"
echo ""

# ── Step 0: build c0c ─────────────────────────────────────────────────────────
echo "[0] Building c0c..."
cc -std=c11 -Wall -Wextra -Wpedantic -Werror \
   -fsanitize=address,undefined -g \
   -o "$BUILD/c0c" \
   "$SRCDIR/lexer.c" "$SRCDIR/ast.c" "$SRCDIR/parser.c" \
   "$SRCDIR/codegen_ll.c" "$SRCDIR/c0c.c"
echo "    OK: c0c compiled"
echo ""

# ── Step 1: also verify Stage 3 still passes ─────────────────────────────────
echo "[1] Stage 3 regression..."
bash "$(dirname "$0")/test_gen_c.sh" > /dev/null 2>&1 \
    && echo "    OK: Stage 3 still passes" \
    || { echo "    FAIL: Stage 3 regressed"; exit 1; }
echo ""

# ── Step 2: compile and run each test case ────────────────────────────────────
echo "[2] Running test cases..."
PASS=0; FAIL=0; ERRORS=""

for src in "$CASEDIR"/t*.c; do
    base=$(basename "$src" .c)
    ll_file="$BUILD/ll/${base}_c0c.ll"
    ref_bin="$BUILD/ll/${base}_ref"

    # compile with c0c
    if ! "$BUILD/c0c" "$src" -o "$ll_file" -target "$TARGET" 2>"$BUILD/ll/${base}_c0c.err"; then
        ERRORS="$ERRORS\n  FAIL  $base  [c0c parse/codegen error]"
        FAIL=$((FAIL+1))
        continue
    fi

    # validate IR syntax (lli will catch this)
    lli_err=$("$LLI" "$ll_file" 2>&1 || true)
    if echo "$lli_err" | grep -q 'error:'; then
        ERRORS="$ERRORS\n  FAIL  $base  [IR invalid: $(echo "$lli_err" | grep 'error:' | head -1 | sed 's/.*error: //')]"
        FAIL=$((FAIL+1))
        continue
    fi

    # get c0c exit code (programs intentionally return non-zero, so disable errexit)
    set +e
    "$LLI" "$ll_file" 2>/dev/null; c0c_exit=$?

    # get clang reference exit code
    clang "$src" -o "$ref_bin" 2>/dev/null
    "$ref_bin" 2>/dev/null; clang_exit=$?
    set -e

    if [ "$c0c_exit" = "$clang_exit" ]; then
        printf "    PASS  %-25s (exit=%d)\n" "$base" "$c0c_exit"
        PASS=$((PASS+1))
    else
        ERRORS="$ERRORS\n  FAIL  $base  [c0c=$c0c_exit clang=$clang_exit]"
        FAIL=$((FAIL+1))
    fi
done

echo ""
if [ -n "$ERRORS" ]; then
    echo "Failures:"
    printf "%b\n" "$ERRORS"
    echo ""
fi

echo "Results: $PASS passed, $FAIL failed out of $((PASS+FAIL)) tests"
echo ""

# ── Step 3: spot-check IR structure ──────────────────────────────────────────
echo "[3] IR structure checks..."

check_ir() {
    local file="$1"
    local pattern="$2"
    local desc="$3"
    if grep -q "$pattern" "$file" 2>/dev/null; then
        echo "    OK: $desc"
    else
        echo "    WARN: $desc not found"
    fi
}

check_ir "$BUILD/ll/t01_return_c0c.ll"     "alloca i32"           "t01: has alloca"
check_ir "$BUILD/ll/t01_return_c0c.ll"     "ret i32"              "t01: has ret i32"
check_ir "$BUILD/ll/t04_while_c0c.ll"      "while.cond"           "t04: while label"
check_ir "$BUILD/ll/t04_while_c0c.ll"      "icmp"                 "t04: has icmp"
check_ir "$BUILD/ll/t06_func_c0c.ll"       "@add"                 "t06: add function"
check_ir "$BUILD/ll/t06_func_c0c.ll"       "call i32 @add"        "t06: call to add"
check_ir "$BUILD/ll/t09_struct_c0c.ll"     "%struct.Point"        "t09: struct type"
check_ir "$BUILD/ll/t09_struct_c0c.ll"     "getelementptr inbounds" "t09: GEP"
check_ir "$BUILD/ll/t12_linked_list_c0c.ll" "%struct.Node"        "t12: Node struct"
check_ir "$BUILD/ll/t12_linked_list_c0c.ll" "icmp ne ptr"         "t12: ptr comparison"

echo ""
if [ "$FAIL" -eq 0 ]; then
    echo "=== Stage 4 PASSED ==="
else
    echo "=== Stage 4 FAILED ($FAIL failures) ==="
    exit 1
fi
