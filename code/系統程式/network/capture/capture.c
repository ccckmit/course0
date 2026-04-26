/*
 * capture.c - 跨平台封包捕獲程式
 * 支援平台: Linux / macOS (libpcap)
 *           Windows (WinPcap / Npcap)
 *
 * 編譯:
 *   Linux/macOS: gcc -o capture capture.c -lpcap
 *   Windows:     gcc -o capture capture.c -lwpcap -lws2_32
 *
 * 執行 (需要 root / Administrator):
 *   sudo ./capture -i eth0 -c 20 -f "tcp port 80" -w out.pcap
 */

#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <pcap.h>
#else
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <pcap/pcap.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

/* ── 通用型別別名 (Windows 相容) ── */
#ifndef u_int8_t
  typedef unsigned char  u_int8_t;
  typedef unsigned short u_int16_t;
  typedef unsigned int   u_int32_t;
#endif

/* ── 以太網路標頭 (14 bytes) ── */
#define ETHER_ADDR_LEN 6
struct ether_header {
    u_int8_t  dst[ETHER_ADDR_LEN];
    u_int8_t  src[ETHER_ADDR_LEN];
    u_int16_t type;
};
#define ETHERTYPE_IP   0x0800
#define ETHERTYPE_IPV6 0x86DD
#define ETHERTYPE_ARP  0x0806

/* ── IPv4 標頭 ── */
struct ip_header {
    u_int8_t  ver_ihl;      /* version (4 bits) + IHL (4 bits) */
    u_int8_t  tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t  ttl;
    u_int8_t  protocol;
    u_int16_t checksum;
    struct in_addr src;
    struct in_addr dst;
};
#define IP_PROTO_TCP  6
#define IP_PROTO_UDP  17
#define IP_PROTO_ICMP 1

/* ── TCP/UDP 標頭 ── */
struct tcp_header {
    u_int16_t sport;
    u_int16_t dport;
    u_int32_t seq;
    u_int32_t ack;
    u_int8_t  data_off;
    u_int8_t  flags;
    u_int16_t window;
    u_int16_t checksum;
    u_int16_t urg;
};
struct udp_header {
    u_int16_t sport;
    u_int16_t dport;
    u_int16_t length;
    u_int16_t checksum;
};

/* ── 全域設定 ── */
static pcap_t      *g_handle  = NULL;
static pcap_dumper_t *g_dumper = NULL;
static volatile int g_stop    = 0;
static int         g_count    = 0;
static int         g_max      = 0;     /* 0 = 不限制 */
static int         g_verbose  = 0;

/* ── 訊號處理：Ctrl-C 優雅結束 ── */
static void on_signal(int sig) {
    (void)sig;
    g_stop = 1;
    if (g_handle) pcap_breakloop(g_handle);
}

/* ── MAC 位址格式化 ── */
static const char *fmt_mac(const u_int8_t *m, char *buf) {
    snprintf(buf, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
             m[0], m[1], m[2], m[3], m[4], m[5]);
    return buf;
}

/* ── 封包 callback ── */
static void packet_handler(u_char *user,
                           const struct pcap_pkthdr *hdr,
                           const u_char *pkt)
{
    (void)user;

    /* 寫入 pcap 檔案 */
    if (g_dumper) pcap_dump((u_char *)g_dumper, hdr, pkt);

    g_count++;

    /* 時間戳 */
    char tbuf[32];
    time_t t = (time_t)hdr->ts.tv_sec;
    struct tm *tm = localtime(&t);
    strftime(tbuf, sizeof(tbuf), "%H:%M:%S", tm);

    printf("[%s.%06ld] #%-4d  cap=%u bytes\n",
           tbuf, (long)hdr->ts.tv_usec, g_count, hdr->caplen);

    if (!g_verbose) goto done;

    /* ── 解析以太網路 ── */
    if (hdr->caplen < sizeof(struct ether_header)) goto done;
    const struct ether_header *eth = (const struct ether_header *)pkt;

    char smac[18], dmac[18];
    printf("  ETH  %s → %s  type=0x%04x\n",
           fmt_mac(eth->src, smac),
           fmt_mac(eth->dst, dmac),
           ntohs(eth->type));

    if (ntohs(eth->type) != ETHERTYPE_IP) goto done;

    /* ── 解析 IPv4 ── */
    const u_char *ip_start = pkt + sizeof(struct ether_header);
    if (hdr->caplen < sizeof(struct ether_header) + sizeof(struct ip_header))
        goto done;
    const struct ip_header *ip = (const struct ip_header *)ip_start;
    int ip_hlen = (ip->ver_ihl & 0x0F) * 4;

    printf("  IPv4 %s → %s  proto=%d  ttl=%d  len=%d\n",
           inet_ntoa(ip->src), inet_ntoa(ip->dst),
           ip->protocol, ip->ttl, ntohs(ip->tot_len));

    /* ── 解析 TCP/UDP ── */
    const u_char *transport = ip_start + ip_hlen;
    if (ip->protocol == IP_PROTO_TCP &&
        hdr->caplen >= (u_int32_t)(transport - pkt + (int)sizeof(struct tcp_header)))
    {
        const struct tcp_header *tcp = (const struct tcp_header *)transport;
        printf("  TCP  sport=%-5d dport=%-5d  flags=0x%02x  seq=%u\n",
               ntohs(tcp->sport), ntohs(tcp->dport),
               tcp->flags, ntohl(tcp->seq));
    }
    else if (ip->protocol == IP_PROTO_UDP &&
             hdr->caplen >= (u_int32_t)(transport - pkt + (int)sizeof(struct udp_header)))
    {
        const struct udp_header *udp = (const struct udp_header *)transport;
        printf("  UDP  sport=%-5d dport=%-5d  len=%d\n",
               ntohs(udp->sport), ntohs(udp->dport), ntohs(udp->length));
    }
    else if (ip->protocol == IP_PROTO_ICMP) {
        printf("  ICMP\n");
    }

done:
    if (g_max > 0 && g_count >= g_max) {
        g_stop = 1;
        pcap_breakloop(g_handle);
    }
}

