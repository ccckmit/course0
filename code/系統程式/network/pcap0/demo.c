/*
 * demo.c — 使用自製 mypcap 套件的示範程式
 *
 * 編譯:
 *   macOS: gcc -o demo demo.c mypcap.c
 *   Linux: gcc -o demo demo.c mypcap.c
 *
 * 執行:
 *   sudo ./demo en0 10
 *   sudo ./demo eth0 20
 */

#include "mypcap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

/* ── 以太網路標頭 ── */
#define ETHERTYPE_IP  0x0800
#define ETHERTYPE_ARP 0x0806

struct eth_hdr {
    uint8_t  dst[6];
    uint8_t  src[6];
    uint16_t type;
} __attribute__((packed));

/* ── IPv4 標頭 ── */
struct ip_hdr {
    uint8_t  ver_ihl;
    uint8_t  tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t  ttl;
    uint8_t  proto;
    uint16_t cksum;
    uint32_t src;
    uint32_t dst;
} __attribute__((packed));

static int pkt_count = 0;

static void on_packet(void *user,
                      const mypcap_pkthdr_t *hdr,
                      const uint8_t *pkt)
{
    (void)user;
    pkt_count++;

    printf("#%-4d  len=%-5u  ", pkt_count, hdr->caplen);

    /* 解析 Ethernet */
    if (hdr->caplen < sizeof(struct eth_hdr)) {
        printf("(too short)\n");
        return;
    }
    const struct eth_hdr *eth = (const struct eth_hdr *)pkt;
    uint16_t etype = ntohs(eth->type);

    printf("ETH %02x:%02x:%02x:%02x:%02x:%02x → %02x:%02x:%02x:%02x:%02x:%02x  ",
           eth->src[0], eth->src[1], eth->src[2],
           eth->src[3], eth->src[4], eth->src[5],
           eth->dst[0], eth->dst[1], eth->dst[2],
           eth->dst[3], eth->dst[4], eth->dst[5]);

    if (etype == ETHERTYPE_IP) {
        const uint8_t *ip_start = pkt + sizeof(struct eth_hdr);
        if (hdr->caplen < sizeof(struct eth_hdr) + sizeof(struct ip_hdr)) {
            printf("IPv4 (too short)\n");
            return;
        }
        const struct ip_hdr *ip = (const struct ip_hdr *)ip_start;
        struct in_addr s, d;
        s.s_addr = ip->src;
        d.s_addr = ip->dst;

        const char *proto = "?";
        if (ip->proto == 6)  proto = "TCP";
        if (ip->proto == 17) proto = "UDP";
        if (ip->proto == 1)  proto = "ICMP";

        printf("IPv4 %s → %s  %s\n", inet_ntoa(s), inet_ntoa(d), proto);
    } else if (etype == ETHERTYPE_ARP) {
        printf("ARP\n");
    } else {
        printf("type=0x%04x\n", etype);
    }
}

int main(int argc, char *argv[]) {
    const char *iface = (argc >= 2) ? argv[1] : "en0";
    int count         = (argc >= 3) ? atoi(argv[2]) : 10;

    printf("mypcap demo — 介面: %s  最多: %d 個封包\n\n", iface, count);

    mypcap_t *h = mypcap_open(iface);
    if (!h) { fprintf(stderr, "記憶體不足\n"); return 1; }

    if (mypcap_errbuf(h)[0] != '\0') {
        fprintf(stderr, "錯誤: %s\n", mypcap_errbuf(h));
        mypcap_close(h);
        return 1;
    }

    int n = mypcap_loop(h, count, on_packet, NULL);
    printf("\n共捕獲 %d 個封包\n", n);

    mypcap_close(h);
    return 0;
}
