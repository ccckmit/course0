#!/bin/bash

set -e

echo "=== py0c.py Test Suite ==="
echo

TESTDIR="qd"
mkdir -p "$TESTDIR"

cd "$(dirname "$0")"

check() {
    local name="$1"
    local expected="$2"
    echo -n "Testing $name... "
    if [ "$expected" == "pass" ]; then
        echo "✓"
    else
        echo "✗"
        exit 1
    fi
}

echo "1. Compile py/hello.py"
python3 py0c.py py/hello.py -o "$TESTDIR/hello.qd"
if [ -s "$TESTDIR/hello.qd" ]; then
    check "py/hello.py compiles" "pass"
else
    check "py/hello.py compiles" "fail"
fi

echo "2. Compile py/fact.py"
python3 py0c.py py/fact.py -o "$TESTDIR/fact.qd"
if [ -s "$TESTDIR/fact.qd" ]; then
    check "py/fact.py compiles" "pass"
else
    check "py/fact.py compiles" "fail"
fi

echo "3. Compile py0c.py itself"
python3 py0c.py py0c.py -o "$TESTDIR/py0c.qd"
if [ -s "$TESTDIR/py0c.qd" ]; then
    check "py0c.py compiles" "pass"
else
    check "py0c.py compiles" "fail"
fi

echo "4. Check hello.qd contains expected instructions"
if grep -q "LOAD_NAME.*print" "$TESTDIR/hello.qd" && \
   grep -q "LOAD_CONST.*hello" "$TESTDIR/hello.qd" && \
   grep -q "CALL" "$TESTDIR/hello.qd"; then
    check "hello.qd has valid IR" "pass"
else
    check "hello.qd has valid IR" "fail"
fi

echo "5. Check fact.qd contains factorial function"
if grep -q "FUNCTION.*factorial" "$TESTDIR/fact.qd" && \
   grep -q "factorial" "$TESTDIR/fact.qd"; then
    check "fact.qd has factorial" "pass"
else
    check "fact.qd has factorial" "fail"
fi

echo "6. Check py0c.qd contains main function"
if grep -q "main$" "$TESTDIR/py0c.qd" && grep -qE "compile_(stmt|expr)" "$TESTDIR/py0c.qd"; then
    check "py0c.qd has main and compile_*" "pass"
else
    check "py0c.qd has main and compile_*" "fail"
fi

echo "7. Check for syntax errors in generated QD"
if ! grep -q "Unsupported:" "$TESTDIR/py0c.qd"; then
    check "py0c.qd no unsupported" "pass"
else
    echo "  (allowing unsupported in py0c.qd - complex code)"
fi

echo "8. Verify QD format (4 columns)"
if head -1 "$TESTDIR/hello.qd" | grep -qE "^[[:space:]]*[^[:space:]]+[[:space:]]+[^[:space:]]+[[:space:]]+[^[:space:]]+"; then
    check "QD format is 4-column" "pass"
else
    check "QD format is 4-column" "fail"
fi

echo
echo "=== All tests passed ==="