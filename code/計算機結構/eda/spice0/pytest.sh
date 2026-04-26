#!/usr/bin/env bash
# test.sh — spice0.py 自動測試腳本（測試結束後產生綜合圖表）
set -uo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

PASS=0; FAIL=0

info() { echo -e "${CYAN}[INFO]${NC} $*"; }

assert_val() {
    local desc="$1" val="$2" expect="$3" tol="$4"
    local ok
    ok=$(awk -v a="$val" -v b="$expect" -v t="$tol" \
        'BEGIN{d=a-b;if(d<0)d=-d;print(d<=t)?"1":"0"}')
    if [[ "$ok" == "1" ]]; then
        echo -e "  ${GREEN}✓${NC} $desc  (got $val, expect $expect ±$tol)"
        ((PASS++))
    else
        echo -e "  ${RED}✗${NC} $desc  (got $val, expect $expect ±$tol)"
        ((FAIL++))
    fi
}

get_col() { echo "$1" | awk -v k="$2" -v c="$3" '$1==k{printf "%.9g",$c;exit}'; }

filesize() {
    local f="$1"
    if stat -f%z "$f" > /dev/null 2>&1; then
        stat -f%z "$f"   # macOS / BSD
    else
        stat -c%s "$f"   # Linux / GNU
    fi
}

echo -e "\n${BOLD}══════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  SPICE0 Python — Automated Test Suite${NC}"
echo -e "${BOLD}══════════════════════════════════════════════════${NC}\n"

info "Checking Python & dependencies..."
python3 -c "import numpy, matplotlib; print('  deps OK')"

# ── T1: Voltage Divider ───────────────────────────────
echo -e "\n${YELLOW}── Test 1: Voltage Divider (.op) ──────────────────${NC}"
out=$(python3 spice0.py ex1_divider.sp /dev/null 2>/dev/null); echo "$out"
assert_val "V(in)=10V"  "$(get_col "$out" 0 2)" 10     0.01
assert_val "V(mid)=5V"  "$(get_col "$out" 0 3)" 5      0.01
assert_val "I(V1)=-5mA" "$(get_col "$out" 0 4)" -0.005 0.001

# ── T2: RC Transient ─────────────────────────────────
echo -e "\n${YELLOW}── Test 2: RC Transient (.tran) ────────────────────${NC}"
out=$(python3 spice0.py ex2_rc.sp /dev/null 2>/dev/null)
val_1ms=$(echo "$out" | awk '$1+0>=0.001{printf "%.6g",$2;exit}')
assert_val "V(vc,t=1ms)≈3.16V" "$val_1ms" 3.161 0.25
val_5ms=$(echo "$out" | awk '$1+0>=0.005{printf "%.6g",$2;exit}')
assert_val "V(vc,t=5ms)≈5.00V" "$val_5ms" 5.0   0.10

# ── T3: DC Sweep ─────────────────────────────────────
echo -e "\n${YELLOW}── Test 3: DC Sweep (.dc) ──────────────────────────${NC}"
out=$(python3 spice0.py ex3_dc_sweep.sp /dev/null 2>/dev/null); echo "$out"
assert_val "V(mid)|V1=2V=1.0V"  "$(get_col "$out" 2 3)" 1.0   0.01
assert_val "V(mid)|V1=4V=2.0V"  "$(get_col "$out" 4 3)" 2.0   0.01
assert_val "I(V1) |V1=4V=-20mA" "$(get_col "$out" 4 4)" -0.02 0.001

# ── T4: RL Transient ─────────────────────────────────
echo -e "\n${YELLOW}── Test 4: RL Transient (.tran) ────────────────────${NC}"
out=$(python3 spice0.py ex4_rl.sp /dev/null 2>/dev/null)
val_tau=$(echo "$out" | awk '$1+0>=0.0000833{printf "%.6g",$2;exit}')
assert_val "V(vl,t=τ)≈4.4V" "$val_tau" 4.41 1.0
val_ss=$(echo "$out"  | awk '$1+0>=0.00045{printf "%.6g",$2;exit}')
assert_val "V(vl,t>5τ)≈0V"  "$val_ss"  0.0  0.5

# ── T5: Current Source ───────────────────────────────
echo -e "\n${YELLOW}── Test 5: Current Source Network (.op) ────────────${NC}"
out=$(python3 spice0.py ex5_isrc.sp /dev/null 2>/dev/null); echo "$out"
assert_val "V(a)  ≈1.333V"  "$(get_col "$out" 0 2)" 1.3333   0.01
assert_val "V(mid)≈0.667V"  "$(get_col "$out" 0 3)" 0.6667   0.01
assert_val "I(R1) ≈1.333mA" "$(get_col "$out" 0 4)" 0.001333 0.0001
assert_val "I(R2) ≈0.667mA" "$(get_col "$out" 0 5)" 0.000667 0.0001

# ── T6: Edge Cases ───────────────────────────────────
echo -e "\n${YELLOW}── Test 6: Edge Cases ──────────────────────────────${NC}"

printf 'Empty\n.op\n.end\n' > /tmp/t6e.sp
if python3 spice0.py /tmp/t6e.sp /dev/null > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓${NC} Empty netlist: no crash"; ((PASS++))
else
    echo -e "  ${RED}✗${NC} Empty netlist: crashed"; ((FAIL++))
fi

printf 'Two V\nV1 a 0 3\nV2 b a 7\nR1 b 0 1k\n.op\n.print V(a) V(b) I(V1) I(V2)\n.end\n' > /tmp/t6v.sp
out=$(python3 spice0.py /tmp/t6v.sp /dev/null 2>/dev/null); echo "$out"
assert_val "Two-V: V(a)=3V"  "$(get_col "$out" 0 2)" 3.0  0.01
assert_val "Two-V: V(b)=10V" "$(get_col "$out" 0 3)" 10.0 0.01

# ── T7: PNG 個別輸出 ──────────────────────────────────
echo -e "\n${YELLOW}── Test 7: Plot Output (individual PNGs) ───────────${NC}"
python3 spice0.py ex2_rc.sp /tmp/t7_rc.png > /dev/null 2>&1
if [[ -f /tmp/t7_rc.png ]] && [[ $(filesize /tmp/t7_rc.png) -gt 10000 ]]; then
    echo -e "  ${GREEN}✓${NC} PNG generated ($(filesize /tmp/t7_rc.png) bytes)"
    ((PASS++))
else
    echo -e "  ${RED}✗${NC} PNG not generated or too small"
    ((FAIL++))
fi

# ── Summary ──────────────────────────────────────────
TOTAL=$((PASS+FAIL))
echo -e "\n${BOLD}══════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  Results: ${GREEN}${PASS} passed${NC}${BOLD}, ${RED}${FAIL} failed${NC}${BOLD} / ${TOTAL} total${NC}"
echo -e "${BOLD}══════════════════════════════════════════════════${NC}\n"

# ── 最終綜合圖表 ──────────────────────────────────────
OUTPNG="test_results.png"
echo -e "${CYAN}[PLOT]${NC} Generating summary chart → ${BOLD}${OUTPNG}${NC} ..."

python3 test_spice0.py

RET=$?
if [[ $RET -eq 0 && -f test_results.png ]]; then
    SZ=$(filesize test_results.png)
    echo -e "  ${GREEN}✓${NC} Chart saved → ${BOLD}test_results.png${NC}  (${SZ} bytes)"
else
    echo -e "  ${RED}✗${NC} Chart generation failed"
fi

[[ $FAIL -eq 0 ]] && exit 0 || exit 1