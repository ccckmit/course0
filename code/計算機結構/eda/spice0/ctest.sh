#!/usr/bin/env bash
# test.sh — SPICE0 自動測試腳本
set -uo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

PASS=0; FAIL=0

info() { echo -e "${CYAN}[INFO]${NC} $*"; }

assert_val() {
    local desc="$1" val="$2" expect="$3" tol="$4"
    local ok_flag
    ok_flag=$(awk -v a="$val" -v b="$expect" -v t="$tol" \
        'BEGIN{ d=a-b; if(d<0)d=-d; print (d<=t)?"1":"0" }')
    if [[ "$ok_flag" == "1" ]]; then
        echo -e "  ${GREEN}✓${NC} $desc  (got $val, expect $expect ±$tol)"
        ((PASS++))
    else
        echo -e "  ${RED}✗${NC} $desc  (got $val, expect $expect ±$tol)"
        ((FAIL++))
    fi
}

get_col() {
    local out="$1" kw="$2" col="$3"
    echo "$out" | awk -v k="$kw" -v c="$col" '$1==k{printf "%.9g",$c; exit}'
}

echo -e "\n${BOLD}═══════════════════════════════════════════════${NC}"
echo -e "${BOLD}  SPICE0 Automated Test Suite${NC}"
echo -e "${BOLD}═══════════════════════════════════════════════${NC}\n"

info "Compiling spice0.c ..."
if ! gcc -O2 -lm -o spice0 spice0.c 2>&1; then
    echo -e "${RED}Compilation FAILED — aborting${NC}"; exit 1
fi
echo -e "  ${GREEN}Compiled OK${NC}\n"

# ── T1: Voltage Divider ───────────────────────────────
echo -e "${YELLOW}── Test 1: Voltage Divider (.op) ──────────────────${NC}"
out=$(./spice0 ex1_divider.sp); echo "$out"; echo
assert_val "V(in)=10V"   "$(get_col "$out" 0 2)" 10    0.01
assert_val "V(mid)=5V"   "$(get_col "$out" 0 3)" 5     0.01
assert_val "I(V1)=-5mA" "$(get_col "$out" 0 4)" -0.005 0.001

# ── T2: RC Transient ─────────────────────────────────
echo -e "\n${YELLOW}── Test 2: RC Transient (.tran) ────────────────────${NC}"
out=$(./spice0 ex2_rc.sp)
echo "$out" | head -28; echo "  (...共 $(echo "$out"|wc -l|tr -d ' ') 行)"; echo
val_1ms=$(echo "$out" | awk '$1+0 >= 0.001 {printf "%.6g",$2; exit}')
assert_val "V(vc,t=1ms)≈3.16V" "$val_1ms" 3.161 0.20
val_5ms=$(echo "$out" | awk '$1+0 >= 0.005 {printf "%.6g",$2; exit}')
assert_val "V(vc,t=5ms)≈5.00V" "$val_5ms" 5.0   0.10

# ── T3: DC Sweep ─────────────────────────────────────
echo -e "\n${YELLOW}── Test 3: DC Sweep (.dc) ──────────────────────────${NC}"
out=$(./spice0 ex3_dc_sweep.sp); echo "$out"; echo
assert_val "V(mid)|V1=2V=1.0V"  "$(get_col "$out" 2 3)" 1.0   0.01
assert_val "V(mid)|V1=4V=2.0V"  "$(get_col "$out" 4 3)" 2.0   0.01
assert_val "I(V1) |V1=4V=-20mA" "$(get_col "$out" 4 4)" -0.02 0.001

# ── T4: RL Transient ─────────────────────────────────
echo -e "\n${YELLOW}── Test 4: RL Transient (.tran) ────────────────────${NC}"
out=$(./spice0 ex4_rl.sp)
echo "$out" | head -20; echo "  (...共 $(echo "$out"|wc -l|tr -d ' ') 行)"; echo
val_tau=$(echo "$out" | awk '$1+0 >= 0.0000833 {printf "%.6g",$2; exit}')
assert_val "V(vl,t=τ)≈4.4V" "$val_tau" 4.41 1.0
val_ss=$(echo "$out" | awk '$1+0 >= 0.00045 {printf "%.6g",$2; exit}')
assert_val "V(vl,t>5τ)≈0V"  "$val_ss"  0.0  0.5

# ── T5: Current Source Network ───────────────────────
echo -e "\n${YELLOW}── Test 5: Current Source Network (.op) ────────────${NC}"
out=$(./spice0 ex5_isrc.sp); echo "$out"; echo
assert_val "V(a)  ≈1.333V"  "$(get_col "$out" 0 2)" 1.3333   0.01
assert_val "V(mid)≈0.667V"  "$(get_col "$out" 0 3)" 0.6667   0.01
assert_val "I(R1) ≈1.333mA" "$(get_col "$out" 0 4)" 0.001333 0.0001
assert_val "I(R2) ≈0.667mA" "$(get_col "$out" 0 5)" 0.000667 0.0001

# ── T6: Edge Cases ───────────────────────────────────
echo -e "\n${YELLOW}── Test 6: Edge Cases ──────────────────────────────${NC}"

printf 'Empty\n.op\n.end\n' > /tmp/t6_empty.sp
if ./spice0 /tmp/t6_empty.sp > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓${NC} Empty netlist: no crash"; ((PASS++))
else
    echo -e "  ${RED}✗${NC} Empty netlist: crashed"; ((FAIL++))
fi

printf 'Float R\nR1 a b 1k\n.op\n.end\n' > /tmp/t6_float.sp
if ./spice0 /tmp/t6_float.sp > /dev/null 2>&1; then
    echo -e "  ${GREEN}✓${NC} Floating resistor: no crash"; ((PASS++))
else
    echo -e "  ${RED}✗${NC} Floating resistor: crashed"; ((FAIL++))
fi

printf 'Two V\nV1 a 0 3\nV2 b a 7\nR1 b 0 1k\n.op\n.print V(a) V(b) I(V1) I(V2)\n.end\n' \
    > /tmp/t6_twov.sp
out_tv=$(./spice0 /tmp/t6_twov.sp); echo "$out_tv"
assert_val "Two-V: V(a)=3V"  "$(get_col "$out_tv" 0 2)" 3.0  0.01
assert_val "Two-V: V(b)=10V" "$(get_col "$out_tv" 0 3)" 10.0 0.01

# ── Summary ──────────────────────────────────────────
TOTAL=$((PASS+FAIL))
echo -e "\n${BOLD}═══════════════════════════════════════════════${NC}"
echo -e "${BOLD}  Results: ${GREEN}${PASS} passed${NC}${BOLD}, ${RED}${FAIL} failed${NC}${BOLD} / ${TOTAL} total${NC}"
echo -e "${BOLD}═══════════════════════════════════════════════${NC}\n"
[[ $FAIL -eq 0 ]] && exit 0 || exit 1
