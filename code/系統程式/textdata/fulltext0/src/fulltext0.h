#ifndef FULLTEXT0_H
#define FULLTEXT0_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ── Constants ─────────────────────────────────────────────────────────────── */
#define MAX_TOKEN_LEN   64
#define MAX_TOKENS      128
#define MAX_TERMS       (1<<16)
#define BLOCK_SIZE      4096
#define INDEX_PATH      "_index/data.idx"
#define OFFSET_PATH     "_index/offsets.bin"
#define CORPUS_PATH     "_data/corpus.txt"

/* ── Magic / version ─────────────────────────────────────────────────────── */
#define IDX_MAGIC       0x46545831u   /* 'FTX1' bumped from v1 */
#define IDX_VERSION     2

/* ─── On-disk layout (v2) ─────────────────────────────────────────────────
 *
 *  Block 0  Header (4096 bytes)
 *    uint32  magic
 *    uint32  version
 *    uint32  num_terms
 *    uint32  num_docs
 *    uint32  term_block_start
 *    uint32  post_block_start
 *    char    pad[4096-24]
 *
 *  term_block_start … post_block_start-1   Term table
 *    per record:
 *      uint16  term_len
 *      char    term[term_len]
 *      uint32  post_offset       (byte offset into posting section)
 *      uint32  post_count        (number of doc_ids)
 *      uint32  post_byte_len     (② compressed byte length)
 *
 *  post_block_start …   Posting lists
 *    ② VarInt delta-encoded doc_ids (sorted ascending)
 *       7-bit groups, little-endian, MSB=1 means more bytes follow
 *
 *  offsets.bin  (separate side-car file, ③)
 *    uint64  offset[num_docs]    (byte offset of each line in corpus)
 *
 * ─────────────────────────────────────────────────────────────────────────── */

/* ── In-memory structures ─────────────────────────────────────────────────── */
typedef struct {
    char     term[MAX_TOKEN_LEN];
    uint32_t post_offset;
    uint32_t post_count;
    uint32_t post_byte_len;   /* ② compressed size */
} TermEntry;

typedef struct {
    uint32_t *doc_ids;
    uint32_t  count;
    uint32_t  cap;
} PostingList;

/* ── ② VarInt encode / decode ─────────────────────────────────────────────── */

static inline int varint_encode(unsigned char *buf, uint32_t v)
{
    int n = 0;
    while (v > 0x7Fu) {
        buf[n++] = (unsigned char)((v & 0x7Fu) | 0x80u);
        v >>= 7;
    }
    buf[n++] = (unsigned char)v;
    return n;
}

/* Returns bytes consumed, or -1 on overflow. */
static inline int varint_decode(const unsigned char *buf, uint32_t *v)
{
    uint32_t result = 0;
    int shift = 0, n = 0;
    unsigned char b;
    do {
        if (n >= 5) return -1;
        b = buf[n++];
        result |= (uint32_t)(b & 0x7Fu) << shift;
        shift += 7;
    } while (b & 0x80u);
    *v = result;
    return n;
}

#define VARINT_MAX 5

/* ── UTF-8 utilities ──────────────────────────────────────────────────────── */

static inline int utf8_decode(const unsigned char *s, uint32_t *cp)
{
    unsigned char c = s[0];
    if (c == 0)   { *cp = 0; return 0; }
    if (c < 0x80) { *cp = c; return 1; }
    if (c < 0xC0) return -1;
    if (c < 0xE0) {
        if ((s[1]&0xC0)!=0x80) return -1;
        *cp = ((c&0x1F)<<6)|(s[1]&0x3F); return 2;
    }
    if (c < 0xF0) {
        if ((s[1]&0xC0)!=0x80||(s[2]&0xC0)!=0x80) return -1;
        *cp = ((c&0x0F)<<12)|((s[1]&0x3F)<<6)|(s[2]&0x3F); return 3;
    }
    if (c < 0xF8) {
        if ((s[1]&0xC0)!=0x80||(s[2]&0xC0)!=0x80||(s[3]&0xC0)!=0x80) return -1;
        *cp = ((c&0x07)<<18)|((s[1]&0x3F)<<12)|((s[2]&0x3F)<<6)|(s[3]&0x3F);
        return 4;
    }
    return -1;
}

static inline int is_cjk(uint32_t cp)
{
    return (cp >= 0x4E00 && cp <= 0x9FFF) ||
           (cp >= 0x3400 && cp <= 0x4DBF) ||
           (cp >= 0xF900 && cp <= 0xFAFF) ||
           (cp >= 0x3000 && cp <= 0x303F) ||
           (cp >= 0xFF00 && cp <= 0xFFEF);
}

static inline int is_ascii_alnum(char c)
{
    return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9');
}

static inline char ascii_lower(char c)
{
    return (c>='A'&&c<='Z') ? (char)(c+32) : c;
}

/* ── Shared declarations ──────────────────────────────────────────────────── */

int tokenize(const char *line, char out[][MAX_TOKEN_LEN], int max_out);

int intersect(const uint32_t *a, int na,
              const uint32_t *b, int nb,
              uint32_t *res, int max_res);

int unite(const uint32_t *a, int na,
          const uint32_t *b, int nb,
          uint32_t *res, int max_res);

#endif /* FULLTEXT0_H */
