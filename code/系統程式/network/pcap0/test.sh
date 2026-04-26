#!/bin/bash
# test.sh — 編譯並測試 mypcap
set -e

# ── 偵測介面 ────────────────────────────────────────
if [[ "$(uname)" == "Darwin" ]]; then
    IFACE="en0"
else
    IFACE=$(ip -o link show | awk -F': ' '$2 != "lo" {print $2; exit}')
fi

COUNT=10

echo "========================================"
echo " mypcap 測試腳本"
echo " 平台 : $(uname)"
echo " 介面 : $IFACE"
echo " 封包數: $COUNT"
echo "========================================"
echo ""

# ── 編譯 ─────────────────────────────────────────────
echo "[1] 編譯..."
gcc -Wall -Wextra -o demo demo.c mypcap.c
echo "    OK"
echo ""

# ── 背景產生流量 ─────────────────────────────────────
echo "[2] 背景產生流量 (ping 8.8.8.8)..."
ping -c "$COUNT" 8.8.8.8 > /dev/null 2>&1 &
PING_PID=$!

# ── 捕獲封包 ─────────────────────────────────────────
echo "[3] 開始捕獲 $COUNT 個封包..."
echo ""
sudo ./demo "$IFACE" "$COUNT"

# ── 清理 ─────────────────────────────────────────────
wait "$PING_PID" 2>/dev/null || true
echo ""
echo "========================================"
echo " 完成"
echo "========================================"