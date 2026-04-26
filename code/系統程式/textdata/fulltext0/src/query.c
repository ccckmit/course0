#include "fulltext0.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* ── ① Query-side hash map for O(1) term lookup ─────────────────────────── */
#define QHASH_SIZE (1<<17)   /* 131072 buckets */
static int32_t g_qhash[QHASH_SIZE];   /* value: index into idx.terms[], or -1 */

static uint32_t hash_str(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (unsigned char)*s++; h *= 16777619u; }
    return h;
}

static void qhash_build(const TermEntry *terms, uint32_t n)
{
    memset(g_qhash, -1, sizeof(g_qhash));
    for (uint32_t i = 0; i < n; i++) {
        uint32_t h = hash_str(terms[i].term) & (QHASH_SIZE-1);
        while (g_qhash[h] != -1) h = (h+1) & (QHASH_SIZE-1);
        g_qhash[h] = (int32_t)i;
    }
}

static int qhash_lookup(const TermEntry *terms, const char *term)
{
    uint32_t h = hash_str(term) & (QHASH_SIZE-1);
    while (g_qhash[h] != -1) {
        if (strcmp(terms[g_qhash[h]].term, term) == 0) return g_qhash[h];
        h = (h+1) & (QHASH_SIZE-1);
    }
    return -1;
}

/* ── Index structure ─────────────────────────────────────────────────────── */
typedef struct {
    /* term table (loaded into memory) */
    TermEntry *terms;
    uint32_t   num_terms;
    uint32_t   num_docs;

    /* ④ posting section memory-mapped */
    int            post_fd;
    unsigned char *post_mmap;   /* base of mmap'd posting region */
    size_t         post_mmap_len;
    uint32_t       post_block_start;   /* first block of posting section */

    /* ③ doc offset table (loaded into memory) */
    uint64_t *doc_offsets;
} Index;

static int index_open(Index *idx, const char *idx_path, const char *off_path)
{
    memset(idx, 0, sizeof(*idx));

    /* ── Open file and read header ── */
    idx->post_fd = open(idx_path, O_RDONLY);
    if (idx->post_fd < 0) { perror("open index"); return -1; }

    unsigned char hdr[BLOCK_SIZE];
    if (read(idx->post_fd, hdr, BLOCK_SIZE) != BLOCK_SIZE) {
        fprintf(stderr,"Short header\n"); close(idx->post_fd); return -1;
    }

    uint32_t magic; memcpy(&magic, hdr, 4);
    if (magic != IDX_MAGIC) {
        fprintf(stderr,"Bad magic 0x%08X (expected 0x%08X)\n", magic, IDX_MAGIC);
        fprintf(stderr,"Please re-run ./index to rebuild with the new format.\n");
        close(idx->post_fd); return -1;
    }

    uint32_t term_block_start;
    memcpy(&idx->num_terms,       hdr+ 8, 4);
    memcpy(&idx->num_docs,        hdr+12, 4);
    memcpy(&term_block_start,     hdr+16, 4);
    memcpy(&idx->post_block_start,hdr+20, 4);

    /* ── Load term table ── */
    idx->terms = malloc(idx->num_terms * sizeof(TermEntry));
    if (!idx->terms) { fprintf(stderr,"OOM\n"); close(idx->post_fd); return -1; }

    uint32_t term_bytes = (idx->post_block_start - term_block_start) * BLOCK_SIZE;
    unsigned char *tbuf = malloc(term_bytes);
    if (!tbuf) { fprintf(stderr,"OOM\n"); free(idx->terms); close(idx->post_fd); return -1; }

    off_t toff = (off_t)term_block_start * BLOCK_SIZE;
    if (pread(idx->post_fd, tbuf, term_bytes, toff) != (ssize_t)term_bytes) {
        fprintf(stderr,"Short term table read\n");
    }

    size_t pos = 0;
    for (uint32_t i = 0; i < idx->num_terms; i++) {
        if (pos + 2 > term_bytes) break;
        uint16_t tlen; memcpy(&tlen, tbuf+pos, 2); pos += 2;
        if (pos + tlen > term_bytes) break;
        memcpy(idx->terms[i].term, tbuf+pos, tlen);
        idx->terms[i].term[tlen] = '\0';
        pos += tlen;
        memcpy(&idx->terms[i].post_offset,   tbuf+pos, 4); pos += 4;
        memcpy(&idx->terms[i].post_count,    tbuf+pos, 4); pos += 4;
        memcpy(&idx->terms[i].post_byte_len, tbuf+pos, 4); pos += 4;
    }
    free(tbuf);

    /* ① Build in-memory hash map for O(1) term lookup */
    qhash_build(idx->terms, idx->num_terms);

    /* ④ mmap the posting section */
    {
        struct stat st;
        if (fstat(idx->post_fd, &st) < 0) { perror("fstat"); return -1; }
        long post_start = (long)idx->post_block_start * BLOCK_SIZE;
        idx->post_mmap_len = (size_t)(st.st_size - post_start);
        if (idx->post_mmap_len > 0) {
            idx->post_mmap = mmap(NULL, idx->post_mmap_len,
                                  PROT_READ, MAP_PRIVATE,
                                  idx->post_fd, post_start);
            if (idx->post_mmap == MAP_FAILED) {
                perror("mmap posting"); idx->post_mmap = NULL;
            } else {
                /* Advise sequential-ish read patterns */
                madvise(idx->post_mmap, idx->post_mmap_len, MADV_WILLNEED);
            }
        }
    }

    /* ③ Load doc offset table */
    {
        FILE *of = fopen(off_path, "rb");
        if (!of) {
            fprintf(stderr,"Warning: cannot open %s – will scan corpus for each result\n",
                    off_path);
            idx->doc_offsets = NULL;
        } else {
            idx->doc_offsets = malloc(idx->num_docs * sizeof(uint64_t));
            if (!idx->doc_offsets) { fprintf(stderr,"OOM\n"); fclose(of); return -1; }
            if (fread(idx->doc_offsets, sizeof(uint64_t), idx->num_docs, of)
                    != idx->num_docs) {
                fprintf(stderr,"Short offsets read\n");
            }
            fclose(of);
        }
    }

    return 0;
}

