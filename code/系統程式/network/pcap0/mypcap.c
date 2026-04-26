/*
 * mypcap.c — 自製迷你封包捕獲套件實作
 *
 * macOS/BSD: 直接開 /dev/bpf，用 ioctl 綁定介面
 * Linux:     建立 AF_PACKET raw socket，bind 到介面
 *
 * 編譯:
 *   macOS: gcc -o demo demo.c mypcap.c
 *   Linux: gcc -o demo demo.c mypcap.c
 */

#include "mypcap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <net/if.h>

/* ════════════════════════════════════════
 *  平台偵測
 * ════════════════════════════════════════ */
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__)
  #define PLATFORM_BPF
#elif defined(__linux__)
  #define PLATFORM_LINUX
#else
  #error "不支援的平台"
#endif

/* ════════════════════════════════════════
 *  macOS / BSD — BPF 實作
 * ════════════════════════════════════════ */
#ifdef PLATFORM_BPF

#include <net/bpf.h>

#define BPF_BUF_SIZE (1 << 20)   /* 1 MB 接收緩衝 */
#define BPF_DEV_MAX  99

struct mypcap {
    int   fd;                    /* /dev/bpfN 的 fd */
    uint8_t *buf;                /* 核心給的 bpf 緩衝區 */
    size_t   bufsize;            /* 實際緩衝區大小 */
    char  errbuf[256];
};

/* 找到一個空閒的 /dev/bpfN 並開啟 */
static int bpf_open(char *errbuf) {
    char dev[32];
    for (int i = 0; i <= BPF_DEV_MAX; i++) {
        snprintf(dev, sizeof(dev), "/dev/bpf%d", i);
        int fd = open(dev, O_RDONLY);
        if (fd >= 0) return fd;
        if (errno == EBUSY) continue;   /* 被佔用，試下一個 */
        snprintf(errbuf, 256, "open %s: %s", dev, strerror(errno));
        return -1;
    }
    snprintf(errbuf, 256, "所有 /dev/bpf 都被佔用");
    return -1;
}

mypcap_t *mypcap_open(const char *iface) {
    mypcap_t *h = calloc(1, sizeof(*h));
    if (!h) return NULL;

    /* 1. 開啟 BPF 裝置 */
    h->fd = bpf_open(h->errbuf);
    if (h->fd < 0) return h;

    /* 2. 設定緩衝區大小 */
    u_int blen = BPF_BUF_SIZE;
    if (ioctl(h->fd, BIOCSBLEN, &blen) < 0) {
        /* 失敗無妨，用系統預設值 */
    }
    /* 查詢實際核心給的大小 */
    if (ioctl(h->fd, BIOCGBLEN, &blen) < 0) {
        snprintf(h->errbuf, 256, "BIOCGBLEN: %s", strerror(errno));
        close(h->fd); h->fd = -1; return h;
    }
    h->bufsize = blen;
    h->buf = malloc(blen);
    if (!h->buf) {
        snprintf(h->errbuf, 256, "malloc 失敗");
        close(h->fd); h->fd = -1; return h;
    }

    /* 3. 綁定網路介面 */
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(h->fd, BIOCSETIF, &ifr) < 0) {
        snprintf(h->errbuf, 256, "BIOCSETIF %s: %s", iface, strerror(errno));
        close(h->fd); h->fd = -1; return h;
    }

    /* 4. 立即返回模式（不等緩衝區填滿才 read）*/
    u_int imm = 1;
    ioctl(h->fd, BIOCIMMEDIATE, &imm);

    /* 5. 關閉混雜模式（Wi-Fi 不支援）*/
    /* 若要開啟: ioctl(h->fd, BIOCPROMISC, NULL); */

    return h;
}

void mypcap_close(mypcap_t *h) {
    if (!h) return;
    if (h->fd >= 0) close(h->fd);
    free(h->buf);
    free(h);
}

/*
 * BPF 緩衝區裡可能塞了多個封包，每個用 BPF_WORDALIGN 對齊。
 * 結構: [bpf_hdr][raw packet bytes][padding]...重複
 */
