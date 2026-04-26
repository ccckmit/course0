#include "fulltext0.h"

/* ── tokenize ─────────────────────────────────────────────────────────────── */
int tokenize(const char *line, char out[][MAX_TOKEN_LEN], int max_out)
{
    const unsigned char *p = (const unsigned char *)line;
    int n = 0;

    while (*p && n < max_out) {
        uint32_t cp;
        int len = utf8_decode(p, &cp);
        if (len <= 0) { p++; continue; }

        if (is_cjk(cp)) {
            /* ── CJK section: emit bigrams and unigrams ── */
            /* bigram: current + next CJK */
            uint32_t cp2;
            int len2 = 0;
            const unsigned char *q = p + len;
            if (*q) len2 = utf8_decode(q, &cp2);

            /* Emit bigram if next char is also CJK */
            if (len2 > 0 && is_cjk(cp2) && n < max_out) {
                int blen = len + len2;
                if (blen < MAX_TOKEN_LEN) {
                    memcpy(out[n], p, blen);
                    out[n][blen] = '\0';
                    n++;
                }
            }

            /* Emit unigram */
            if (n < max_out && len < MAX_TOKEN_LEN) {
                memcpy(out[n], p, len);
                out[n][len] = '\0';
                n++;
            }

            p += len;
        } else if (is_ascii_alnum((char)cp)) {
            /* ── ASCII word token ── */
            char buf[MAX_TOKEN_LEN];
            int bi = 0;
            while (*p && bi < MAX_TOKEN_LEN-1) {
                uint32_t c2; int l2 = utf8_decode(p, &c2);
                if (l2 != 1 || !is_ascii_alnum((char)c2)) break;
                buf[bi++] = ascii_lower((char)c2);
                p += l2;
            }
            buf[bi] = '\0';
            if (bi > 0 && n < max_out) {
                memcpy(out[n], buf, bi+1);
                n++;
            }
        } else {
            /* skip punctuation / whitespace */
            p += (len > 0 ? len : 1);
        }
    }
    return n;
}

/* ── intersect ───────────────────────────────────────────────────────────── */
int intersect(const uint32_t *a, int na,
              const uint32_t *b, int nb,
              uint32_t *res, int max_res)
{
    int i=0, j=0, k=0;
    while (i<na && j<nb && k<max_res) {
        if      (a[i] < b[j]) i++;
        else if (a[i] > b[j]) j++;
        else { res[k++] = a[i]; i++; j++; }
    }
    return k;
}

/* ── unite ───────────────────────────────────────────────────────────────── */
int unite(const uint32_t *a, int na,
          const uint32_t *b, int nb,
          uint32_t *res, int max_res)
{
    int i=0, j=0, k=0;
    while ((i<na || j<nb) && k<max_res) {
        uint32_t va = (i<na) ? a[i] : UINT32_MAX;
        uint32_t vb = (j<nb) ? b[j] : UINT32_MAX;
        if (va < vb)       { res[k++]=va; i++; }
        else if (va > vb)  { res[k++]=vb; j++; }
        else               { res[k++]=va; i++; j++; }
    }
    return k;
}