static void index_close(Index *idx)
{
    if (idx->post_mmap && idx->post_mmap != MAP_FAILED)
        munmap(idx->post_mmap, idx->post_mmap_len);
    if (idx->post_fd >= 0) close(idx->post_fd);
    free(idx->terms);
    free(idx->doc_offsets);
}

/* ── ② Decode VarInt delta posting list from mmap'd region ─────────────── */
static uint32_t *decode_posting(Index *idx, int ti, uint32_t *count_out)
{
    uint32_t count  = idx->terms[ti].post_count;
    uint32_t offset = idx->terms[ti].post_offset;
    uint32_t blen   = idx->terms[ti].post_byte_len;
    *count_out = count;
    if (count == 0 || !idx->post_mmap) return NULL;

    uint32_t *buf = malloc(count * sizeof(uint32_t));
    if (!buf) { fprintf(stderr,"OOM\n"); return NULL; }

    const unsigned char *p   = idx->post_mmap + offset;
    const unsigned char *end = p + blen;
    uint32_t prev = 0;
    for (uint32_t i = 0; i < count && p < end; i++) {
        uint32_t delta;
        int n = varint_decode(p, &delta);
        if (n < 0) break;
        prev += delta;
        buf[i] = prev;
        p += n;
    }
    return buf;
}

/* ── ③ Fetch corpus line by doc_id using offset index ──────────────────── */
static void fetch_line(FILE *corp, const Index *idx,
                       uint32_t doc_id, char *line, size_t line_cap)
{
    if (idx->doc_offsets) {
        /* Direct seek — O(1) */
        fseek(corp, (long)idx->doc_offsets[doc_id], SEEK_SET);
        if (fgets(line, (int)line_cap, corp)) {
            int ll = (int)strlen(line);
            while (ll > 0 && (line[ll-1]=='\n'||line[ll-1]=='\r')) line[--ll]='\0';
        }
    } else {
        /* Fallback: linear scan */
        rewind(corp);
        uint32_t cur = 0;
        while (fgets(line, (int)line_cap, corp)) {
            if (cur++ == doc_id) {
                int ll=(int)strlen(line);
                while(ll>0&&(line[ll-1]=='\n'||line[ll-1]=='\r')) line[--ll]='\0';
                return;
            }
        }
    }
}

