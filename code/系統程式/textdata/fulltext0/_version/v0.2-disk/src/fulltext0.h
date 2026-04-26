#ifndef FULLTEXT0_H
#define FULLTEXT0_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ── Constants ─────────────────────────────────────────────────────────────── */
#define MAX_TOKEN_LEN   64          /* max bytes in one token (UTF-8)           */
#define MAX_TOKENS      128         /* max tokens per document row              */
#define MAX_POSTINGS    (1<<20)     /* max total posting entries in memory      */
#define MAX_TERMS       (1<<16)     /* max unique terms in the index            */
#define BLOCK_SIZE      4096        /* index output block size (4K)             */
#define INDEX_PATH      "_index/data.idx"
#define CORPUS_PATH     "_data/corpus.txt"

/* ── Index file magic / version ───────────────────────────────────────────── */
#define IDX_MAGIC       0x46545830u  /* 'FTX0' */
#define IDX_VERSION     1

/* ─── On-disk layout ──────────────────────────────────────────────────────────
 *
 *  [Block 0]  Header block (4096 bytes)
 *    uint32  magic
 *    uint32  version
 *    uint32  num_terms
 *    uint32  num_docs
 *    uint32  term_block_start   (block index where term table begins)
 *    uint32  post_block_start   (block index where posting lists begin)
 *    char    pad[4096-24]
 *
 *  [Block term_block_start … post_block_start-1]  Term table
 *    Repeated records, each:
 *      uint16  term_len          (bytes)
 *      char    term[term_len]
 *      uint32  post_offset       (byte offset into posting section)
 *      uint32  post_count        (number of doc_ids in posting list)
 *
 *  [Block post_block_start …]   Posting lists
 *    Packed uint32 doc_ids (0-based row numbers), sorted ascending
 *
 * ─────────────────────────────────────────────────────────────────────────── */

/* In-memory structures */
typedef struct {
    char    term[MAX_TOKEN_LEN];
    uint32_t post_offset;   /* byte offset into posting section          */
    uint32_t post_count;
} TermEntry;

typedef struct {
    uint32_t *doc_ids;      /* sorted array of doc ids                   */
    uint32_t  count;
    uint32_t  cap;
} PostingList;

/* ── Utility: decode one UTF-8 codepoint ────────────────────────────────── */
/* Returns byte length (1-4) or 0 on error. Sets *cp to codepoint.          */
static inline int utf8_decode(const unsigned char *s, uint32_t *cp)
{
    unsigned char c = s[0];
    if (c == 0)          { *cp = 0; return 0; }
    if (c < 0x80)        { *cp = c; return 1; }
    if (c < 0xC0)        return -1;
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

/* ── Is codepoint a CJK character? ─────────────────────────────────────── */
static inline int is_cjk(uint32_t cp)
{
    return (cp >= 0x4E00 && cp <= 0x9FFF)   ||  /* CJK Unified Ideographs  */
           (cp >= 0x3400 && cp <= 0x4DBF)   ||  /* Extension A             */
           (cp >= 0xF900 && cp <= 0xFAFF)   ||  /* CJK Compatibility       */
           (cp >= 0x3000 && cp <= 0x303F)   ||  /* CJK Symbols/Punct       */
           (cp >= 0xFF00 && cp <= 0xFFEF);       /* Halfwidth/Fullwidth     */
}

/* ── Is byte an ASCII letter or digit? ──────────────────────────────────── */
static inline int is_ascii_alnum(char c)
{
    return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9');
}

/* ── Lowercase ASCII char ───────────────────────────────────────────────── */
static inline char ascii_lower(char c)
{
    return (c>='A'&&c<='Z') ? c+32 : c;
}

/* ── Tokenise one line, fill out[] with NUL-terminated tokens ───────────────
 *
 *  Strategy:
 *    • ASCII text  → split on non-alnum boundaries, lowercase
 *    • CJK text    → bigram (2-char) + unigram (1-char) tokens
 *
 *  Returns number of tokens stored in out[].
 */
int tokenize(const char *line, char out[][MAX_TOKEN_LEN], int max_out);

/* ── Intersect two sorted doc-id arrays ─────────────────────────────────── */
/* Result stored in res[], returns count.                                     */
int intersect(const uint32_t *a, int na,
              const uint32_t *b, int nb,
              uint32_t *res, int max_res);

/* ── Union two sorted doc-id arrays ─────────────────────────────────────── */
int unite(const uint32_t *a, int na,
          const uint32_t *b, int nb,
          uint32_t *res, int max_res);

#endif /* FULLTEXT0_H */