/* ── 列出所有網路介面 ── */
static void list_interfaces(void) {
    pcap_if_t *devs, *d;
    char errbuf[PCAP_ERRBUF_SIZE];
    if (pcap_findalldevs(&devs, errbuf) < 0) {
        fprintf(stderr, "findalldevs: %s\n", errbuf);
        return;
    }
    printf("可用網路介面:\n");
    int i = 0;
    for (d = devs; d; d = d->next) {
        printf("  [%d] %s", ++i, d->name);
        if (d->description) printf("  (%s)", d->description);
        printf("\n");
    }
    pcap_freealldevs(devs);
}

/* ── 使用說明 ── */
static void usage(const char *prog) {
    printf("用法: %s [選項]\n"
           "  -i <iface>   網路介面 (預設: 自動選擇)\n"
           "  -c <count>   最多捕獲封包數 (預設: 不限制)\n"
           "  -f <filter>  BPF 過濾條件 (例: \"tcp port 443\")\n"
           "  -w <file>    輸出 pcap 檔案\n"
           "  -s <snap>    每包截取長度 (預設: 65535)\n"
           "  -v           顯示詳細解析\n"
           "  -l           列出所有介面\n"
           "  -h           顯示此說明\n", prog);
}

/* ── 主程式 ── */
int main(int argc, char *argv[]) {
    const char *iface   = NULL;
    const char *filter  = NULL;
    const char *outfile = NULL;
    int snaplen = 65535;
    int promisc = 0;    /* 混雜模式 */
    int timeout = 1000; /* ms */

    /* 解析命令列 */
    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-i") && i+1 < argc) iface   = argv[++i];
        else if (!strcmp(argv[i], "-c") && i+1 < argc) g_max   = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-f") && i+1 < argc) filter  = argv[++i];
        else if (!strcmp(argv[i], "-w") && i+1 < argc) outfile = argv[++i];
        else if (!strcmp(argv[i], "-s") && i+1 < argc) snaplen = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-v")) g_verbose = 1;
        else if (!strcmp(argv[i], "-l")) { list_interfaces(); return 0; }
        else if (!strcmp(argv[i], "-h")) { usage(argv[0]); return 0; }
        else { fprintf(stderr, "未知選項: %s\n", argv[i]); usage(argv[0]); return 1; }
    }

    char errbuf[PCAP_ERRBUF_SIZE];

    /* 自動選擇介面：用 pcap_findalldevs 取第一個 */
    static char auto_iface[128];
    if (!iface) {
        pcap_if_t *devs;
        if (pcap_findalldevs(&devs, errbuf) < 0 || !devs) {
            fprintf(stderr, "找不到介面: %s\n", errbuf);
            return 1;
        }
        strncpy(auto_iface, devs->name, sizeof(auto_iface) - 1);
        pcap_freealldevs(devs);
        iface = auto_iface;
        printf("使用介面: %s\n", iface);
    }

    /* 開啟介面 */
    g_handle = pcap_open_live(iface, snaplen, promisc, timeout, errbuf);
    if (!g_handle) { fprintf(stderr, "pcap_open_live: %s\n", errbuf); return 1; }

    /* 確認是乙太網路 (link type) */
    int lt = pcap_datalink(g_handle);
    printf("Link type: %s (%d)\n", pcap_datalink_val_to_name(lt), lt);
    if (lt != DLT_EN10MB) {
        fprintf(stderr, "警告: 此程式只解析乙太網路封包\n");
    }

    /* 套用 BPF 過濾器 */
    if (filter) {
        struct bpf_program fp;
        bpf_u_int32 net, mask;
        pcap_lookupnet(iface, &net, &mask, errbuf);
        if (pcap_compile(g_handle, &fp, filter, 1, mask) < 0) {
            fprintf(stderr, "pcap_compile: %s\n", pcap_geterr(g_handle));
            pcap_close(g_handle); return 1;
        }
        if (pcap_setfilter(g_handle, &fp) < 0) {
            fprintf(stderr, "pcap_setfilter: %s\n", pcap_geterr(g_handle));
            pcap_freecode(&fp); pcap_close(g_handle); return 1;
        }
        pcap_freecode(&fp);
        printf("BPF 過濾器: %s\n", filter);
    }

    /* 開啟輸出 pcap 檔案 */
    if (outfile) {
        g_dumper = pcap_dump_open(g_handle, outfile);
        if (!g_dumper) {
            fprintf(stderr, "pcap_dump_open: %s\n", pcap_geterr(g_handle));
            pcap_close(g_handle); return 1;
        }
        printf("輸出至: %s\n", outfile);
    }

    /* 訊號處理 */
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    printf("開始捕獲 (Ctrl-C 停止)\n\n");

    pcap_loop(g_handle, -1, packet_handler, NULL);

    /* 清理 */
    struct pcap_stat st;
    if (pcap_stats(g_handle, &st) == 0) {
        printf("\n統計: 接收=%u  丟棄=%u  介面丟棄=%u\n",
               st.ps_recv, st.ps_drop, st.ps_ifdrop);
    }
    printf("共捕獲 %d 個封包\n", g_count);

    if (g_dumper) pcap_dump_close(g_dumper);
    pcap_close(g_handle);
    return 0;
}