#include "fulltext0.h"

/* ── Index reader ─────────────────────────────────────────────────────────── */
typedef struct {
    FILE     *fp;
    uint32_t  num_terms;
    uint32_t  num_docs;
    uint32_t  term_block_start;
    uint32_t  post_block_start;

    /* In-memory term table (loaded on open) */
    TermEntry *terms;
    uint32_t   terms_cap;
} Index;

static int index_open(Index *idx, const char *path)
{
    memset(idx, 0, sizeof(*idx));
    idx->fp = fopen(path, "rb");
    if (!idx->fp) { perror("fopen index"); return -1; }

    /* Read header block */
    unsigned char hdr[BLOCK_SIZE];
    if (fread(hdr, 1, BLOCK_SIZE, idx->fp) != BLOCK_SIZE) {
        fprintf(stderr, "Short header read\n"); fclose(idx->fp); return -1;
    }
    uint32_t magic; memcpy(&magic, hdr, 4);
    if (magic != IDX_MAGIC) { fprintf(stderr,"Bad magic\n"); fclose(idx->fp); return -1; }

    memcpy(&idx->num_terms,        hdr+ 8, 4);
    memcpy(&idx->num_docs,         hdr+12, 4);
    memcpy(&idx->term_block_start, hdr+16, 4);
    memcpy(&idx->post_block_start, hdr+20, 4);

    /* Load term table into memory */
    idx->terms_cap = idx->num_terms;
    idx->terms = malloc(idx->num_terms * sizeof(TermEntry));
    if (!idx->terms) { fprintf(stderr,"OOM\n"); fclose(idx->fp); return -1; }

    /* Seek to term table start */
    fseek(idx->fp, (long)idx->term_block_start * BLOCK_SIZE, SEEK_SET);

    /* Read term table raw bytes */
    uint32_t term_bytes = (idx->post_block_start - idx->term_block_start) * BLOCK_SIZE;
    unsigned char *tbuf = malloc(term_bytes);
    if (!tbuf) { fprintf(stderr,"OOM\n"); free(idx->terms); fclose(idx->fp); return -1; }
    if (fread(tbuf, 1, term_bytes, idx->fp) != term_bytes) {
        fprintf(stderr,"Warning: short term table read\n");
    }

    size_t pos = 0;
    for (uint32_t i=0; i<idx->num_terms; i++) {
        if (pos+2 > term_bytes) break;
        uint16_t tlen; memcpy(&tlen, tbuf+pos, 2); pos+=2;
        if (pos+tlen > term_bytes) break;
        memcpy(idx->terms[i].term, tbuf+pos, tlen);
        idx->terms[i].term[tlen] = '\0';
        pos += tlen;
        memcpy(&idx->terms[i].post_offset, tbuf+pos, 4); pos+=4;
        memcpy(&idx->terms[i].post_count,  tbuf+pos, 4); pos+=4;
    }
    free(tbuf);
    return 0;
}

static void index_close(Index *idx)
{
    if (idx->fp)    fclose(idx->fp);
    if (idx->terms) free(idx->terms);
}

/* Binary search for a term; returns index or -1 */
static int term_lookup(Index *idx, const char *term)
{
    /* Linear scan (term table not sorted — use linear for correctness)
     * For 1000-doc corpus this is fast enough. For large corpora use a hash. */
    for (uint32_t i=0; i<idx->num_terms; i++) {
        if (strcmp(idx->terms[i].term, term)==0) return (int)i;
    }
    return -1;
}

/* Read posting list for term index ti into *out (caller frees) */
static uint32_t *read_posting(Index *idx, int ti, uint32_t *count_out)
{
    uint32_t count = idx->terms[ti].post_count;
    uint32_t offset = idx->terms[ti].post_offset;
    *count_out = count;
    if (count==0) return NULL;

    uint32_t *buf = malloc(count * sizeof(uint32_t));
    if (!buf) { fprintf(stderr,"OOM\n"); return NULL; }

    long base = (long)idx->post_block_start * BLOCK_SIZE;
    fseek(idx->fp, base + offset, SEEK_SET);

    unsigned char raw[4];
    for (uint32_t i=0;i<count;i++) {
        if (fread(raw, 1, 4, idx->fp) != 4) break;
        buf[i] = (uint32_t)raw[0]|(raw[1]<<8)|(raw[2]<<16)|(raw[3]<<24);
    }
    return buf;
}

/* ── fetch a single doc line by row id ─────────────────────────────────── */
/* (available for future use) */

/* ── Search: AND of query tokens ─────────────────────────────────────────── */
/* Query strategy:
 *   1. Tokenise the query (same as indexing)
 *   2. For each query token:
 *        - if token is a bigram → look up bigram posting list directly
 *        - if token is a unigram and bigram not found → look up unigram
 *   3. Remove duplicate tokens
 *   4. Intersect all posting lists (AND semantics)
 *   5. Load matching lines from corpus, filter those that contain ALL query
 *      tokens as substrings (post-filter to remove false positives from bigram
 *      overlap).
 */

