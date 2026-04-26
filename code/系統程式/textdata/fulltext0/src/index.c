#include "fulltext0.h"
#include <sys/stat.h>
#include <sys/types.h>

/* ── In-memory index ─────────────────────────────────────────────────────── */
static TermEntry   g_terms[MAX_TERMS];
static PostingList g_posts[MAX_TERMS];
static int         g_num_terms = 0;

/* Open-addressing hash: term → index in g_terms[] */
#define HASH_SIZE (1<<17)   /* 131072 buckets, load < 1% for 1297 terms */
static int32_t g_hash[HASH_SIZE];

static uint32_t hash_term(const char *s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (unsigned char)*s++; h *= 16777619u; }
    return h;
}

static int find_or_insert(const char *term)
{
    uint32_t h = hash_term(term) & (HASH_SIZE-1);
    for (;;) {
        if (g_hash[h] == -1) {
            if (g_num_terms >= MAX_TERMS) { fprintf(stderr,"Too many terms\n"); exit(1); }
            int idx = g_num_terms++;
            g_hash[h] = idx;
            size_t tl = strlen(term);
            if (tl >= MAX_TOKEN_LEN) tl = MAX_TOKEN_LEN-1;
            memcpy(g_terms[idx].term, term, tl);
            g_terms[idx].term[tl]       = '\0';
            g_terms[idx].post_offset    = 0;
            g_terms[idx].post_count     = 0;
            g_terms[idx].post_byte_len  = 0;
            g_posts[idx].doc_ids = NULL;
            g_posts[idx].count   = 0;
            g_posts[idx].cap     = 0;
            return idx;
        }
        if (strcmp(g_terms[g_hash[h]].term, term) == 0) return g_hash[h];
        h = (h+1) & (HASH_SIZE-1);
    }
}

static void add_posting(int ti, uint32_t doc_id)
{
    PostingList *pl = &g_posts[ti];
    if (pl->count > 0 && pl->doc_ids[pl->count-1] == doc_id) return; /* dedup */
    if (pl->count >= pl->cap) {
        pl->cap = pl->cap ? pl->cap*2 : 8;
        pl->doc_ids = realloc(pl->doc_ids, pl->cap * sizeof(uint32_t));
        if (!pl->doc_ids) { fprintf(stderr,"OOM\n"); exit(1); }
    }
    pl->doc_ids[pl->count++] = doc_id;
}

/* ── ② Write posting lists with VarInt delta encoding ──────────────────── */
/*
 * For each term's posting list [d0, d1, d2, …]:
 *   encode d0 as varint, then (d1-d0), (d2-d1), … as varints.
 * This turns monotonically increasing IDs into small deltas,
 * most fitting in 1 byte each.
 */
static size_t encode_postings(unsigned char *out, const uint32_t *ids, uint32_t count)
{
    size_t pos = 0;
    uint32_t prev = 0;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t delta = ids[i] - prev;
        pos += varint_encode(out + pos, delta);
        prev = ids[i];
    }
    return pos;
}

