#!/usr/bin/env bash
set -u
set -o pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export DYLD_SHARED_REGION=avoid
export DYLD_SHARED_CACHE_DIR=/System/Library/dyld

usage() {
  echo "Usage:"
  echo "  $0 <file.c> [file2.c ...]"
  echo "  $0 --all"
  echo ""
  echo "Notes:"
  echo "  - Expected value is read from '// expected:' comment in the source."
}

C0C="$ROOT/c0/c0c/c0c"
LL0C="$ROOT/ll0/ll0c/ll0c"
RV0AS="$ROOT/rv0/rv0as"
RV0VM="$ROOT/rv0/rv0vm"

OUTDIR="$ROOT/_data/build"
mkdir -p "$OUTDIR"

echo "=========================================="
echo "Building toolchain..."
echo "=========================================="
(cd "$ROOT/c0/c0c" && make)
(cd "$ROOT/ll0/ll0c" && make ll0c)
(cd "$ROOT/rv0" && make rv0as rv0vm)

collect_tests() {
  local -a inputs=()
  if [[ $# -eq 0 ]]; then
    usage
    exit 1
  fi
  if [[ "$1" == "--all" ]]; then
    while IFS= read -r -d '' f; do
      inputs+=("$f")
    done < <(find "$ROOT/_data" -maxdepth 1 -name '*.c' -print0 | sort -z)
  else
    inputs=("$@")
  fi
  echo "${inputs[@]}"
}

get_expected() {
  local f="$1"
  local line
  line="$(head -n 5 "$f" | grep -m 1 -E "expected" || true)"
  if [[ -z "$line" ]]; then
    echo ""
    return
  fi
  echo "$line" | sed -E 's/.*expected:[^0-9-]*(-?[0-9]+).*/\1/'
}

run_one() {
  local src="$1"
  local skip_reason=""
  if [[ ! -f "$src" || "${src##*.}" != "c" ]]; then
    echo "SKIP: $src (not a .c file)"
    return 2
  fi

  if grep -q "sqrt(" "$src" || grep -q "<math.h>" "$src"; then
    skip_reason="requires math library (sqrt)"
  fi
  if [[ -n "$skip_reason" ]]; then
    echo "SKIP: $src ($skip_reason)"
    return 2
  fi

  local base out_ll out_s out_o
  base="$(basename "$src" .c)"
  out_ll="$OUTDIR/$base.ll"
  out_s="$OUTDIR/$base.s"
  out_o="$OUTDIR/$base.o"

  echo ""
  echo "------------------------------------------"
  echo "Test: $base"
  echo "Input: $src"
  echo "------------------------------------------"

  if ! "$C0C" "$src" -o "$out_ll"; then
    echo "FAIL: c0c"
    return 1
  fi
  local attempt
  for attempt in 1 2 3 4 5; do
    if env DYLD_SHARED_REGION=avoid DYLD_SHARED_CACHE_DIR=/System/Library/dyld "$LL0C" "$out_ll" -o "$out_s"; then
      break
    fi
    if [[ $attempt -eq 3 ]]; then
      (cd "$ROOT/ll0/ll0c" && make ll0c >/dev/null 2>&1) || true
    fi
    if [[ $attempt -eq 5 ]]; then
      echo "FAIL: ll0c"
      return 1
    fi
    sleep 0.2
  done
  if ! "$RV0AS" "$out_s" -o "$out_o"; then
    echo "FAIL: rv0as"
    return 1
  fi

  local vm_out result_line result_val expected
  vm_out="$("$RV0VM" "$out_o" 2>&1 || true)"
  echo "$vm_out"
  result_line="$(echo "$vm_out" | grep -m 1 'a0 =')"
  if [[ -n "$result_line" ]]; then
    result_val="$(echo "$result_line" | sed 's/.*a0 = *\([-0-9]*\).*/\1/')"
  else
    result_val=""
  fi
  expected="$(get_expected "$src")"

  if [[ -n "$expected" && -n "$result_val" ]]; then
    if [[ "$expected" == "$result_val" ]]; then
      echo "PASS: got $result_val"
      return 0
    else
      echo "FAIL: got $result_val, expected $expected"
      return 1
    fi
  elif [[ -n "$result_val" ]]; then
    echo "DONE: a0 = $result_val (no expected)"
    return 0
  else
    echo "FAIL: a0 not found"
    return 1
  fi
}

TESTS=($(collect_tests "$@"))

echo ""
echo "=========================================="
echo "Pipeline: .c => .ll => .s => .o => rv0vm"
echo "=========================================="

PASS=0
FAIL=0
SKIP=0

for src in "${TESTS[@]}"; do
  if run_one "$src"; then
    ((PASS++))
  else
    rc=$?
    if [[ $rc -eq 2 ]]; then
      ((SKIP++))
    else
      ((FAIL++))
    fi
  fi
done

echo ""
echo "=========================================="
echo "Summary"
echo "=========================================="
echo "PASSED: $PASS"
echo "FAILED: $FAIL"
echo "SKIPPED: $SKIP"

if [[ $FAIL -ne 0 ]]; then
  exit 1
fi
