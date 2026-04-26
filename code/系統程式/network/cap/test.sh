#!/bin/bash
# test.sh — 編譯並測試 capture
set -x

# ── 偵測介面 ────────────────────────────────────────
if [[ "$(uname)" == "Darwin" ]]; then
    IFACE="en0"
else
    IFACE=$(ip -o link show | awk -F': ' '$2 != "lo" {gsub(/:/, "", $2); print $2; exit}')
fi

echo "介面: $IFACE"

# ── 編譯 ─────────────────────────────────────────────
echo "=== [1] 編譯 capture ==="
gcc -o capture capture.c -lpcap
echo "OK"

# ── 測試 1：列出介面 ──────────────────────────────────
echo ""
echo "=== [2] 列出介面 ==="
sudo ./capture -l

# ── 測試 2：抓 ICMP（ping 產生流量）─────────────────
echo ""
echo "=== [3] 抓 ICMP 封包 ==="
ping -c 10 8.8.8.8 > /dev/null 2>&1 &
PING_PID=$!
sudo ./capture -i "$IFACE" -c 5 -f "icmp" -v
wait "$PING_PID" 2>/dev/null || true

# ── 測試 3：抓 DNS（UDP port 53）─────────────────────
echo ""
echo "=== [4] 抓 DNS 封包 ==="
(sleep 0.5 && for i in 1 2 3 4 5; do nslookup google.com > /dev/null 2>&1; done) &
DNS_PID=$!
sudo ./capture -i "$IFACE" -c 5 -f "udp port 53" -v
wait "$DNS_PID" 2>/dev/null || true

# ── 測試 4：抓 HTTP（TCP port 80）────────────────────
echo ""
echo "=== [5] 抓 HTTP 封包 ==="
(sleep 0.5 && curl -s http://example.com > /dev/null 2>&1) &
CURL_PID=$!
sudo ./capture -i "$IFACE" -c 5 -f "tcp port 80" -v
wait "$CURL_PID" 2>/dev/null || true

# ── 測試 5：存成 pcap 檔 ──────────────────────────────
echo ""
echo "=== [6] 存成 pcap 檔 ==="
ping -c 5 8.8.8.8 > /dev/null 2>&1 &
PING_PID=$!
sudo ./capture -i "$IFACE" -c 5 -f "icmp" -w out.pcap
wait "$PING_PID" 2>/dev/null || true
ls -lh out.pcap 2>/dev/null && echo "pcap 檔案寫入成功" || echo "pcap 檔案未產生"

echo ""
echo "=== 全部完成 ==="