/* ── main ────────────────────────────────────────────────────────────────── */
#define MAX_RESULT 65536

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <query> [corpus] [index] [offsets]\n", argv[0]);
        return 1;
    }
    const char *query_str  = argv[1];
    const char *corpus     = (argc>=3) ? argv[2] : CORPUS_PATH;
    const char *index_path = (argc>=4) ? argv[3] : INDEX_PATH;
    const char *off_path   = (argc>=5) ? argv[4] : OFFSET_PATH;

    Index idx;
    if (index_open(&idx, index_path, off_path) != 0) return 1;

    /* Tokenise and dedup query */
    char qtoks[MAX_TOKENS][MAX_TOKEN_LEN];
    int  nq = tokenize(query_str, qtoks, MAX_TOKENS);
    if (nq == 0) {
        printf("No tokens in query.\n");
        index_close(&idx); return 0;
    }

    char utoks[MAX_TOKENS][MAX_TOKEN_LEN];
    int  nu = 0;
    for (int i = 0; i < nq; i++) {
        int dup = 0;
        for (int j = 0; j < nu; j++)
            if (strcmp(utoks[j], qtoks[i])==0) { dup=1; break; }
        if (!dup) { memcpy(utoks[nu++], qtoks[i], MAX_TOKEN_LEN); }
    }

    /* ① Lookup each token — O(1) per term */
    uint32_t *plists[MAX_TOKENS];
    uint32_t  pcount[MAX_TOKENS];
    int       valid[MAX_TOKENS];
    int       nvalid = 0;

    for (int i = 0; i < nu; i++) {
        int ti = qhash_lookup(idx.terms, utoks[i]);  /* O(1) */
        if (ti < 0) {
            fprintf(stderr, "Term not found: '%s'\n", utoks[i]);
            plists[i] = NULL; pcount[i] = 0;
        } else {
            plists[i] = decode_posting(&idx, ti, &pcount[i]);
            valid[nvalid++] = i;
        }
    }

    if (nvalid == 0) {
        printf("No matching documents.\n");
        index_close(&idx); return 0;
    }

    /* Sort valid terms by posting list size (shortest first) → faster intersect */
    for (int a = 0; a < nvalid-1; a++)
        for (int b = a+1; b < nvalid; b++)
            if (pcount[valid[a]] > pcount[valid[b]]) {
                int tmp = valid[a]; valid[a] = valid[b]; valid[b] = tmp;
            }

    /* AND intersect posting lists */
    uint32_t *result = malloc(MAX_RESULT * sizeof(uint32_t));
    if (!result) { fprintf(stderr,"OOM\n"); index_close(&idx); return 1; }

    int first = valid[0];
    uint32_t rcount = pcount[first] < MAX_RESULT ? pcount[first] : MAX_RESULT;
    memcpy(result, plists[first], rcount * sizeof(uint32_t));

    for (int k = 1; k < nvalid && rcount > 0; k++) {
        int vi = valid[k];
        uint32_t *tmp = malloc(rcount * sizeof(uint32_t));
        if (!tmp) { fprintf(stderr,"OOM\n"); break; }
        rcount = (uint32_t)intersect(result, (int)rcount,
                                     plists[vi], (int)pcount[vi],
                                     tmp, (int)rcount);
        memcpy(result, tmp, rcount * sizeof(uint32_t));
        free(tmp);
    }

    /* ③ Fetch and post-filter results using offset index */
    FILE *corp = fopen(corpus, "r");
    if (!corp) { perror("fopen corpus"); free(result); index_close(&idx); return 1; }

    /* Pre-lowercase line buffer for ASCII matching */
    char line[4096], lower_line[4096];
    int found = 0;

    printf("Results for query: \"%s\"\n", query_str);
    printf("────────────────────────────────────────\n");

    for (uint32_t ri = 0; ri < rcount; ri++) {
        uint32_t doc_id = result[ri];
        fetch_line(corp, &idx, doc_id, line, sizeof(line));  /* ③ O(1) seek */

        /* Post-filter: all tokens must be present as substrings */
        int pass = 1;
        int ll = (int)strlen(line);
        /* Build lowercase version once per line */
        for (int j = 0; j < ll; j++) lower_line[j] = ascii_lower(line[j]);
        lower_line[ll] = '\0';

        for (int i = 0; i < nu && pass; i++) {
            const char *tok = utoks[i];
            uint32_t cp0 = 0;
            utf8_decode((const unsigned char*)tok, &cp0);
            if (cp0 < 0x80) {
                if (!strstr(lower_line, tok)) pass = 0;
            } else {
                if (!strstr(line, tok)) pass = 0;
            }
        }

        if (pass) {
            printf("[%u] %s\n", doc_id+1, line);
            found++;
        }
    }
    fclose(corp);

    printf("────────────────────────────────────────\n");
    printf("Found %d document(s).\n", found);

    /* Cleanup */
    free(result);
    for (int i = 0; i < nu; i++) free(plists[i]);
    index_close(&idx);
    return 0;
}