int mypcap_loop(mypcap_t *h, int count, mypcap_handler cb, void *user) {
    if (!h || h->fd < 0) return -1;

    int captured = 0;

    while (count == 0 || captured < count) {
        /* read 會阻塞直到有封包 */
        ssize_t n = read(h->fd, h->buf, h->bufsize);
        if (n < 0) {
            if (errno == EINTR) continue;   /* 被 signal 中斷，繼續 */
            snprintf(h->errbuf, 256, "read: %s", strerror(errno));
            return -1;
        }

        /* 走訪緩衝區裡的每個封包 */
        uint8_t *p = h->buf;
        uint8_t *end = h->buf + n;

        while (p < end) {
            /* BPF 標頭 */
            struct bpf_hdr *bh = (struct bpf_hdr *)p;

            mypcap_pkthdr_t hdr;
            hdr.ts.tv_sec  = bh->bh_tstamp.tv_sec;
            hdr.ts.tv_usec = bh->bh_tstamp.tv_usec;
            hdr.caplen  = bh->bh_caplen;
            hdr.origlen = bh->bh_datalen;

            /* 封包資料緊接在 BPF 標頭之後 */
            const uint8_t *pkt = p + bh->bh_hdrlen;

            cb(user, &hdr, pkt);
            captured++;

            if (count > 0 && captured >= count) goto done;

            /* 跳到下一個封包（對齊到 word boundary）*/
            p += BPF_WORDALIGN(bh->bh_hdrlen + bh->bh_caplen);
        }
    }

done:
    return captured;
}

#endif /* PLATFORM_BPF */

/* ════════════════════════════════════════
 *  Linux — AF_PACKET 實作
 * ════════════════════════════════════════ */
#ifdef PLATFORM_LINUX

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#define LINUX_BUF_SIZE 65536

struct mypcap {
    int   fd;
    uint8_t *buf;
    char  errbuf[256];
};

mypcap_t *mypcap_open(const char *iface) {
    mypcap_t *h = calloc(1, sizeof(*h));
    if (!h) return NULL;

    /* 1. 建立 AF_PACKET raw socket
     *    ETH_P_ALL = 接收所有乙太網路類型 (htons 轉換位元組序) */
    h->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (h->fd < 0) {
        snprintf(h->errbuf, 256, "socket: %s", strerror(errno));
        return h;
    }

    /* 2. 查詢介面 index */
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    if (ioctl(h->fd, SIOCGIFINDEX, &ifr) < 0) {
        snprintf(h->errbuf, 256, "SIOCGIFINDEX %s: %s", iface, strerror(errno));
        close(h->fd); h->fd = -1; return h;
    }
    int ifindex = ifr.ifr_ifindex;

    /* 3. bind 到指定介面（不 bind 則接收所有介面）*/
    struct sockaddr_ll sll = {0};
    sll.sll_family   = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex  = ifindex;
    if (bind(h->fd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        snprintf(h->errbuf, 256, "bind: %s", strerror(errno));
        close(h->fd); h->fd = -1; return h;
    }

    /* 4. 選用：開啟混雜模式 */
    /*
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type    = PACKET_MR_PROMISC;
    setsockopt(h->fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr));
    */

    h->buf = malloc(LINUX_BUF_SIZE);
    if (!h->buf) {
        snprintf(h->errbuf, 256, "malloc 失敗");
        close(h->fd); h->fd = -1; return h;
    }

    return h;
}

void mypcap_close(mypcap_t *h) {
    if (!h) return;
    if (h->fd >= 0) close(h->fd);
    free(h->buf);
    free(h);
}

int mypcap_loop(mypcap_t *h, int count, mypcap_handler cb, void *user) {
    if (!h || h->fd < 0) return -1;

    int captured = 0;

    while (count == 0 || captured < count) {
        /* recvfrom 阻塞等待封包 */
        ssize_t n = recvfrom(h->fd, h->buf, LINUX_BUF_SIZE, 0, NULL, NULL);
        if (n < 0) {
            if (errno == EINTR) continue;
            snprintf(h->errbuf, 256, "recvfrom: %s", strerror(errno));
            return -1;
        }

        /* 取時間戳 */
        struct timeval tv;
        gettimeofday(&tv, NULL);

        mypcap_pkthdr_t hdr;
        hdr.ts      = tv;
        hdr.caplen  = (uint32_t)n;
        hdr.origlen = (uint32_t)n;  /* AF_PACKET 不提供原始長度 */

        cb(user, &hdr, h->buf);
        captured++;
    }

    return captured;
}

#endif /* PLATFORM_LINUX */

/* ── 共用：取錯誤訊息 ── */
const char *mypcap_errbuf(mypcap_t *h) {
    return h ? h->errbuf : "handle 為 NULL";
}