/* ── Write index to disk in 4K blocks ────────────────────────────────────── */
static void write_index(uint32_t num_docs, const uint64_t *doc_offsets)
{
    mkdir("_index", 0755);

    /* ── ③ Write offsets.bin ── */
    {
        FILE *of = fopen(OFFSET_PATH, "wb");
        if (!of) { perror("fopen offsets.bin"); exit(1); }
        if (fwrite(doc_offsets, sizeof(uint64_t), num_docs, of) != num_docs) {
            fprintf(stderr,"short write offsets\n"); exit(1);
        }
        fclose(of);
        printf("Offsets written: %s  (%u entries, %zu bytes)\n",
               OFFSET_PATH, num_docs, (size_t)num_docs * sizeof(uint64_t));
    }

    /* ── Build compressed posting buffer ── */
    /* Worst case: VARINT_MAX bytes per doc_id */
    size_t post_cap = 0;
    for (int i = 0; i < g_num_terms; i++)
        post_cap += g_posts[i].count * VARINT_MAX + 1;

    unsigned char *post_buf = malloc(post_cap);
    if (!post_buf) { fprintf(stderr,"OOM\n"); exit(1); }
    size_t pb_pos = 0;

    /* ── Build term table buffer ── */
    /* per record: 2 + term_len + 4 + 4 + 4 = 14 + term_len */
    size_t term_cap = 0;
    for (int i = 0; i < g_num_terms; i++)
        term_cap += 14 + strlen(g_terms[i].term);

    unsigned char *term_buf = malloc(term_cap + 64);
    if (!term_buf) { fprintf(stderr,"OOM\n"); exit(1); }
    size_t tb_pos = 0;

    for (int i = 0; i < g_num_terms; i++) {
        /* Compress posting list */
        g_terms[i].post_offset   = (uint32_t)pb_pos;
        g_terms[i].post_count    = g_posts[i].count;
        size_t blen = encode_postings(post_buf + pb_pos,
                                      g_posts[i].doc_ids, g_posts[i].count);
        g_terms[i].post_byte_len = (uint32_t)blen;
        pb_pos += blen;

        /* Serialise term record */
        uint16_t tlen = (uint16_t)strlen(g_terms[i].term);
        term_buf[tb_pos]   =  tlen & 0xFF;
        term_buf[tb_pos+1] = (tlen >> 8) & 0xFF;
        tb_pos += 2;
        memcpy(term_buf + tb_pos, g_terms[i].term, tlen);
        tb_pos += tlen;

        /* post_offset, post_count, post_byte_len (all uint32 LE) */
        uint32_t fields[3] = {
            g_terms[i].post_offset,
            g_terms[i].post_count,
            g_terms[i].post_byte_len
        };
        for (int f = 0; f < 3; f++) {
            term_buf[tb_pos+0] =  fields[f]        & 0xFF;
            term_buf[tb_pos+1] = (fields[f] >>  8) & 0xFF;
            term_buf[tb_pos+2] = (fields[f] >> 16) & 0xFF;
            term_buf[tb_pos+3] = (fields[f] >> 24) & 0xFF;
            tb_pos += 4;
        }
    }

    /* Block positions */
    uint32_t term_block_start = 1;
    uint32_t term_blocks      = (uint32_t)((tb_pos + BLOCK_SIZE-1) / BLOCK_SIZE);
    uint32_t post_block_start = term_block_start + term_blocks;

    /* Open index file */
    FILE *fp = fopen(INDEX_PATH, "wb");
    if (!fp) { perror("fopen index"); exit(1); }

    /* Block 0: header */
    unsigned char hdr[BLOCK_SIZE];
    memset(hdr, 0, BLOCK_SIZE);
    uint32_t magic = IDX_MAGIC, version = IDX_VERSION;
    memcpy(hdr+ 0, &magic,            4);
    memcpy(hdr+ 4, &version,          4);
    memcpy(hdr+ 8, &g_num_terms,      4);
    memcpy(hdr+12, &num_docs,         4);
    memcpy(hdr+16, &term_block_start, 4);
    memcpy(hdr+20, &post_block_start, 4);
    if (fwrite(hdr, 1, BLOCK_SIZE, fp) != BLOCK_SIZE) {
        fprintf(stderr,"short write hdr\n"); exit(1);
    }

    /* Term table blocks */
    size_t written = 0;
    while (written < tb_pos) {
        unsigned char blk[BLOCK_SIZE];
        size_t chunk = tb_pos - written;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;
        memcpy(blk, term_buf + written, chunk);
        if (chunk < BLOCK_SIZE) memset(blk + chunk, 0, BLOCK_SIZE - chunk);
        if (fwrite(blk, 1, BLOCK_SIZE, fp) != BLOCK_SIZE) {
            fprintf(stderr,"short write term\n"); exit(1);
        }
        written += chunk;
    }

    /* Posting blocks */
    written = 0;
    while (written < pb_pos) {
        unsigned char blk[BLOCK_SIZE];
        size_t chunk = pb_pos - written;
        if (chunk > BLOCK_SIZE) chunk = BLOCK_SIZE;
        memcpy(blk, post_buf + written, chunk);
        if (chunk < BLOCK_SIZE) memset(blk + chunk, 0, BLOCK_SIZE - chunk);
        if (fwrite(blk, 1, BLOCK_SIZE, fp) != BLOCK_SIZE) {
            fprintf(stderr,"short write post\n"); exit(1);
        }
        written += chunk;
    }

    fclose(fp);
    free(term_buf);
    free(post_buf);

    /* Stats */
    long fsize = 0;
    { FILE *f2 = fopen(INDEX_PATH,"rb");
      if (f2) { fseek(f2,0,SEEK_END); fsize=ftell(f2); fclose(f2); } }

    /* Compute raw posting size for comparison */
    size_t raw_post = 0;
    for (int i = 0; i < g_num_terms; i++) raw_post += g_posts[i].count * 4;

    printf("Index written : %s\n", INDEX_PATH);
    printf("  docs        : %u\n",  num_docs);
    printf("  terms       : %d\n",  g_num_terms);
    printf("  file size   : %ld bytes (%ld blocks)\n", fsize, fsize/BLOCK_SIZE);
    printf("  posting raw : %zu bytes\n", raw_post);
    printf("  posting zip : %zu bytes  (%.1f%% of raw)\n",
           pb_pos, raw_post ? 100.0*pb_pos/raw_post : 0.0);
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    const char *corpus = (argc >= 2) ? argv[1] : CORPUS_PATH;

    memset(g_hash, -1, sizeof(g_hash));

    FILE *fp = fopen(corpus, "r");
    if (!fp) { perror("fopen corpus"); return 1; }

    /* Pre-allocate offset table (dynamic, grows as needed) */
    uint32_t  off_cap = 4096;
    uint64_t *doc_offsets = malloc(off_cap * sizeof(uint64_t));
    if (!doc_offsets) { fprintf(stderr,"OOM\n"); return 1; }

    char     line[4096];
    char     tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    uint32_t doc_id = 0;

    while (1) {
        /* ③ Record byte offset before reading the line */
        int64_t line_offset = (int64_t)ftell(fp);
        if (line_offset < 0) break;

        if (!fgets(line, sizeof(line), fp)) break;

        int ll = (int)strlen(line);
        while (ll > 0 && (line[ll-1]=='\n'||line[ll-1]=='\r')) line[--ll] = '\0';
        if (ll == 0) continue;

        /* Grow offset table if needed */
        if (doc_id >= off_cap) {
            off_cap *= 2;
            doc_offsets = realloc(doc_offsets, off_cap * sizeof(uint64_t));
            if (!doc_offsets) { fprintf(stderr,"OOM\n"); return 1; }
        }
        doc_offsets[doc_id] = (uint64_t)line_offset;

        int ntok = tokenize(line, tokens, MAX_TOKENS);
        for (int t = 0; t < ntok; t++) {
            int idx = find_or_insert(tokens[t]);
            add_posting(idx, doc_id);
        }
        doc_id++;

        if (doc_id % 100 == 0)
            fprintf(stderr, "\r  Indexed %u docs, %d terms ...", doc_id, g_num_terms);
    }
    fprintf(stderr, "\n");
    fclose(fp);

    write_index(doc_id, doc_offsets);

    free(doc_offsets);
    for (int i = 0; i < g_num_terms; i++) free(g_posts[i].doc_ids);
    return 0;
}
