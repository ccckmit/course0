#ifndef MYPCAP_H
#define MYPCAP_H

/*
 * mypcap.h — 自製迷你封包捕獲套件
 *
 * 支援:
 *   macOS / BSD  → /dev/bpf  (BPF 裝置)
 *   Linux        → AF_PACKET raw socket
 *
 * 使用方式:
 *   mypcap_t *h = mypcap_open("en0");
 *   mypcap_loop(h, 10, my_callback, NULL);
 *   mypcap_close(h);
 */

#include <stdint.h>
#include <sys/time.h>

/* ── 封包描述子（模仿 pcap_pkthdr）── */
typedef struct {
    struct timeval ts;      /* 時間戳 */
    uint32_t       caplen;  /* 實際捕獲長度 */
    uint32_t       origlen; /* 原始封包長度 */
} mypcap_pkthdr_t;

/* ── callback 型別 ── */
typedef void (*mypcap_handler)(void *user,
                               const mypcap_pkthdr_t *hdr,
                               const uint8_t *pkt);

/* ── handle（不透明結構，實作在 .c 裡）── */
typedef struct mypcap mypcap_t;

/* ── API ── */
mypcap_t *mypcap_open(const char *iface);   /* 開啟介面 */
void      mypcap_close(mypcap_t *h);        /* 關閉並釋放 */
int       mypcap_loop(mypcap_t *h,          /* 捕獲迴圈   */
                      int count,            /*   0=無限   */
                      mypcap_handler cb,
                      void *user);
const char *mypcap_errbuf(mypcap_t *h);     /* 取得錯誤訊息 */

#endif /* MYPCAP_H */