#define MAX_RESULT  4096

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <query> [corpus]\n", argv[0]);
        fprintf(stderr, "       query enclosed in quotes if multi-word\n");
        return 1;
    }
    const char *query_str = argv[1];
    const char *corpus    = (argc>=3) ? argv[2] : CORPUS_PATH;
    const char *index_path= (argc>=4) ? argv[3] : INDEX_PATH;

    Index idx;
    if (index_open(&idx, index_path) != 0) return 1;

    /* Tokenise query */
    char qtoks[MAX_TOKENS][MAX_TOKEN_LEN];
    int  nq = tokenize(query_str, qtoks, MAX_TOKENS);

    if (nq == 0) {
        printf("No tokens in query.\n");
        index_close(&idx);
        return 0;
    }

    /* Deduplicate query tokens */
    char utoks[MAX_TOKENS][MAX_TOKEN_LEN];
    int  nu = 0;
    for (int i=0;i<nq;i++) {
        int dup=0;
        for (int j=0;j<nu;j++) if(strcmp(utoks[j],qtoks[i])==0){dup=1;break;}
        if (!dup) { memcpy(utoks[nu], qtoks[i], MAX_TOKEN_LEN); nu++; }
    }

    /* For each unique token, get posting list */
    uint32_t *plists[MAX_TOKENS];
    uint32_t  pcount[MAX_TOKENS];
    int valid_terms[MAX_TOKENS];
    int nvalid = 0;

    for (int i=0;i<nu;i++) {
        int ti = term_lookup(&idx, utoks[i]);
        if (ti < 0) {
            /* term not found → no results */
            fprintf(stderr, "Term not found: '%s'\n", utoks[i]);
            plists[i] = NULL; pcount[i] = 0;
        } else {
            plists[i] = read_posting(&idx, ti, &pcount[i]);
            valid_terms[nvalid++] = i;
        }
    }

    if (nvalid == 0) {
        printf("No matching documents.\n");
        index_close(&idx);
        return 0;
    }

    /* Intersect all valid posting lists */
    uint32_t *result = malloc(idx.num_docs * sizeof(uint32_t));
    uint32_t  rcount = 0;

    if (result == NULL) { fprintf(stderr,"OOM\n"); index_close(&idx); return 1; }

    /* Start with first valid */
    int first = valid_terms[0];
    rcount = pcount[first];
    memcpy(result, plists[first], rcount * sizeof(uint32_t));

    for (int k=1; k<nvalid; k++) {
        int vi = valid_terms[k];
        uint32_t *tmp = malloc(rcount * sizeof(uint32_t));
        if (!tmp) { fprintf(stderr,"OOM\n"); break; }
        rcount = intersect(result, rcount, plists[vi], pcount[vi], tmp, rcount);
        memcpy(result, tmp, rcount * sizeof(uint32_t));
        free(tmp);
    }

    /* ── Post-filter: verify each candidate contains all raw query chars ── */
    /* We open corpus once, scan line by line, print matching ones */
    FILE *corp = fopen(corpus, "r");
    if (!corp) { perror("fopen corpus"); free(result); index_close(&idx); return 1; }

    char line[4096];
    uint32_t doc_id = 0;
    uint32_t ri = 0;
    int found = 0;

    printf("Results for query: \"%s\"\n", query_str);
    printf("────────────────────────────────────────\n");

    while (fgets(line, sizeof(line), corp) && ri < rcount) {
        if (doc_id == result[ri]) {
            int ll=strlen(line);
            while(ll>0&&(line[ll-1]=='\n'||line[ll-1]=='\r')) line[--ll]='\0';

            /* Post-filter: check original query_str tokens exist in line */
            /* We check each unique token (lowercased for ASCII) */
            int pass = 1;
            /* Simple substring check */
            /* For CJK: direct strstr works on UTF-8 */
            /* For ASCII: case-insensitive check */
            for (int i=0;i<nu;i++) {
                const char *tok = utoks[i];
                /* Check if tok is ASCII or CJK */
                uint32_t cp = 0; utf8_decode((const unsigned char*)tok, &cp);
                if (cp < 0x80) {
                    /* ASCII: case-insensitive search */
                    char lower_line[4096];
                    int j=0;
                    for(;line[j]&&j<4095;j++) lower_line[j]=ascii_lower(line[j]);
                    lower_line[j]='\0';
                    if (!strstr(lower_line, tok)) { pass=0; break; }
                } else {
                    if (!strstr(line, tok)) { pass=0; break; }
                }
            }

            if (pass) {
                printf("[%u] %s\n", doc_id+1, line);
                found++;
            }
            ri++;
        }
        doc_id++;
    }
    fclose(corp);

    printf("────────────────────────────────────────\n");
    printf("Found %d document(s).\n", found);

    /* Cleanup */
    free(result);
    for (int i=0;i<nu;i++) if(plists[i]) free(plists[i]);
    index_close(&idx);
    return 0;
}
